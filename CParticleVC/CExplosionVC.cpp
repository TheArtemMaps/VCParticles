#include "plugin.h"

#include "CAutomobile.h"
#include "CBike.h"
#include "CCamera.h"
#include "CCoronas.h"
#include "CEntity.h"
#include "EventList.h"
#include "CExplosionVC.h"
#include "CGeneral.h"
#include "CFire.h"
#include "CPad.h"
#include "CParticleVC.h"
#include "CPointLights.h"
#include "CShadows.h"
#include "Timer.h"
#include "CVehicle.h"
#include "CWaterLevel.h"
#include "CWorld.h"
#include "MemoryMgr.h"
#include "CAudioEngine.h"
#include "CFireManager.h"
#include "extensions\ScriptCommands.h"
#include "CTxdStore.h"
#include <CStats.h>
#include <game_sa/Fx_c.h>
#include "ParticleObject.h"
using namespace plugin;
CAEExplosionAudioEntity& m_ExplosionAudioEntity2 = *(CAEExplosionAudioEntity*)0xC888D0;
CExplosionVC gaExplosion[NUM_EXPLOSIONS];
// these two were not initialised in original code, I'm really not sure what were they meant to be
RwRGBA colMedExpl = { 0, 0, 0, 0 };
RwRGBA colUpdate = { 0, 0, 0, 0 };
const RwRGBA colAddExplosion = { 160, 160, 160, 255 };
const RwRGBA colGrenade = { 96, 96, 96, 255 };

int AudioHandle = 0;
auto PlayExplosionSound = (void(__thiscall*)(CAEExplosionAudioEntity * AudioEntity, int arg0, CVector * a2, float a4))0x4DCBE0;
auto PlayFireSound = (void(__thiscall*)(CAEFireAudioEntity * ent, int eventId, CVector * posn))0x4DD3C0;
void
CExplosionVC::Initialise()
{
	log("Initialising CExplosion...\n");
	debug("Initialising CExplosion...\n");
	ClearAllExplosions();
	//AudioHandle = DMAudio.CreateEntity(AUDIOTYPE_EXPLOSION, (void*)1);
	//if (AudioHandle >= 0)
	//	DMAudio.SetEntityStatus(AudioHandle, TRUE);
	log("CExplosion ready\n");
	debug("CExplosion ready\n");
}

void
CExplosionVC::ClearAllExplosions()
{
	for (int i = 0; i < ARRAY_SIZE(gaExplosion); i++) {
		gaExplosion[i].m_ExplosionType = EXPLOSION_GRENADE;
		gaExplosion[i].m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
		gaExplosion[i].m_fRadius = 1.0f;
		gaExplosion[i].m_fPropagationRate = 0.0f;
		gaExplosion[i].m_fZshift = 0.0f;
		gaExplosion[i].m_pCreatorEntity = nullptr;
		gaExplosion[i].m_pVictimEntity = nullptr;
		gaExplosion[i].m_fStopTime = 0.0f;
		gaExplosion[i].m_nIteration = 0;
		gaExplosion[i].m_fStartTime = 0.0f;
		gaExplosion[i].m_bIsBoat = false;
		gaExplosion[i].m_bMakeSound = true;
	}
}

void
CExplosionVC::Shutdown()
{
	log("Shutting down CExplosion...\n");
	debug("Shutting down CExplosion...\n");
	//if (AudioHandle >= 0) {
	//	DMAudio.DestroyEntity(AudioHandle);
	//	AudioHandle = AEHANDLE_NONE;
	//}
	log("CExplosion shut down\n");
	debug("CExplosion shut down\n");
}

int8_t
CExplosionVC::GetExplosionActiveCounter(uint8_t id)
{
	return gaExplosion[id].m_nActiveCounter;
}

void
CExplosionVC::ResetExplosionActiveCounter(uint8_t id)
{
	gaExplosion[id].m_nActiveCounter = 0;
}

uint8_t
CExplosionVC::GetExplosionType(uint8_t id)
{
	return gaExplosion[id].m_ExplosionType;
}

bool
CExplosionVC::DoesExplosionMakeSound(uint8_t id)
{
	return gaExplosion[id].m_bMakeSound;
};

