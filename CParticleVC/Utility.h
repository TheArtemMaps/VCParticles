#pragma once
#include "CHeli.h"
#include "CRGBA.h"
#include <span>
#include "CProjectileInfo.h"
#include "CAEVehicleAudioEntity.h"
#include "eAudioEvents.h"
#include "CAudioEngine.h"
#include "eSurfaceType.h"
#include "CPedModelInfo.h"
#include "CTimer.h"
#include <game_sa/CGeneral.h>
#include "plugin.h"
#include <CStreaming.h>
#include "CClouds.h"
#include "MemoryMgr.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace plugin;
#define PERCENT(x, p)                    ((float(x) * (float(p) / 100.0f)))
#define ARRAY_SIZE(array)                (sizeof(array) / sizeof(array[0]))
#define BIT(num)                         (1 << (num))
#define SQR(x) ((x) * (x))
#define PI (float)M_PI
#define TWOPI (PI*2)
#define FIX_BUGS // Undefine to play with bugs
#define GTAVC_JP_PATCH
//#define GTA_SCENE_EDIT
#define RpAtomicGetFrameMacro(_atomic)                                  \
    ((RwFrame *) rwObjectGetParent(_atomic))
#define RpAtomicGetFrame(_atomic) \
    RpAtomicGetFrameMacro(_atomic)
#define RwCameraGetRasterMacro(_camera)                         \
    ((_camera)->frameBuffer)
#define RwCameraGetRaster(_camera)                              \
    RwCameraGetRasterMacro(_camera)
#define RwRasterGetHeightMacro(_raster) \
    ((_raster)->height)
#define RwRasterGetHeight(_raster)                  \
    RwRasterGetHeightMacro(_raster)
#define RwRasterGetWidthMacro(_raster) \
    ((_raster)->width)
#define RwRasterGetWidth(_raster)                   \
    RwRasterGetWidthMacro(_raster)
#define RwCameraGetFrameMacro(_camera)                          \
    ((RwFrame *)rwObjectGetParent((_camera)))

#define RwCameraGetFrame(_camera)                               \
    RwCameraGetFrameMacro(_camera)
#define RwRasterGetDepthMacro(_raster) ((_raster)->depth)
#define RwRasterGetDepth(_raster) RwRasterGetDepthMacro(_raster)
#define RwRasterGetDepth(_raster)                   \
    RwRasterGetDepthMacro(_raster)
#define RwIm2DGetNearScreenZMacro() (RWSRCGLOBAL(dOpenDevice).zBufferNear)
#define RwIm2DGetNearScreenZ() RwIm2DGetNearScreenZMacro()

#define RwIm2DGetFarScreenZMacro() (RWSRCGLOBAL(dOpenDevice).zBufferFar)
#define RwIm2DGetFarScreenZ() RwIm2DGetFarScreenZMacro()

#define RwCameraGetNearClipPlaneMacro(_camera) ((_camera)->nearPlane)
#define RwCameraGetNearClipPlane(_camera) RwCameraGetNearClipPlaneMacro(_camera)

#define RwCameraSetRasterMacro(_camera, _raster)                \
    (((_camera)->frameBuffer = (_raster)), (_camera))
#define RwCameraSetRasterVoidMacro(_camera, _raster)            \

#define RwCameraSetRaster(_camera, _raster)                     \
    RwCameraSetRasterMacro(_camera, _raster)
#define RwV3dSubMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) - ( (b)->x));                            \
    (o)->y = (((a)->y) - ( (b)->y));                            \
    (o)->z = (((a)->z) - ( (b)->z));                            \
}                                                               \
MACRO_STOP
#define RwV3dAddMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) + ( (b)->x));                            \
    (o)->y = (((a)->y) + ( (b)->y));                            \
    (o)->z = (((a)->z) + ( (b)->z));                            \
}                                                               \
MACRO_STOP
#define RwV3dSub(o, a, b)               RwV3dSubMacro(o, a, b)
#define RwV3dAdd(o, a, b)               RwV3dAddMacro(o, a, b)

