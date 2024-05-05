#include "plugin.h"

#include "ParticleObject.h"
#include "CTimer.h"
#include "CGeneral.h"
#include "ParticleMgr.h"
#include "CParticleVC.h"
#include "CCamera.h"
#include "CGame.h"

#ifdef COMPATIBLE_SAVES
#define PARTICLE_OBJECT_SIZEOF 0x84
#else
#define PARTICLE_OBJECT_SIZEOF sizeof(CParticleObject)
#endif


CParticleObject gPObjectArray[MAX_PARTICLEOBJECTS];

CParticleObject* CParticleObject::pCloseListHead;
CParticleObject* CParticleObject::pFarListHead;
CParticleObject* CParticleObject::pUnusedListHead;

CParticleObject::CParticleObject() : CPlaceable(*this),
	m_nFrameCounter(0),
	m_nState(POBJECTSTATE_INITIALISED),
	m_pNext(NULL),
	m_pPrev(NULL),
	m_nRemoveTimer(0)

{
	;
}

CParticleObject::~CParticleObject()
{

}

void
CParticleObject::Initialise()
{
	pCloseListHead = NULL;
	pFarListHead = NULL;

	pUnusedListHead = &gPObjectArray[0];

	for (int32_t i = 0; i < MAX_PARTICLEOBJECTS; i++)
	{
		if (i == 0)
			gPObjectArray[i].m_pPrev = NULL;
		else
			gPObjectArray[i].m_pPrev = &gPObjectArray[i - 1];

		if (i == MAX_PARTICLEOBJECTS - 1)
			gPObjectArray[i].m_pNext = NULL;
		else
			gPObjectArray[i].m_pNext = &gPObjectArray[i + 1];

		gPObjectArray[i].m_nState = POBJECTSTATE_FREE;
	}
}

CParticleObject*
CParticleObject::AddObject(uint16_t type, CVector const& pos, uint8_t remove)
{
	RwRGBA color(0, 0, 0, 0);
	CVector target(0.0f, 0.0f, 0.0f);
	return AddObject(type, pos, target, 0.0f, 0, color, remove);
}

CParticleObject*
CParticleObject::AddObject(uint16_t type, CVector const& pos, float size, uint8_t remove)
{
	RwRGBA color(0, 0, 0, 0);
	CVector target(0.0f, 0.0f, 0.0f);
	return AddObject(type, pos, target, size, 0, color, remove);
}

CParticleObject*
CParticleObject::AddObject(uint16_t type, CVector const& pos, CVector const& target, float size, uint8_t remove)
{
	RwRGBA color(0, 0, 0, 0);
	return AddObject(type, pos, target, size, 0, color, remove);
}