CVector*
CExplosionVC::GetExplosionPosition(uint8_t id)
{
	return &gaExplosion[id].m_vecPosition;
}
bool DoesNeedToVehProcessBombTimer(eExplosionTypeVC type) {
	switch (type) {
	case eExplosionTypeVC::EXPLOSION_ROCKET:
	case eExplosionTypeVC::EXPLOSION_QUICK_CAR:
	case eExplosionTypeVC::EXPLOSION_MINE:
	case eExplosionTypeVC::EXPLOSION_OBJECT:
	case eExplosionTypeVC::EXPLOSION_TANK_FIRE:
		return true;
	}
	return false;
};
// Debug purposes
auto ExplosionTypeToString(eExplosionTypeVC explosionType) {
	switch (explosionType) {
	case EXPLOSION_UNDEFINED: return "Undefined";
	case EXPLOSION_GRENADE: return "Grenade";
	case EXPLOSION_MOLOTOV: return "Molotov";
	case EXPLOSION_ROCKET: return "Rocket";
	case EXPLOSION_WEAK_ROCKET: return "Weak Rocket";
	case EXPLOSION_CAR: return "Car";
	case EXPLOSION_QUICK_CAR: return "Quick Car";
	case EXPLOSION_BOAT: return "Boat";
	case EXPLOSION_AIRCRAFT: return "Aircraft";
	case EXPLOSION_MINE: return "Mine";
	case EXPLOSION_OBJECT: return "Object";
	case EXPLOSION_TANK_FIRE: return "Tank Fire";
	case EXPLOSION_SMALL: return "Small";
	case EXPLOSION_RC_VEHICLE: return "RC Vehicle";
	case EXPLOSION_HELI: return "Helicopter";
	case EXPLOSION_HELI2: return "Helicopter 2";
	case EXPLOSION_BARREL: return "Barrel";
	case EXPLOSION_CAR_QUICK: return "Quick Car";
	case EXPLOSION_TANK_GRENADE: return "Tank Grenade";
	case EXPLOSION_HELI_BOMB: return "Helicopter Bomb";
	default: return "Unknown explosion type";
	}
}