#define RpClumpGetFrameMacro(_clump)                                    \
    ((RwFrame *) rwObjectGetParent(_clump))
#define RpClumpGetFrame(_clump) \
    RpClumpGetFrameMacro(_clump)
inline float Pow(float x, float y) { return powf(x, y); }
#define RwCameraGetViewWindow(_camera)                     \
    (&((_camera)->viewWindow))
#define RwCameraGetFarClipPlane(_camera)                   \
    ((_camera)->farPlane)
#define IMPROVED_TECH_PART
#define HALFPX (0.5f)
#define HALFPI (PI/2)
#define SCREEN_ASPECT_RATIO (CDraw::ms_fAspectRatio)
#define DEFAULT_SCREEN_WIDTH  (640)
#define DEFAULT_SCREEN_HEIGHT (448)
#define DEFAULT_ASPECT_RATIO (4.0f/3.0f)
#define SCREEN_STRETCH_X(a)   ((a) * (float) SCREEN_WIDTH / DEFAULT_SCREEN_WIDTH)
#define SCREEN_STRETCH_Y(a)   ((a) * (float) SCREEN_HEIGHT / DEFAULT_SCREEN_HEIGHT)
#define SCREEN_SCALE_AR(a) ((a) * DEFAULT_ASPECT_RATIO / SCREEN_ASPECT_RATIO)
#define SCREEN_SCALE_X(a) SCREEN_SCALE_AR(SCREEN_STRETCH_X(a))
#define SCREEN_SCALE_Y(a) SCREEN_STRETCH_Y(a)
#define SCREEN_SCALE_FROM_RIGHT(a) (SCREEN_WIDTH - SCREEN_SCALE_X(a))
#define SCREEN_SCALE_FROM_BOTTOM(a) (SCREEN_HEIGHT - SCREEN_SCALE_Y(a))
#define SCREEN_STRETCH_FROM_RIGHT(a)  (SCREEN_WIDTH - SCREEN_STRETCH_X(a))
#define SCREEN_STRETCH_FROM_BOTTOM(a) (SCREEN_HEIGHT - SCREEN_STRETCH_Y(a))
#define RwV3dAssign(_target, _source)                     \
    ( *(_target) = *(_source) )