CParticleObject*
CParticleObject::AddObject(uint16_t type, CVector const& pos, CVector const& target, float size, uint32_t lifeTime, RwRGBA const& color, uint8_t remove)
{
	CParticleObject* pobj = pUnusedListHead;

	if (pobj == NULL)
	{
		log("Error: No particle objects available!\n");
		debug("Error: No particle objects available!\n");
		return NULL;
	}

	MoveToList(&pUnusedListHead, &pCloseListHead, pobj);

	pobj->m_nState = POBJECTSTATE_UPDATE_CLOSE;
	pobj->m_Type = (eParticleObjectType)type;

	pobj->SetPosn(pos);
	pobj->m_vecTarget = target;

	pobj->m_nNumEffectCycles = 1;
	pobj->m_nSkipFrames = 1;
	pobj->m_nCreationChance = 0;
	pobj->m_nFrameCounter = 0;

	pobj->m_bRemove = remove;

	pobj->m_pParticle = NULL;

	if (lifeTime != 0)
		pobj->m_nRemoveTimer = CTimer::m_snTimeInMilliseconds + lifeTime;
	else
		pobj->m_nRemoveTimer = 0;

	if (color.alpha != 0)
		pobj->m_Color = color;
	else
		pobj->m_Color.alpha = 0;

	pobj->m_fSize = size;
	pobj->m_fRandVal = 0.0f;
	if (type <= POBJECT_CATALINAS_SHOTGUNFLASH)
	{
		switch (type)
		{
		case POBJECT_PAVEMENT_STEAM:
		{
			pobj->m_ParticleType = PARTICLE_STEAM_NY;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 3;
			pobj->m_nCreationChance = 8;
			break;
		}

		case POBJECT_PAVEMENT_STEAM_SLOWMOTION:
		{
			pobj->m_ParticleType = PARTICLE_STEAM_NY_SLOWMOTION;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 8;
			break;
		}

		case POBJECT_WALL_STEAM:
		{
			pobj->m_ParticleType = PARTICLE_STEAM_NY;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 3;
			pobj->m_nCreationChance = 8;
			break;
		}

		case POBJECT_WALL_STEAM_SLOWMOTION:
		{
			pobj->m_ParticleType = PARTICLE_STEAM_NY_SLOWMOTION;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 8;
			break;
		}

		case POBJECT_DARK_SMOKE:
		{
			pobj->m_ParticleType = PARTICLE_STEAM_NY;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 3;
			pobj->m_nCreationChance = 8;
			pobj->m_Color = RwRGBA(16, 16, 16, 255);
			break;
		}

		case POBJECT_WATER_FOUNTAIN_VERT:
		{
			pobj->m_ParticleType = PARTICLE_WATER_HYDRANT;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			pobj->m_vecTarget = CVector(0.0f, 0.0f, 0.1f);
			break;
		}

		case POBJECT_WATER_FOUNTAIN_HORIZ:
		{
			pobj->m_ParticleType = PARTICLE_WATER_HYDRANT;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			break;
		}

		case POBJECT_FIRE_HYDRANT:
		{
			pobj->m_ParticleType = PARTICLE_WATER_HYDRANT;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			pobj->m_vecTarget = CVector(0.0f, 0.0f, 0.3f);
			pobj->m_nRemoveTimer = CTimer::m_snTimeInMilliseconds + 5000;
			//CAudioHydrant::Add(pobj);
			break;
		}

		case POBJECT_CAR_WATER_SPLASH:
		case POBJECT_PED_WATER_SPLASH:
		{
			pobj->m_ParticleType = PARTICLE_CAR_SPLASH;
			pobj->m_nNumEffectCycles = 0;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			if (WaterDrops) {
				CVector dropDir(CGeneral::GetRandomNumberInRange(-0.15f, 0.15f), CGeneral::GetRandomNumberInRange(0.1f, 0.35f), 0.f);
				CVector dropPos(CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_X(50.0f), SCREEN_STRETCH_FROM_RIGHT(50.0f)),
					CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_Y(50.0f), SCREEN_STRETCH_FROM_BOTTOM(50.0f)), 1.f);
				CParticle::AddParticle(PARTICLE_WATERDROP, dropPos, dropDir, NULL, CGeneral::GetRandomNumberInRange(0.1f, 0.15f),
					RwRGBA(0, 0, 0, 0), 0, 0, GetRandomNumber() & 1, 0);
			}
			break;
		}

		case POBJECT_SPLASHES_AROUND:
		{
			pobj->m_ParticleType = PARTICLE_SPLASH;
			pobj->m_nNumEffectCycles = 15;
			pobj->m_nSkipFrames = 2;
			pobj->m_nCreationChance = 0;
			break;
		}

		case POBJECT_SMALL_FIRE:
		{
			pobj->m_ParticleType = PARTICLE_FLAME;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 2;
			pobj->m_nCreationChance = 2;
			pobj->m_vecTarget = CVector(0.0f, 0.0f, 0.0f);
			break;
		}

		case POBJECT_BIG_FIRE:
		{
			pobj->m_ParticleType = PARTICLE_FLAME;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 2;
			pobj->m_nCreationChance = 4;
			pobj->m_vecTarget = CVector(0.0f, 0.0f, 0.0f);
			break;
		}

		case POBJECT_DRY_ICE:
		{
			pobj->m_ParticleType = PARTICLE_SMOKE;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			pobj->m_vecTarget = CVector(0.0f, 0.0f, 0.0f);
			break;
		}

		case POBJECT_DRY_ICE_SLOWMOTION:
		{
			pobj->m_ParticleType = PARTICLE_SMOKE_SLOWMOTION;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			pobj->m_vecTarget = CVector(0.0f, 0.0f, 0.0f);
			break;
		}

		case POBJECT_FIRE_TRAIL:
		{
			pobj->m_ParticleType = PARTICLE_EXPLOSION_MEDIUM;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 3;
			pobj->m_nCreationChance = 2;
			pobj->m_fRandVal = 0.01f;
			break;
		}

		case POBJECT_SMOKE_TRAIL:
		{
			pobj->m_ParticleType = PARTICLE_FIREBALL_SMOKE;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 2;
			pobj->m_fRandVal = 0.02f;
			break;
		}

		case POBJECT_FIREBALL_AND_SMOKE:
		{
			pobj->m_ParticleType = PARTICLE_FLAME;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			pobj->m_fRandVal = 0.1f;
			break;
		}

		case POBJECT_ROCKET_TRAIL:
		{
			pobj->m_ParticleType = PARTICLE_FLAME;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 2;
			pobj->m_nCreationChance = 8;
			pobj->m_fRandVal = 0.1f;
			break;
		}

		case POBJECT_EXPLOSION_ONCE:
		{
			pobj->m_ParticleType = PARTICLE_EXPLOSION_LARGE;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			pobj->m_nRemoveTimer = CTimer::m_snTimeInMilliseconds;
			break;
		}
		case POBJECT_CATALINAS_GUNFLASH:
		case POBJECT_CATALINAS_SHOTGUNFLASH:
		{
			pobj->m_ParticleType = PARTICLE_GUNFLASH_NOANIM;
			pobj->m_nNumEffectCycles = 1;
			pobj->m_nSkipFrames = 1;
			pobj->m_nCreationChance = 0;
			pobj->m_nRemoveTimer = CTimer::m_snTimeInMilliseconds;
			pobj->m_vecTarget.Normalise();
			break;
		}
		}
	}

	return pobj;
}

CParticleObject*
CParticleObject::AddObject(tParticleType type, CVector const& pos, CVector const& target, float size, uint32_t lifeTime, uint8_t numEffectCycles, uint8_t skipFrames, uint16_t creationChance, uint8_t remove)
{
	CParticleObject* pobj = pUnusedListHead;

	ASSERT(pobj != NULL);

	if (pobj == NULL)
	{
		log("Error: No particle objects available!\n");
		debug("Error: No particle objects available!\n");
		return NULL;
	}

	MoveToList(&pUnusedListHead, &pCloseListHead, pobj);

	pobj->m_nState = POBJECTSTATE_UPDATE_CLOSE;
	pobj->m_Type = (eParticleObjectType)-1;
	pobj->m_ParticleType = type;

	pobj->SetPosn(pos);
	pobj->m_vecTarget = target;

	pobj->m_nNumEffectCycles = numEffectCycles;
	pobj->m_nSkipFrames = skipFrames;
	pobj->m_nCreationChance = creationChance;
	pobj->m_nFrameCounter = 0;

	pobj->m_bRemove = remove;

	if (lifeTime != 0)
		pobj->m_nRemoveTimer = CTimer::m_snTimeInMilliseconds + lifeTime;
	else
		pobj->m_nRemoveTimer = 0;

	pobj->m_Color.alpha = 0;

	pobj->m_fSize = size;
	pobj->m_fRandVal = 0.0f;

	return pobj;
}