bool
#ifdef SIMPLER_MISSIONS
CExplosionVC::AddExplosion(CEntity* explodingEntity, CEntity* culprit, eExplosionTypeVC type, const CVector& pos, uint32_t lifetime, bool makeSound, float radius)
#else
CExplosionVC::AddExplosion(CEntity* victim, CEntity* creator, eExplosionTypeVC type, CVector pos, uint32_t lifetime, uint8_t usesSound, float cameraShake, uint8_t bInvisible)
#endif
{
	CVector pPosn;
	CVector dir;
	CVector posGround;
	RwRGBA colorMedium = colMedExpl;
	RwRGBA color = colAddExplosion;
	RwRGBA colorGrenade = colGrenade;
	bool bDontExplode = false;
	pPosn = pos;
	pPosn.z += 5.0f;
#ifdef FIX_BUGS
	CShadows::AddPermanentShadow(SHADOWTEX_CAR, gpShadowHeliTex, &pPosn, 8.0f, 0.0f, 0.0f, -8.0f, 200, 0, 0, 0, 10.0f, 30000, 1.0f);
#else
	// last two arguments are swapped resulting in no shadow
	CShadows::AddPermanentShadow(SHADOWTEX_CAR, gpShadowHeliTex, &pPosn, 8.0f, 0.0f, 0.0f, -8.0f, 200, 0, 0, 0, 10.0f, 1, 30000.0f);
#endif

	int n = 0;
#ifdef FIX_BUGS
	while (n < ARRAY_SIZE(gaExplosion) && gaExplosion[n].m_nIteration != 0)
#else
	// array overrun is UB
	while (gaExplosion[n].m_nIteration != 0 && n < ARRAY_SIZE(gaExplosion))
#endif
		n++;
	if (n == ARRAY_SIZE(gaExplosion))
		return false;
	CExplosionVC& explosion = gaExplosion[n];
	explosion.m_ExplosionType = type;
	explosion.m_vecPosition = pos;
	explosion.m_fRadius = 1.0f;
	explosion.m_fZshift = 0.0f;
	explosion.m_pCreatorEntity = creator;
	if (creator != nullptr)
		creator->RegisterReference(&explosion.m_pCreatorEntity);
	explosion.m_pVictimEntity = victim;
	if (victim != nullptr)
		victim->RegisterReference(&explosion.m_pVictimEntity);
	explosion.m_nIteration = 1;
	explosion.m_nActiveCounter = 1;
	explosion.m_bIsBoat = false;
	explosion.m_bMakeSound = usesSound;
	float ff = ((float)explosion.m_nIteration * 0.55f);
	explosion.m_nParticlesExpireTime = lifetime != 0 ? CTimer::m_snTimeInMilliseconds + lifetime : 0;
	switch (type)
	{
	case EXPLOSION_GRENADE:
#ifdef SIMPLER_MISSIONS
		explosion.m_fRadius = (radius == -1.0f ? 9.0f : radius);
#else
		explosion.m_fRadius = 9.0f;
#endif
		explosion.m_fPower = 300.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 750;
		explosion.m_fPropagationRate = 0.5f;
		posGround = pos;
		posGround.z = CWorld::FindGroundZFor3DCoord(posGround.x, posGround.y, posGround.z + 3.0f, nullptr, nullptr);
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
			//PlayFireSound(, AE_FIRE, &pos);
			//((FireAudio*)explosion.m_pCreatorEntity)->AddAudioEvent(AE_FIRE, &pos);
		}
		//CEventList::RegisterEvent(EVENT_EXPLOSION, posGround, 250);
		if (Distance(explosion.m_vecPosition, TheCamera.GetPosition()) < 40.0f) {
			uint8_t tmp = CGeneral::GetRandomNumberInRange(0, 64) - 64;
			colorGrenade.green += tmp;
			colorGrenade.blue += tmp;
			CParticle::AddParticle(PARTICLE_EXPLOSION_LFAST, explosion.m_vecPosition, CVector(0.0f, 0.0f, 0.0f), nullptr, 4.5f, colorGrenade);
		}
		break;
	case EXPLOSION_OBJECT: 
		if (!bInvisible) {
			explosion.m_fRadius = 10.0f;
		}
		//explosion.m_nParticlesExpireTime = (float)(CTimer::m_snTimeInMilliseconds + lifetime + 750);
		explosion.m_fPropagationRate = 0.5f;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		break;
	
	case EXPLOSION_MOLOTOV:
	{
		explosion.m_fRadius = 6.0f;
		explosion.m_fPower = 0.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 3000;
		explosion.m_fPropagationRate = 0.5f;
		posGround = pos;
		bool found;
		float tmp = CWorld::FindGroundZFor3DCoord(posGround.x, posGround.y, posGround.z + 3.0f, &found, nullptr);
		if (found)
			posGround.z = tmp;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, -36.0f);
		}
		float waterLevel;
		if (CWaterLevel::GetWaterLevelNoWaves(posGround.x, posGround.y, posGround.z, &waterLevel, nullptr, nullptr)
			&& posGround.z < waterLevel && waterLevel - 6.0f < posGround.z) { // some subway/tunnels check?
			bDontExplode = true;
		}
		else if (found) {
			//gFireManager.StartFire(posGround, 1.8f, false);
			gFireManager.StartFire(posGround, 1.8f, 0, explosion.m_pCreatorEntity, (uint32_t)(CGeneral::GetRandomNumberInRange(5'600.0f, 12'600.0f) * 0.4f), 3, 1);
			//gFireManager.StartFire(posGround, 1.8f, 0, explosion.m_pCreatorEntity, 5000, 5, 0);
		}
		break;
	}
	case EXPLOSION_TANK_FIRE: {
		if (!bInvisible) {
			explosion.m_fRadius = 10.0f;
		}
		//explosion.m_nParticlesExpireTime = (float)(CTimer::m_snTimeInMilliseconds + lifetime + 750);
		explosion.m_fPropagationRate = 0.5f;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		break;
	}
	case EXPLOSION_ROCKET:
	case EXPLOSION_WEAK_ROCKET:
		explosion.m_fRadius = 10.0f;
		explosion.m_fPower = 300.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 750;
		explosion.m_fPropagationRate = 0.5f;
		//CEventList::RegisterEvent(EVENT_EXPLOSION, pos, 250);
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		if (Distance(explosion.m_vecPosition, TheCamera.GetPosition()) < 40.0f)
			CParticle::AddParticle(PARTICLE_EXPLOSION_LFAST, explosion.m_vecPosition, CVector(0.0f, 0.0f, 0.0f), nullptr, 5.5f, color);
		break;
	case EXPLOSION_CAR:
	case EXPLOSION_CAR_QUICK:
	case EXPLOSION_QUICK_CAR:
	case EXPLOSION_BOAT:
		explosion.m_fRadius = 9.0f;
		explosion.m_fPower = 300.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 4250;
		explosion.m_fPropagationRate = 0.5f;
		explosion.m_fStartTime = CTimer::m_snTimeInMilliseconds;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		if (explosion.m_pVictimEntity != nullptr) {
			if (explosion.m_pVictimEntity->m_nType == ENTITY_TYPE_VEHICLE && ((CVehicle*)explosion.m_pVictimEntity)->m_nVehicleSubClass == VEHICLE_BOAT)
				explosion.m_bIsBoat = true;
		//	CEventList::RegisterEvent(EVENT_EXPLOSION, EVENT_ENTITY_VEHICLE, explosion.m_pVictimEntity, nullptr, 1000);
		}
		else {
			;
			//CEventList::RegisterEvent(EVENT_EXPLOSION, pos, 1000);
		}

		if (explosion.m_pVictimEntity != nullptr && !explosion.m_bIsBoat) {
			CVehicle* veh = (CVehicle*)explosion.m_pVictimEntity;
			CVector componentPos;

			if (veh->m_nVehicleSubClass == VEHICLE_BIKE) {
				veh->GetComponentWorldPosition(BIKE_FORKS_REAR, componentPos);
			}
			else if (veh->IsComponentPresent(CAR_BUMP_REAR) && veh->IsComponentPresent(CAR_WHEEL_LB)) { //mb it's another enum
				CVector tmpVec;
				veh->GetComponentWorldPosition(CAR_BUMP_REAR, componentPos);
				veh->GetComponentWorldPosition(CAR_WHEEL_LB, tmpVec);
				componentPos += tmpVec;
				componentPos /= 2.0f;
			}
			else if (veh->IsComponentPresent(CAR_BOOT)) {
				veh->GetComponentWorldPosition(CAR_BOOT, componentPos);
			}
			if (componentPos.x != 0.0f) {
				int rn = (GetRandomNumber() & 1) + 1;
				for (int i = 0; i < rn; i++)
					CParticle::AddJetExplosion(componentPos, (GetRandomNumber() & 7) / 7.0f + 1.5f, 0.5f);
			}
		}
		break;
	case EXPLOSION_HELI:
	case EXPLOSION_HELI2:
		if (type == EXPLOSION_HELI2) {
			explosion.m_fRadius = 12.0f;
			explosion.m_fPower = 500.0f;
		}
		else {
			explosion.m_fRadius = 6.0f;
			explosion.m_fPower = 300.0f;
		}
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 750;
		explosion.m_fPropagationRate = 0.5f;
		explosion.m_fStartTime = CTimer::m_snTimeInMilliseconds;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		for (int i = 0; i < 10; i++) {
			CVector randpos;

			randpos.x = GetRandomNumber();
			randpos.y = GetRandomNumber();
			randpos.z = GetRandomNumber();
			randpos -= CVector(128, 128, 128);
			randpos /= 20.0f;
			randpos += pos;

			CParticle::AddParticle(PARTICLE_EXPLOSION_MFAST, randpos, CVector(0.0f, 0.0f, 0.0f), nullptr, 2.5f, color);

			randpos.x = GetRandomNumber();
			randpos.y = GetRandomNumber();
			randpos.z = GetRandomNumber();
			randpos -= CVector(128, 128, 128);
			randpos /= 20.0f;
			randpos += pos;

			CParticle::AddParticle(PARTICLE_EXPLOSION_LFAST, randpos, CVector(0.0f, 0.0f, 0.0f), nullptr, 5.0f, color);

			randpos.x = GetRandomNumber();
			randpos.y = GetRandomNumber();
			randpos.z = GetRandomNumber();
			randpos -= CVector(128, 128, 128);
			randpos /= 20.0f;
			randpos += pos;
			RwRGBA colors[8];

			colors[0] = RwRGBA(0, 0, 0, 255);
			colors[1] = RwRGBA(224, 224, 224, 255);
			colors[2] = RwRGBA(0, 0, 0, 255);
			colors[3] = RwRGBA(0, 0, 0, 255);
			colors[4] = RwRGBA(66, 162, 252, 255);
			colors[5] = RwRGBA(0, 0, 0, 255);
			colors[6] = RwRGBA(0, 0, 0, 255);
			colors[7] = RwRGBA(0, 0, 0, 255);
					CVector dir;
					static int nFrameGen;
					for (int j = 0; j < 40; j++) {
						dir.x = CGeneral::GetRandomNumberInRange(-2.0f, 2.0f);
						dir.y = CGeneral::GetRandomNumberInRange(-2.0f, 2.0f);
						dir.z = CGeneral::GetRandomNumberInRange(0.0f, 2.0f);
						int rotSpeed = CGeneral::GetRandomNumberInRange(10, 30);
						if (GetRandomNumber() & 1)
							rotSpeed = -rotSpeed;
						int f = ++nFrameGen & 3;
						CParticle::AddParticle(PARTICLE_HELI_DEBRIS, pos, dir,
							NULL, CGeneral::GetRandomNumberInRange(0.1f, 1.0f),
							colors[nFrameGen & 7], rotSpeed, 0, f, 0);
					}
			CParticle::AddJetExplosion(randpos, 1.4f, 3.0f);
		}
		//CEventList::RegisterEvent(EVENT_EXPLOSION, pos, 1000);
		break;
	case EXPLOSION_MINE:
		explosion.m_fRadius = 10.0f;
		explosion.m_fPower = 150.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 750;
		explosion.m_fPropagationRate = 0.5f;
		posGround = pos;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		//posGround.z = 
		CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z + 4.0f, nullptr, nullptr); // BUG? result is unused
		//CEventList::RegisterEvent(EVENT_EXPLOSION, posGround, 250);
		break;
	case EXPLOSION_BARREL:
		explosion.m_fRadius = 7.0f;
		explosion.m_fPower = 150.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 750;
		explosion.m_fPropagationRate = 0.5f;
		for (int i = 0; i < 6; i++) {
			CVector randpos;
			randpos.x = GetRandomNumber();
			randpos.y = GetRandomNumber();
			randpos.z = GetRandomNumber();
			randpos -= CVector(128, 128, 128);
			randpos.x /= 50.0f;
			randpos.y /= 50.0f;
			randpos.z /= 25.0f;
			randpos += pos;
			CParticle::AddParticle(PARTICLE_EXPLOSION_MEDIUM, randpos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, colorMedium);
		}
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		posGround = pos;
		//posGround.z = 
		CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z + 4.0f, nullptr, nullptr); // BUG? result is unused
		//CEventList::RegisterEvent(EVENT_EXPLOSION, posGround, 250);
		break;
	case EXPLOSION_TANK_GRENADE:
		explosion.m_fRadius = 10.0f;
		explosion.m_fPower = 150.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 750;
		explosion.m_fPropagationRate = 0.5f;
		posGround = pos;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		//posGround.z = 
		CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z + 4.0f, nullptr, nullptr); // BUG? result is unused
		//CEventList::RegisterEvent(EVENT_EXPLOSION, posGround, 250);
		break;
	case EXPLOSION_AIRCRAFT:
		if (!bInvisible) {
			explosion.m_fRadius = 35.0f;
		}
		//explosion.m_nParticlesExpireTime = static_cast<float>(CTimer::m_snTimeInMilliseconds + lifetime + 3000);
		CParticle::AddJetExplosion(pos, 2.5f, 8.3f);
		static int nFrameGen;
		RwRGBA colors[8];

		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		colors[0] = RwRGBA(0, 0, 0, 255);
		colors[1] = RwRGBA(224, 230, 238, 255);
		colors[2] = RwRGBA(224, 230, 238, 255);
		colors[3] = RwRGBA(0, 0, 0, 255);
		colors[4] = RwRGBA(224, 230, 238, 255);
		colors[5] = RwRGBA(0, 0, 0, 255);
		colors[6] = RwRGBA(0, 0, 0, 255);
		colors[7] = RwRGBA(224, 230, 238, 255);
				for (int i = 0; i < 40; i++) {
					dir.x = CGeneral::GetRandomNumberInRange(-2.0f, 2.0f);
					dir.y = CGeneral::GetRandomNumberInRange(-2.0f, 2.0f);
					dir.z = CGeneral::GetRandomNumberInRange(0.0f, 2.0f);
					int rotSpeed = CGeneral::GetRandomNumberInRange(10, 30);
					if (GetRandomNumber() & 1)
						rotSpeed = -rotSpeed;
					int f = ++nFrameGen & 3;
					CParticle::AddParticle(PARTICLE_HELI_DEBRIS, pos, dir,
						NULL, CGeneral::GetRandomNumberInRange(0.1f, 1.0f),
						colors[nFrameGen & 7], rotSpeed, 0, f, 0);
				}
		for (int i = 0; i < 20; i++) {
			
			CParticle::AddParticle(PARTICLE_ENGINE_SMOKE2, pos, CVector(0.0f, 0.0f, 0.35f), nullptr, 10.0f, colUpdate, 0, 0, 35000);
			//CParticle::AddParticle(PARTICLE_EXPLOSION_MFAST, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 2.5f, color);

		//	CParticle::AddParticle(PARTICLE_EXPLOSION_LFAST, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 5.0f, color);

		//	CParticle::AddJetExplosion(pos, 1.4f, 4.0f);
		}

		break;
	case EXPLOSION_RC_VEHICLE:
		if (!bInvisible) {
			explosion.m_fRadius = 3.0f;
		}
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		//explosion.m_nParticlesExpireTime = (float)(CTimer::m_snTimeInMilliseconds + lifetime + 750);
		explosion.m_fPropagationRate = 0.5f;
		CParticle::AddParticle(PARTICLE_EXPLOSION_MEDIUM, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, color);
		break;
	
	case EXPLOSION_SMALL:
		if (!bInvisible) {
			explosion.m_fRadius = 3.0f;
		}
		//explosion.m_nParticlesExpireTime = (float)(CTimer::m_snTimeInMilliseconds + lifetime + 750);
		explosion.m_fPropagationRate = 0.5f;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		for (int i = 0; i < 3; i++) {
			CParticle::AddParticle(PARTICLE_EXPLOSION_MEDIUM, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, colorMedium);
		}
		break;
	case EXPLOSION_HELI_BOMB:
		explosion.m_fRadius = 8.0f;
		explosion.m_fPower = 50.0f;
		explosion.m_fStopTime = lifetime + CTimer::m_snTimeInMilliseconds + 750;
		explosion.m_fPropagationRate = 0.5f;
		posGround = pos;
		if (explosion.m_bMakeSound) {
			PlayExplosionSound(&m_ExplosionAudioEntity2, AE_EXPLOSION, &explosion.m_vecPosition, 0.0f);
		}
		//posGround.z = 
		CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z + 4.0f, nullptr, nullptr); // BUG? result is unused
		//CEventList::RegisterEvent(EVENT_EXPLOSION, posGround, 250);
		break;
	default:
		log("Undefined explosion type, AddExplosion, CExplosionVC.cpp");
		debug("Undefined explosion type, AddExplosion, CExplosionVC.cpp");
		break;
	}
	log("Explosion with type %s was created at pos X: %4.0f Y: %4.0f Z: %4.0f",
		ExplosionTypeToString(explosion.m_ExplosionType),
		explosion.m_vecPosition.x, explosion.m_vecPosition.y, explosion.m_vecPosition.z);
	debug("Explosion with type %s was created at pos X: %4.0f Y: %4.0f Z: %4.0f",
		ExplosionTypeToString(explosion.m_ExplosionType),
		explosion.m_vecPosition.x, explosion.m_vecPosition.y, explosion.m_vecPosition.z);

	if (bDontExplode) {
		explosion.m_nIteration = 0;
		return false;
	}

	if (!bDontExplode) {
		switch (type) {
		case EXPLOSION_MOLOTOV:
		case EXPLOSION_ROCKET:
		case EXPLOSION_WEAK_ROCKET:
		case EXPLOSION_OBJECT: {
			const auto numFires = (type == EXPLOSION_MOLOTOV) ? (GetRandomNumber() - 2) % 4 : (GetRandomNumber() + 1) % 4;

			if (numFires) {
				for (auto i = 0; i < numFires; i++) {
					CVector firePos = explosion.m_vecPosition + CVector{ CGeneral::GetRandomNumberInRange(-4.0f, 4.0f), CGeneral::GetRandomNumberInRange(-4.0f, 4.0f), 0.0f };
					bool bHitGround{};
					firePos.z = CWorld::FindGroundZFor3DCoord( firePos.x, firePos.y, firePos.z + 3.0f, &bHitGround, nullptr); // 0x73735C
					if (bHitGround && std::fabs(firePos.z - explosion.m_vecPosition.z) < 10.0f) {
						gFireManager.StartFire(firePos, 0.8f, 0, explosion.m_pCreatorEntity, (uint32_t)(CGeneral::GetRandomNumberInRange(5'600.0f, 12'600.0f) * 0.4f), 3, 1);
					}
				}
				if (creator && creator->m_nType == ENTITY_TYPE_PED && ((CPed*)creator)->IsPlayer()) {
					CStats::IncrementStat(eStats::STAT_FIRES_STARTED, 1.0f);
				}
			}
			break;
		}
		}
	}

	if (explosion.m_fPower != 0.0f && explosion.m_nParticlesExpireTime == 0)
		CWorld::TriggerExplosion(pos, explosion.m_fRadius, explosion.m_fPower, victim, creator, DoesNeedToVehProcessBombTimer(explosion.m_ExplosionType), 1.0f);

	if (type == EXPLOSION_MOLOTOV) {
		TheCamera.CamShake(0.2f, pos.x, pos.y, pos.z);
	}
	else {
		TheCamera.CamShake(0.6f, pos.x, pos.y, pos.z);
		CPad::GetPad(0)->StartShake_Distance(300, 128, pos.x, pos.y, pos.z);
	}
	return true;
}