#define DROPLETS_LEFT_OFFSET (10.0f)
#define DROPLETS_RIGHT_OFFSET (10.0f)
#define DROPLETS_TOP_OFFSET (10.0f)
#define DROPLETS_BOTTOM_OFFSET (10.0f)
extern uint8_t work_buff[55000];
static bool IsModelLoaded(int32_t model) { return CStreaming::ms_aInfoForModel[model].m_nLoadState == eStreamingLoadState::LOADSTATE_LOADED; }
enum CarPiece {
    CAR_PIECE_DEFAULT = 0,
    CAR_PIECE_BONNET,
    CAR_PIECE_BOOT,
    CAR_PIECE_BUMP_FRONT,
    CAR_PIECE_BUMP_REAR,
    CAR_PIECE_DOOR_LF,
    CAR_PIECE_DOOR_RF,
    CAR_PIECE_DOOR_LR,
    CAR_PIECE_DOOR_RR,
    CAR_PIECE_WING_LF,
    CAR_PIECE_WING_RF,
    CAR_PIECE_WING_LR,
    CAR_PIECE_WING_RR,
    CAR_PIECE_WHEEL_LF, // front wheel for 2 wheel bike
    CAR_PIECE_WHEEL_RF,
    CAR_PIECE_WHEEL_RL, // rear wheel for 2 wheel bike
    CAR_PIECE_WHEEL_RR,
    CAR_PIECE_WINDSCREEN = 19,
};
static CVector
CrossProduct(const CVector& v1, const CVector& v2)
{
    return CVector(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

static int32_t GetRandomNumber() {
    return rand() & RAND_MAX;
}
static bool GetRandomTrueFalse(void)
{
    return GetRandomNumber() < RAND_MAX / 2;
}
static RwTexture* RwTextureRead(const char* name, const char* mask) {
    return plugin::CallAndReturn<RwTexture*, 0x07F3AC0>(name, mask);
}
static float DotProduct2D(const CVector2D& v1, const CVector2D& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}
inline float
DotProduct(const CVector& v1, const CVector& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float
CrossProduct2D(const CVector2D& v1, const CVector2D& v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

CVector RandomVec1(float min, float max);
const int   re3_buffsize = 1024;
static char re3_buff[re3_buffsize];
inline float Sqrt(float x) { return sqrtf(x); }
inline float RecipSqrt(float x, float y) { return x / Sqrt(y); }
inline float RecipSqrt(float x) { return RecipSqrt(1.0f, x); }
#define DEFAULT_SCREEN_WIDTH  (640)
#define DEFAULT_SCREEN_HEIGHT (448)
inline CVector operator/(const CVector& left, float right)
{
    return CVector(left.x / right, left.y / right, left.z / right);
}
inline CVector2D operator*(float left, const CVector2D& right)
{
    return CVector2D(left * right.x, left * right.y);
}

inline CVector2D operator*(const CVector2D& left, float right)
{
    return CVector2D(left.x * right, left.y * right);
}

inline float
Distance(const CVector& v1, const CVector& v2)
{
    return (v2 - v1).Magnitude();
}

class Matrix : public CMatrix {
public:
    CVector TransformPoint(CVector pt) const {
        return TransformVector(pt) + GetPosition();
    }
    CMatrix Inverted() const;
    /*!
     * @notsa
     * @brief Transform a direction vector - will not take into account translation part of the Matrix.
     * @param pt The vector (direction) to transform
     */
    CVector TransformVector(CVector v) const {
        // Inlined:
        // > m_right.x * v.x + m_forward.x * v.y + m_up.x * v.z,
        // > m_right.y * v.x + m_forward.y * v.y + m_up.y * v.z,
        // > m_right.z * v.x + m_forward.z * v.y + m_up.z * v.z,
        return v.x * GetRight() + v.y * GetForward() + v.z * GetUp();
    }
};
class Vec2D : public CVector2D {
public:
    void Normalise(void) {
        float sq = MagnitudeSqr();
        // assert(sq != 0.0f);	// just be safe here
        float invsqrt = RecipSqrt(sq);
        x *= invsqrt;
        y *= invsqrt;
    }
    float MagnitudeSqr(void) const { return x * x + y * y; }
    inline void operator*=(CVector2D multiplier) {
        x *= multiplier.x;
        y *= multiplier.y;
    }
};
class Vec : public CVector
{
public:
    const bool operator!=(CVector const& right) {
        return x != right.x || y != right.y || z != right.z;
    }
    float MagnitudeSqr(void) const { return x * x + y * y + z * z; }
    void Normalise2D(void) {
        float sq = MagnitudeSqr2D();
        float invsqrt = RecipSqrt(sq);
        x *= invsqrt;
        y *= invsqrt;
    }
    auto Normalized() const->CVector;
    CVector operator-() const {
        return CVector(-x, -y, -z);
    }
    Vec& operator=(const RwV3d& right) {
        x = right.x;
        y = right.y;
        z = right.z;
        return *this;
    }
};

class RGBA : public CRGBA {
public:
    CRGBA operator*(float mult) const {
        return {
            (uint8_t)((float)r * mult),
            (uint8_t)((float)g * mult),
            (uint8_t)((float)b * mult),
            (uint8_t)((float)a * mult)
        };
    }
    RwRGBA ToRwRGBA() const;
};


inline float sq(float x) { return x * x; }

class CEnt : public CEntity {
public:
     void AddSteamsFromGround(CVector* unused);
};

class BoundBox : public CBox {
public:
    bool IsPointWithin(const CVector& point) const;
};

class CollData : public CCollisionData {
public:
    auto GetTris()           const { return std::span{ m_pTriangles, m_nNumTriangles }; }
    [[nodiscard]] auto GetTriVertices(const CColTriangle& tri) const->std::array<CVector, 3>;
};


template<typename T>
T lerp(const T& from, const T& to, float t) {
    // Same as `from + (to - from) * t` (Or `from + t * (to - from)`
    return static_cast<T>(to * t + from * (1.f - t));
}

class Auto : public CAutomobile {
public:
    void dmgDrawCarCollidingParticles(const CVector& position, float force, eWeaponType weapon);
    void ProcessHarvester();
    void StopNitroEffect();
    void DoNitroEffect(float power);
    void ProcessCarOnFireAndExplode(bool bExplodeImmediately);
    int32_t AddWheelDirtAndWater(CColPoint* colpoint, uint32_t belowEffectSpeed);
};

class VehAudio : public CAEVehicleAudioEntity {
public:
    void AddAudioEvent(eAudioEvents audioevent, float volume);
};
template<typename T>
T& StaticRef(uintptr_t addr) {
    return *reinterpret_cast<T*>(addr);
}
// TODO: Replace this with the one above
template<typename T, uintptr_t Addr>
T& StaticRef() {
    return StaticRef<T>(Addr);
}
class Clouds : public CClouds {
public:
    static void MovingFogRender();
};
class FireAudio : public CAEFireAudioEntity {
public:
    void AddAudioEvent(eAudioEvents audioevent, CVector* pos);
};
enum ePedPieces {
    PED_PIECE_UNKNOWN = 0,
    PED_PIECE_TORSO = 3,
    PED_PIECE_ASS = 4,
    PED_PIECE_LEFT_ARM = 5,
    PED_PIECE_RIGHT_ARM = 6,
    PED_PIECE_LEFT_LEG = 7,
    PED_PIECE_RIGHT_LEG = 8,
    PED_PIECE_HEAD = 9
};
class Heli : public CHeli {
public:
    void DoHeliDustEffect(float timeConstMult, float fxMaxZMult);
};

constexpr inline bool operator==(const CVector& vec, float equalTo) {
    return vec.x == equalTo && vec.y == equalTo && vec.z == equalTo;
}
class Veh : public CVehicle {
public:
    void AddExhaustParticles();
    void AddWaterSplashParticles();
    void AddDamagedVehicleParticles();
    void DoBoatSplashes(float fWaterDamping);
    void InflictDamage(CEntity* damager, eWeaponType weapon, float intensity, CVector coords);
    bool BladeColSectorList(CPtrList& ptrList, CColModel& colModel, CMatrix& matrix, int16_t rotorType, float damageMult);
};

class ProjectileInfo : public CProjectileInfo {
public:
    void Update();
};
static bool HasTimePointPassed(uint32_t timeMs) { return CTimer::m_snTimeInMilliseconds >= timeMs; }
static bool IsTimeInRange(uint32_t fromMs, uint32_t toMs) { return HasTimePointPassed(fromMs) && !HasTimePointPassed(toMs); }
static CVector Normalized(CVector v) { v.Normalise(); return v; }

class Obj : public CObject {
public:
    void DoBurnEffect() const;
    void ObjectDamage(float damage, const CVector* fxOrigin, const CVector* fxDirection, CEntity* damager, eWeaponType weaponType);
};
enum
{
    ROTOR_TOP = 3,
    ROTOR_FRONT = 4,
    ROTOR_RIGHT = 5,
    ROTOR_LEFT = 7,
    ROTOR_BACK = 8,
    ROTOR_BOTTOM = 9,
};
enum {
    MODELINFOSIZE = 6700
};
void
AsciiToUnicode(const char* src, char* dst);
class Cam : public CCam {
public:
    void Process_Editor(const CVector&, float, float, float);
};
class Firev : public CFire {
public:
    auto GetFireParticleNameForStrength() const;
    void CreateFxSysForStrength(const CVector& point, RwMatrix* matrix);
};
class AudioEnginee : public CAudioEngine {
public:
    void ReportWaterSplash(CVector posn, float volume);
    void ReportWaterSplash(CPhysical* physical, float volume, bool forcePlaySplashSound);
    void ReportCollision(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, CVector& point, CVector* normal, float fCollisionImpact1, float fCollisionImpact2, bool playOnlyOneShotCollisionSound, bool unknown);
};
class PedModelInfo : public CPedModelInfo {
public:
    CColModel* AnimatePedColModelSkinned(RpClump* clump);
};

class PedAudio : public CAEPedAudioEntity {
public:
    void AddAudioEvent(eAudioEvents event, float volume = 0.0f, float speed = 1.0f, CPhysical* ped = nullptr, uint8_t surfaceId = 0, int32_t a7 = 0, uint32_t maxVol = 0);
};

class Ped : public CPed {
public:
    void PlayFootSteps();
};

class CWeap : public CWeapon {
public:
    void AddGunshell(CPed* creator, CVector& position, const CVector2D& direction, float size);
    bool FireMelee(CEntity* shooter, CVector& fireSource);
    void DoBulletImpact(CEntity* shooter, CEntity* victim,
            CVector* source, CVector* target, CColPoint* point, CVector2D ahead);
    void DoWeaponEffect(CVector origin, CVector dir);
     bool FireSniper(CPed* shooter, CEntity* victim, CVector* target);
  static bool ProcessLineOfSight(const CVector& startPoint, const CVector& endPoint, CColPoint& outColPoint, CEntity*& outEntity, eWeaponType weaponType, CEntity* arg5,
        bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool arg11, bool doIgnoreCameraCheck);
};
inline float DistanceBetweenPointsSquared(const CVector& pointOne, const CVector& pointTwo) {
    return static_cast<Vec>((pointTwo - pointOne)).MagnitudeSqr();
}

static bool IsPointInSphere(const CVector& point, const CVector& center, float r) {
    return DistanceBetweenPointsSquared(point, center) <= sq(r);
}
class Physical : public CPhysical {
public:
    bool ApplyFriction(float adhesiveLimit, CColPoint& colpoint);
};
 inline CVector operator*(const CVector& vecOne, const CVector& vecTwo) {
    return { vecOne.x * vecTwo.x, vecOne.y * vecTwo.y, vecOne.z * vecTwo.z };
}
static ThiscallEvent <AddressList<0x6C4523, H_CALL,
    0x6C4400, H_JUMP>, PRIORITY_BEFORE, ArgPickN<CHeli*, 0>, void(CHeli*)> heliRenderEvent;
void debugshit(const char* text, ...);
void re3_assert(const char* expr, const char* filename, unsigned int lineno, const char* func);
void Log(const char* msg, ...);
void ErrorWindow(const char* msg, ...);
void ClearLogFile();
void CloseLog();
void
SetMotionBlur(int r, int g, int b, int a, int type);
void re3_trace(const char* filename, unsigned int lineno, const char* func, const char* format, ...);
#define TRACE(f, ...) re3_trace(__FILE__, __LINE__, __FUNCTION__, f, ## __VA_ARGS__)
#define assert(_Expression) (void)( (!!(_Expression)) || (re3_assert(#_Expression, __FILE__, __LINE__, __FUNCTION__), 0) )
#define ASSERT assert
#define debug(f, ...) debugshit("[DBG]:" f, ## __VA_ARGS__)
#define error(f, ...) debugshit("[ERROR]:" f, ## __VA_ARGS__)
#define log(f, ...) Log("[LOG]:" f, ## __VA_ARGS__)