void
CParticleObject::RemoveObject(void)
{
	switch (this->m_nState)
	{
	case POBJECTSTATE_UPDATE_CLOSE:
	{
		MoveToList(&pCloseListHead, &pUnusedListHead, this);
		this->m_nState = POBJECTSTATE_FREE;
		break;
	}
	case POBJECTSTATE_UPDATE_FAR:
	{
		MoveToList(&pFarListHead, &pUnusedListHead, this);
		this->m_nState = POBJECTSTATE_FREE;
		break;
	}
	}
}

void
CParticleObject::RemoveObject(CParticleObject* obj)
{
	switch (obj->m_nState)
	{
	case POBJECTSTATE_UPDATE_CLOSE:
	{
		MoveToList(&pCloseListHead, &pUnusedListHead, obj);
		obj->m_nState = POBJECTSTATE_FREE;
		break;
	}
	case POBJECTSTATE_UPDATE_FAR:
	{
		MoveToList(&pFarListHead, &pUnusedListHead, obj);
		obj->m_nState = POBJECTSTATE_FREE;
		break;
	}
	}
}

void
CParticleObject::UpdateAll(void)
{
	{
		CParticleObject* pobj = pCloseListHead;
		CParticleObject* nextpobj;
		if (pobj != NULL)
		{
			do
			{
				nextpobj = pobj->m_pNext;
				pobj->UpdateClose();
				pobj = nextpobj;
			} while (nextpobj != NULL);
		}
	}

	{
		int32_t frame = CTimer::m_FrameCounter & 31;
		int32_t counter = 0;

		CParticleObject* pobj = pFarListHead;
		CParticleObject* nextpobj;
		if (pobj != NULL)
		{
			do
			{
				nextpobj = pobj->m_pNext;

				if (counter == frame)
				{
					pobj->UpdateFar();
					frame += 32;
				}

				counter++;

				pobj = nextpobj;
			} while (nextpobj != NULL);
		}
	}
}

