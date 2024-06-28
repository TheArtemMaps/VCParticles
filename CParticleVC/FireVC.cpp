#include "plugin.h"

#include "CEntity.h"
#include "CPointLights.h"
#include "CParticleVC.h"
#include "CTimer.h"
#include "CVehicle.h"
#include "CShadows.h"
#include "CAutomobile.h"
#include "CWorld.h"
#include "CGeneral.h"
#include "CDamageManager.h"
#include "CPed.h"
#include "FireVC.h"
#include "CGameLogic.h"
#include "CCarAI.h"
#include <game_sa/CModelInfo.h>
#include "MemoryMgr.h"
using namespace std;

CFireManagerVC gFireManager;

CFireVC::CFireVC()
{
	m_bIsOngoing = false;
	m_bIsScriptFire = false;
	m_bPropagationFlag = true;
	m_bAudioSet = true;
	m_vecPos = CVector(0.0f, 0.0f, 0.0f);
	m_nExtinguishTime = 0;
	m_nStartTime = 0;
	m_pEntity = NULL;
	m_pSource = NULL;
	m_fStrength = 0.8f;
	m_fWaterExtinguishCountdown = 1.0f;
  	m_bExtinguishedWithWater = false;
}

CFireVC::~CFireVC() {}

void
CFireVC::ProcessFire(void)
{
	float fDamagePlayer;
	float fDamagePeds;
	float fDamageVehicle;
	int16_t nRandNumber;
	float fGreen;
	float fRed;
	CVector lightpos;
	CVector firePos;
	CPed *ped = (CPed *)m_pEntity;
	CVehicle *veh = (CVehicle*)m_pEntity;

	m_fWaterExtinguishCountdown = min(1.0f, 0.002f * CTimer::ms_fTimeStep + m_fWaterExtinguishCountdown);

	if (m_pEntity) {
		m_vecPos = m_pEntity->GetPosition();

		if (((CPed *)m_pEntity)->m_nType == ENTITY_TYPE_PED) {
			if (((CFireVC*)ped->m_pFire) != this) {
				Extinguish();
				return;
			}
#if defined GTAVC_JP_PATCH && !defined FIX_BUGS
			if (m_pEntity == CGameLogic::pShortCutTaxi && CGameLogic::ShortCutState == CGameLogic::SHORTCUT_TRANSITION) {
				Extinguish();
				return;
			}
#endif
			if (ped->m_nMoveState != PEDMOVE_RUN)
				m_vecPos.z -= 1.0f;
			if (ped->m_nPedFlags.bInVehicle && ped->m_pVehicle) {
				if (ped->m_pVehicle->m_nVehicleSubClass == VEHICLE_AUTOMOBILE)
					ped->m_pVehicle->m_fHealth = 75.0f;
			} else if (m_pEntity == (CPed *)FindPlayerPed()) {
				fDamagePlayer = 1.2f * CTimer::ms_fTimeStep;

				//((CPlayerPed *)m_pEntity)->InflictDamage(
					//(CPlayerPed *)m_pSource, WEAPONTYPE_FLAMETHROWER,
						//fDamagePlayer, PEDPIECE_TORSO, 0);
			} else {
				fDamagePeds = 1.2f * CTimer::ms_fTimeStep;

				//if (((CPlayerPed *)m_pEntity)->InflictDamage(
				//	(CPlayerPed *)m_pSource, WEAPONTYPE_FLAMETHROWER,
				//	fDamagePeds, PEDPIECE_TORSO, 0)) {
					m_pEntity->m_bRenderDamaged = true;
				//}
			}
		} else if (m_pEntity->m_nType == ENTITY_TYPE_VEHICLE) {
			if (((CFireVC*)veh->m_pFire) != this) {
				Extinguish();
				return;
			}
//#ifdef FIX_BUGS
	//		if (m_pEntity == CGameLogic::taxi && CGameLogic::ShortCutState == CGameLogic::shortc) {
	//			Extinguish();
	//			return;
	//		}
//#endif
			if (!m_bIsScriptFire) {
				fDamageVehicle = 1.2f * CTimer::ms_fTimeStep;
				veh->InflictDamage((CVehicle *)m_pSource, WEAPON_FTHROWER, fDamageVehicle, m_pEntity->GetPosition());
			}
		}
	}
	if (!FindPlayerVehicle(-1, false) &&
#ifdef FIX_BUGS
		FindPlayerPed() &&
#endif
		!FindPlayerPed()->m_pFire && !(FindPlayerPed()->m_nPhysicalFlags.bFireProof)
		&& (static_cast<Vec>((FindPlayerPed()->GetPosition() - m_vecPos)).MagnitudeSqr() < 2.0f)) {
		FindPlayerPed()->DoStuffToGoOnFire();
		gFireManager.StartFireEnt(FindPlayerPed(), m_pSource, 0.8f, 1, 0, 0);
	}
	if (CTimer::m_snTimeInMilliseconds > m_nNextTimeToAddFlames) {
		m_nNextTimeToAddFlames = CTimer::m_snTimeInMilliseconds + (m_fWaterExtinguishCountdown < 0.3f ? 400 : (m_fWaterExtinguishCountdown < 0.7f ? 200 : 80));
		firePos = m_vecPos;

		if (veh && veh->m_nVehicleSubClass == VEHICLE_AUTOMOBILE) {
			CVehicleModelInfo *mi = ((CVehicleModelInfo*)CModelInfo::GetModelInfo(veh->m_nModelIndex));
			CVector ModelInfo = mi->m_pVehicleStruct->m_avDummyPos[0];
			ModelInfo = *m_pEntity->GetMatrix() * ModelInfo;

			firePos.x = ModelInfo.x;
			firePos.y = ModelInfo.y;
			firePos.z = ModelInfo.z + 0.15f;
		}

		CParticle::AddParticle(PARTICLE_CARFLAME, firePos,
			CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.0125f, 0.1f) * m_fStrength),
				0, m_fStrength, 0, 0, 0, 0);

		GetRandomNumber(); GetRandomNumber(); GetRandomNumber(); /* unsure why these three rands are called */

		CParticle::AddParticle(PARTICLE_CARFLAME_SMOKE, firePos,
			CVector(0.0f, 0.0f, 0.0f), 0, 0.0f, 0, 0, 0, 0);
	}
	if (CTimer::m_snTimeInMilliseconds < m_nExtinguishTime || m_bIsScriptFire) {
		if (CTimer::m_snTimeInMilliseconds > m_nStartTime)
			m_nStartTime = CTimer::m_snTimeInMilliseconds + 400;

		nRandNumber = GetRandomNumber() & 127;
		lightpos.x = m_vecPos.x;
		lightpos.y = m_vecPos.y;
		lightpos.z = m_vecPos.z + 5.0f;

		if (!m_pEntity) {
			CShadows::StoreStaticShadow((uintptr_t)this, SHADOW_ADDITIVE, gpShadowExplosionTex, &lightpos, 7.0f, 0.0f, 0.0f, -7.0f, 0, nRandNumber / 2,
			                            nRandNumber / 2, 0, 10.0f, 1.0f, 40.0f, 0, 0.0f);
		}
		fGreen = nRandNumber / 128.f;
		fRed = nRandNumber / 128.f;

		CPointLights::AddLight(0, m_vecPos, CVector(0.0f, 0.0f, 0.0f), 12.0f, fRed, fGreen, 0.0f, 0, 0, m_pEntity);
	} else {
		Extinguish();
	}
}