void
CExplosionVC::Update()
{
	RwRGBA color = colUpdate;
	for (int i = 0; i < ARRAY_SIZE(gaExplosion); i++) {
		CExplosionVC& explosion = gaExplosion[i];
		if (explosion.m_nIteration == 0) continue;

		if (explosion.m_nParticlesExpireTime != 0) {
			if (CTimer::m_snTimeInMilliseconds > explosion.m_nParticlesExpireTime) {
				explosion.m_nParticlesExpireTime = 0;
				if (explosion.m_fPower != 0.0f)
					CWorld::TriggerExplosion(explosion.m_vecPosition, explosion.m_fRadius, explosion.m_fPower, explosion.m_pCreatorEntity, nullptr, DoesNeedToVehProcessBombTimer(explosion.m_ExplosionType), 1.0f );
			}
		}
		else {
			explosion.m_fRadius += explosion.m_fPropagationRate * CTimer::ms_fTimeStep;
			int32_t someTime = explosion.m_fStopTime - CTimer::m_snTimeInMilliseconds;
			switch (explosion.m_ExplosionType)
			{
			case EXPLOSION_GRENADE:
			case EXPLOSION_ROCKET:
			case EXPLOSION_HELI:
			case EXPLOSION_HELI2:
			case EXPLOSION_WEAK_ROCKET:
			case EXPLOSION_OBJECT:
			case EXPLOSION_MINE:
			case EXPLOSION_BARREL:
				if (explosion.m_ExplosionType == EXPLOSION_AIRCRAFT && CGeneral::GetRandomNumberInRange(0, 100) < 5) {
					if (explosion.m_pVictimEntity) {
						CExplosionVC::AddExplosion(explosion.m_pVictimEntity, explosion.m_pCreatorEntity, EXPLOSION_ROCKET, explosion.m_pVictimEntity->GetPosition(), 0, true, -1.0f, false);
					}
				}
				if (CTimer::m_FrameCounter & 1) {
					CPointLights::AddLight(0, explosion.m_vecPosition, CVector(0.0f, 0.0f, 0.0f), 20.0f, 1.0f, 1.0f, 0.5f, 0, true, nullptr);
					CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(&explosion) + 50 + 6 + 2, nullptr,
						255, 255, 200, 255,
						explosion.m_vecPosition,8.0f, 120.0f,
						gpCoronaTexture[0], FLARETYPE_NONE, true, false, 1, 0.0f, false, 1.5f, false, 255.0f, false, true);
					CCoronas::RegisterCorona((uintptr_t)&explosion, nullptr, 255, 255, 200, 255, explosion.m_vecPosition, 8.0f, 120.0f, gpCoronaTexture[0], FLARETYPE_NONE, true, false, 0, 0.0f, false, 1.5f, 0, 255.0f, false, true);
				}
				else
					CCoronas::RegisterCorona((uintptr_t)&explosion, nullptr, 128, 128, 100, 255, explosion.m_vecPosition, 8.0f, 120.0f, gpCoronaTexture[0], FLARETYPE_NONE, true, false, 0, 0.0f, false, 1.5f, 0, 255.0f, false, true);
				CCoronas::RegisterCorona((uintptr_t)&explosion + 1, nullptr, 30, 30, 25, 255, explosion.m_vecPosition, explosion.m_fRadius, 120.0f, coronaringa, FLARETYPE_NONE, true, false, 0, 0.0f, false, 1.5f, 0, 255.0f, false, true);
				break;
			case EXPLOSION_MOLOTOV:
				CWorld::SetPedsOnFire(explosion.m_vecPosition.x, explosion.m_vecPosition.y, explosion.m_vecPosition.z, 6.0f, explosion.m_pCreatorEntity);
				CWorld::SetWorldOnFire(explosion.m_vecPosition.x, explosion.m_vecPosition.y, explosion.m_vecPosition.z, 6.0f, explosion.m_pCreatorEntity);
				CWorld::SetCarsOnFire(explosion.m_vecPosition.x, explosion.m_vecPosition.y, explosion.m_vecPosition.z, 6.0f, explosion.m_pCreatorEntity);
				if (explosion.m_nIteration < 10) {
					if (explosion.m_nIteration == 1) {
						CVector point1 = explosion.m_vecPosition;
						point1.z += 5.0f;
						CColPoint colPoint;
						CEntity* pEntity;
						if (CWorld::ProcessVerticalLine(point1, -1000.0f, colPoint, pEntity, true, false, false, false, true, false, nullptr))
							explosion.m_fZshift = colPoint.m_vecPoint.z;
						else
							explosion.m_fZshift = explosion.m_vecPosition.z;
					}
					float ff = ((float)explosion.m_nIteration * 0.55f);
					for (int i = 0; i < 5 * ff; i++) {
						float angle = GetRandomNumber() / 256.0f * 6.28f;

						CVector pos = explosion.m_vecPosition;
						pos.x += ff * sinf(angle);
						pos.y += ff * cosf(angle);
						pos.z = explosion.m_fZshift + 0.5f;
						CParticle::AddParticle(PARTICLE_EXPLOSION_MEDIUM, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, color, CGeneral::GetRandomNumberInRange(-3.0f, 3.0f), CGeneral::GetRandomNumberInRange(-180.0f, 180.0f));
					}
				}
				break;
			case EXPLOSION_CAR:
			case EXPLOSION_CAR_QUICK:
			case EXPLOSION_QUICK_CAR:
			case EXPLOSION_BOAT:
			case EXPLOSION_AIRCRAFT:
				if (someTime >= 3500) {
					if (explosion.m_pVictimEntity != nullptr) {
						if ((GetRandomNumber() & 0xF) == 0 && !explosion.m_bIsBoat) {
							CVehicle* veh = (CVehicle*)explosion.m_pVictimEntity;
							uint8_t component = eCarNodes::CAR_WING_LF;

							// miami leftover
							if (veh->m_nVehicleSubClass == VEHICLE_BIKE)
								component = BIKE_FORKS_REAR;

							if (veh->IsComponentPresent(component)) {
								CVector componentPos;
								veh->GetComponentWorldPosition(component, componentPos);
								CParticle::AddJetExplosion(componentPos, 0.5f, 0.0f);
							}
						}
						if (CTimer::m_snTimeInMilliseconds > explosion.m_fStartTime) {
							explosion.m_fStartTime = CTimer::m_snTimeInMilliseconds + 125 + (GetRandomNumber() & 0x7F);
							CVector pos = explosion.m_pVictimEntity->GetPosition();
							for (int i = 0; i < (GetRandomNumber() & 1) + 1; i++)
								CParticle::AddParticle(PARTICLE_EXPLOSION_MEDIUM, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 3.5f, color);
						}
					}
					if (CTimer::m_FrameCounter & 1) {
						CPointLights::AddLight(0, explosion.m_vecPosition, CVector(0.0f, 0.0f, 0.0f), 15.0f, 1.0f, 0.0f, 0.0f, 0, true, nullptr);
						
						CCoronas::RegisterCorona((uintptr_t)&explosion, nullptr, 200, 100, 0, 255, explosion.m_vecPosition, 6.0f, 80.0f, gpCoronaTexture[0], FLARETYPE_NONE, true, false, 0, 0.0f, false, 1.5f, 0, 255.0f, false, true);
					}
					else
						CCoronas::RegisterCorona((uintptr_t)&explosion, nullptr, 128, 0, 0, 255, explosion.m_vecPosition, 8.0f, 80.0f, gpCoronaTexture[0], FLARETYPE_NONE, true, false, 0, 0.0f, false, 1.5f, 0, 255.0f, false, true);

					CCoronas::RegisterCorona((uintptr_t)&explosion + 1, nullptr, 30, 15, 0, 255, explosion.m_vecPosition, explosion.m_fRadius, 80.0f, coronaringa, FLARETYPE_NONE, true, false, 0, 0.0f, false, 1.5f, 0, 255.0f, false, true);
				}
				else if (explosion.m_nIteration & 1) {
					if (explosion.m_pVictimEntity != nullptr)
						CParticle::AddParticle(PARTICLE_ENGINE_SMOKE2, explosion.m_pVictimEntity->GetPosition(), CVector(0.0f, 0.0f, 0.0f), nullptr, CGeneral::GetRandomNumberInRange(0.5f, 0.8f), color);
					CVector pos = explosion.m_vecPosition;
					pos.z += 1.0f;
					CParticle::AddParticle(PARTICLE_ENGINE_SMOKE2, pos, CVector(0.0f, 0.0f, 0.11f), nullptr, CGeneral::GetRandomNumberInRange(0.5f, 2.0f), color);
				}
				break;

			case EXPLOSION_TANK_GRENADE:
			case EXPLOSION_TANK_FIRE:
			case EXPLOSION_HELI_BOMB:
				if (explosion.m_nIteration < 5) {
					float ff = ((float)explosion.m_nIteration * 0.65f);
					for (int i = 0; i < 10 * ff; i++) {
						uint8_t x = GetRandomNumber(), y = GetRandomNumber(), z = GetRandomNumber();
						CVector pos(x - 128, y - 128, (z % 128) + 1);

						pos.Normalise();
						pos *= (explosion.m_nIteration + 1) * ff / 5.0f;
						pos += explosion.m_vecPosition;
						pos.z += 0.5f;
						CParticle::AddParticle(PARTICLE_EXPLOSION_LARGE, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, color, CGeneral::GetRandomNumberInRange(-3.0f, 3.0f), CGeneral::GetRandomNumberInRange(-180.0f, 180.0f));
					}
				}
				break;
			default:
				break;
			}
			if (someTime > 0)
				explosion.m_nIteration++;
			else
				explosion.m_nIteration = 0;
		}
	}
}