void CParticleObject::UpdateClose(void)
{
	//if (!CGame::intro)
	//{
		if ((this->GetPosition() - TheCamera.GetPosition()).MagnitudeSqr2D() > SQR(100.0f))
		{
			if (this->m_bRemove)
			{
			//	if (this->m_Type == POBJECT_FIRE_HYDRANT)
				//	CAudioHydrant::Remove(this);

				MoveToList(&pCloseListHead, &pUnusedListHead, this);
				this->m_nState = POBJECTSTATE_FREE;
			}
			else
			{
				MoveToList(&pCloseListHead, &pFarListHead, this);
				this->m_nState = POBJECTSTATE_UPDATE_FAR;
			}

			return;
		}
	//}

	if (++this->m_nFrameCounter >= this->m_nSkipFrames)
	{
		this->m_nFrameCounter = 0;

		int32_t randVal;
		if (this->m_nCreationChance != 0)
			randVal = GetRandomNumber() % this->m_nCreationChance;

		if (this->m_nCreationChance == 0
			|| randVal == 0 && this->m_nCreationChance < 0
			|| randVal != 0 && this->m_nCreationChance > 0)
		{
			switch (this->m_Type)
			{
			case POBJECT_SMALL_FIRE:
			{
				CVector pos = this->GetPosition();
				CVector vel = this->m_vecTarget;
				float size = this->m_fSize;

				CVector flamevel;

				flamevel.x = vel.x;
				flamevel.y = vel.y;
				flamevel.z = CGeneral::GetRandomNumberInRange(0.0125f * size, 0.1f * size);

				CParticle::AddParticle(PARTICLE_FLAME, pos, flamevel, NULL, size);


				CVector possmoke = pos;

				possmoke.x += CGeneral::GetRandomNumberInRange(0.625f * -size, size * 0.625f);
				possmoke.y += CGeneral::GetRandomNumberInRange(0.625f * -size, size * 0.625f);
				possmoke.z += CGeneral::GetRandomNumberInRange(0.625f * size, size * 2.5f);

				CParticle::AddParticle(PARTICLE_CARFLAME_SMOKE, possmoke, vel);

				break;
			}

			case POBJECT_BIG_FIRE:
			{
				CVector pos = this->GetPosition();
				CVector vel = this->m_vecTarget;
				float size = this->m_fSize;


				float s = 0.7f * size;

				CVector flamevel;

				flamevel.x = vel.x;
				flamevel.y = vel.y;
				flamevel.z = CGeneral::GetRandomNumberInRange(0.0125f * s, 0.1f * s);

				float flamesize = 0.8f * size;

				CParticle::AddParticle(PARTICLE_FLAME, pos, flamevel, NULL, flamesize);


				for (int32_t i = 0; i < 4; i++)
				{
					CVector smokepos = pos;

					smokepos.x += CGeneral::GetRandomNumberInRange(0.625f * -size, 0.625f * size);
					smokepos.y += CGeneral::GetRandomNumberInRange(0.625f * -size, 0.625f * size);
					smokepos.z += CGeneral::GetRandomNumberInRange(0.625f * size, 3.5f * size);

					CParticle::AddParticle(PARTICLE_CARFLAME_SMOKE, smokepos, vel);
				}

				break;
			}

			case POBJECT_FIREBALL_AND_SMOKE:
			{
				if (this->m_pParticle == NULL)
				{
					CVector pos = this->GetPosition();
					CVector vel = this->m_vecTarget;
					float size = this->m_fSize;

					CVector expvel = 1.2f * vel;
					float expsize = 1.2f * size;
					this->m_pParticle = CParticle::AddParticle(PARTICLE_EXPLOSION_MEDIUM, pos, expvel, NULL, expsize);
				}
				else
				{
					CVector pos = this->GetPosition(); // this->m_pParticle->m_vecPosition ?
					CVector vel = this->m_vecTarget;
					float size = this->m_fSize;

					CVector veloffset = 0.35f * vel;
					CVector fireballvel = vel;

					for (int32_t i = 0; i < this->m_nNumEffectCycles; i++)
					{
						fireballvel.x += CGeneral::GetRandomNumberInRange(-veloffset.x, veloffset.x);
						fireballvel.y += CGeneral::GetRandomNumberInRange(-veloffset.y, veloffset.y);
						fireballvel.z += CGeneral::GetRandomNumberInRange(-veloffset.z, veloffset.z);

						CParticle::AddParticle(PARTICLE_FIREBALL_SMOKE, pos, fireballvel, NULL, size);
					}
				}

				break;
			}

			case POBJECT_ROCKET_TRAIL:
			{
				if (this->m_pParticle == NULL)
				{
					CVector pos = this->GetPosition();
					CVector vel = this->m_vecTarget;
					float size = this->m_fSize;

					this->m_pParticle = CParticle::AddParticle(PARTICLE_EXPLOSION_MEDIUM, pos, vel, NULL, size);
				}
				else
				{
					CVector pos = this->m_pParticle->m_vecPosition;
					CVector vel = this->m_vecTarget;
					float size = this->m_fSize;

					float fireballsize = size * 1.5f;
					CVector fireballvel = vel * -0.8f;

					for (int32_t i = 0; i < this->m_nNumEffectCycles; i++)
					{
						CParticle::AddParticle(PARTICLE_FIREBALL_SMOKE, pos, fireballvel, NULL, fireballsize);
					}
				}

				break;
			}

			case POBJECT_FIRE_TRAIL:
			{
				for (int32_t i = 0; i < this->m_nNumEffectCycles; i++)
				{
					CVector vel = this->m_vecTarget;

					if (vel.x != 0.0f)
						vel.x += CGeneral::GetRandomNumberInRange(-this->m_fRandVal, this->m_fRandVal);

					if (vel.y != 0.0f)
						vel.y += CGeneral::GetRandomNumberInRange(-this->m_fRandVal, this->m_fRandVal);

					if (vel.z != 0.0f)
						vel.z += CGeneral::GetRandomNumberInRange(-this->m_fRandVal, this->m_fRandVal);

					CParticle::AddParticle(this->m_ParticleType, this->GetPosition(), vel, NULL, this->m_fSize,
						CGeneral::GetRandomNumberInRange(-6.0f, 6.0f));
				}

				break;
			}

			case POBJECT_PED_WATER_SPLASH:
			{
				RwRGBA colorsmoke(255, 255, 255, 196);

				CVector pos = this->GetPosition();
				CVector vel = this->m_vecTarget;

				for (int32_t i = 0; i < 3; i++)
				{
					int32_t angle = 90 * i;
					float fCos = CParticle::Cos(angle);
					float fSin = CParticle::Sin(angle);

					CVector splashpos;
					CVector splashvel;

					splashpos = pos + CVector(0.75f * fCos, 0.75f * fSin, 0.0f);
					splashvel = vel + CVector(0.05f * fCos, 0.05f * fSin, CGeneral::GetRandomNumberInRange(0.04f, 0.08f));

					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.8f), colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.5f), this->m_Color);


					splashpos = pos + CVector(0.75f * fCos, 0.75f * -fSin, 0.0f);
					splashvel = vel + CVector(0.05f * fCos, 0.05f * -fSin, CGeneral::GetRandomNumberInRange(0.04f, 0.08f));


					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.8f), colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.5f), this->m_Color);


					splashpos = pos + CVector(0.75f * -fCos, 0.75f * fSin, 0.0f);
					splashvel = vel + CVector(0.05f * -fCos, 0.05f * fSin, CGeneral::GetRandomNumberInRange(0.04f, 0.08f));


					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.8f), colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.5f), this->m_Color);


					splashpos = pos + CVector(0.75f * -fCos, 0.75f * -fSin, 0.0f);
					splashvel = vel + CVector(0.05f * -fCos, 0.05f * -fSin, CGeneral::GetRandomNumberInRange(0.04f, 0.08f));

					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.8f), colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.1f, 0.5f), this->m_Color);
				}

				for (int32_t i = 0; i < 1; i++)
				{
					int32_t angle = 180 * (i + 1);

					float fCos = CParticle::Cos(angle);
					float fSin = CParticle::Sin(angle);

					CVector splashpos;
					CVector splashvel;

					splashpos = pos + CVector(0.5f * fCos, 0.5f * fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.05f, 0.25f);

					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.4f, 1.0f), this->m_Color);


					splashpos = pos + CVector(0.5f * fCos, 0.5f * -fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * -fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.05f, 0.25f);

					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.4f, 1.0f), this->m_Color);


					splashpos = pos + CVector(0.5f * -fCos, 0.5f * fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * -fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.05f, 0.25f);

					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.4f, 1.0f), this->m_Color);


					splashpos = pos + CVector(0.5f * -fCos, 0.5f * -fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * -fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.25f, 0.25f) * -fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.05f, 0.25f);

					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.4f, 1.0f), this->m_Color);
				}

				break;
			}

			case POBJECT_CAR_WATER_SPLASH:
			{
				RwRGBA colorsmoke(255, 255, 255, 196);

				CVector pos = this->GetPosition();
				CVector vel = this->m_vecTarget;

				float size = CGeneral::GetRandomNumberInRange(1.0f, 2.5f);

				for (int32_t i = 0; i < 3; i++)
				{
					int32_t angle = 90 * i;

					float fCos = CParticle::Cos(angle);
					float fSin = CParticle::Sin(angle);

					CVector splashpos;
					CVector splashvel;

					splashpos = pos + CVector(2.0f * fCos, 2.0f * fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.01f, 0.03f);

					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL, size, colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);


					splashpos = pos + CVector(2.0f * fCos, 2.0f * -fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * -fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.01f, 0.03f);

					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL, size, colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);

					splashpos = pos + CVector(2.0f * -fCos, 2.0f * fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * -fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.01f, 0.03f);

					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL, size, colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);

					splashpos = pos + CVector(2.0f * -fCos, 2.0f * -fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * -fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) * -fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.01f, 0.03f);

					CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, splashpos, splashvel, NULL, size, colorsmoke);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);
				}

				for (int32_t i = 0; i < 1; i++)
				{
					int32_t angle = 180 * (i + 1);

					float fCos = CParticle::Cos(angle);
					float fSin = CParticle::Sin(angle);

					CVector splashpos;
					CVector splashvel;


					splashpos = pos + CVector(1.25f * fCos, 1.25f * fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.26f, 0.53f);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);

					splashpos = pos + CVector(1.25f * fCos, 1.25f * -fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * -fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.26f, 0.53f);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);

					splashpos = pos + CVector(1.25f * -fCos, 1.25f * fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * -fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.26f, 0.53f);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);

					splashpos = pos + CVector(1.25f * -fCos, 1.25f * -fSin, 0.0f);
					splashvel = vel;
					splashvel.x += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * -fCos;
					splashvel.y += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f) * -fSin;
					splashvel.z += CGeneral::GetRandomNumberInRange(0.26f, 0.53f);
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL, 0.0f, this->m_Color);
				}

				break;
			}

			case POBJECT_SPLASHES_AROUND:
			{
				CVector pos = this->GetPosition();
				float size = this->m_fSize;

				for (int32_t i = 0; i < this->m_nNumEffectCycles; i++)
				{
					CVector splashpos = pos;

					splashpos.x += CGeneral::GetRandomNumberInRange(-size, size);
					splashpos.y += CGeneral::GetRandomNumberInRange(-size, size);

					if (GetRandomNumber() & 1)
					{
						CParticle::AddParticle(PARTICLE_RAIN_SPLASH, splashpos, CVector(0.0f, 0.0f, 0.0f),
							NULL, 0.1f, this->m_Color);
					}
					else
					{
						CParticle::AddParticle(PARTICLE_RAIN_SPLASHUP, splashpos, CVector(0.0f, 0.0f, 0.0f),
							NULL, 0.12f, this->m_Color);
					}
				}

				break;
			}
			case POBJECT_CATALINAS_GUNFLASH:
			{
				RwRGBA flashcolor(120, 120, 120, 255);

				CVector vel = this->m_vecTarget;
				CVector pos = this->GetPosition();

				float size = 1.0f;
				if (this->m_fSize != 0.0f)
					size = this->m_fSize;

				CParticle::AddParticle(PARTICLE_GUNFLASH, pos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.12f * size, flashcolor);

				pos += size * (0.06f * vel);
				CParticle::AddParticle(PARTICLE_GUNFLASH, pos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.08f * size, flashcolor);

				pos += size * (0.04f * vel);
				CParticle::AddParticle(PARTICLE_GUNFLASH, pos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.04f * size, flashcolor);

				CVector smokepos = this->GetPosition();
				CVector smokevel = 0.1f * vel;
				CParticle::AddParticle(PARTICLE_GUNSMOKE2, smokepos, smokevel, NULL, 0.005f * size);

				break;
			}

			case POBJECT_CATALINAS_SHOTGUNFLASH:
			{
				RwRGBA flashcolor(120, 120, 120, 255);

				CVector vel = this->m_vecTarget;

				float size = 1.0f;
				if (this->m_fSize != 0.0f)
					size = this->m_fSize;

				CVector pos = this->GetPosition();

				CVector velstep = size * (0.1f * vel);
				CVector flashpos = pos;

				flashpos += velstep;
				CParticle::AddParticle(PARTICLE_GUNFLASH, flashpos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, flashcolor);

				flashpos += velstep;
				CParticle::AddParticle(PARTICLE_GUNFLASH, flashpos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.15f * size, flashcolor);

				flashpos += velstep;
				CParticle::AddParticle(PARTICLE_GUNFLASH, flashpos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.2f * size, flashcolor);


				CParticle::AddParticle(PARTICLE_GUNFLASH, pos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, flashcolor);

				CVector smokepos = this->GetPosition();
				CVector smokevel = 0.1f * vel;
				CParticle::AddParticle(PARTICLE_GUNSMOKE2, smokepos, smokevel, NULL, 0.1f * size);

				break;
			}

			case POBJECT_FIRE_HYDRANT:
			{
				CVector pos = this->GetPosition();
				CVector vel = this->m_vecTarget;

				if ((TheCamera.GetPosition() - pos).Magnitude() > 5.0f)
				{
					for (int32_t i = 0; i < 1; i++)
					{
						int32_t angle = 180 * i;

						float fCos = CParticle::Cos(angle);
						float fSin = CParticle::Sin(angle);

						CVector splashpos, splashvel;

						splashpos = pos + CVector(0.01f * fCos, 0.01f * fSin, 0.0f);
						splashvel = vel + CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

						CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
							CGeneral::GetRandomNumberInRange(0.005f, 0.0075f), this->m_Color, 0, 0, 1, 300);

						splashpos = pos + CVector(0.01f * fCos, 0.01f * -fSin, 0.0f);
						splashvel = vel + CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

						CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
							CGeneral::GetRandomNumberInRange(0.005f, 0.0075f), this->m_Color, 0, 0, 1, 300);

						splashpos = pos + CVector(0.01f * -fCos, 0.01f * fSin, 0.0f);
						splashvel = vel + CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

						CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
							CGeneral::GetRandomNumberInRange(0.005f, 0.0075f), this->m_Color, 0, 0, 1, 300);

						splashpos = pos + CVector(0.01f * -fCos, 0.01f * -fSin, 0.0f);
						splashvel = vel + CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

						CParticle::AddParticle(PARTICLE_CAR_SPLASH, splashpos, splashvel, NULL,
							CGeneral::GetRandomNumberInRange(0.005f, 0.0075f), this->m_Color, 0, 0, 1, 300);
					}
					for (int32_t i = 0; i < this->m_nNumEffectCycles; i++)
					{
						CParticle::AddParticle(this->m_ParticleType, pos, vel, NULL, 0.0f, this->m_Color);
					}
				}

				break;
			}

			case POBJECT_WATER_FOUNTAIN_VERT:
			{
				CVector pos = this->GetPosition();
				CVector vel = this->m_vecTarget;

				for (int32_t i = 0; i < 2; i++)
				{
					int32_t angle = 180 * i;

					float fCos = CParticle::Cos(angle);
					float fSin = CParticle::Sin(angle);

					CVector splashpos, splashvel;

					splashpos = pos + CVector(0.015f * fCos, 0.015f * fSin, 0.0f);
					splashvel = vel + CVector(0.015f * fCos, 0.015f * fSin, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

					CParticle::AddParticle(PARTICLE_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.001f, 0.005f), this->m_Color, 0, 0, 1, 1000);

					splashpos = pos + CVector(0.015f * fCos, 0.015f * -fSin, 0.0f);
					splashvel = vel + CVector(0.015f * fCos, 0.015f * -fSin, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

					CParticle::AddParticle(PARTICLE_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.001f, 0.005f), this->m_Color, 0, 0, 1, 1000);

					splashpos = pos + CVector(0.015f * -fCos, 0.015f * fSin, 0.0f);
					splashvel = vel + CVector(0.015f * -fCos, 0.015f * fSin, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

					CParticle::AddParticle(PARTICLE_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.001f, 0.005f), this->m_Color, 0, 0, 1, 1000);

					splashpos = pos + CVector(0.015f * -fCos, 0.015f * -fSin, 0.0f);
					splashvel = vel + CVector(0.015f * -fCos, 0.015f * -fSin, CGeneral::GetRandomNumberInRange(0.004f, 0.008f));

					CParticle::AddParticle(PARTICLE_SPLASH, splashpos, splashvel, NULL,
						CGeneral::GetRandomNumberInRange(0.001f, 0.005f), this->m_Color, 0, 0, 1, 1000);
				}

				break;
			}

			case POBJECT_WATER_FOUNTAIN_HORIZ:
			{
				CVector pos = this->GetPosition();
				CVector vel = this->m_vecTarget;

				for (int32_t i = 0; i < 3; i++)
				{
					CParticle::AddParticle(PARTICLE_CAR_SPLASH, pos, vel, NULL, 0.001f, this->m_Color, 0, 0, 1, 1000);
				}

				break;
			}

			default:
			{
				if (this->m_fRandVal != 0.0f)
				{
					for (int32_t i = 0; i < this->m_nNumEffectCycles; i++)
					{
						CVector vel = this->m_vecTarget;

						if (vel.x != 0.0f)
							vel.x += CGeneral::GetRandomNumberInRange(-this->m_fRandVal, this->m_fRandVal);

						if (vel.y != 0.0f)
							vel.y += CGeneral::GetRandomNumberInRange(-this->m_fRandVal, this->m_fRandVal);

						if (vel.z != 0.0f)
							vel.z += CGeneral::GetRandomNumberInRange(-this->m_fRandVal, this->m_fRandVal);

						CParticle::AddParticle(this->m_ParticleType, this->GetPosition(), vel, NULL,
							this->m_fSize, this->m_Color);
					}
				}
				else
				{
					for (int32_t i = 0; i < this->m_nNumEffectCycles; i++)
					{
						CParticle::AddParticle(this->m_ParticleType, this->GetPosition(), this->m_vecTarget, NULL,
							this->m_fSize, this->m_Color);
					}
				}

				break;
			}
			}
		}
	}

	if (this->m_nRemoveTimer != 0 && this->m_nRemoveTimer < CTimer::m_snTimeInMilliseconds)
	{
		MoveToList(&pCloseListHead, &pUnusedListHead, this);
		this->m_nState = POBJECTSTATE_FREE;

	//	if (this->m_Type == POBJECT_FIRE_HYDRANT)
			//CAudioHydrant::Remove(this);
	}
}
void
CParticleObject::UpdateFar(void)
{
	if (this->m_nRemoveTimer != 0 && this->m_nRemoveTimer < CTimer::m_snTimeInMilliseconds)
	{
		MoveToList(&pFarListHead, &pUnusedListHead, this);
		this->m_nState = POBJECTSTATE_FREE;

		//if (this->m_Type == POBJECT_FIRE_HYDRANT)
			//CAudioHydrant::Remove(this);
	}

	CVector2D dist = this->GetPosition() - TheCamera.GetPosition();
	if (dist.MagnitudeSqr() < SQR(100.0f)/*10000.0f*/)
	{
		MoveToList(&pFarListHead, &pCloseListHead, this);
		this->m_nState = POBJECTSTATE_UPDATE_CLOSE;
	}
}

