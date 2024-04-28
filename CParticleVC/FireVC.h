#pragma once
#include "plugin.h"
class CEntity;
enum {
	NUM_FIRES = 40
};
class CFireVC
{
public:
	bool m_bIsOngoing;
	bool m_bIsScriptFire;
	bool m_bPropagationFlag;
	bool m_bAudioSet;
	CVector m_vecPos;
	CEntity *m_pEntity;
	CEntity *m_pSource;
	uint32_t m_nExtinguishTime;
	uint32_t m_nStartTime;
	uint32_t m_nNextTimeToAddFlames;
	float m_fStrength;
	float m_fWaterExtinguishCountdown;
	bool m_bExtinguishedWithWater;

	CFireVC();
	~CFireVC();
	void ProcessFire(void);
	void ReportThisFire(void);
	void Extinguish(void);
};

class CFireManagerVC
{
	enum {
		MAX_FIREMEN_ATTENDING = 2,
	};
public:
	uint32_t m_nTotalFires;
	CFireVC m_aFires[NUM_FIRES];

	void StartFire(CVector pos, float size, uint8_t propagation);
	CFireVC *StartFireEnt(CEntity *entityOnFire, CEntity *fleeFrom, float strength, uint8_t propagation);
	void Update(void);
	CFireVC *FindFurthestFire_NeverMindFireMen(CVector coords, float minRange, float maxRange);
	CFireVC *FindNearestFire(CVector vecPos, float *pDistance);
	CFireVC *GetNextFreeFire(void);
	uint32_t GetTotalActiveFires() const;
	void ExtinguishPoint(CVector point, float range);
	bool ExtinguishPointWithWater(CVector point, float range);
	int32_t StartScriptFire(const CVector &pos, CEntity *target, float strength, uint8_t propagation);
	bool IsScriptFireExtinguish(int16_t index);
	void RemoveAllScriptFires(void);
	void RemoveScriptFire(int16_t index);
	void SetScriptFireAudio(int16_t index, bool state);
};
extern CFireManagerVC gFireManager;