bool
CExplosionVC::TestForExplosionInArea(eExplosionTypeVC type, float x1, float x2, float y1, float y2, float z1, float z2)
{
	for (int i = 0; i < ARRAY_SIZE(gaExplosion); i++) {
		if (gaExplosion[i].m_nIteration != 0) {
			if (type == gaExplosion[i].m_ExplosionType) {
				if (gaExplosion[i].m_vecPosition.x >= x1 && gaExplosion[i].m_vecPosition.x <= x2) {
					if (gaExplosion[i].m_vecPosition.y >= y1 && gaExplosion[i].m_vecPosition.y <= y2) {
						if (gaExplosion[i].m_vecPosition.z >= z1 && gaExplosion[i].m_vecPosition.z <= z2)
							return true;
					}
				}
			}
		}
	}
	return false;
}

void
CExplosionVC::RemoveAllExplosionsInArea(CVector pos, float radius)
{
	for (int i = 0; i < ARRAY_SIZE(gaExplosion); i++) {
		if (gaExplosion[i].m_nIteration != 0) {
			if (static_cast<Vec>((pos - gaExplosion[i].m_vecPosition)).MagnitudeSqr() < SQR(radius))
				gaExplosion[i].m_nIteration = 0;
		}
	}
}

class Explosion {
public:
	Explosion() {
		Events::initGameEvent += []() {
			if (ExplosionsParticles) {
				patch::RedirectJump(0x736A40, CExplosionVC::Initialise);
				patch::RedirectJump(0x737620, CExplosionVC::Update);
				patch::RedirectJump(0x736A50, CExplosionVC::AddExplosion);
				patch::RedirectJump(0x7369E0, CExplosionVC::RemoveAllExplosionsInArea);
				patch::RedirectJump(0x736950, CExplosionVC::TestForExplosionInArea);
				patch::RedirectJump(0x736940, CExplosionVC::GetExplosionPosition);
				patch::RedirectJump(0x736930, CExplosionVC::GetExplosionType);
				patch::RedirectJump(0x736920, CExplosionVC::DoesExplosionMakeSound);
				patch::RedirectJump(0x736910, CExplosionVC::ResetExplosionActiveCounter);
				patch::RedirectJump(0x736840, CExplosionVC::ClearAllExplosions);
			}
		};

		/*Events::gameProcessEvent += []() {
			CExplosionVC::Update();
		};

		Events::shutdownRwEvent += []() {
			CExplosionVC::Shutdown();
		};*/
	}
} _Explosion;