#ifdef COMPATIBLE_SAVES
static inline void
SaveOneParticle(CParticleObject* p, uint8_t*& buffer)
{
#define SkipBuf(buf, num) buf += num
#define ZeroBuf(buf, num) memset(buf, 0, num); SkipBuf(buf, num)
#define CopyToBuf(buf, data) memcpy(buf, &data, sizeof(data)); SkipBuf(buf, sizeof(data))
	// CPlaceable
	{
		CopyToBuf(buffer, p->GetMatrix()->GetForward());
		ZeroBuf(buffer, 4);
		//CopyToBuf(buffer, p->GetMatrix()->rwm);
		//ZeroBuf(buffer, 3);
	}

	// CParticleObject
	{
		ZeroBuf(buffer, 4);
		ZeroBuf(buffer, 4);
		ZeroBuf(buffer, 4);
		CopyToBuf(buffer, p->m_nRemoveTimer);
		CopyToBuf(buffer, p->m_Type);
		CopyToBuf(buffer, p->m_ParticleType);
		CopyToBuf(buffer, p->m_nNumEffectCycles);
		CopyToBuf(buffer, p->m_nSkipFrames);
		CopyToBuf(buffer, p->m_nFrameCounter);
		CopyToBuf(buffer, p->m_nState);
		ZeroBuf(buffer, 2);
		CopyToBuf(buffer, p->m_vecTarget);
		CopyToBuf(buffer, p->m_fRandVal);
		CopyToBuf(buffer, p->m_fSize);
		CopyToBuf(buffer, p->m_Color);
		CopyToBuf(buffer, p->m_bRemove);
		CopyToBuf(buffer, p->m_nCreationChance);
		ZeroBuf(buffer, 2);
	}
#undef SkipBuf
#undef ZeroBuf
#undef CopyToBuf
}
#endif

