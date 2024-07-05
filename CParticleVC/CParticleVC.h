#pragma once
#include "ParticleMgr.h"
enum eParticleSwitch
{
	PS_ORIGINAL = 0,
	PS_PS2,
	PS_XBOX,
	PS_XBOX2,
	PS_MAX,
};
class CEntity;
class CParticle
{
public:
	enum
	{
		RAND_TABLE_SIZE = 20,
		SIN_COS_TABLE_SIZE = 1024
	};

	CVector   m_vecPosition;
	CVector   m_vecVelocity;
	uint32_t    m_nTimeWhenWillBeDestroyed;
	uint32_t    m_nTimeWhenColorWillBeChanged;
	float     m_fZGround;
	CVector   m_vecParticleMovementOffset;
	int16_t     m_nCurrentZRotation;
	uint16_t    m_nZRotationTimer;
	float     m_fCurrentZRadius;
	uint16_t    m_nZRadiusTimer;
	uint8_t     m_nColorIntensity;
	uint8_t     m_nAlpha;
	float     m_fSize;
	float     m_fExpansionRate;
	int16_t     m_nFadeToBlackTimer;
	int16_t     m_nFadeAlphaTimer;
	int16_t     m_nAnimationSpeedTimer;
	int16_t     m_nRotationStep;
	int16_t     m_nRotation;
	uint8_t     m_nCurrentFrame;
	RwRGBA    m_Color;
	CParticle* m_pNext;

	CParticle()
	{
		;
	}

	~CParticle()
	{
		;
	}

	static float      ms_afRandTable[RAND_TABLE_SIZE];
	static CParticle* m_pUnusedListHead;

	static float      m_SinTable[SIN_COS_TABLE_SIZE];
	static float      m_CosTable[SIN_COS_TABLE_SIZE];

	static float Sin(int32_t value) { return m_SinTable[value]; }
	static float Cos(int32_t value) { return m_CosTable[value]; }

	static void ReloadConfig();
	static void Initialise();
	static void ReloadTXD();
	static void Shutdown();

	static void AddParticlesAlongLine(tParticleType type, CVector const& vecStart, CVector const& vecEnd, CVector const& vecDir, float fPower, CEntity* pEntity = NULL, float fSize = 0.0f, int32_t nRotationSpeed = 0, int32_t nRotation = 0, int32_t nCurFrame = 0, int32_t nLifeSpan = 0);
	static void AddParticlesAlongLine(tParticleType type, CVector const& vecStart, CVector const& vecEnd, CVector const& vecDir, float fPower, CEntity* pEntity, float fSize, RwRGBA const& color, int32_t nRotationSpeed = 0, int32_t nRotation = 0, int32_t nCurFrame = 0, int32_t nLifeSpan = 0);

	static CParticle* AddParticle(tParticleType type, CVector const& vecPos, CVector const& vecDir, CEntity* pEntity = NULL, float fSize = 0.0f, int32_t nRotationSpeed = 0, int32_t nRotation = 0, int32_t nCurFrame = 0, int32_t nLifeSpan = 0);
	static CParticle* AddParticle(tParticleType type, CVector const& vecPos, CVector const& vecDir, CEntity* pEntity, float fSize, RwRGBA const& color, int32_t nRotationSpeed = 0, int32_t nRotation = 0, int32_t nCurFrame = 0, int32_t nLifeSpan = 0);

	static void Update();
	static void Render();

	static void RemovePSystem(tParticleType type);
	static void RemoveParticle(CParticle* pParticle, CParticle* pPrevParticle, tParticleSystemData* pPSystemData);

	static void _Next(CParticle*& pParticle, CParticle*& pPrevParticle, tParticleSystemData* pPSystemData, bool bRemoveParticle)
	{
		if (bRemoveParticle)
		{
			RemoveParticle(pParticle, pPrevParticle, pPSystemData);

			if (pPrevParticle)
				pParticle = pPrevParticle->m_pNext;
			else
				pParticle = pPSystemData->m_pParticles;
		}
		else
		{
			pPrevParticle = pParticle;
			pParticle = pParticle->m_pNext;
		}
	}
	static void AddJetExplosion(CVector const& vecPos, float fPower, float fSize);
	static void AddYardieDoorSmoke(CVector const& vecPos, CMatrix const& matMatrix);
	static void CalWindDir(CVector* vecDirIn, CVector* vecDirOut);

	static void HandleShipsAtHorizonStuff();
	static void HandleShootableBirdsStuff(CEntity* entity, CVector const& camPos);
	static void HandleShootableBirdsStuff2(CEntity* entity, const CVector* pointA, const CVector* pointB);
};

class CParticleEx
{
	static eParticleSwitch m_eCurrentParticle;
	static eParticleSwitch m_eCurrentParticleV;

public:

	static bool m_bUseOriginalSystem;


	static void ClearPSystem(eParticleSwitch s);
	static void UpdatePObjects(eParticleSwitch s);

	static void SetParticleSwitch(eParticleSwitch s);

	static eParticleSwitch GetCurrentParticle() { return m_eCurrentParticleV; }
	static eParticleSwitch GetCurrentParticleReal() { return m_eCurrentParticle; }

	static bool IsOrig() { return GetCurrentParticle() == PS_ORIGINAL; }
	static bool IsPS2() { return GetCurrentParticle() == PS_PS2; }
	static bool IsXbox() { return GetCurrentParticle() == PS_XBOX; }
	static bool IsXbox2() { return GetCurrentParticle() == PS_XBOX2; }
};

class CSpecialParticleStuff {
	static uint32_t BoatFromStart;
public:
	static void CreateFoamAroundObject(CMatrix*, float, float, float, int32_t);
	static void StartBoatFoamAnimation();
	static void UpdateBoatFoamAnimation(CMatrix*);

};

extern bool clearWaterDrop;
extern int32_t numWaterDropOnScreen;
extern RwRaster* gpCarSplashRaster[];
extern RwRaster* gpHeatHazeRaster;
extern RwRaster* gpDotRaster;
extern RwRaster* gpRainDripRaster[];
extern RwRaster* gpRainDripDarkRaster[];
extern RwTexture* coronaringa;
extern bool ExplosionsParticles;
extern bool WaterDrops;
extern bool ParticleExEffects;
extern bool Trails;

VALIDATE_SIZE(CParticle, 0x58);