void
CFireVC::ReportThisFire(void)
{
	gFireManager.m_nTotalFires++;
	//CEventList::RegisterEvent(EVENT_FIRE, m_vecPos, 1000);
}

void
CFireVC::Extinguish(void)
{
	if (m_bIsOngoing) {
		if (!m_bIsScriptFire)
			gFireManager.m_nTotalFires--;

		m_nExtinguishTime = 0;
		m_bIsOngoing = false;
		m_bExtinguishedWithWater = false;

		if (m_pEntity) {
			if (m_pEntity->m_nType == ENTITY_TYPE_PED) {
				CPed *ped = (CPed*)m_pEntity;
				if (ped->CanSetPedState()) {
					if (ped->m_ePedState != PEDSTATE_DRIVING && ped->m_ePedState != PEDSTATE_FALL) {
						if (ped->IsPlayer()) {
							ped->SetIdle();
						} else {
							//ped->lastpeds = PED_NONE;
							//ped->wander(0);
							//ped->wait(WAITSTATE_FINISH_FLEE, 0);
							;
						}
					}
				}
				ped->m_pFire = NULL;
			} else if (m_pEntity->m_nType == ENTITY_TYPE_VEHICLE) {
				((CVehicle *)m_pEntity)->m_pFire = NULL;
			}
			m_pEntity = NULL;
		}
	}
}