bool
CParticleObject::SaveParticle(uint8_t* buffer, uint32_t* length)
{
	ASSERT(buffer != NULL);
	ASSERT(length != NULL);

	int32_t numObjects = 0;

	for (CParticleObject* p = pCloseListHead; p != NULL; p = p->m_pNext)
		++numObjects;

	for (CParticleObject* p = pFarListHead; p != NULL; p = p->m_pNext)
		++numObjects;

	*(int32_t*)buffer = numObjects;
	buffer += sizeof(int32_t);

	int32_t objectsLength = PARTICLE_OBJECT_SIZEOF * (numObjects + 1);
	int32_t dataLength = objectsLength + sizeof(int32_t);

	for (CParticleObject* p = pCloseListHead; p != NULL; p = p->m_pNext)
	{
#ifdef COMPATIBLE_SAVES
		SaveOneParticle(p, buffer);
#else
#ifdef THIS_IS_STUPID
		* (CParticleObject*)buffer = *p;
#else
		memcpy(buffer, p, sizeof(CParticleObject));
#endif
		buffer += sizeof(CParticleObject);
#endif
	}

	for (CParticleObject* p = pFarListHead; p != NULL; p = p->m_pNext)
	{
#ifdef COMPATIBLE_SAVES
		SaveOneParticle(p, buffer);
#else
#ifdef THIS_IS_STUPID
		* (CParticleObject*)buffer = *p;
#else
		memcpy(buffer, p, sizeof(CParticleObject));
#endif
		buffer += sizeof(CParticleObject);
#endif
	}

	*length = dataLength;

	return true;
}

