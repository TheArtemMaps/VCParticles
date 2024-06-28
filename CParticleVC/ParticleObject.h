#pragma once

#include "ParticleType.h"
#include "CPlaceable.h"

#define MAX_PARTICLEOBJECTS 9000
#define MAX_AUDIOHYDRANTS   8

enum eParticleObjectType
{
	POBJECT_PAVEMENT_STEAM = 0,
	POBJECT_PAVEMENT_STEAM_SLOWMOTION,
	POBJECT_WALL_STEAM,
	POBJECT_WALL_STEAM_SLOWMOTION,
	POBJECT_DARK_SMOKE,
	POBJECT_FIRE_HYDRANT,
	POBJECT_CAR_WATER_SPLASH,
	POBJECT_PED_WATER_SPLASH,
	POBJECT_SPLASHES_AROUND,
	POBJECT_SMALL_FIRE,
	POBJECT_BIG_FIRE,
	POBJECT_DRY_ICE,
	POBJECT_DRY_ICE_SLOWMOTION,
	POBJECT_WATER_FOUNTAIN_VERT,
	POBJECT_WATER_FOUNTAIN_HORIZ,
	POBJECT_FIRE_TRAIL,
	POBJECT_SMOKE_TRAIL,
	POBJECT_FIREBALL_AND_SMOKE,
	POBJECT_ROCKET_TRAIL,
	POBJECT_EXPLOSION_ONCE,
	POBJECT_CATALINAS_GUNFLASH,
	POBJECT_CATALINAS_SHOTGUNFLASH,
	POBJECT_UNKNOWN = -1 // If user made a typo in a CLEO script, return this...
};

enum eParticleObjectState
{
	POBJECTSTATE_INITIALISED = 0,
	POBJECTSTATE_UPDATE_CLOSE,
	POBJECTSTATE_UPDATE_FAR,
	POBJECTSTATE_FREE,
};

class CParticle;

class CParticleObject : public CPlaceable
{
public:
	CParticleObject* m_pNext;
	CParticleObject* m_pPrev;
	CParticle* m_pParticle;
	uint32_t               m_nRemoveTimer;
	eParticleObjectType m_Type;
	tParticleType       m_ParticleType;
	uint8_t               m_nNumEffectCycles;
	uint8_t               m_nSkipFrames;
	uint16_t              m_nFrameCounter;
	uint16_t              m_nState;
	CVector             m_vecTarget;
	float               m_fRandVal;
	float               m_fSize;
	RwRGBA               m_Color;
	uint8_t               m_bRemove;
	int8_t                m_nCreationChance;

	static CParticleObject* pCloseListHead;
	static CParticleObject* pFarListHead;
	static CParticleObject* pUnusedListHead;

	CParticleObject();
	~CParticleObject();

	static void Initialise(void);

	static CParticleObject* AddObject(uint16_t        type, CVector const& pos, uint8_t remove);
	static CParticleObject* AddObject(uint16_t        type, CVector const& pos, float size, uint8_t remove);
	static CParticleObject* AddObject(uint16_t        type, CVector const& pos, CVector const& target, float size, uint8_t remove);
	static CParticleObject* AddObject(uint16_t        type, CVector const& pos, CVector const& target, float size, uint32_t lifeTime, RwRGBA const& color, uint8_t remove);
	static CParticleObject* AddObject(tParticleType type, CVector const& pos, CVector const& target, float size, uint32_t lifeTime, uint8_t numEffectCycles, uint8_t skipFrames, uint16_t creationChance, uint8_t remove);

	void RemoveObject(void);
	static void RemoveObject(CParticleObject* obj);

	static void UpdateAll(void);
	void UpdateClose(void);
	void UpdateFar(void);

	static bool SaveParticle(uint8_t* buffer, uint32_t* length);
	static bool LoadParticle(uint8_t* buffer, uint32_t  length);

	static void RemoveAllExpireableParticleObjects(void);
	static void RemoveAllParticleObjects(void);
	static void MoveToList(CParticleObject** from, CParticleObject** to, CParticleObject* obj);
	static const char* ParticleObjectTypeToString(uint16_t type);
	static const char* ParticleObjectStateToString(uint16_t type);
};

/*class CParticleObjectPool : public CPool<CParticleObject> {
public:
	CParticleObjectPool(int maxSize) : CPool(maxSize, "ParticleObjectPool") {}
};

extern CParticleObjectPool gParticleObjectPool;*/

extern CParticleObject gPObjectArray[MAX_PARTICLEOBJECTS];

class CAudioHydrant
{
public:
	int32_t AudioEntity;
	CParticleObject* pParticleObject;

	CAudioHydrant() :
		AudioEntity(0),
		pParticleObject(NULL)
	{ }

	static bool Add(CParticleObject* particleobject);
	static void Remove(CParticleObject* particleobject);
};