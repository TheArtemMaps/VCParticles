#pragma once

#include "CVehicle.h"

enum ePlaneNodes2
{
#ifdef CPLANE_ROTORS
	// for heli
	PLANE_TOPROTOR,
	PLANE_BACKROTOR,
#endif
	PLANE_WHEEL_FRONT = 2,
	PLANE_WHEEL_READ,
	NUM_PLANE_NODES
};

struct CPlaneNode
{
	CVector p;	// position
	float t;	// xy-distance from start on path
	bool bOnGround;	// i.e. not flying
};

struct CPlaneInterpolationLine
{
	uint8_t type;
	float time;	// when does this keyframe start
	// initial values at start of frame
	float position;
	float speed;
	float acceleration;
};

class CPlane2 : public CVehicle
{
public:
#ifdef CPLANE_ROTORS
	RwFrame *m_aPlaneNodes[NUM_PLANE_NODES];
	float m_fRotorRotation;
#endif
	int16_t m_nPlaneId;
	int16_t m_isFarAway;
	int16_t m_nCurPathNode;
	float m_fSpeed;
	uint32_t m_nFrameWhenHit;
	bool m_bHasBeenHit;
	bool m_bIsDrugRunCesna;
	bool m_bIsDropOffCesna;
	bool m_bTempPlane;

	CPlane2(int32_t id, uint8_t CreatedBy);
	~CPlane2(void);

	// from CEntity
	void SetModelIndex(uint32_t id);
	void DeleteRwObject(void);
	void ProcessControl(void);
	void PreRender(void);
	void Render(void);
	void FlagToDestroyWhenNextProcessed() { m_bRemoveFromWorld = true; }

	static void InitPlanes(void);
	static void Shutdown(void);
	static CPlaneNode *LoadPath(char const *filename, int32_t &numNodes, float &totalLength, bool loop);
	static void RemoveTemporaryPlanes(void);
	static void UpdatePlanes(void);
	static bool TestRocketCollision(CVector *rocketPos);
	static void CreateIncomingCesna(void);
	static void CreateDropOffCesna(void);
	static const CVector FindDrugPlaneCoordinates(void);
	static const CVector FindDropOffCesnaCoordinates(void);
	static bool HasCesnaLanded(void);
	static bool HasCesnaBeenDestroyed(void);
	static bool HasDropOffCesnaBeenShotDown(void);
	static void Load(void);
	static void Save(void);
};

extern float LandingPoint;
extern float TakeOffPoint;
extern float PlanePathPosition[3];