bool
CParticleObject::LoadParticle(uint8_t* buffer, uint32_t  length)
{
	ASSERT(buffer != NULL);

	RemoveAllParticleObjects();

	int32_t numObjects = *(int32_t*)buffer;
	buffer += sizeof(int32_t);

	if (length != PARTICLE_OBJECT_SIZEOF * (numObjects + 1) + sizeof(int32_t))
		return false;

	if (numObjects == 0)
		return true;


	int32_t i = 0;
	while (i < numObjects)
	{
		CParticleObject* dst = pUnusedListHead;
#ifndef COMPATIBLE_SAVES
		CParticleObject* src = (CParticleObject*)buffer;
		buffer += sizeof(CParticleObject);
#endif

		if (dst == NULL)
			return false;

		MoveToList(&pUnusedListHead, &pCloseListHead, dst);

#ifndef COMPATIBLE_SAVES
		dst->m_nState = POBJECTSTATE_UPDATE_CLOSE;
		dst->m_Type = src->m_Type;
		dst->m_ParticleType = src->m_ParticleType;
		dst->SetPosn(src->GetPosition());
		dst->m_vecTarget = src->m_vecTarget;
		dst->m_nFrameCounter = src->m_nFrameCounter;
		dst->m_bRemove = src->m_bRemove;
		dst->m_pParticle = NULL;
		dst->m_nRemoveTimer = src->m_nRemoveTimer;
		dst->m_Color = src->m_Color;
		dst->m_fSize = src->m_fSize;
		dst->m_fRandVal = src->m_fRandVal;
		dst->m_nNumEffectCycles = src->m_nNumEffectCycles;
		dst->m_nSkipFrames = src->m_nSkipFrames;
		dst->m_nCreationChance = src->m_nCreationChance;
#else
		dst->m_nState = POBJECTSTATE_UPDATE_CLOSE;
		dst->m_pParticle = NULL;

#define SkipBuf(buf, num) buf += num
#define CopyFromBuf(buf, data) memcpy(&data, buf, sizeof(data)); SkipBuf(buf, sizeof(data))
		// CPlaceable
		{
			CMatrix matrix;
			CopyFromBuf(buffer, matrix.GetForward());
			SkipBuf(buffer, 4);
			//CopyFromBuf(buffer, matrix.m_hasRwMatrix);
		//	SkipBuf(buffer, 3);
			dst->SetPosn(matrix.GetPosition());
		}

		// CParticleObject
		{
			SkipBuf(buffer, 4);
			SkipBuf(buffer, 4);
			SkipBuf(buffer, 4);
			CopyFromBuf(buffer, dst->m_nRemoveTimer);
			CopyFromBuf(buffer, dst->m_Type);
			CopyFromBuf(buffer, dst->m_ParticleType);
			CopyFromBuf(buffer, dst->m_nNumEffectCycles);
			CopyFromBuf(buffer, dst->m_nSkipFrames);
			CopyFromBuf(buffer, dst->m_nFrameCounter);
			SkipBuf(buffer, 2);
			SkipBuf(buffer, 2);
			CopyFromBuf(buffer, dst->m_vecTarget);
			CopyFromBuf(buffer, dst->m_fRandVal);
			CopyFromBuf(buffer, dst->m_fSize);
			CopyFromBuf(buffer, dst->m_Color);
			CopyFromBuf(buffer, dst->m_bRemove);
			CopyFromBuf(buffer, dst->m_nCreationChance);
			SkipBuf(buffer, 2);
		}
#undef CopyFromBuf
#undef SkipBuf
#endif

		i++;
	}

	return true;
}

