#pragma once
#include "plugin.h"
class CEntity;
class CVector;
class CAEExplosionAudioEntity;

enum eExplosionTypeVC : int32_t {
	EXPLOSION_UNDEFINED = -1,
	EXPLOSION_GRENADE = 0,
	EXPLOSION_MOLOTOV,
	EXPLOSION_ROCKET,
	EXPLOSION_WEAK_ROCKET,
	EXPLOSION_CAR,
	EXPLOSION_QUICK_CAR,
	EXPLOSION_BOAT,
	EXPLOSION_AIRCRAFT,
	EXPLOSION_MINE,
	EXPLOSION_OBJECT,
	EXPLOSION_TANK_FIRE,
	EXPLOSION_SMALL,
	EXPLOSION_RC_VEHICLE,
	EXPLOSION_HELI,
	EXPLOSION_HELI2,
	EXPLOSION_BARREL,
	EXPLOSION_CAR_QUICK,
	EXPLOSION_TANK_GRENADE,
	EXPLOSION_HELI_BOMB
};

/*enum eExplosionTypeVC
{
	EXPLOSION_GRENADE,
	EXPLOSION_MOLOTOV,
	EXPLOSION_ROCKET,
	EXPLOSION_CAR,
	EXPLOSION_CAR_QUICK,
	EXPLOSION_BOAT,
	EXPLOSION_HELI,
	EXPLOSION_HELI2,
	EXPLOSION_MINE,
	EXPLOSION_BARREL,
	EXPLOSION_TANK_GRENADE,
	EXPLOSION_HELI_BOMB
};*/

enum {
	NUM_EXPLOSIONS = 128 // 48
};

class CExplosionVC
{
	eExplosionTypeVC m_ExplosionType;
	CVector m_vecPosition;
	float m_fRadius;
	float m_fPropagationRate;
	CEntity* m_pCreatorEntity;
	CEntity* m_pVictimEntity;
	float m_fStopTime;
	uint8_t m_nIteration;
	uint8_t m_nActiveCounter;
	bool m_bIsBoat;
	bool m_bMakeSound;
	float m_fStartTime;
	uint32_t m_nParticlesExpireTime;
	float m_fPower;
	float m_fZshift;
public:
#ifdef SIMPLER_MISSIONS
	static bool AddExplosion(CEntity* explodingEntity, CEntity* culprit, eExplosionTypeVC type, const CVector& pos, uint32_t lifetime, bool makeSound = true, float radius = -1.0f);
#else
	static bool AddExplosion(CEntity* victim, CEntity* creator, eExplosionTypeVC type, CVector pos, uint32_t lifetime, uint8_t usesSound, float cameraShake, uint8_t bInvisible);
#endif
	static void ClearAllExplosions(); //done
	static bool DoesExplosionMakeSound(uint8_t id); //done
	static int8_t GetExplosionActiveCounter(uint8_t id); //done
	static CVector* GetExplosionPosition(uint8_t id); //done
	static uint8_t GetExplosionType(uint8_t id); //done, mb need change type to tExplosionType
	static void Initialise(); //done
	static void RemoveAllExplosionsInArea(CVector pos, float radius); //done
	static void ResetExplosionActiveCounter(uint8_t id); //done
	static void Shutdown(); //done
	static void Update(); //done
	static bool TestForExplosionInArea(eExplosionTypeVC type, float x1, float x2, float y1, float y2, float z1, float z2); //done, not used
};

extern CExplosionVC gaExplosion[NUM_EXPLOSIONS];