void
CFireManagerVC::StartFire(CVector pos, float size, uint8 propagation, CEntity* creator, uint32 nTimeToBurn, int8 nGenerations, uint8 unused_)
{
	CFireVC *fire = GetNextFreeFire();

	if (fire) {
		fire->m_bIsOngoing = true;
		fire->m_bIsScriptFire = false;
		fire->m_bPropagationFlag = propagation;
		fire->m_bAudioSet = true;
		fire->m_vecPos = pos;
		fire->m_nExtinguishTime = CTimer::m_snTimeInMilliseconds + 10000;
		fire->m_nStartTime = CTimer::m_snTimeInMilliseconds + 400;
		fire->m_pEntity = NULL;
		fire->m_pSource = NULL;
		fire->m_nNextTimeToAddFlames = 0;
		fire->ReportThisFire();
		fire->m_fStrength = size;
		fire->m_bExtinguishedWithWater = false;
	}
}

CFireVC *
CFireManagerVC::StartFireEnt(CEntity* target, CEntity* creator, float size, uint8 propagation, uint32 lifetime, int8 numGenerations)
{
	CPed *ped = (CPed *)target;
	CVehicle *veh = (CVehicle *)target;
	if (target->m_nType == ENTITY_TYPE_PED) {
		if (ped->m_pFire)
			return NULL;
		if (!ped->IsPedInControl())
			return NULL;
	} else if (target->m_nType == ENTITY_TYPE_VEHICLE) {
		if (veh->m_pFire)
			return NULL;
		if (veh->m_nVehicleSubClass == VEHICLE_AUTOMOBILE &&  ((CAutomobile *)veh)->m_damageManager.GetEngineStatus() >= 225)
			return NULL;
	}
	CFireVC *fire = GetNextFreeFire();

	if (fire) {
		if (target->m_nType == ENTITY_TYPE_PED) {
			//ped->m_pFire = fire;
			if (ped != FindPlayerPed()) {
				//if (fleeFrom) {
					//ped->SetFlee(fleeFrom, 10000);
				//	;
				//} else {
					CVector2D pos = target->GetPosition();
					//ped->SetFlee(pos, 10000);
					//ped->m_fleeFrom = NULL;
				//}
				//ped->fleet = CTimer::GetTimeInMilliseconds() + 10000;
				ped->m_bDrawLast = false;
				ped->SetMoveState(PEDMOVE_SPRINT);
				ped->SetMoveAnim();
				ped->SetPedState(PEDSTATE_ON_FIRE);
			}
		//	if (fleeFrom) {
				//if (ped->m_nPedType == PEDTYPE_COP) {
				//	CEventList::RegisterEvent(EVENT_COP_SET_ON_FIRE, EVENT_ENTITY_PED,
				//		entityOnFire, (CPed *)fleeFrom, 10000);
				//} else {
				//	CEventList::RegisterEvent(EVENT_PED_SET_ON_FIRE, EVENT_ENTITY_PED,
					//	entityOnFire, (CPed *)fleeFrom, 10000);
				//}
			//	;
			//}
		} else {
			if (target->m_nType == ENTITY_TYPE_VEHICLE) {
				//veh->fire = fire;
				if (CModelInfo::IsBikeModel(veh->m_nModelIndex) || CModelInfo::IsCarModel(veh->m_nModelIndex))
        				CCarAI::TellOccupantsToLeaveCar(veh);

				//if (fleeFrom) {
					//CEventList::RegisterEvent(EVENT_CAR_SET_ON_FIRE, EVENT_ENTITY_VEHICLE,
					//	entityOnFire, (CPed *)fleeFrom, 10000);
				//}
			}
		}

		fire->m_bIsOngoing = true;
		fire->m_bExtinguishedWithWater = false;
		fire->m_bIsScriptFire = false;
		fire->m_vecPos = target->GetPosition();

		if (target && target->m_nType == ENTITY_TYPE_PED && ped->IsPlayer()) {
			fire->m_nExtinguishTime = CTimer::m_snTimeInMilliseconds + 3333;
		} else if (target->m_nType == ENTITY_TYPE_VEHICLE) {
			fire->m_nExtinguishTime = CTimer::m_snTimeInMilliseconds + CGeneral::GetRandomNumberInRange(4000, 5000);
		} else {
			fire->m_nExtinguishTime = CTimer::m_snTimeInMilliseconds + CGeneral::GetRandomNumberInRange(10000, 11000);
		}
		fire->m_nStartTime = CTimer::m_snTimeInMilliseconds + 400;
		fire->m_pEntity = target;

		target->RegisterReference(&fire->m_pEntity);
		fire->m_pSource = creator;

		if (creator)
			creator->RegisterReference(&fire->m_pSource);
		fire->ReportThisFire();
		fire->m_nNextTimeToAddFlames = 0;
		fire->m_fStrength = size;
		fire->m_bPropagationFlag = propagation;
		fire->m_bAudioSet = true;
	}
	return fire;
}