void
CParticleObject::RemoveAllExpireableParticleObjects(void)
{
	{
		CParticleObject* pobj = pCloseListHead;
		CParticleObject* nextpobj;
		if (pobj != NULL)
		{
			do
			{
				nextpobj = pobj->m_pNext;
				if (pobj->m_nRemoveTimer != 0)
				{
					MoveToList(&pCloseListHead, &pUnusedListHead, pobj);
					pobj->m_nState = POBJECTSTATE_FREE;
				}
				pobj = nextpobj;
			} while (nextpobj != NULL);
		}
	}

	{
		CParticleObject* pobj = pFarListHead;
		CParticleObject* nextpobj;
		if (pobj != NULL)
		{
			do
			{
				nextpobj = pobj->m_pNext;
				if (pobj->m_nRemoveTimer != 0)
				{
					MoveToList(&pFarListHead, &pUnusedListHead, pobj);
					pobj->m_nState = POBJECTSTATE_FREE;
				}
				pobj = nextpobj;
			} while (nextpobj != NULL);
		}
	}
}

void
CParticleObject::RemoveAllParticleObjects(void)
{
	pUnusedListHead = &gPObjectArray[0];

	pCloseListHead = NULL;
	pFarListHead = NULL;

	for (int32_t i = 0; i < MAX_PARTICLEOBJECTS; i++)
	{
		if (i == 0)
			gPObjectArray[i].m_pPrev = NULL;
		else
			gPObjectArray[i].m_pPrev = &gPObjectArray[i - 1];

		if (i == MAX_PARTICLEOBJECTS - 1)
			gPObjectArray[i].m_pNext = NULL;
		else
			gPObjectArray[i].m_pNext = &gPObjectArray[i + 1];

		gPObjectArray[i].m_nState = POBJECTSTATE_FREE;
	}
}

void
CParticleObject::MoveToList(CParticleObject** from, CParticleObject** to, CParticleObject* obj)
{
	ASSERT(from != NULL);
	ASSERT(to != NULL);
	ASSERT(obj != NULL);

	if (obj->m_pPrev == NULL)
	{
		*from = obj->m_pNext;
		if (*from)
			(*from)->m_pPrev = NULL;
	}
	else
	{
		if (obj->m_pNext == NULL)
			obj->m_pPrev->m_pNext = NULL;
		else
		{
			obj->m_pNext->m_pPrev = obj->m_pPrev;
			obj->m_pPrev->m_pNext = obj->m_pNext;
		}
	}

	obj->m_pNext = *to;
	obj->m_pPrev = NULL;
	*to = obj;

	if (obj->m_pNext)
		obj->m_pNext->m_pPrev = obj;
}