void
CFireManagerVC::Update(void)
{
	for (int i = 0; i < NUM_FIRES; i++) {
		if (m_aFires[i].m_bIsOngoing)
			m_aFires[i].ProcessFire();
	}
}

CFireVC* CFireManagerVC::FindNearestFire(const CVector& point, bool bCheckIsBeingExtinguished, bool bCheckWasCreatedByScript)
{
	int fireId = -1;
	float minDistance = 999999;
	for (int j = 0; j < NUM_FIRES; j++) {
		if (!m_aFires[j].m_bIsOngoing)
			continue;
		if (m_aFires[j].m_bIsScriptFire)
			continue;
		float distance = (m_aFires[j].m_vecPos - point).Magnitude2D();
		if (distance < minDistance) {
			minDistance = distance;
			fireId = j;
		}
	}
	//*pDistance = minDistance;
	if (fireId != -1)
		return &m_aFires[fireId];

	return NULL;
}

CFireVC *
CFireManagerVC::FindFurthestFire_NeverMindFireMen(CVector coords, float minRange, float maxRange)
{
	int furthestFire = -1;
	float lastFireDist = 0.0f;
	float fireDist;

	for (int i = 0; i < NUM_FIRES; i++) {
		if (m_aFires[i].m_bIsOngoing && !m_aFires[i].m_bIsScriptFire) {
			fireDist = (m_aFires[i].m_vecPos - coords).Magnitude2D();
			if (fireDist > minRange && fireDist < maxRange && fireDist > lastFireDist) {
				lastFireDist = fireDist;
				furthestFire = i;
			}
		}
	}
	if (furthestFire == -1)
		return NULL;
	else
		return &m_aFires[furthestFire];
}

CFireVC *
CFireManagerVC::GetNextFreeFire(void)
{
	for (int i = 0; i < NUM_FIRES; i++) {
		if (!m_aFires[i].m_bIsOngoing && !m_aFires[i].m_bIsScriptFire)
			return &m_aFires[i];
	}
	return NULL;
}

uint32_t
CFireManagerVC::GetTotalActiveFires(void) const
{
	return m_nTotalFires;
}

void
CFireManagerVC::ExtinguishPoint(CVector point, float range)
{
	for (int i = 0; i < NUM_FIRES; i++) {
		if (m_aFires[i].m_bIsOngoing) {
			if (static_cast<Vec>((point - m_aFires[i].m_vecPos)).MagnitudeSqr() < sq(range))
				m_aFires[i].Extinguish();
		}
	}
}

bool
CFireManagerVC::ExtinguishPointWithWater(CVector point, float range, float fWaterStrength)
{
    int i;
    for (i = 0; i < NUM_FIRES;) {
        if (m_aFires[i].m_bIsOngoing && static_cast<Vec>((point - m_aFires[i].m_vecPos)).MagnitudeSqr() < sq(range)) {
                break;
        }
        if (++i >= NUM_FIRES)
          return false;
    }
    
	CFireVC *fireToExtinguish = &m_aFires[i];
	fireToExtinguish->m_fWaterExtinguishCountdown -= 0.012f * CTimer::ms_fTimeStep;
	CVector steamPos = fireToExtinguish->m_vecPos +
		CVector((GetRandomNumber() - 128) * 3.1f / 200.f,
			(GetRandomNumber() - 128) * 3.1f / 200.f,
			GetRandomNumber() / 200.f);

	CParticle::AddParticle(PARTICLE_STEAM_NY_SLOWMOTION, steamPos, CVector(0.f, 0.f, 0.2f), NULL, 0.5f);
	CParticle::AddParticle(PARTICLE_STEAM_NY_SLOWMOTION, steamPos, CVector(0.f, 0.f, 0.1f), NULL, 0.8f);
	fireToExtinguish->m_bExtinguishedWithWater = true;
	if (fireToExtinguish->m_fWaterExtinguishCountdown < 0.0f )
	  fireToExtinguish->Extinguish();

	return true;
}

int32_t
CFireManagerVC::StartScriptFire(const CVector& pos, CEntity* target, float strength, uint8 propagation, int8 nGenerations, int32 nStrength)
{
	CFireVC *fire;
	CPed *ped = (CPed *)target;
	CVehicle *veh = (CVehicle *)target;

	if (target) {
		if (target->m_nType == ENTITY_TYPE_PED) {
			if (ped->m_pFire)
				ped->m_pFire->Extinguish();
		} else if (target->m_nType == ENTITY_TYPE_VEHICLE) {
			if (veh->m_pFire)
				reinterpret_cast<CFireVC*>(veh->m_pFire)->Extinguish();
			if (veh->m_nVehicleSubClass == VEHICLE_AUTOMOBILE && ((CAutomobile *)veh)->m_damageManager.GetEngineStatus() >= 225) {
				((CAutomobile *)veh)->m_damageManager.SetEngineStatus(215);
			}
		}
	}

	fire = GetNextFreeFire();
	fire->m_bIsOngoing = true;
	fire->m_bIsScriptFire = true;
	fire->m_bPropagationFlag = propagation;
	fire->m_bAudioSet = true;
	fire->m_vecPos = pos;
	fire->m_nStartTime = CTimer::m_snTimeInMilliseconds + 400;
	fire->m_pEntity = target;
	fire->m_bExtinguishedWithWater = false;

	if (target)
		target->RegisterReference(&fire->m_pEntity);
	fire->m_pSource = NULL;
	fire->m_nNextTimeToAddFlames = 0;
	fire->m_fStrength = strength;
	fire->m_fWaterExtinguishCountdown = 1.0f;

	if (target) {
		if (target->m_nType == ENTITY_TYPE_PED) {
			//reinterpret_cast<CFireVC*>(ped->m_pFire) = fire;
			;
			if (target != FindPlayerPed()) {
				CVector2D pos = target->GetPosition();
				//ped->flee(pos, 10000);
				ped->SetMoveAnim();
				ped->SetPedState(PEDSTATE_ON_FIRE);
			}
		} else if (target->m_nType == ENTITY_TYPE_VEHICLE) {
			//reinterpret_cast<CFireVC*>(veh->m_pFire) = fire;
			;
		}
	}
	return fire - m_aFires;
}

bool
CFireManagerVC::IsScriptFireExtinguish(int16_t index)
{
	return !m_aFires[index].m_bIsOngoing;
}

void
CFireManagerVC::RemoveAllScriptFires(void)
{
	for (int i = 0; i < NUM_FIRES; i++) {
		if (m_aFires[i].m_bIsScriptFire) {
			RemoveScriptFire(i);
		}
	}
}

void
CFireManagerVC::RemoveScriptFire(int16_t index)
{
	m_aFires[index].Extinguish();
	m_aFires[index].m_bIsScriptFire = false;
}

void
CFireManagerVC::SetScriptFireAudio(int16_t index, bool state)
{
	m_aFires[index].m_bAudioSet = state;
}

class Fire {
public:
	Fire() {
		/*Events::initRwEvent += []() {
			//CExplosionVC::Initialise();
			Memory::InjectHook(0x5393F0, &CFireVC::Extinguish, PATCH_JUMP);
			Memory::InjectHook(0x53A570, &CFireVC::ProcessFire, PATCH_JUMP);
			Memory::InjectHook(0x53AF00, &CFireManagerVC::Update, PATCH_JUMP);
			Memory::InjectHook(0x53A270, &CFireManagerVC::StartScriptFire, PATCH_JUMP);
			Memory::InjectHook(0x539450, &CFireManagerVC::ExtinguishPoint, PATCH_JUMP);
			Memory::InjectHook(0x5394C0, &CFireManagerVC::ExtinguishPointWithWater, PATCH_JUMP);
			Memory::InjectHook(0x5396E0, &CFireManagerVC::IsScriptFireExtinguish, PATCH_JUMP);
			Memory::InjectHook(0x53A050, &CFireManagerVC::StartFireEnt, PATCH_JUMP);
			Memory::InjectHook(0x539F00, &CFireManagerVC::StartFire, PATCH_JUMP);
			Memory::InjectHook(0x538F40, &CFireManagerVC::FindNearestFire, PATCH_JUMP);
		};*/

		//Events::gameProcessEvent += []() {
		//	gFireManager.Update();
			//gFireManager.StartFire(FindPlayerCoors(-1), 1.0f, 1);
	//	};

		/*Events::shutdownRwEvent += []() {
			CExplosionVC::Shutdown();
		};*/
	}
} _Fire;
