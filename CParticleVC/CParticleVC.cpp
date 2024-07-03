// Some of these includes may not be necessary, but, i'm lazy to find & remove unnecessary ones
#include "plugin.h"
#include "CFileMgr.h"
#include "CGeneral.h"
#include "CTimer.h"
#include "CTxdStore.h"
#include "CSprite.h"
#include "CCamera.h"
#include "CClock.h"
#include "CCollision.h"
#include "CWorld.h"
#include "CShadows.h"
#include "CReplay.h"
#include "CStats.h"
#include "CWeather.h"
#include "CGame.h"
#include "CDraw.h"
#include "CScene.h"
#include "CWaterLevel.h"
#include "CClouds.h"
#include "CMessages.h"
#include "CCullZones.h"
#include "CCutsceneMgr.h"
#include "RenderBuffer.h"
#include "CTimeCycle.h"
#include "Utility.h"
#include "debugmenu_public.h"
#include "CFont.h"
#include "CParticleVC.h"
#include "ParticleMgr.h"
#include "CSprite2.h"
#include "CDebug.h"
#include "CModelInfo.h"
#include <eSurfaceType.h>
#include "MemoryMgr.h"
#include "CAudioEngine.h"
#include "ePedBones.h"
#include <CProjectileInfo.h>
#include "MBlurSA.h"
#include "CDarkel.h"
#include "IniReader.h"
#include <filesystem>
#include <game_sa/CStreaming.h>
#include "CFireManager.h"
#include <game_sa/CBoat.h>
#include "CVisibilityPlugins.h"
#include <d3dtypes.h>
#include <CTaskSimpleIKManager.h>
#include <game_sa/CPointLights.h>
#include "CExplosionVC.h"
#include "Audio.h"
#include <game_sa/Fx_c.h>
#include <CPlane.h>
#include "CPedModelInfo.h"
#include <ranges>
#include "bass/bass.h"
#include <game_sa/CCoronas.h>
#include "ParticleObject.h"
#include "WaterCannon.h"
#include "extensions/ScriptCommands.h"
#include "Plane.h"
#include "postfx.h"
#include "CHud.h"
#include <windows.h>
#include <wininet.h>
#include "Audio.h"
#include "CTheScripts.h"
#include "Buoyancy.h"
#include "CAnimManager.h"
#pragma comment(lib, "wininet.lib")
using namespace std;
using namespace plugin;
CAEExplosionAudioEntity& m_ExplosionAudioEntity1 = *(CAEExplosionAudioEntity*)0xC888D0;
DebugMenuAPI gDebugMenuAPI;
int32_t particles = PARTICLE_FIRST;
int32_t particleobjects = POBJECT_PAVEMENT_STEAM;
FxType BlurType = FXTYPE_HEATHAZE;
int32_t explosiontype = EXPLOSION_MOLOTOV;
static FILE* LogFile = 0;
uint64_t maxBytesInLog; // Maximum number of bytes that will be written in the log file
uint64_t numBytesInLog; // Number of bytes currently written into thelog file
uint8_t m_fireParticleCounter;
float m_fBurnTimer;
bool DisableExhaustParticles = false;
bool gbDebugStuffInRelease = false;
bool ParticlesReview = false;
bool ParticleObjectsReview = false;
bool Logging = true;
bool TestExplosions = false;
bool GTAIIIStyleRain = false;
bool GTAIIIVCRainEnable = true;
bool DamagedEngineSmoke = true;
bool FlamethrowerFlame = true;
bool FireWhenAboutToExplode = true;
bool ExhaustSmoke = true;
bool dmgParticles = true;
bool SandStorm = true;
bool Gunshells = true;
//bool FrictionParticles = true; // Obsolete
bool ShipsOnHorizon = true;
bool SeaBirds = true;
bool RainOnRoofParticles = true;
bool WaterSparks = true;
bool Beasties = true;
bool HeliDust = true;
bool WeaponEffects = true;
bool HarvesterParticles = true;
bool IsShotgun = false;
bool BloodnGore = true;
bool MuzzleFlashnSmoke = true;
bool IMFX = false;
bool BulletImpactParticles = true;
bool WheelParticles = true;
bool MissileSmoke = true;
bool ObjectBurnParticles = true;
bool WaterParticles = true;
bool PunchImpactParticles = true;
bool ExplosionsParticles = true;
bool PlaneSmokeParticles = true;
bool WaterCannînParticles = true;
bool FiråExtinguishParticles = true;
bool Snow = false;
bool FootDustParticles = true;
bool FootSplashesParticles = true;
bool RiotExplosions = true;
bool PoliceBoatGunParticles = true;
bool FireOnExhaust = true;
bool TearGasSmoke = true;
bool WoodImpactParticles = true;
bool WaterDrops = true;
bool BloodDrops = true;
bool ExplosionsFlash = true;
bool SteamDuringRain = true, SteamAfterRain = true;
bool DieselVehiclesBlackSmoke = true;
int SnowFlakes = 1;
bool ShouldHaveFireExhaust[1024] = { false };
bool InitializedVehicle[1024] = { false };
bool ParticleExEffects = false, ParticleEXFire = false, ParticleExExplosions = false;
bool PS2 = false, XBOX = false, XBOX2 = false;
// bool NitroParticles = false;
bool VCIIIDamagedVehicleSmokePos = true;
bool GunshellSounds = false;
bool ShatteredGlassParticles = true;
float lim = 0.2f;
float OffsetX[1024] = { 0.2f }, OffsetY[1024] = { 0.0f }, OffsetZ[1024] = { 0.0f };
bool InitializedWeapon[1024] = { false };
enum {
    NUM_RAIN_STREAKS = 35
};
struct tRainStreak
{
    CVector position;
    CVector direction;
    uint32_t timer;
};

class CXboxFireSets
{
public:
    int32_t nCycles;
    float fParticleSizeMul;
    float fRandomMin;
    float fRandomMax;
    float fMovingFlameTime;
    float fVelZMul;
    float fPosZMul;
    float fMovingSpeed;


    CXboxFireSets()
    {
        nCycles = 1;
        fParticleSizeMul = 1.4f;
        fRandomMin = 0.0f;
        fRandomMax = 20.0f;
        fMovingFlameTime = 5.0f;
        fVelZMul = 1.0f;
        fPosZMul = 0.2f;
        fMovingSpeed = 0.003f;
    }
}xbxfire;

RwTexture* GetTexture(const char* path) {
    // Set image path
    RwImageSetPath(path);

    // Load the texture
    RwTexture* texture = RwTextureRead(path, NULL);

    if (!texture) {
        // Texture was not found, return nullptr
        ErrorWindow("Texture %d at path: %s was not found!", texture, path);
        return nullptr;
    }

    return texture;
}

RwTexture* GetTextureFromPath(char* TextureName)
{
    char Path[MAX_PATH];
    sprintf(Path, "%s%s", ".\\Textures\\VCParticle\\", TextureName);
    return GetTexture(Path);
}
struct NewParticle {

};

void Log(const char* msg, ...) {
    // If log file ain't open yet, open it
    if (LogFile == nullptr && Logging) {
        LogFile = fopen(PLUGIN_PATH((char*)"CParticleVC.SA.log"), "a");
        if (LogFile == nullptr) {
            ErrorWindow("Failed to open the log file! Reason: %s.", strerror(errno));
            return;
        }
    }

    if (!Logging) {
        // If logging is disabled, write a farewell message, and close for good...
        if (LogFile != nullptr) {
            SYSTEMTIME systemTime;
            GetLocalTime(&systemTime);
            fprintf(LogFile, "[%02d/%02d/%d %02d:%02d:%02d] [LOG]: Logging is disabled. Closing log file...\n",
                systemTime.wDay, systemTime.wMonth, systemTime.wYear,
                systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
            CloseLog();
        }
        return;
    }


    // Logging is on, writing sum logging stuff
    va_list args;
    va_start(args, msg);
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    fprintf(LogFile, "[%02d/%02d/%d %02d:%02d:%02d]",
        systemTime.wDay, systemTime.wMonth, systemTime.wYear,
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
    numBytesInLog += vfprintf(LogFile, msg, args) + 2;
    // Truncate the log if it's too big
    if (numBytesInLog >= maxBytesInLog) {
        ClearLogFile();
    }
    fprintf(LogFile, "\n");
    //CloseLog();
    fflush(LogFile);
    va_end(args);
}

void CloseLog()
{
    // If the log file is open...
    if (LogFile)
    {
        // ...then close it
        fclose(LogFile);
        LogFile = 0;
    }
}

void ClearLogFile()
{
    // Clears the amount of bytes written...
    auto path = PLUGIN_PATH((char*)"CParticleVC.SA.log");
    numBytesInLog = 0;
    if (LogFile == nullptr)
    {
        // Log file hadn't been open before, open it now.
        LogFile = fopen(path, "w");
        if (!LogFile) {
            ErrorWindow("Failed to open log file for clearing. Reason: %s.", strerror(errno));
        }
    }
    else
    {
        // Reopen the file for truncation
        if ((LogFile = freopen(path, "w", LogFile)) == 0)
        {
            // Wuut, we couldn't do it?
            ErrorWindow("Failed to clear the log file! Reason: %s.", strerror(errno));
            CloseLog();
        }

    }
}
tRainStreak Streaks[NUM_RAIN_STREAKS];
#define STREAK_U (10.0f)
#define STREAK_V (18.0f)
#define LARGE_STREAK_COEFFICIENT (1.23f)
#define STREAK_MIN_DISTANCE (8.0f)
#define STREAK_MAX_DISTANCE (16.0f)

#define SPLASH_CHECK_RADIUS (7.0f)
#define SPLASH_OFFSET_RADIUS (2.0f)

#define STREAK_LIFETIME (4.0f)
#define STREAK_INTEROLATION_TIME (0.3f)

#define RAIN_COLOUR_R (200)
#define RAIN_COLOUR_G (200)
#define RAIN_COLOUR_B (256)
#define RAIN_ALPHA (255)
ThiscallEvent <AddressList<
    0x742548, H_CALL, 0x742495, H_CALL, 0x7424EC, H_CALL>, PRIORITY_AFTER, ArgPick9N < CWeapon*, 0,
    CEntity*, 1,
    CVector*, 2,
    CVector*, 3,
    CEntity*, 4,
    CVector*, 5,
    CVector*, 6,
    char, 7,
    char, 8>,
    void(CWeapon*,
        CEntity*,
        CVector*,
        CVector*,
        CEntity*,
        CVector*,
        CVector*,
        char,
        char)> FireInstantHit;

StdcallEvent <AddressList<
    0x49F567, H_CALL>, PRIORITY_AFTER, ArgPick4N <CVector&, 0,
    CVector&, 1,
    int, 2,
    float, 3>,
    void(CVector&, CVector&, int, float)> AddWoodEvent;

ThiscallEvent <AddressList<
    0x73F3E6, H_CALL>, PRIORITY_AFTER, ArgPick5N <CWeapon*, 0,
    CVector, 1,
    CVector, 2,
    CVehicle*, 3,
    CPed*, 4>,
    void(CWeapon*,
        CVector,
        CVector,
        CVehicle*,
        CPed*)> FireFromCarEvent;

ThiscallEvent <AddressList<
    0x628328, H_CALL>, PRIORITY_BEFORE, ArgPick7N <CWeapon*, 0,
    CPed*, 1,
    CVector*, 2,
    CVector*, 3,
    CEntity*, 4,
    CVector*, 5,
    CVector*, 6>,
    void(CWeapon*,
        CPed*,
        CVector*,
        CVector*,
        CEntity*,
        CVector*,
        CVector*)> DriveByEvent;

CdeclEvent <AddressList<0x4A0D70, H_JUMP>, PRIORITY_AFTER, ArgPickN <RwV3d*, 0>,
    void(RwV3d*)> WaterHydrantEvent;

ThiscallEvent <AddressList<0x7386C5, H_CALL>, PRIORITY_AFTER, ArgPick5N <void*, 0,
    char*, 1, RwV3d*, 2,
    RwMatrix*, 3, int, 4 >,
    void(void*, char*, RwV3d*, RwMatrix*, int)> TearGasSmokeEvent;

ThiscallEvent <AddressList<0x5A1546, H_CALL, 0x5A121B, H_CALL>, PRIORITY_AFTER, ArgPick5N <FxManager_c*, 0,
    FxSystemBP_c* , 1,
    RwV3d* , 2,
    RwMatrixTag* , 3,
    char, 4 >,
    void(FxManager_c* ,
        FxSystemBP_c* ,
        RwV3d* ,
        RwMatrixTag* ,
        char )> ObjectExplodeParticles;

ThiscallEvent <AddressList<0x5A1183, H_CALL>, PRIORITY_AFTER, ArgPick5N <void*, 0, int, 1, RwMatrix*, 2, RwMatrix*, 3, char, 4>,
    void(void* , int , RwMatrix* , RwMatrix* , char )> ObjectExplodeParticles2;

ThiscallEvent <AddressList<
    0x7424CB, H_CALL>, PRIORITY_AFTER, ArgPick4N < CWeapon*, 0,
    CPed*, 1, CEntity*, 2,
    CVector*, 3 >,
    void(CWeapon*, CPed*,
        CEntity*,
        CVector*)> FireSniperEvent;

ThiscallEvent <AddressList<0x6244E5, H_CALL>, PRIORITY_AFTER, ArgPick6N < CTaskSimpleFight*, 0,
    CPed*, 1,
    CPed*, 2,
    CVector*, 3,
    CVector*, 4,
    __int16, 5>,
    void(CTaskSimpleFight*,
        CPed*,
        CPed*,
        CVector*,
        CVector*,
        __int16)> MeleeWeaponEvent;

ThiscallEvent <AddressList<0x738F0C, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> RocketSmokeEvent;

ThiscallEvent <AddressList<0x5E222E, H_CALL>, PRIORITY_AFTER, ArgPick2N <Fx_c*, 0, CVector*, 1>,
    void(Fx_c*, CVector*)> PedWaterSplashEvent;

ThiscallEvent <AddressList<0x68AC07, H_CALL>, PRIORITY_AFTER, ArgPick2N <Fx_c*, 0, CVector*, 1>,
    void(Fx_c*, CVector*)> VehicleWaterSplashEvent;

ThiscallEvent <AddressList<0x53B16E, H_CALL>, PRIORITY_AFTER, ArgPick5N <void*, 0,
    char*, 1, RwV3d*, 2,
    RwMatrix*, 3, int, 4 >,
    void(void*, char*, RwV3d*, RwMatrix*, int)> RiotSmokeEvent;

ThiscallEvent <AddressList<0x4A1139, H_CALL>, PRIORITY_AFTER, ArgPick5N <void*, 0,
    char*, 1, RwV3d*, 2,
    RwMatrix*, 3, int, 4 >,
    void(void*, char*, RwV3d*, RwMatrix*, int)> BulletSplashEvent;

ThiscallEvent <AddressList<0x4AADB5, H_CALL>, PRIORITY_AFTER, ArgPick3N <CAEFireAudioEntity*, 0, int, 1, CVector*, 2 >,
    void(CAEFireAudioEntity*, int, CVector*)> FireAudioEvent;

ThiscallEvent <AddressList<0x5E3943, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> FootSmokeEvent;

CdeclEvent <AddressList<0x497843, H_CALL, 0x6C7773, H_CALL, 0x73E0BA, H_CALL, 0x73E1F7, H_CALL>, PRIORITY_AFTER, ArgPick3N <
    CVector*, 0,
    CVector*,
    1, int, 2>,
    void(CVector*, CVector*, int)> FireOneInstantRoundEvent;

ThiscallEvent <AddressList<0x5E3782, H_CALL, 0x5E37AF, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> FootSplashEvent;

ThiscallEvent <AddressList<0x6C3939, H_CALL, 0x5E74DC, H_CALL, 0x5E7649, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> WaterParticlesSplashEvent;

/*ThiscallEvent <AddressList<0x6C3ABE, H_CALL, 0x68ADE2, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> WaterParticlesWakeEvent;*/

ThiscallEvent <AddressList<0x53963F, H_CALL, 0x53966C, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> FireExtinguishParticles;

ThiscallEvent <AddressList<0x6DD589, H_CALL, 0x6DD6C5, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> BoatSplashEvent;

ThiscallEvent <AddressList<0x729A90, H_CALL, 0x729AC6, H_CALL, 0x7294DB, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> WaterCannonParticles;

ThiscallEvent <AddressList<0x49F646, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> SandBulletImpactEvent;

ThiscallEvent <AddressList<0x6DF303, H_CALL, 0x6DFC03, H_CALL, 0x6DEE83, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> BurnoutEvent;

ThiscallEvent <AddressList<0x6CC0D4, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> PlaneSmokeEvent;

ThiscallEvent <AddressList<0x6CAA8E, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> StuntPlaneSmokeEvent;

ThiscallEvent <AddressList<0x6C942B, H_CALL, 0x6C945E, H_CALL>, PRIORITY_AFTER, ArgPick9N <void*, 0,
    RwV3d*, 1,
    RwV3d*, 2,
    int, 3,
    FxPrtMult_c*, 4,
    float, 5,
    float, 6,
    float, 7,
    int, 8 >,
    void(void*,
        RwV3d*,
        RwV3d*,
        int,
        FxPrtMult_c*,
        float,
        float,
        float,
        int)> ExplodedPlaneSmokeEvent;

StdcallEvent <AddressList<0x59E1DB, H_CALL, 0x61D1A9, H_CALL/*, 0x6DEB5A, H_CALL*/>, PRIORITY_AFTER, ArgPick10N <RwV3d*, 0,
    RwV3d*, 1,
    float, 2,
    signed int, 3,
    float, 4,
    float, 5,
    float, 6,
    int, 7,
    float, 8,
    float, 9 >,
    void(RwV3d*,
        RwV3d*,
        float,
        signed int,
        float,
        float,
        float,
        int,
        float,
        float)> AddSparksEvent;

StdcallEvent <AddressList<0x545953, H_CALL>, PRIORITY_AFTER, ArgPick10N <RwV3d*, 0,
    RwV3d*, 1,
    float, 2,
    signed int, 3,
    float, 4,
    float, 5,
    float, 6,
    int, 7,
    float, 8,
    float, 9 >,
    void(RwV3d*,
        RwV3d*,
        float,
        signed int,
        float,
        float,
        float,
        int,
        float,
        float)> FrictionSparksEvent;

ThiscallEvent <AddressList <0x6C5135, H_CALL>, PRIORITY_AFTER, ArgPick6N<CVehicle*, 0,
    CVector, 1,
    CMatrix*, 2,
    __int16, 3,
    float, 4,
    float, 5>, void(CVehicle*,
        CVector,
        CMatrix*,
        __int16,
        float,
        float)> HeliBladeEvent;
ThiscallEvent <AddressList <0x5E7228, H_CALL>, PRIORITY_AFTER, ArgPick5N<Fx_c*, 0, CVector*, 1, CVector*, 2, int, 3, float, 4>, void(Fx_c*, CVector*, CVector*, int, float)> HeadShotEvent;
ThiscallEvent <AddressList<0x61D068, H_CALL, 0x73BADE, H_CALL, 0x4C08E0, H_CALL, 0x736374, H_CALL>, PRIORITY_AFTER, ArgPick5N<Fx_c*, 0, CVector*, 1, CVector*, 2, int, 3, float, 4>, void(Fx_c*, CVector*, CVector*, int, float)> BloodEvent;
ThiscallEvent <AddressList<0x6DB402, H_CALL>, PRIORITY_AFTER, ArgPick5N<Fx_c*, 0, CVector*, 1, CVector*, 2, int, 3, float, 4>, void(Fx_c*, CVector*, CVector*, int, float)> HeliBloodEvent;
ThiscallEvent <AddressList<0x6DF1A5, H_CALL, 0x6DEBE4, H_CALL>, PRIORITY_AFTER, ArgPick5N<CVehicle*, 0, CColPoint&, 1, uint32_t, 2, uint8_t, 3, uint8_t, 4>, void(CVehicle*, CColPoint&, uint32_t, uint8_t, uint8_t)> WheelDirtAndWaterEvent;
CdeclEvent <AddressList</*0x53E227*/0x726AD0, /*H_JUMP*/H_CALL>, /*PRIORITY_BEFORE*/PRIORITY_AFTER, ArgPickNone, void()> RenderEffectsEvent;
ThiscallEvent <AddressList</*0x53E227*/0x53EB0D, /*H_JUMP*/H_CALL>, /*PRIORITY_BEFORE*/PRIORITY_AFTER, ArgPickN<CCamera*, 0>, void(CCamera*)> IdleEvent;
CdeclEvent <AddressList<0x53E52B, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> RenderFontsEvent;
CdeclEvent <AddressList<0x6CD077, H_CALL, 0x6C6FD3, H_CALL>, PRIORITY_AFTER, ArgPick9N<CEntity*, 0,
    CPed*, 1,
    int, 2,
    CVector2D, 3,
    float, 4,
    int, 5,
    char, 6,
    float, 7,
    char, 8 >, void(CEntity*,
        CPed*,
        int,
        CVector2D,
        float,
        int,
        char,
        float,
        char)> PlanenHeliBlowUpEvent;
StdcallEvent <AddressList<0x736445, H_CALL, 0x73BD62, H_CALL>, PRIORITY_AFTER, ArgPick2N<CVector&, 0, CVector&, 1>, void(CVector&, CVector&)> TyreBurstEvent;
StdcallEvent <AddressList<0x6D2F77, H_CALL, 0x6D2EFA, H_CALL>, PRIORITY_AFTER, ArgPick4N<CVehicle*, 0, CVector, 1, bool, 2, float, 3 >, void(CVehicle*, CVector, bool, float)> WheelSandEvent;
StdcallEvent <AddressList<0x6D2E34, H_CALL>, PRIORITY_AFTER, ArgPick4N<CVehicle*, 0, CVector, 1, bool, 2, float, 3 >, void(CVehicle*, CVector, bool, float)> WheelGravelEvent;
StdcallEvent <AddressList<0x6D2E7D, H_CALL>, PRIORITY_AFTER, ArgPick4N<CVehicle*, 0, CVector, 1, bool, 2, float, 3 >, void(CVehicle*, CVector, bool, float)> WheelMudEvent;
StdcallEvent <AddressList<0x6D2DEE, H_CALL>, PRIORITY_AFTER, ArgPick4N<CVehicle*, 0, CVector, 1, bool, 2, float, 3 >, void(CVehicle*, CVector, bool, float)> WheelGrassEvent;
ThiscallEvent <AddressList<0x61D3DF, H_CALL, 0x61D5DF, H_CALL>, PRIORITY_AFTER, ArgPick4N<Fx_c*, 0, CVector&, 1, CVector&, 2, int32_t, 3>, void(Fx_c*, CVector&, CVector&, int32_t)> PunchImpactEvent;
CdeclEvent <AddressList<0x748E6B, H_CALL>, PRIORITY_BEFORE, ArgPickNone, void()> shutdownGameEvent;
ThiscallEvent <AddressList<0x53EB0D, H_CALL>, PRIORITY_BEFORE, ArgPickN<CCamera*, 0>, void(CCamera* camera)> renderBlurEvent;

#define MAX_PARTICLES_ON_SCREEN   (100000) // 750

//(5)
#define MAX_SMOKE_FILES           ARRAY_SIZE(SmokeFiles)

//(5) 
#define MAX_RUBBER_FILES          ARRAY_SIZE(RubberFiles)
//(5)
#define MAX_RAINSPLASH_FILES      ARRAY_SIZE(RainSplashFiles)
//(3)
#define MAX_WATERSPRAY_FILES      ARRAY_SIZE(WatersprayFiles)
//(6)
#define MAX_EXPLOSIONMEDIUM_FILES ARRAY_SIZE(ExplosionMediumFiles)
//(32)
#define MAX_EXPLOSIONMASSIVE_FILES ARRAY_SIZE(ExplosionMassiveFiles)
//(15)
#define MAX_EXPLOSIONPARTICLEEX_FILES ARRAY_SIZE(ExplosionParticleEXFiles)
//(4)
#define MAX_GUNFLASH_FILES        ARRAY_SIZE(GunFlashFiles)
//(2)
#define MAX_GUNFLASHVCS_FILES     ARRAY_SIZE(GunFlashFilesVCS)
//(2)
#define MAX_RAINSPLASHUP_FILES    ARRAY_SIZE(RainSplashupFiles)
//(4)
#define MAX_BIRDFRONT_FILES       ARRAY_SIZE(BirdfrontFiles)
//(8)
#define MAX_BOAT_FILES            ARRAY_SIZE(BoatFiles)
//(4)
#define MAX_CARDEBRIS_FILES       ARRAY_SIZE(CardebrisFiles)
//(4)
#define MAX_CARSPLASH_FILES       ARRAY_SIZE(CarsplashFiles)

//(46)
#define MAX_FLAME_FILES           ARRAY_SIZE(FlameFiles)

#define MAX_RAINDRIP_FILES       (2)

#define MAX_LEAF_FILES           (2)


const char SmokeFiles[][6 + 1] =
{
    "smoke1",
    "smoke2",
    "smoke3",
    "smoke4",
    "smoke5"
};


const char RubberFiles[][7 + 1] =
{
    "rubber1",
    "rubber2",
    "rubber3",
    "rubber4",
    "rubber5"
};

const char RainSplashFiles[][7 + 1] =
{
    "splash1",
    "splash2",
    "splash3",
    "splash4",
    "splash5"
};

const char WatersprayFiles[][11 + 1] =
{
    "waterspray1",
    "waterspray2",
    "waterspray3"
};

const char ExplosionMediumFiles[][7 + 1] =
{
    "explo01",
    "explo02",
    "explo03",
    "explo04",
    "explo05",
    "explo06"
};

const char ExplosionParticleEXFiles[][10 + 1] =
{
    "explo01_EX",
    "explo02_EX",
    "explo03_EX",
    "explo04_EX",
    "explo05_EX",
    "explo06_EX",
    "explo07_EX",
    "explo08_EX",
    "explo09_EX",
    "explo10_EX",
    "explo11_EX",
    "explo12_EX",
    "explo13_EX",
    "explo14_EX",
    "explo15_EX"
};

const char ExplosionMassiveFiles[][31 + 1] =
{
    "sprite_0_0",
    "sprite_0_1",
    "sprite_0_2",
    "sprite_0_3",
    "sprite_0_4",
    "sprite_0_5",
    "sprite_0_6",
    "sprite_0_7",
    "sprite_0_8",
    "sprite_0_9",
    "sprite_1_0",
    "sprite_1_1",
    "sprite_1_2",
    "sprite_1_3",
    "sprite_1_4",
    "sprite_1_5",
    "sprite_1_6",
    "sprite_1_7",
    "sprite_1_8",
    "sprite_1_9",
    "sprite_2_0",
    "sprite_2_1",
    "sprite_2_2",
    "sprite_2_3",
    "sprite_2_4",
    "sprite_2_5",
    "sprite_2_6",
    "sprite_2_7",
    "sprite_2_8",
    "sprite_2_9",
    "sprite_3_0",
    "sprite_3_1"
};

const char FlameFiles[][8 + 1] =
{
    "flame01",
    "flame02",
    "flame03",
    "flame04",
    "flame05",
    "flame06",
    "flame07",
    "flame08",
    "flame09",
    "flame10",
    "flame11",
    "flame12",
    "flame13",
    "flame14",
    "flame15",
    "flame16",
    "flame17",
    "flame18",
    "flame19",
    "flame20",
    "flame21",
    "flame22",
    "flame23",
    "flame24",
    "flame25",
    "flame26",
    "flame27",
    "flame28",
    "flame29",
    "flame30",
    "flame31",
    "flame32",
    "flame33",
    "flame34",
    "flame35",
    "flame36",
    "flame37",
    "flame38",
    "flame39",
    "flame40",
    "flame41",
    "flame42",
    "flame43",
    "flame44",
    "flame45"
};

const char GunFlashFiles[][9 + 1] =
{
    "gunflash1",
    "gunflash2",
    "gunflash3",
    "gunflash4"
};

const char GunFlashFilesVCS[][15 + 1] =
{
    "muzzleflash",
    "muzzleflashback"
};

const char RainSplashupFiles[][10 + 1] =
{
    "splash_up1",
    "splash_up2"
};

const char BirdfrontFiles[][8 + 1] =
{
    "birdf_01",
    "birdf_02",
    "birdf_03",
    "birdf_04"
};

const char BoatFiles[][8 + 1] =
{
    "boats_01",
    "boats_02",
    "boats_03",
    "boats_04",
    "boats_05",
    "boats_06",
    "boats_07",
    "boats_08"
};

const char CardebrisFiles[][12 + 1] =
{
    "cardebris_01",
    "cardebris_02",
    "cardebris_03",
    "cardebris_04"
};

const char CarsplashFiles[][12 + 1] =
{
    "carsplash_01",
    "carsplash_02",
    "carsplash_03",
    "carsplash_04"
};

CParticle gParticleArray[MAX_PARTICLES_ON_SCREEN];
//CParticle gParticleArrayINI[1024];

RwTexture* gpFlameTex[MAX_FLAME_FILES];
RwRaster* gpFlameRaster[MAX_FLAME_FILES];

RwTexture* gpSmokeTex[MAX_SMOKE_FILES];
RwTexture* gpSmoke2Tex;
RwTexture* gpRubberTex[MAX_RUBBER_FILES];
RwTexture* gpRainSplashTex[MAX_RAINSPLASH_FILES];
RwTexture* gpWatersprayTex[MAX_WATERSPRAY_FILES];
RwTexture* gpExplosionMediumTex[MAX_EXPLOSIONMEDIUM_FILES];
RwTexture* gpExplosionMassiveTex[MAX_EXPLOSIONMASSIVE_FILES];
RwTexture* gpExplosionParticleEXTex[MAX_EXPLOSIONPARTICLEEX_FILES];
RwTexture* gpGunFlashTex[MAX_GUNFLASH_FILES];
RwTexture* gpRainSplashupTex[MAX_RAINSPLASHUP_FILES];
RwTexture* gpBirdfrontTex[MAX_BIRDFRONT_FILES];
RwTexture* gpBoatTex[MAX_BOAT_FILES];
RwTexture* gpCarDebrisTex[MAX_CARDEBRIS_FILES];
RwTexture* gpCarSplashTex[MAX_CARSPLASH_FILES];

RwTexture* gpBoatWakeTex;
RwTexture* gpFlame1Tex;
RwTexture* gpFlame5Tex;
RwTexture* gpRainDropSmallTex;
RwTexture* gpBloodTex;
RwTexture* gpLeafTex[MAX_LEAF_FILES];
RwTexture* gpCloudTex1;
RwTexture* gpCloudTex4;
RwTexture* gpBloodSmallTex;
RwTexture* gpGungeTex;
RwTexture* gpCollisionSmokeTex;
RwTexture* gpBulletHitTex;
RwTexture* gpGunShellTex;
RwTexture* gpPointlightTex;
RwTexture* gpSnowTex;
RwTexture* coronaringa;

RwRaster* gpSmokeRaster[MAX_SMOKE_FILES];
RwRaster* gpSmoke2Raster;
RwRaster* gpRubberRaster[MAX_RUBBER_FILES];
RwRaster* gpRainSplashRaster[MAX_RAINSPLASH_FILES];
RwRaster* gpWatersprayRaster[MAX_WATERSPRAY_FILES];
RwRaster* gpExplosionMediumRaster[MAX_EXPLOSIONMEDIUM_FILES];
RwRaster* gpExplosionMassiveRaster[MAX_EXPLOSIONMASSIVE_FILES];
RwRaster* gpExplosionParticleEXRaster[MAX_EXPLOSIONPARTICLEEX_FILES];
RwRaster* gpGunFlashRaster[MAX_GUNFLASH_FILES];
RwRaster* gpRainSplashupRaster[MAX_RAINSPLASHUP_FILES];
RwRaster* gpBirdfrontRaster[MAX_BIRDFRONT_FILES];
RwRaster* gpBoatRaster[MAX_BOAT_FILES];
RwRaster* gpCarDebrisRaster[MAX_CARDEBRIS_FILES];
RwRaster* gpCarSplashRaster[MAX_CARSPLASH_FILES];

RwRaster* gpBoatWakeRaster;
RwRaster* gpFlame1Raster;
RwRaster* gpFlame5Raster;
RwRaster* gpRainDropSmallRaster;
RwRaster* gpBloodRaster;
RwRaster* gpLeafRaster[MAX_LEAF_FILES];
RwRaster* gpCloudRaster1;
RwRaster* gpCloudRaster4;
RwRaster* gpBloodSmallRaster;
RwRaster* gpGungeRaster;
RwRaster* gpCollisionSmokeRaster;
RwRaster* gpSnowRaster;
RwRaster* gpBulletHitRaster;
RwRaster* gpGunShellRaster;
RwRaster* gpPointlightRaster;

RwTexture* gpRainDropTex;
RwRaster* gpRainDropRaster;

RwRaster* gpWakeOldRaster;
RwTexture* gpWakeOldTex;

RwTexture* gpMultiPlayerHitTex;
RwRaster* gpMultiPlayerHitRaster;

RwTexture* gpGunFlashTexVCS[MAX_GUNFLASHVCS_FILES];

RwRaster* gpGunFlashRasterVCS[MAX_GUNFLASHVCS_FILES];

RwTexture* gpSparkTex;
RwTexture* gpNewspaperTex;
RwTexture* gpGunSmokeTex;
RwTexture* gpDotTex;
RwTexture* gpHeatHazeTex;
RwTexture* gpBeastieTex;
RwTexture* gpRainDripTex[MAX_RAINDRIP_FILES];
RwTexture* gpRainDripDarkTex[MAX_RAINDRIP_FILES];

RwRaster* gpSparkRaster;
RwRaster* gpNewspaperRaster;
RwRaster* gpGunSmokeRaster;
RwRaster* gpDotRaster;
RwRaster* gpHeatHazeRaster;
RwRaster* gpBeastieRaster;
RwRaster* gpRainDripRaster[MAX_RAINDRIP_FILES];
RwRaster* gpRainDripDarkRaster[MAX_RAINDRIP_FILES];

float      CParticle::ms_afRandTable[RAND_TABLE_SIZE];
CParticle* CParticle::m_pUnusedListHead;
float      CParticle::m_SinTable[SIN_COS_TABLE_SIZE];
float      CParticle::m_CosTable[SIN_COS_TABLE_SIZE];

int32_t Randomizer;
int32_t nParticleCreationInterval = 1;
int32_t nParticleCreationIntervalnew;
float PARTICLE_WIND_TEST_SCALE = 0.002f;
float fParticleScaleLimit = 0.5f;
float fParticleScaleLimitnew;
float PARTICLE_WIND_TEST_SCALE_NEW;
bool clearWaterDrop;
int32_t numWaterDropOnScreen;

const char* aParticle[] =
{
    "PC",
    "PS2",
    "XBOX",
    "XBOX+III",
};

void CParticle::ReloadConfig()
{
    log("Initialising CParticleMgr...");
    debug("Initialising CParticleMgr...");

    mod_ParticleSystemManager.Initialise();

    log("Initialising CParticle...");
    debug("Initialising CParticle...");
    log("Reloading ini file...");
    debug("Reloading ini file...");
    // You can hot-reload these through debug menu
    CIniReader ini(PLUGIN_PATH((char*)"CParticleVC.ini"));
    for (int32_t i = 0; i < 1024; i++) {
        auto string1 = std::format("{}{}", "VEH", i);
        auto string2 = string1.c_str();
        bool Should = ini.ReadBoolean(string2, "Should have fire on exhaust", false);
        if (Should != false) { // Check if the section exists by checking a default value
            InitializedVehicle[i] = true;
            ShouldHaveFireExhaust[i] = Should;
            log("Section1 (Vehicles): %s", string1);
        }
    }
    gbDebugStuffInRelease = ini.ReadBoolean("MAIN", "Enable debug text", false);
    maxBytesInLog = ini.ReadInteger("MAIN", "MaxLogSize", 5242880);
    Logging = ini.ReadBoolean("MAIN", "Enable logging", false);
    GTAIIIStyleRain = ini.ReadBoolean("VISUAL", "GTA III rain behaviour", false);
    GTAIIIVCRainEnable = ini.ReadBoolean("VISUAL", "GTA III/VC style rain", true);
    DamagedEngineSmoke = ini.ReadBoolean("VISUAL", "Damaged engine smoke", true);
    //FlamethrowerFlame = ini.ReadBoolean("VISUAL", "Flamethrower flame", true);
    FireWhenAboutToExplode = ini.ReadBoolean("VISUAL", "Fire when about to explode", true);
    Gunshells = ini.ReadBoolean("VISUAL", "Gunshells", true);
    SandStorm = ini.ReadBoolean("VISUAL", "Sandstorm", true);
    dmgParticles = ini.ReadBoolean("VISUAL", "Damage particles", true);
    ExhaustSmoke = ini.ReadBoolean("VISUAL", "Exhaust smoke", true);
    // FrictionParticles = ini.ReadBoolean("VISUAL", "Friction particles", true); // Obsolete
    RainOnRoofParticles = ini.ReadBoolean("VISUAL", "Rain on roof particles", true);
    ShipsOnHorizon = ini.ReadBoolean("VISUAL", "Ships on horizon", true);
    SeaBirds = ini.ReadBoolean("VISUAL", "Sea birds", true);
    WaterSparks = ini.ReadBoolean("VISUAL", "Water sparks", true);
    Beasties = ini.ReadBoolean("VISUAL", "Beasties", true);
    HeliDust = ini.ReadBoolean("VISUAL", "Heli dust", true);
    WeaponEffects = ini.ReadBoolean("VISUAL", "Weapon effects", true);
    HarvesterParticles = ini.ReadBoolean("VISUAL", "Harvester particles", true);
    BloodnGore = ini.ReadBoolean("VISUAL", "Blood & gore", true);
    MuzzleFlashnSmoke = ini.ReadBoolean("VISUAL", "Muzzle flash & smoke", true);
    BulletImpactParticles = ini.ReadBoolean("VISUAL", "Bullet impact particles", true);
    WheelParticles = ini.ReadBoolean("VISUAL", "Wheel particles", true);
    ObjectBurnParticles = ini.ReadBoolean("VISUAL", "Object burn particles", true);
    WaterParticles = ini.ReadBoolean("VISUAL", "Water particles", true);
    MissileSmoke = ini.ReadBoolean("VISUAL", "Missiles smoke", true);
    PunchImpactParticles = ini.ReadBoolean("VISUAL", "Punch impact particles", true);
    ExplosionsParticles = ini.ReadBoolean("VISUAL", "Explosions particles", true);
    PlaneSmokeParticles = ini.ReadBoolean("VISUAL", "Plane smoke particles", true);
    FiråExtinguishParticles = ini.ReadBoolean("VISUAL", "Fire extinguish particles", true);
    WaterCannînParticles = ini.ReadBoolean("VISUAL", "Water cannon particles", true);
    Snow = ini.ReadBoolean("VISUAL", "Snow", false);
    FootDustParticles = ini.ReadBoolean("VISUAL", "Foot dust particles", true);
    FootSplashesParticles = ini.ReadBoolean("VISUAL", "Foot splashes particles", true);
    RiotExplosions = ini.ReadBoolean("VISUAL", "Riot explosions", false);
    PoliceBoatGunParticles = ini.ReadBoolean("VISUAL", "Police boat gun particles", true);
    FireOnExhaust = ini.ReadBoolean("VISUAL", "Fire on exhaust", true);
    TearGasSmoke = ini.ReadBoolean("VISUAL", "Tear gas smoke particles", true);
    WoodImpactParticles = ini.ReadBoolean("VISUAL", "Wood impact particles", true);
    WaterDrops = ini.ReadBoolean("VISUAL", "Water drops", true);
    BloodDrops = ini.ReadBoolean("VISUAL", "Blood drops", true);
    ExplosionsFlash = ini.ReadBoolean("VISUAL", "Explosions flash", true);
    SteamDuringRain = ini.ReadBoolean("VISUAL", "Steam during rain", true);
    SteamAfterRain = ini.ReadBoolean("VISUAL", "Steam after rain", true);
    DieselVehiclesBlackSmoke = ini.ReadBoolean("VISUAL", "Diesel vehicles black exhaust smoke", true);
    ParticleExEffects = ini.ReadBoolean("ParticleEX", "Use ParticleEx particles", false);
    ParticleEXFire = ini.ReadBoolean("ParticleEX", "Use ParticleEx fire particles", false);
    ParticleExExplosions = ini.ReadBoolean("ParticleEX", "Use ParticleEx explosion particles", false);
   // NitroParticles = ini.ReadBoolean("VISUAL", "Nitro particles", true);
    VCIIIDamagedVehicleSmokePos = ini.ReadBoolean("VISUAL", "Use headlights pos for damaged engine smoke", true);
    ShatteredGlassParticles = ini.ReadBoolean("VISUAL", "Replace shattered glass particles", true);
    GunshellSounds = ini.ReadBoolean("MISC", "Play gunshell sounds", false);
    nParticleCreationInterval = ini.ReadInteger("MISC", "Particles creation interval", 1);
    PARTICLE_WIND_TEST_SCALE = ini.ReadFloat("MISC", "PARTICLE_WIND_TEST_SCALE", 0.002f);
    fParticleScaleLimit = ini.ReadFloat("MISC", "Particles scale limit", 0.5f);
    SnowFlakes = ini.ReadInteger("MISC", "Max snow flakes", 1);
    lim = ini.ReadFloat("MISC", "Min distance to camera", 0.2f);
    for (int32_t i = 0; i < 1024; ++i) {
        auto string1 = std::format("{}{}", "WEAP", i);
        auto string2 = string1.c_str();
        float x = ini.ReadFloat(string2, "Second gunflash offset x", -1);
        if (x != -1) {
            InitializedWeapon[i] = true;
            OffsetX[i] = x;
            OffsetY[i] = ini.ReadFloat(string2, "Second gunflash offset y", 0.0f);
            OffsetZ[i] = ini.ReadFloat(string2, "Second gunflash offset z", 0.0f);
            log("Section1 (Weapons): %s", string1);
        }
    }

   /* XBOX = ini.ReadBoolean("ParticleEX", "XBOX", false);
    XBOX2 = ini.ReadBoolean("ParticleEX", "XBOX2", false);
    PS2 = ini.ReadBoolean("ParticleEX", "PS2", false);*/
    /*for (int32_t i = 0; i < MAX_PARTICLES_INI; i++)
    {
        auto ParticleSystem = &mod_ParticleSystemManager.m_aParticles[MAX_PARTICLES_INI];
        std::string name = "RUBBISH";
        std::string formatted_str = std::format(
            "{}{}", name,
            i);

        const char* formatted_str2 = formatted_str.c_str();
        std::string strb = ini.ReadString(formatted_str2, "Texture", nullptr);
        const char* strb2 = strb.c_str();
        ParticleSystem->m_ppRaster = (RwRaster**)strb2;
        if (strb.empty()) {
            //RubbishTotalTexturesCount2 *= 2;
            break;
        }
    }*/
    log("Ini file reloaded!");
    debug("Ini file reloaded!");
    m_pUnusedListHead = gParticleArray;
    for (int32_t i = 0; i < MAX_PARTICLES_ON_SCREEN; i++)
    {
        if (i == MAX_PARTICLES_ON_SCREEN - 1)
            gParticleArray[i].m_pNext = nullptr;
        else
            gParticleArray[i].m_pNext = &gParticleArray[i + 1];

        gParticleArray[i].m_vecPosition = CVector(0.0f, 0.0f, 0.0f);

        gParticleArray[i].m_vecVelocity = CVector(0.0f, 0.0f, 0.0f);

        gParticleArray[i].m_nTimeWhenWillBeDestroyed = 0;

        gParticleArray[i].m_nTimeWhenColorWillBeChanged = 0;

        gParticleArray[i].m_fSize = 0.2f;

        gParticleArray[i].m_fExpansionRate = 0.0f;

        gParticleArray[i].m_nColorIntensity = 255;

        gParticleArray[i].m_nFadeToBlackTimer = 0;

        gParticleArray[i].m_nAlpha = 255;

        gParticleArray[i].m_nFadeAlphaTimer = 0;

        gParticleArray[i].m_nCurrentZRotation = 0;

        gParticleArray[i].m_nZRotationTimer = 0;

        gParticleArray[i].m_fCurrentZRadius = 0.0f;

        gParticleArray[i].m_nZRadiusTimer = 0;

        gParticleArray[i].m_nCurrentFrame = 0;

        gParticleArray[i].m_nAnimationSpeedTimer = 0;

        gParticleArray[i].m_nRotation = 0;

        gParticleArray[i].m_nRotationStep = 0;
    }
}

void CParticle::Initialise()
{
    ReloadConfig();

    CParticleObject::Initialise();

    if (!std::filesystem::exists(PLUGIN_PATH((char*)"CParticleVC.ini"))) {
        ErrorWindow("Configuration file 'CParticleVC.ini' was not found at %s. Exiting game...", PLUGIN_PATH((char*)""));
    }
    m_fBurnTimer = 0.0f;
    m_fireParticleCounter = 0;
    float randVal = -1.0f;
    for (int32_t i = 0; i < RAND_TABLE_SIZE; i++)
    {
        ms_afRandTable[i] = randVal;
        randVal += 0.1f;
    }

    for (int32_t i = 0; i < SIN_COS_TABLE_SIZE; i++)
    {
        float angle = DEGTORAD(float(i) * float(360.0f / SIN_COS_TABLE_SIZE));

        m_SinTable[i] = sinf(angle);
        m_CosTable[i] = cosf(angle);
    }
    if (!std::filesystem::exists(PLUGIN_PATH((char*)"MODELS\\PARTICLEVC.TXD"))) {
        ErrorWindow("TXD file 'particleVC.txd' was not found at %s. Exiting game...", PLUGIN_PATH((char*)"MODELS"));
    }
    CTxdStore::PushCurrentTxd();
    int32_t slot2 = CTxdStore::AddTxdSlot("particleVC");
    CTxdStore::LoadTxd(slot2, PLUGIN_PATH((char*)"MODELS\\PARTICLEVC.TXD"));
    CTxdStore::AddRef(slot2);
    int32_t slot = CTxdStore::FindTxdSlot("particleVC");
    CTxdStore::SetCurrentTxd(slot);

    for (int32_t i = 0; i < MAX_SMOKE_FILES; i++)
    {
        gpSmokeTex[i] = RwTextureRead(SmokeFiles[i], nullptr);
        assert(gpSmokeTex[i] != NULL);
        gpSmokeRaster[i] = RwTextureGetRaster(gpSmokeTex[i]);
    }

    gpSmoke2Tex = RwTextureRead("smokeII_3", nullptr);
    assert(gpSmoke2Tex != NULL);
    gpSmoke2Raster = RwTextureGetRaster(gpSmoke2Tex);

    for (int32_t i = 0; i < MAX_RUBBER_FILES; i++)
    {
        gpRubberTex[i] = RwTextureRead(RubberFiles[i], nullptr);
        assert(gpRubberTex[i] != NULL);
        gpRubberRaster[i] = RwTextureGetRaster(gpRubberTex[i]);
    }

    for (int32_t i = 0; i < MAX_RAINSPLASH_FILES; i++)
    {
        gpRainSplashTex[i] = RwTextureRead(RainSplashFiles[i], nullptr);
        assert(gpRainSplashTex[i] != NULL);
        gpRainSplashRaster[i] = RwTextureGetRaster(gpRainSplashTex[i]);
    }

    for (int32_t i = 0; i < MAX_WATERSPRAY_FILES; i++)
    {
        gpWatersprayTex[i] = RwTextureRead(WatersprayFiles[i], nullptr);
        assert(gpWatersprayTex[i] != NULL);
        gpWatersprayRaster[i] = RwTextureGetRaster(gpWatersprayTex[i]);
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONMEDIUM_FILES; i++)
    {
        gpExplosionMediumTex[i] = RwTextureRead(ExplosionMediumFiles[i], nullptr);
        assert(gpExplosionMediumTex[i] != NULL);
        gpExplosionMediumRaster[i] = RwTextureGetRaster(gpExplosionMediumTex[i]);
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONMASSIVE_FILES; i++)
    {
        gpExplosionMassiveTex[i] = RwTextureRead(ExplosionMassiveFiles[i], nullptr);
        assert(gpExplosionMassiveTex[i] != NULL);
        gpExplosionMassiveRaster[i] = RwTextureGetRaster(gpExplosionMassiveTex[i]);
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONPARTICLEEX_FILES; i++)
    {
        gpExplosionParticleEXTex[i] = RwTextureRead(ExplosionParticleEXFiles[i], nullptr);
        assert(gpExplosionParticleEXTex[i] != NULL);
        gpExplosionParticleEXRaster[i] = RwTextureGetRaster(gpExplosionParticleEXTex[i]);
    }

    for (int32_t i = 0; i < MAX_GUNFLASH_FILES; i++)
    {
        gpGunFlashTex[i] = RwTextureRead(GunFlashFiles[i], nullptr);
        assert(gpGunFlashTex[i] != NULL);
        gpGunFlashRaster[i] = RwTextureGetRaster(gpGunFlashTex[i]);
    }

    for (int32_t i = 0; i < MAX_GUNFLASHVCS_FILES; i++)
    {
        gpGunFlashTexVCS[i] = RwTextureRead(GunFlashFilesVCS[i], nullptr);
        assert(gpGunFlashTexVCS[i] != NULL);
        gpGunFlashRasterVCS[i] = RwTextureGetRaster(gpGunFlashTexVCS[i]);
    }

    gpRainDropTex = RwTextureRead("raindrop4", nullptr);
    assert(gpRainDropTex != NULL);
    gpRainDropRaster = RwTextureGetRaster(gpRainDropTex);

    // Snow
    gpSnowTex = RwTextureRead("snowflake", nullptr);
    assert(gpSnowTex != NULL);
    gpSnowRaster = RwTextureGetRaster(gpSnowTex);

    for (int32_t i = 0; i < MAX_RAINSPLASHUP_FILES; i++)
    {
        gpRainSplashupTex[i] = RwTextureRead(RainSplashupFiles[i], nullptr);
        assert(gpRainSplashupTex[i] != NULL);
        gpRainSplashupRaster[i] = RwTextureGetRaster(gpRainSplashupTex[i]);
    }

    for (int32_t i = 0; i < MAX_BIRDFRONT_FILES; i++)
    {
        gpBirdfrontTex[i] = RwTextureRead(BirdfrontFiles[i], nullptr);
        assert(gpBirdfrontTex[i] != NULL);
        gpBirdfrontRaster[i] = RwTextureGetRaster(gpBirdfrontTex[i]);
    }

    for (int32_t i = 0; i < MAX_BOAT_FILES; i++)
    {
        gpBoatTex[i] = RwTextureRead(BoatFiles[i], nullptr);
        assert(gpBoatTex[i] != NULL);
        gpBoatRaster[i] = RwTextureGetRaster(gpBoatTex[i]);
    }

    for (int32_t i = 0; i < MAX_CARDEBRIS_FILES; i++)
    {
        gpCarDebrisTex[i] = RwTextureRead(CardebrisFiles[i], nullptr);
        assert(gpCarDebrisTex[i] != NULL);
        gpCarDebrisRaster[i] = RwTextureGetRaster(gpCarDebrisTex[i]);
    }

    for (int32_t i = 0; i < MAX_CARSPLASH_FILES; i++)
    {
        gpCarSplashTex[i] = RwTextureRead(CarsplashFiles[i], nullptr);
        assert(gpCarSplashTex[i] != NULL);
        gpCarSplashRaster[i] = RwTextureGetRaster(gpCarSplashTex[i]);
    }

    gpBoatWakeTex = RwTextureRead("boatwake2", nullptr);
    assert(gpBoatWakeTex != NULL);
    gpBoatWakeRaster = RwTextureGetRaster(gpBoatWakeTex);

    gpFlame1Tex = RwTextureRead("flame1", nullptr);
    assert(gpFlame1Tex != NULL);
    gpFlame1Raster = RwTextureGetRaster(gpFlame1Tex);

    gpFlame5Tex = RwTextureRead("flame5", nullptr);
    assert(gpFlame5Tex != NULL);

    for (int32_t i = 0; i < MAX_FLAME_FILES; i++)
    {
        gpFlameTex[i] = RwTextureRead(FlameFiles[i], NULL);

        if (gpFlameTex[i] != NULL)
            gpFlameRaster[i] = RwTextureGetRaster(gpFlameTex[i]);
    }

    //#ifdef FIX_BUGS
#if 0
    gpFlame5Raster = RwTextureGetRaster(gpFlame5Tex);
#else
// this seems to have become more of a design choice
    gpFlame5Raster = RwTextureGetRaster(gpFlame1Tex); // copy-paste bug ?
#endif

    gpRainDropSmallTex = RwTextureRead("rainsmall", nullptr);
    assert(gpRainDropSmallTex != NULL);
    gpRainDropSmallRaster = RwTextureGetRaster(gpRainDropSmallTex);

    gpBloodTex = RwTextureRead("blood", nullptr);
    assert(gpBloodTex != NULL);
    gpBloodRaster = RwTextureGetRaster(gpBloodTex);

    gpLeafTex[0] = RwTextureRead("gameleaf01_64", nullptr);
    assert(gpLeafTex[0] != NULL);
    gpLeafRaster[0] = RwTextureGetRaster(gpLeafTex[0]);

    gpLeafTex[1] = RwTextureRead("letter", nullptr);
    assert(gpLeafTex[1] != NULL);
    gpLeafRaster[1] = RwTextureGetRaster(gpLeafTex[1]);

    gpCloudTex1 = RwTextureRead("cloud3", nullptr);
    assert(gpCloudTex1 != NULL);
    gpCloudRaster1 = RwTextureGetRaster(gpCloudTex1);

    gpCloudTex4 = RwTextureRead("cloudmasked", nullptr);
    assert(gpCloudTex4 != NULL);
    gpCloudRaster4 = RwTextureGetRaster(gpCloudTex4);

    gpBloodSmallTex = RwTextureRead("bloodsplat2", nullptr);
    assert(gpBloodSmallTex != NULL);
    gpBloodSmallRaster = RwTextureGetRaster(gpBloodSmallTex);

    gpGungeTex = RwTextureRead("gunge", nullptr);
    assert(gpGungeTex != NULL);
    gpGungeRaster = RwTextureGetRaster(gpGungeTex);

    gpCollisionSmokeTex = RwTextureRead("collisionsmoke", nullptr);
    assert(gpCollisionSmokeTex != NULL);
    gpCollisionSmokeRaster = RwTextureGetRaster(gpCollisionSmokeTex);

    gpBulletHitTex = RwTextureRead("bullethitsmoke", nullptr);
    assert(gpBulletHitTex != NULL);
    gpBulletHitRaster = RwTextureGetRaster(gpBulletHitTex);

    gpGunShellTex = RwTextureRead("gunshell", nullptr);
    assert(gpGunShellTex != NULL);
    gpGunShellRaster = RwTextureGetRaster(gpGunShellTex);

    gpPointlightTex = RwTextureRead("pointlight", nullptr);
    assert(gpPointlightTex != NULL);
    gpPointlightRaster = RwTextureGetRaster(gpPointlightTex);

    gpSparkTex = RwTextureRead("spark", nullptr);
    assert(gpSparkTex != NULL);
    gpSparkRaster = RwTextureGetRaster(gpSparkTex);

    gpNewspaperTex = RwTextureRead("newspaper02_64", nullptr);
    assert(gpNewspaperTex != NULL);
    gpNewspaperRaster = RwTextureGetRaster(gpNewspaperTex);

    gpGunSmokeTex = RwTextureRead("gunsmoke3", nullptr);
    assert(gpGunSmokeTex != NULL);
    gpGunSmokeRaster = RwTextureGetRaster(gpGunSmokeTex);

    gpDotTex = RwTextureRead("dot", nullptr);
    assert(gpDotTex != NULL);
    gpDotRaster = RwTextureGetRaster(gpDotTex);

    gpHeatHazeTex = RwTextureRead("heathaze", nullptr);
    assert(gpHeatHazeTex != NULL);
    gpHeatHazeRaster = RwTextureGetRaster(gpHeatHazeTex);

    gpBeastieTex = RwTextureRead("beastie", nullptr);
    assert(gpBeastieTex != NULL);
    gpBeastieRaster = RwTextureGetRaster(gpBeastieTex);

    gpRainDripTex[0] = RwTextureRead("raindrip64", nullptr);
    assert(gpRainDripTex[0] != NULL);
    gpRainDripRaster[0] = RwTextureGetRaster(gpRainDripTex[0]);

    gpRainDripTex[1] = RwTextureRead("raindripb64", nullptr);
    assert(gpRainDripTex[1] != NULL);
    gpRainDripRaster[1] = RwTextureGetRaster(gpRainDripTex[1]);

    gpRainDripDarkTex[0] = RwTextureRead("raindrip64_d", nullptr);
    assert(gpRainDripDarkTex[0] != NULL);
    gpRainDripDarkRaster[0] = RwTextureGetRaster(gpRainDripDarkTex[0]);

    gpRainDripDarkTex[1] = RwTextureRead("raindripb64_d", nullptr);
    assert(gpRainDripDarkTex[1] != NULL);
    gpRainDripDarkRaster[1] = RwTextureGetRaster(gpRainDripDarkTex[1]);

    gpWakeOldTex = RwTextureRead("wake_old", nullptr);
    assert(gpWakeOldTex != NULL);
    gpWakeOldRaster = RwTextureGetRaster(gpWakeOldTex);

    gpMultiPlayerHitTex = RwTextureRead("mphit", nullptr);
    assert(gpMultiPlayerHitTex != NULL);
    gpMultiPlayerHitRaster = RwTextureGetRaster(gpMultiPlayerHitTex);

    // CExplosionVC - for the shock wave
    coronaringa = RwTextureRead("coronaringa", nullptr);
    assert(coronaringa != NULL);

    for (int32_t i = 0; i < MAX_PARTICLES; i++)
    {
        tParticleSystemData* entry = &mod_ParticleSystemManager.m_aParticles[i];

        switch (i)
        {
        case PARTICLE_SPARK:
        case PARTICLE_SPARK_SMALL:
        case PARTICLE_RAINDROP_SMALL:
        case PARTICLE_HELI_ATTACK:
            entry->m_ppRaster = &gpRainDropSmallRaster;
            break;

        case PARTICLE_WATER_SPARK:
            entry->m_ppRaster = &gpSparkRaster;
            break;

        case PARTICLE_WHEEL_DIRT:
        case PARTICLE_SAND:
        case PARTICLE_STEAM2:
        case PARTICLE_STEAM_NY:
        case PARTICLE_STEAM_NY_SLOWMOTION:
        case PARTICLE_GROUND_STEAM:
        case PARTICLE_ENGINE_STEAM:
        case PARTICLE_PEDFOOT_DUST:
        case PARTICLE_CAR_DUST:
        case PARTICLE_EXHAUST_FUMES:
        case PARTICLE_HYDRANT_STEAM:
            entry->m_ppRaster = &gpSmoke2Raster;
            break;

        case PARTICLE_WHEEL_WATER:
        case PARTICLE_WATER:
        case PARTICLE_SMOKE:
        case PARTICLE_SMOKE_SLOWMOTION:
        case PARTICLE_DRY_ICE:
        case PARTICLE_GARAGEPAINT_SPRAY:
        case PARTICLE_STEAM:
        case PARTICLE_WATER_CANNON:
        case PARTICLE_EXTINGUISH_STEAM:
        case PARTICLE_HELI_DUST:
        case PARTICLE_PAINT_SMOKE:
        case PARTICLE_BULLETHIT_SMOKE:
            entry->m_ppRaster = gpSmokeRaster;
            break;

        case PARTICLE_BLOOD:
            entry->m_ppRaster = &gpBloodRaster;
            break;

        case PARTICLE_BLOOD_SMALL:
        case PARTICLE_BLOOD_SPURT:
            entry->m_ppRaster = &gpBloodSmallRaster;
            break;

        case PARTICLE_DEBRIS:
        case PARTICLE_TREE_LEAVES:
            entry->m_ppRaster = gpLeafRaster;
            break;

        case PARTICLE_DEBRIS2:
            entry->m_ppRaster = &gpGungeRaster;
            break;

        case PARTICLE_FLYERS:
            entry->m_ppRaster = &gpNewspaperRaster;
            break;

        case PARTICLE_FLAME:
        case PARTICLE_CARFLAME:
                entry->m_ppRaster = &gpFlame1Raster;
            break;

        case PARTICLE_CARFLAME_MOVING:
            entry->m_ppRaster = gpFlameRaster;
            break;

        case PARTICLE_FIREBALL:
                entry->m_ppRaster = &gpFlame5Raster;
            break;

        case PARTICLE_GUNFLASH:
        case PARTICLE_GUNFLASH_NOANIM:
            entry->m_ppRaster = gpGunFlashRaster;
            break;

        case PARTICLE_GUNFLASH_VCS:
        case PARTICLE_GUNFLASH_NOANIM_VCS:
            entry->m_ppRaster = gpGunFlashRasterVCS;
            break;

        case PARTICLE_GUNSMOKE:
        case PARTICLE_WATERDROP:
        case PARTICLE_BLOODDROP:
        case PARTICLE_HEATHAZE:
        case PARTICLE_HEATHAZE_IN_DIST:
            entry->m_ppRaster = nullptr;
            break;

        case PARTICLE_GUNSMOKE2:
        case PARTICLE_BOAT_THRUSTJET:
        case PARTICLE_RUBBER_SMOKE:
            entry->m_ppRaster = gpRubberRaster;
            break;

        case PARTICLE_CIGARETTE_SMOKE:
            entry->m_ppRaster = &gpGunSmokeRaster;
            break;

        case PARTICLE_TEARGAS:
            entry->m_ppRaster = &gpHeatHazeRaster;
            break;

        case PARTICLE_SHARD:
        case PARTICLE_RAINDROP:
        case PARTICLE_RAINDROP_2D:
            entry->m_ppRaster = &gpRainDropRaster;
            break;

        case PARTICLE_SPLASH:
        case PARTICLE_PED_SPLASH:
        case PARTICLE_CAR_SPLASH:
        case PARTICLE_WATER_HYDRANT:
            entry->m_ppRaster = gpCarSplashRaster;
            break;

        case PARTICLE_RAIN_SPLASH:
        case PARTICLE_RAIN_SPLASH_BIGGROW:
            entry->m_ppRaster = gpRainSplashRaster;
            break;

        case PARTICLE_RAIN_SPLASHUP:
            entry->m_ppRaster = gpRainSplashupRaster;
            break;

        case PARTICLE_WATERSPRAY:
            entry->m_ppRaster = gpWatersprayRaster;
            break;

        case PARTICLE_EXPLOSION_MEDIUM:
        case PARTICLE_EXPLOSION_LARGE:
        case PARTICLE_EXPLOSION_MFAST:
        case PARTICLE_EXPLOSION_LFAST:
            if (ParticleExEffects)
            entry->m_ppRaster = gpExplosionParticleEXRaster;
            else
            entry->m_ppRaster = gpExplosionMediumRaster;
            break;
        case PARTICLE_EXPLOSION_MASSIVE:
            entry->m_ppRaster = gpExplosionMassiveRaster;
            break;

        case PARTICLE_EXPLOSION_PARTICLEEX:
            entry->m_ppRaster = gpExplosionParticleEXRaster;
            break;

        case PARTICLE_BOAT_SPLASH:
            entry->m_ppRaster = &gpBoatWakeRaster;
            break;

        case PARTICLE_ENGINE_SMOKE:
        case PARTICLE_ENGINE_SMOKE2:
        case PARTICLE_CARFLAME_SMOKE:
        case PARTICLE_FIREBALL_SMOKE:
        case PARTICLE_ROCKET_SMOKE:
        case PARTICLE_TEST:
            entry->m_ppRaster = &gpCloudRaster4;
            break;

        case PARTICLE_FIREBALL_PARTICLEEX:
             if (gpFlameRaster)
            entry->m_ppRaster = gpFlameRaster;
              else
             entry->m_ppRaster = &gpFlame5Raster;
            break;

        case PARTICLE_CARCOLLISION_DUST:
        case PARTICLE_BURNINGRUBBER_SMOKE:
            entry->m_ppRaster = &gpCollisionSmokeRaster;
            break;

        case PARTICLE_SNOW:
            entry->m_ppRaster = &gpSnowRaster;
            break;

        case PARTICLE_BOAT_WAKE:
            entry->m_ppRaster = &gpWakeOldRaster;
            break;

        case PARTICLE_FERRY_CHIM_SMOKE:
            entry->m_ppRaster = gpSmokeRaster;
            break;

        case PARTICLE_MULTIPLAYER_HIT:
            entry->m_ppRaster = &gpMultiPlayerHitRaster;
            break;

        case PARTICLE_CAR_DEBRIS:
        case PARTICLE_HELI_DEBRIS:
        case PARTICLE_BIRD_DEBRIS:
            entry->m_ppRaster = gpCarDebrisRaster;
            break;

        case PARTICLE_GUNSHELL_FIRST:
        case PARTICLE_GUNSHELL:
        case PARTICLE_GUNSHELL_BUMP1:
        case PARTICLE_GUNSHELL_BUMP2:
            entry->m_ppRaster = &gpGunShellRaster;
            break;


        case PARTICLE_BIRD_FRONT:
            entry->m_ppRaster = gpBirdfrontRaster;
            break;

        case PARTICLE_SHIP_SIDE:
            entry->m_ppRaster = gpBoatRaster;
            break;

        case PARTICLE_BEASTIE:
            entry->m_ppRaster = &gpBeastieRaster;
            break;
        }
    }
    CTxdStore::PopCurrentTxd();
    log("CParticle ready");
    debug("CParticle ready");
}

void CParticle::ReloadTXD() {
    int32_t ParticleVC = CTxdStore::FindTxdSlot("particleVC");
    CTxdStore::RemoveTxdSlot(ParticleVC);
    log("Reloading particleVC.txd...\n");
    debug("Reloading particleVC.txd...\n");
    CTxdStore::PushCurrentTxd();
    int32_t slot2 = CTxdStore::AddTxdSlot("particleVC");
    CTxdStore::LoadTxd(slot2, PLUGIN_PATH((char*)"MODELS\\PARTICLEVC.TXD"));
    CTxdStore::AddRef(slot2);
    int32_t slot = CTxdStore::FindTxdSlot("particleVC");
    CTxdStore::SetCurrentTxd(slot);
    for (int32_t i = 0; i < MAX_SMOKE_FILES; i++)
    {
        gpSmokeTex[i] = RwTextureRead(SmokeFiles[i], nullptr);
        assert(gpSmokeTex[i] != NULL);
        gpSmokeRaster[i] = RwTextureGetRaster(gpSmokeTex[i]);
    }

    gpSmoke2Tex = RwTextureRead("smokeII_3", nullptr);
    assert(gpSmoke2Tex != NULL);
    gpSmoke2Raster = RwTextureGetRaster(gpSmoke2Tex);

    for (int32_t i = 0; i < MAX_RUBBER_FILES; i++)
    {
        gpRubberTex[i] = RwTextureRead(RubberFiles[i], nullptr);
        assert(gpRubberTex[i] != NULL);
        gpRubberRaster[i] = RwTextureGetRaster(gpRubberTex[i]);
    }

    for (int32_t i = 0; i < MAX_RAINSPLASH_FILES; i++)
    {
        gpRainSplashTex[i] = RwTextureRead(RainSplashFiles[i], nullptr);
        assert(gpRainSplashTex[i] != NULL);
        gpRainSplashRaster[i] = RwTextureGetRaster(gpRainSplashTex[i]);
    }

    for (int32_t i = 0; i < MAX_WATERSPRAY_FILES; i++)
    {
        gpWatersprayTex[i] = RwTextureRead(WatersprayFiles[i], nullptr);
        assert(gpWatersprayTex[i] != NULL);
        gpWatersprayRaster[i] = RwTextureGetRaster(gpWatersprayTex[i]);
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONMEDIUM_FILES; i++)
    {
        gpExplosionMediumTex[i] = RwTextureRead(ExplosionMediumFiles[i], nullptr);
        assert(gpExplosionMediumTex[i] != NULL);
        gpExplosionMediumRaster[i] = RwTextureGetRaster(gpExplosionMediumTex[i]);
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONMASSIVE_FILES; i++)
    {
        gpExplosionMassiveTex[i] = RwTextureRead(ExplosionMassiveFiles[i], nullptr);
        assert(gpExplosionMassiveTex[i] != NULL);
        gpExplosionMassiveRaster[i] = RwTextureGetRaster(gpExplosionMassiveTex[i]);
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONPARTICLEEX_FILES; i++)
    {
        gpExplosionParticleEXTex[i] = RwTextureRead(ExplosionParticleEXFiles[i], nullptr);
        assert(gpExplosionParticleEXTex[i] != NULL);
        gpExplosionParticleEXRaster[i] = RwTextureGetRaster(gpExplosionParticleEXTex[i]);
    }

    for (int32_t i = 0; i < MAX_GUNFLASH_FILES; i++)
    {
        gpGunFlashTex[i] = RwTextureRead(GunFlashFiles[i], nullptr);
        assert(gpGunFlashTex[i] != NULL);
        gpGunFlashRaster[i] = RwTextureGetRaster(gpGunFlashTex[i]);
    }

    gpRainDropTex = RwTextureRead("raindrop4", nullptr);
    assert(gpRainDropTex != NULL);
    gpRainDropRaster = RwTextureGetRaster(gpRainDropTex);

    // Snow
    gpSnowTex = RwTextureRead("snowflake", nullptr);
    assert(gpSnowTex != NULL);
    gpSnowRaster = RwTextureGetRaster(gpSnowTex);

    for (int32_t i = 0; i < MAX_RAINSPLASHUP_FILES; i++)
    {
        gpRainSplashupTex[i] = RwTextureRead(RainSplashupFiles[i], nullptr);
        assert(gpRainSplashupTex[i] != NULL);
        gpRainSplashupRaster[i] = RwTextureGetRaster(gpRainSplashupTex[i]);
    }

    for (int32_t i = 0; i < MAX_BIRDFRONT_FILES; i++)
    {
        gpBirdfrontTex[i] = RwTextureRead(BirdfrontFiles[i], nullptr);
        assert(gpBirdfrontTex[i] != NULL);
        gpBirdfrontRaster[i] = RwTextureGetRaster(gpBirdfrontTex[i]);
    }

    for (int32_t i = 0; i < MAX_BOAT_FILES; i++)
    {
        gpBoatTex[i] = RwTextureRead(BoatFiles[i], nullptr);
        assert(gpBoatTex[i] != NULL);
        gpBoatRaster[i] = RwTextureGetRaster(gpBoatTex[i]);
    }

    for (int32_t i = 0; i < MAX_CARDEBRIS_FILES; i++)
    {
        gpCarDebrisTex[i] = RwTextureRead(CardebrisFiles[i], nullptr);
        assert(gpCarDebrisTex[i] != NULL);
        gpCarDebrisRaster[i] = RwTextureGetRaster(gpCarDebrisTex[i]);
    }

    for (int32_t i = 0; i < MAX_CARSPLASH_FILES; i++)
    {
        gpCarSplashTex[i] = RwTextureRead(CarsplashFiles[i], nullptr);
        assert(gpCarSplashTex[i] != NULL);
        gpCarSplashRaster[i] = RwTextureGetRaster(gpCarSplashTex[i]);
    }

    gpBoatWakeTex = RwTextureRead("boatwake2", nullptr);
    assert(gpBoatWakeTex != NULL);
    gpBoatWakeRaster = RwTextureGetRaster(gpBoatWakeTex);

    gpFlame1Tex = RwTextureRead("flame1", nullptr);
    assert(gpFlame1Tex != NULL);
    gpFlame1Raster = RwTextureGetRaster(gpFlame1Tex);

    gpFlame5Tex = RwTextureRead("flame5", nullptr);
    assert(gpFlame5Tex != NULL);

    //#ifdef FIX_BUGS
#if 0
    gpFlame5Raster = RwTextureGetRaster(gpFlame5Tex);
#else
// this seems to have become more of a design choice
    gpFlame5Raster = RwTextureGetRaster(gpFlame1Tex); // copy-paste bug ?
#endif

    gpRainDropSmallTex = RwTextureRead("rainsmall", nullptr);
    assert(gpRainDropSmallTex != NULL);
    gpRainDropSmallRaster = RwTextureGetRaster(gpRainDropSmallTex);

    gpBloodTex = RwTextureRead("blood", nullptr);
    assert(gpBloodTex != NULL);
    gpBloodRaster = RwTextureGetRaster(gpBloodTex);

    gpLeafTex[0] = RwTextureRead("gameleaf01_64", nullptr);
    assert(gpLeafTex[0] != NULL);
    gpLeafRaster[0] = RwTextureGetRaster(gpLeafTex[0]);

    gpLeafTex[1] = RwTextureRead("letter", nullptr);
    assert(gpLeafTex[1] != NULL);
    gpLeafRaster[1] = RwTextureGetRaster(gpLeafTex[1]);

    gpCloudTex1 = RwTextureRead("cloud3", nullptr);
    assert(gpCloudTex1 != NULL);
    gpCloudRaster1 = RwTextureGetRaster(gpCloudTex1);

    gpCloudTex4 = RwTextureRead("cloudmasked", nullptr);
    assert(gpCloudTex4 != NULL);
    gpCloudRaster4 = RwTextureGetRaster(gpCloudTex4);

    gpBloodSmallTex = RwTextureRead("bloodsplat2", nullptr);
    assert(gpBloodSmallTex != NULL);
    gpBloodSmallRaster = RwTextureGetRaster(gpBloodSmallTex);

    gpGungeTex = RwTextureRead("gunge", nullptr);
    assert(gpGungeTex != NULL);
    gpGungeRaster = RwTextureGetRaster(gpGungeTex);

    gpCollisionSmokeTex = RwTextureRead("collisionsmoke", nullptr);
    assert(gpCollisionSmokeTex != NULL);
    gpCollisionSmokeRaster = RwTextureGetRaster(gpCollisionSmokeTex);

    gpBulletHitTex = RwTextureRead("bullethitsmoke", nullptr);
    assert(gpBulletHitTex != NULL);
    gpBulletHitRaster = RwTextureGetRaster(gpBulletHitTex);

    gpGunShellTex = RwTextureRead("gunshell", nullptr);
    assert(gpGunShellTex != NULL);
    gpGunShellRaster = RwTextureGetRaster(gpGunShellTex);

    gpPointlightTex = RwTextureRead("pointlight", nullptr);
    assert(gpPointlightTex != NULL);
    gpPointlightRaster = RwTextureGetRaster(gpPointlightTex);

    gpSparkTex = RwTextureRead("spark", nullptr);
    assert(gpSparkTex != NULL);
    gpSparkRaster = RwTextureGetRaster(gpSparkTex);

    gpNewspaperTex = RwTextureRead("newspaper02_64", nullptr);
    assert(gpNewspaperTex != NULL);
    gpNewspaperRaster = RwTextureGetRaster(gpNewspaperTex);

    gpGunSmokeTex = RwTextureRead("gunsmoke3", nullptr);
    assert(gpGunSmokeTex != NULL);
    gpGunSmokeRaster = RwTextureGetRaster(gpGunSmokeTex);

    gpDotTex = RwTextureRead("dot", nullptr);
    assert(gpDotTex != NULL);
    gpDotRaster = RwTextureGetRaster(gpDotTex);

    gpHeatHazeTex = RwTextureRead("heathaze", nullptr);
    assert(gpHeatHazeTex != NULL);
    gpHeatHazeRaster = RwTextureGetRaster(gpHeatHazeTex);

    gpBeastieTex = RwTextureRead("beastie", nullptr);
    assert(gpBeastieTex != NULL);
    gpBeastieRaster = RwTextureGetRaster(gpBeastieTex);

    gpRainDripTex[0] = RwTextureRead("raindrip64", nullptr);
    assert(gpRainDripTex[0] != NULL);
    gpRainDripRaster[0] = RwTextureGetRaster(gpRainDripTex[0]);

    gpRainDripTex[1] = RwTextureRead("raindripb64", nullptr);
    assert(gpRainDripTex[1] != NULL);
    gpRainDripRaster[1] = RwTextureGetRaster(gpRainDripTex[1]);

    gpRainDripDarkTex[0] = RwTextureRead("raindrip64_d", nullptr);
    assert(gpRainDripDarkTex[0] != NULL);
    gpRainDripDarkRaster[0] = RwTextureGetRaster(gpRainDripDarkTex[0]);

    gpRainDripDarkTex[1] = RwTextureRead("raindripb64_d", nullptr);
    assert(gpRainDripDarkTex[1] != NULL);
    gpRainDripDarkRaster[1] = RwTextureGetRaster(gpRainDripDarkTex[1]);

    gpWakeOldTex = RwTextureRead("wake_old", nullptr);
    assert(gpWakeOldTex != NULL);
    gpWakeOldRaster = RwTextureGetRaster(gpWakeOldTex);

    gpMultiPlayerHitTex = RwTextureRead("mphit", nullptr);
    assert(gpMultiPlayerHitTex != NULL);
    gpMultiPlayerHitRaster = RwTextureGetRaster(gpMultiPlayerHitTex);

    // CExplosionVC - for the shock wave
    coronaringa = RwTextureRead("coronaringa", nullptr);
    assert(coronaringa != NULL);

    for (int32_t i = 0; i < MAX_PARTICLES; i++)
    {
        tParticleSystemData* entry = &mod_ParticleSystemManager.m_aParticles[i];

        switch (i)
        {
        case PARTICLE_SPARK:
        case PARTICLE_SPARK_SMALL:
        case PARTICLE_RAINDROP_SMALL:
        case PARTICLE_HELI_ATTACK:
            entry->m_ppRaster = &gpRainDropSmallRaster;
            break;

        case PARTICLE_WATER_SPARK:
            entry->m_ppRaster = &gpSparkRaster;
            break;

        case PARTICLE_WHEEL_DIRT:
        case PARTICLE_SAND:
        case PARTICLE_STEAM2:
        case PARTICLE_STEAM_NY:
        case PARTICLE_STEAM_NY_SLOWMOTION:
        case PARTICLE_GROUND_STEAM:
        case PARTICLE_ENGINE_STEAM:
        case PARTICLE_PEDFOOT_DUST:
        case PARTICLE_CAR_DUST:
        case PARTICLE_EXHAUST_FUMES:
        case PARTICLE_HYDRANT_STEAM:
            entry->m_ppRaster = &gpSmoke2Raster;
            break;

        case PARTICLE_WHEEL_WATER:
        case PARTICLE_WATER:
        case PARTICLE_SMOKE:
        case PARTICLE_SMOKE_SLOWMOTION:
        case PARTICLE_DRY_ICE:
        case PARTICLE_GARAGEPAINT_SPRAY:
        case PARTICLE_STEAM:
        case PARTICLE_WATER_CANNON:
        case PARTICLE_EXTINGUISH_STEAM:
        case PARTICLE_HELI_DUST:
        case PARTICLE_PAINT_SMOKE:
        case PARTICLE_BULLETHIT_SMOKE:
            entry->m_ppRaster = gpSmokeRaster;
            break;

        case PARTICLE_BLOOD:
            entry->m_ppRaster = &gpBloodRaster;
            break;

        case PARTICLE_BLOOD_SMALL:
        case PARTICLE_BLOOD_SPURT:
            entry->m_ppRaster = &gpBloodSmallRaster;
            break;

        case PARTICLE_DEBRIS:
        case PARTICLE_TREE_LEAVES:
            entry->m_ppRaster = gpLeafRaster;
            break;

        case PARTICLE_DEBRIS2:
            entry->m_ppRaster = &gpGungeRaster;
            break;

        case PARTICLE_FLYERS:
            entry->m_ppRaster = &gpNewspaperRaster;
            break;

        case PARTICLE_FLAME:
        case PARTICLE_CARFLAME:
            entry->m_ppRaster = &gpFlame1Raster;
            break;

        case PARTICLE_FIREBALL:
            entry->m_ppRaster = &gpFlame5Raster;
            break;

        case PARTICLE_GUNFLASH:
        case PARTICLE_GUNFLASH_NOANIM:
            entry->m_ppRaster = gpGunFlashRaster;
            break;
        case PARTICLE_GUNFLASH_VCS:
        case PARTICLE_GUNFLASH_NOANIM_VCS:
            entry->m_ppRaster = gpGunFlashRasterVCS;
            break;


        case PARTICLE_GUNSMOKE:
        case PARTICLE_WATERDROP:
        case PARTICLE_BLOODDROP:
        case PARTICLE_HEATHAZE:
        case PARTICLE_HEATHAZE_IN_DIST:
            entry->m_ppRaster = nullptr;
            break;

        case PARTICLE_GUNSMOKE2:
        case PARTICLE_BOAT_THRUSTJET:
        case PARTICLE_RUBBER_SMOKE:
            entry->m_ppRaster = gpRubberRaster;
            break;

        case PARTICLE_CIGARETTE_SMOKE:
            entry->m_ppRaster = &gpGunSmokeRaster;
            break;

        case PARTICLE_TEARGAS:
            entry->m_ppRaster = &gpHeatHazeRaster;
            break;

        case PARTICLE_SHARD:
        case PARTICLE_RAINDROP:
        case PARTICLE_RAINDROP_2D:
            entry->m_ppRaster = &gpRainDropRaster;
            break;

        case PARTICLE_SPLASH:
        case PARTICLE_PED_SPLASH:
        case PARTICLE_CAR_SPLASH:
        case PARTICLE_WATER_HYDRANT:
            entry->m_ppRaster = gpCarSplashRaster;
            break;

        case PARTICLE_RAIN_SPLASH:
        case PARTICLE_RAIN_SPLASH_BIGGROW:
            entry->m_ppRaster = gpRainSplashRaster;
            break;

        case PARTICLE_RAIN_SPLASHUP:
            entry->m_ppRaster = gpRainSplashupRaster;
            break;

        case PARTICLE_WATERSPRAY:
            entry->m_ppRaster = gpWatersprayRaster;
            break;

        case PARTICLE_EXPLOSION_MEDIUM:
        case PARTICLE_EXPLOSION_LARGE:
        case PARTICLE_EXPLOSION_MFAST:
        case PARTICLE_EXPLOSION_LFAST:
            if (ParticleExEffects)
                entry->m_ppRaster = gpExplosionParticleEXRaster;
            else
                entry->m_ppRaster = gpExplosionMediumRaster;
            break;

        case PARTICLE_BOAT_SPLASH:
            entry->m_ppRaster = &gpBoatWakeRaster;
            break;

        case PARTICLE_ENGINE_SMOKE:
        case PARTICLE_ENGINE_SMOKE2:
        case PARTICLE_CARFLAME_SMOKE:
        case PARTICLE_FIREBALL_SMOKE:
        case PARTICLE_ROCKET_SMOKE:
        case PARTICLE_TEST:
            entry->m_ppRaster = &gpCloudRaster4;
            break;

        case PARTICLE_CARCOLLISION_DUST:
        case PARTICLE_BURNINGRUBBER_SMOKE:
            entry->m_ppRaster = &gpCollisionSmokeRaster;
            break;

        case PARTICLE_SNOW:
            entry->m_ppRaster = &gpSnowRaster;
            break;

        case PARTICLE_BOAT_WAKE:
            entry->m_ppRaster = &gpWakeOldRaster;
            break;

        case PARTICLE_FERRY_CHIM_SMOKE:
            entry->m_ppRaster = gpSmokeRaster;
            break;

        case PARTICLE_MULTIPLAYER_HIT:
            entry->m_ppRaster = &gpMultiPlayerHitRaster;
            break;

        case PARTICLE_CAR_DEBRIS:
        case PARTICLE_HELI_DEBRIS:
        case PARTICLE_BIRD_DEBRIS:
            entry->m_ppRaster = gpCarDebrisRaster;
            break;

        case PARTICLE_GUNSHELL_FIRST:
        case PARTICLE_GUNSHELL:
        case PARTICLE_GUNSHELL_BUMP1:
        case PARTICLE_GUNSHELL_BUMP2:
            entry->m_ppRaster = &gpGunShellRaster;
            break;


        case PARTICLE_BIRD_FRONT:
            entry->m_ppRaster = gpBirdfrontRaster;
            break;

        case PARTICLE_SHIP_SIDE:
            entry->m_ppRaster = gpBoatRaster;
            break;

        case PARTICLE_BEASTIE:
            entry->m_ppRaster = &gpBeastieRaster;
            break;
        }
    }
    CTxdStore::PopCurrentTxd();
    log("Reloaded particleVC.txd!\n");
    debug("Reloaded particleVC.txd!\n");
}


void CParticle::Shutdown()
{
    log("Shutting down CParticle...\n");
    debug("Shutting down CParticle...\n");

    for (int32_t i = 0; i < MAX_SMOKE_FILES; i++)
    {
        RwTextureDestroy(gpSmokeTex[i]);
        gpSmokeTex[i] = nullptr;
    }

    RwTextureDestroy(gpSmoke2Tex);
    gpSmoke2Tex = nullptr;

    for (int32_t i = 0; i < MAX_RUBBER_FILES; i++)
    {
        RwTextureDestroy(gpRubberTex[i]);
        gpRubberTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_RAINSPLASH_FILES; i++)
    {
        RwTextureDestroy(gpRainSplashTex[i]);
        gpRainSplashTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_WATERSPRAY_FILES; i++)
    {
        RwTextureDestroy(gpWatersprayTex[i]);
        gpWatersprayTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONMEDIUM_FILES; i++)
    {
        RwTextureDestroy(gpExplosionMediumTex[i]);
        gpExplosionMediumTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONMASSIVE_FILES; i++)
    {
        RwTextureDestroy(gpExplosionMassiveTex[i]);
        gpExplosionMassiveTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_EXPLOSIONPARTICLEEX_FILES; i++)
    {
        RwTextureDestroy(gpExplosionParticleEXTex[i]);
        gpExplosionParticleEXTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_GUNFLASH_FILES; i++)
    {
        RwTextureDestroy(gpGunFlashTex[i]);
        gpGunFlashTex[i] = nullptr;
    }

    RwTextureDestroy(gpRainDropTex);
    gpRainDropTex = nullptr;

    for (int32_t i = 0; i < MAX_RAINSPLASHUP_FILES; i++)
    {
        RwTextureDestroy(gpRainSplashupTex[i]);
        gpRainSplashupTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_BIRDFRONT_FILES; i++)
    {
        RwTextureDestroy(gpBirdfrontTex[i]);
        gpBirdfrontTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_BOAT_FILES; i++)
    {
        RwTextureDestroy(gpBoatTex[i]);
        gpBoatTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_CARDEBRIS_FILES; i++)
    {
        RwTextureDestroy(gpCarDebrisTex[i]);
        gpCarDebrisTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_CARSPLASH_FILES; i++)
    {
        RwTextureDestroy(gpCarSplashTex[i]);
        gpCarSplashTex[i] = nullptr;
    }

    for (int32_t i = 0; i < MAX_RAINDRIP_FILES; i++)
    {
        RwTextureDestroy(gpRainDripTex[i]);
        gpRainDripTex[i] = nullptr;

        RwTextureDestroy(gpRainDripDarkTex[i]);
        gpRainDripDarkTex[i] = nullptr;
    }
    for (int32_t i = 0; i < MAX_FLAME_FILES; i++)
    {
        RwTextureDestroy(gpFlameTex[i]);
        gpFlameTex[i] = NULL;
    }
    RwTextureDestroy(gpBoatWakeTex);
    gpBoatWakeTex = nullptr;

    RwTextureDestroy(gpFlame1Tex);
    gpFlame1Tex = nullptr;

    RwTextureDestroy(gpFlame5Tex);
    gpFlame5Tex = nullptr;

    RwTextureDestroy(gpRainDropSmallTex);
    gpRainDropSmallTex = nullptr;

    RwTextureDestroy(gpBloodTex);
    gpBloodTex = nullptr;

    RwTextureDestroy(gpLeafTex[0]);
    gpLeafTex[0] = nullptr;

    RwTextureDestroy(gpLeafTex[1]);
    gpLeafTex[1] = nullptr;

    RwTextureDestroy(gpCloudTex1);
    gpCloudTex1 = nullptr;

    RwTextureDestroy(gpCloudTex4);
    gpCloudTex4 = nullptr;

    RwTextureDestroy(gpBloodSmallTex);
    gpBloodSmallTex = nullptr;

    RwTextureDestroy(gpGungeTex);
    gpGungeTex = nullptr;

    RwTextureDestroy(gpCollisionSmokeTex);
    gpCollisionSmokeTex = nullptr;

    RwTextureDestroy(gpBulletHitTex);
    gpBulletHitTex = nullptr;

    RwTextureDestroy(gpGunShellTex);
    gpGunShellTex = nullptr;
    RwTextureDestroy(gpPointlightTex);
    gpPointlightTex = nullptr;

    RwTextureDestroy(gpSparkTex);
    gpSparkTex = nullptr;

    RwTextureDestroy(gpNewspaperTex);
    gpNewspaperTex = nullptr;

    RwTextureDestroy(gpGunSmokeTex);
    gpGunSmokeTex = nullptr;

    RwTextureDestroy(gpDotTex);
    gpDotTex = nullptr;

    RwTextureDestroy(gpHeatHazeTex);
    gpHeatHazeTex = nullptr;

    RwTextureDestroy(gpBeastieTex);
    gpBeastieTex = nullptr;

    RwTextureDestroy(gpSnowTex);
    gpSnowTex = nullptr;

    RwTextureDestroy(gpMultiPlayerHitTex);
    gpMultiPlayerHitTex = nullptr;

    RwTextureDestroy(gpWakeOldTex);
    gpWakeOldTex = nullptr;

    int32_t slot;
    slot = CTxdStore::FindTxdSlot("particleVC");
    CTxdStore::RemoveRef(slot);
    CTxdStore::RemoveTxdSlot(slot);
    log("CParticle shut down\n");
    debug("CParticle shut down\n");
}


void CParticle::AddParticlesAlongLine(tParticleType type, CVector const& vecStart, CVector const& vecEnd, CVector const& vecDir, float fPower, CEntity* pEntity, float fSize, int32_t nRotationSpeed, int32_t nRotation, int32_t nCurFrame, int32_t nLifeSpan)
{
    CVector vecDist = vecEnd - vecStart;
    float fDist = vecDist.Magnitude();
    float fSteps = max(fDist / fPower, 1.0f);
    int32_t nSteps = (int32_t)fSteps;

    CVector vecStep = vecDist * (1.0f / (float)nSteps);

    for (int32_t i = 0; i < nSteps; i++)
    {
        CVector vecPos = float(i) * vecStep + vecStart;
        AddParticle(type, vecPos, vecDir, pEntity, fSize, nRotationSpeed, nRotation, nCurFrame, nLifeSpan);
    }
}

void CParticle::AddParticlesAlongLine(tParticleType type, CVector const& vecStart, CVector const& vecEnd, CVector const& vecDir, float fPower, CEntity* pEntity, float fSize, RwRGBA const& color, int32_t nRotationSpeed, int32_t nRotation, int32_t nCurFrame, int32_t nLifeSpan)
{
    CVector vecDist = vecEnd - vecStart;
    float fDist = vecDist.Magnitude();
    float fSteps = max(fDist / fPower, 1.0f);
    int32_t nSteps = (int32_t)fSteps;

    CVector vecStep = vecDist * (1.0f / (float)nSteps);

    for (int32_t i = 0; i < nSteps; i++)
    {
        CVector vecPos = float(i) * vecStep + vecStart;

        AddParticle(type, vecPos, vecDir, pEntity, fSize, color, nRotationSpeed, nRotation, nCurFrame, nLifeSpan);
    }
}

CParticle* CParticle::AddParticle(tParticleType type, CVector const& vecPos, CVector const& vecDir, CEntity* pEntity, float fSize, int32_t nRotationSpeed, int32_t nRotation, int32_t nCurFrame, int32_t nLifeSpan)
{
    RwRGBA color(0, 0, 0, 0);
    return AddParticle(type, vecPos, vecDir, pEntity, fSize, color, nRotationSpeed, nRotation, nCurFrame, nLifeSpan);
}
CParticle* CParticle::AddParticle(tParticleType type, CVector const& vecPos, CVector const& vecDir, CEntity* pEntity, float fSize, RwRGBA const& color, int32_t nRotationSpeed, int32_t nRotation, int32_t nCurFrame, int32_t nLifeSpan)
{
    if (CTimer::m_UserPause || CTimer::m_CodePause)
        return nullptr;

    if ((type == PARTICLE_ENGINE_SMOKE
        || type == PARTICLE_ENGINE_SMOKE2
        || type == PARTICLE_ENGINE_STEAM
        || type == PARTICLE_CARFLAME_SMOKE
        || type == PARTICLE_RUBBER_SMOKE
        || type == PARTICLE_BURNINGRUBBER_SMOKE
        || type == PARTICLE_EXHAUST_FUMES
        || type == PARTICLE_CARCOLLISION_DUST
        || type == PARTICLE_FERRY_CHIM_SMOKE
        || type == PARTICLE_HYDRANT_STEAM)
        && nParticleCreationInterval & CTimer::m_FrameCounter)
    {
        return nullptr;
    }

    //if (!CReplay::IsPlayingBack())
        //CReplay::RecordParticle(type, vecPos, vecDir, fSize, color);

    CParticle* pParticle = m_pUnusedListHead;
    if (pParticle == nullptr)
        // log("CParticle::AddParticle - Abort! The particle limit on screen is hit!");
        // debug("CParticle::AddParticle - Abort! The particle limit on screen is hit!");
        return nullptr;
    tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[type];

    if (psystem->m_fCreateRange != 0.0f && psystem->m_fCreateRange < static_cast<Vec>((TheCamera.GetPosition() - vecPos)).MagnitudeSqr())
        return nullptr;
    log("Particle with name %s was created at pos X:%4.0f Y:%4.0f Z:%4.0f with size %f velocity X:%f Y:%f Z:%f lifespan %d and color R:%d G:%d B:%d A:%d",
        psystem->m_aName,
        pParticle->m_vecPosition.x,
        pParticle->m_vecPosition.y, pParticle->m_vecPosition.z, pParticle->m_fSize, pParticle->m_vecVelocity.x, pParticle->m_vecVelocity.y, pParticle->m_vecVelocity.z, nLifeSpan, pParticle->m_Color.red, pParticle->m_Color.green, pParticle->m_Color.blue, pParticle->m_Color.alpha);
    debug("Particle with name %s was created at pos X:%4.0f Y:%4.0f Z:%4.0f",
        psystem->m_aName,
        pParticle->m_vecPosition.x,
        pParticle->m_vecPosition.y, pParticle->m_vecPosition.z);

    pParticle->m_fSize = psystem->m_fDefaultInitialRadius;
    pParticle->m_fExpansionRate = psystem->m_fExpansionRate;
    if (nLifeSpan != 0)
        pParticle->m_nTimeWhenWillBeDestroyed = CTimer::m_snTimeInMilliseconds + nLifeSpan;
    else
        pParticle->m_nTimeWhenWillBeDestroyed = CTimer::m_snTimeInMilliseconds + psystem->m_nLifeSpan;

    pParticle->m_nColorIntensity = psystem->m_nFadeToBlackInitialIntensity;

    pParticle->m_nFadeToBlackTimer = psystem->m_nFadeToBlackAmount;

    if (psystem->m_nFadeToBlackTime)
        pParticle->m_nFadeToBlackTimer /= psystem->m_nFadeToBlackTime;

    pParticle->m_nAlpha = psystem->m_nFadeAlphaInitialIntensity;

    pParticle->m_nFadeAlphaTimer = psystem->m_nFadeAlphaAmount;

    if (psystem->m_nFadeAlphaTime)
        pParticle->m_nFadeAlphaTimer /= psystem->m_nFadeAlphaTime;

    pParticle->m_nCurrentZRotation = psystem->m_nZRotationInitialAngle;
    pParticle->m_fCurrentZRadius = psystem->m_fInitialZRadius;

    if (nCurFrame != 0)
        pParticle->m_nCurrentFrame = nCurFrame;
    else
        pParticle->m_nCurrentFrame = psystem->m_nStartAnimationFrame;


    pParticle->m_nZRotationTimer = 0;
    pParticle->m_nZRadiusTimer = 0;
    pParticle->m_nAnimationSpeedTimer = 0;
    pParticle->m_fZGround = 0.0f;

    if (type != PARTICLE_HEATHAZE)
        pParticle->m_vecPosition = vecPos;
    else
    {
        CVector screen;
        float w, h;

        if (!CSprite2::CalcScreenCoors(vecPos, screen, w, h, true, lim))
            return nullptr;

        pParticle->m_vecPosition = screen;
        psystem->m_vecTextureStretch.x = w;
        psystem->m_vecTextureStretch.y = h;
    }

    pParticle->m_vecVelocity = vecDir;

    pParticle->m_vecParticleMovementOffset = CVector(0.0f, 0.0f, 0.0f);
    pParticle->m_nTimeWhenColorWillBeChanged = 0;

    if (color.alpha != 0)
        RwRGBAAssign(&pParticle->m_Color, &color);
    else
    {
        RwRGBAAssign(&pParticle->m_Color, &psystem->m_RenderColouring);

        if (psystem->m_ColorFadeTime != 0)
            pParticle->m_nTimeWhenColorWillBeChanged = CTimer::m_snTimeInMilliseconds + psystem->m_ColorFadeTime;

        if (psystem->m_InitialColorVariation != 0)
        {
            int32_t ColorVariation = CGeneral::GetRandomNumberInRange(-psystem->m_InitialColorVariation, psystem->m_InitialColorVariation);
            //float ColorVariation = CGeneral::GetRandomNumberInRange((float)-psystem->m_InitialColorVariation, (float)psystem->m_InitialColorVariation);

            pParticle->m_Color.red = Clamp(pParticle->m_Color.red +
                PERCENT(pParticle->m_Color.red, ColorVariation),
                0, 255);

            pParticle->m_Color.green = Clamp(pParticle->m_Color.green +
                PERCENT(pParticle->m_Color.green, ColorVariation),
                0, 255);

            pParticle->m_Color.blue = Clamp(pParticle->m_Color.blue +
                PERCENT(pParticle->m_Color.blue, ColorVariation),
                0, 255);
        }
    }


    pParticle->m_nRotation = nRotation;

    if (nRotationSpeed != 0)
        pParticle->m_nRotationStep = nRotationSpeed;
    else
        pParticle->m_nRotationStep = psystem->m_nRotationSpeed;

    if (GetRandomNumber() & 1)
        pParticle->m_nRotationStep = -pParticle->m_nRotationStep;

    if (psystem->m_fPositionRandomError != 0.0f)
    {
        pParticle->m_vecPosition.x += psystem->m_fPositionRandomError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];
        pParticle->m_vecPosition.y += psystem->m_fPositionRandomError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];

        if (psystem->Flags & RAND_VERT_V)
            pParticle->m_vecPosition.z += psystem->m_fPositionRandomError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];
    }

    if (psystem->m_fVelocityRandomError != 0.0f)
    {
        pParticle->m_vecVelocity.x += psystem->m_fVelocityRandomError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];
        pParticle->m_vecVelocity.y += psystem->m_fVelocityRandomError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];

        if (psystem->Flags & RAND_VERT_V)
            pParticle->m_vecVelocity.z += psystem->m_fVelocityRandomError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];
    }

    if (psystem->m_fExpansionRateError != 0.0f && !(psystem->Flags & SCREEN_TRAIL))
        pParticle->m_fExpansionRate += psystem->m_fExpansionRateError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE] + psystem->m_fExpansionRateError;

    if (psystem->m_nRotationRateError != 0)
        pParticle->m_nRotationStep += CGeneral::GetRandomNumberInRange(-psystem->m_nRotationRateError, psystem->m_nRotationRateError);

    if (psystem->m_nLifeSpanErrorShape != 0)
    {
        float randVal = ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];
        if (randVal > 0.0f)
            pParticle->m_nTimeWhenWillBeDestroyed += int32_t(float(psystem->m_nLifeSpan) * randVal * float(psystem->m_nLifeSpanErrorShape));
        else
            pParticle->m_nTimeWhenWillBeDestroyed += int32_t(float(psystem->m_nLifeSpan) * randVal / float(psystem->m_nLifeSpanErrorShape));
    }

    if (psystem->Flags & ZCHECK_FIRST)
    {
        static bool bValidGroundFound = false;
        static CVector LastTestCoors;
        static float LastTestGroundZ;

        if (bValidGroundFound
            && vecPos.x == LastTestCoors.x
            && vecPos.y == LastTestCoors.y
            && vecPos.z == LastTestCoors.z)
        {
            pParticle->m_fZGround = LastTestGroundZ;
        }
        else
        {
            bValidGroundFound = false;

            CColPoint point;
            CEntity* entity;

            if (!CWorld::ProcessVerticalLine(
                pParticle->m_vecPosition + CVector(0.0f, 0.0f, 0.5f),
                -100.0f, point, entity, true, true, false, false, true, false, nullptr))
            {
                return nullptr;
            }

            if (point.m_vecPoint.z >= pParticle->m_vecPosition.z)
                return nullptr;

            pParticle->m_fZGround = point.m_vecPoint.z;
            bValidGroundFound = true;
            LastTestCoors = vecPos;
            LastTestGroundZ = point.m_vecPoint.z;
        }
    }

    if (psystem->Flags & ZCHECK_BUMP)
    {
        static float Z_Ground = 0.0f;

        if (psystem->Flags & ZCHECK_BUMP_FIRST)
        {
            bool bZFound = false;

            Z_Ground = CWorld::FindGroundZFor3DCoord(vecPos.x, vecPos.y, vecPos.z, (bool*)&bZFound, nullptr);

            if (bZFound == false)
                return nullptr;

            pParticle->m_fZGround = Z_Ground;
        }

        pParticle->m_fZGround = Z_Ground;
    }

    switch (type)
    {
    case PARTICLE_DEBRIS:
        pParticle->m_vecVelocity.z *= CGeneral::GetRandomNumberInRange(0.5f, 3.0f);
        break;

    case PARTICLE_EXPLOSION_MEDIUM:
        pParticle->m_nColorIntensity -= 30 * (GetRandomNumber() & 1); // mb "+= -30 * rand" here ?
        pParticle->m_nAnimationSpeedTimer = GetRandomNumber() & 7;
        pParticle->m_fSize = CGeneral::GetRandomNumberInRange(0.3f, 0.8f);
        pParticle->m_vecPosition.z -= CGeneral::GetRandomNumberInRange(-0.1f, 0.1f);
        break;

    case PARTICLE_EXPLOSION_LARGE:
        pParticle->m_nColorIntensity -= 30 * (GetRandomNumber() & 1); // mb "+= -30 * rand" here ?
        pParticle->m_nAnimationSpeedTimer = GetRandomNumber() & 7;
        pParticle->m_fSize = CGeneral::GetRandomNumberInRange(0.8f, 5.0f); // 0.8f, 1.4f
        pParticle->m_vecPosition.z -= CGeneral::GetRandomNumberInRange(-0.3f, 0.3f);
        break;

    case PARTICLE_WATER_HYDRANT:
        pParticle->m_vecPosition.z += 20.0f * psystem->m_fPositionRandomError * ms_afRandTable[GetRandomNumber() % RAND_TABLE_SIZE];
        break;
    default: break;
    }

    if (fSize != 0.0f)
        pParticle->m_fSize = fSize;

    m_pUnusedListHead = pParticle->m_pNext;

    pParticle->m_pNext = psystem->m_pParticles;

    psystem->m_pParticles = pParticle;

    return pParticle;
}

/*bool UpToDate = false;
bool CheckedAnUpdate = false;
bool NewVersionIsOut = false;
static uint32_t timetocheck = 0;

void CheckUpdates() {
    const char* url = "https://raw.githubusercontent.com/TheArtemMaps/VCParticles/main/updatecheck.txt";
    HINTERNET hInternet = InternetOpen("UpdateChecker", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        log("InternetOpen failed: %lu", GetLastError());
        return;
    }

    HINTERNET hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        log("InternetOpenUrl failed: %lu", GetLastError());
        InternetCloseHandle(hInternet);
        return;
    }

    char buffer[4096];
    DWORD bytesRead;
    std::string content;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead) {
        buffer[bytesRead] = '\0';
        content.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    // Search the version in the file
    std::string::size_type pos = content.find("VERSION = ");
    if (pos != std::string::npos) {
        pos += 10; // Skip "VERSION = "
        std::string::size_type end = content.find_first_not_of("0123456789.", pos);
        if (end == std::string::npos) end = content.length();
        double githubVersion = std::stod(content.substr(pos, end - pos));

        // Compare with current version
        double currentVersion = 1.0;
        if (currentVersion >= githubVersion) {
            UpToDate = true;
            NewVersionIsOut = false;
            std::string message = "VCParticles is up to date: " + std::to_string(currentVersion);
            CHud::SetHelpMessage(message.c_str(), false, false, false);
        }
        else {
            UpToDate = false;
            NewVersionIsOut = true;
            std::string message = "New update available! Current version: " + std::to_string(currentVersion) + ", New version: " + std::to_string(githubVersion);
            CHud::SetHelpMessage(message.c_str(), false, false, false);
        }
    }
    else {
        UpToDate = false;
        NewVersionIsOut = false;
        CHud::SetHelpMessage("Failed to find version info in the file.", false, false, false);
        log("Failed to find version info in the file.");
    }

    CheckedAnUpdate = true;
}

void UpdateCheckHandler() {
    if (CTimer::m_snTimeInMilliseconds - timetocheck > 5000) {
        if (!CheckedAnUpdate || NewVersionIsOut) {
            CheckUpdates();
        }
        timetocheck = CTimer::m_snTimeInMilliseconds;
    }
}*/

void CParticle::Update()
{
    if (CTimer::m_UserPause || CTimer::m_CodePause)
        return;

    RwRGBA color(0, 0, 0, 0);
    RwRGBA ShotgunShellsColor(255, 0, 0, 255);
    float fFricDeccel50 = pow(0.50f, CTimer::ms_fTimeStep);
    float fFricDeccel80 = pow(0.80f, CTimer::ms_fTimeStep);
    float fFricDeccel90 = pow(0.90f, CTimer::ms_fTimeStep);
    float fFricDeccel95 = pow(0.95f, CTimer::ms_fTimeStep);
    float fFricDeccel96 = pow(0.96f, CTimer::ms_fTimeStep);
    float fFricDeccel99 = pow(0.99f, CTimer::ms_fTimeStep);

    CParticleObject::UpdateAll();

    // ejaculations at 23:00, 23:15, 23:30, 23:45 
    if (CClock::ms_nGameClockHours == 23 &&
        (CClock::ms_nGameClockMinutes == 0
            || CClock::ms_nGameClockMinutes == 15
            || CClock::ms_nGameClockMinutes == 30
            || CClock::ms_nGameClockMinutes == 45))
    {
        AddParticle(PARTICLE_CAR_SPLASH,
            CVector(1481.115234f, -1790.420532f, 156.753326f),
            CVector(0.0f, 0.0f, 2.5f),
            nullptr,
            2.0f,
            RwRGBA(255, 255, 255, 255),
            0,
            0,
            1,
            1000);

        AddParticle(PARTICLE_CAR_SPLASH,
            CVector(-426.326904f, 2505.481689f, 124.304688f),
            CVector(0.0f, 0.0f, 2.5f),
            nullptr,
            2.0f,
            RwRGBA(255, 255, 255, 255),
            0,
            0,
            1,
            1000);
    }

    for (int32_t i = 0; i < MAX_PARTICLES; i++)
    {
        tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[i];
        CParticle* particle = psystem->m_pParticles;
        CParticle* prevParticle = nullptr;
        // Glass, actually.
        auto PlayGunshellSound = (void(__thiscall*)(CAudioEngine * engine, eAudioEvents glassSoundType, CVector * posn))0x506EE0;
        bool bRemoveParticle;

        if (particle == nullptr)
            continue;

        for (; particle != nullptr; _Next(particle, prevParticle, psystem, bRemoveParticle))
        {
            CVector vecWind(0.0f, 0.0f, 0.0f);

            bRemoveParticle = false;

            CVector vecMoveStep = particle->m_vecVelocity * CTimer::ms_fTimeStep;
            CVector vecPos = particle->m_vecPosition;

            if (numWaterDropOnScreen == 0)
                clearWaterDrop = false;

            if (psystem->m_Type == PARTICLE_WATERDROP)
            {
                if (CGame::currArea > 0 || clearWaterDrop == true)
                {
                    bRemoveParticle = true;
                    continue;
                }

                static uint8_t nWaterDropCount;

                if (nWaterDropCount == 5)
                {
                    vecMoveStep = CVector(0.0f, 0.0f, 0.0f);
                    particle->m_nTimeWhenWillBeDestroyed += 1250;
                    nWaterDropCount = 0;
                }
                else
                {
                    if (TheCamera.m_fCameraAverageSpeed > 0.35f)
                    {
                        if (vecMoveStep.Magnitude() > 0.5f)
                        {
                            if (vecMoveStep.Magnitude() > 0.4f && vecMoveStep.Magnitude() < 0.8f)
                            {
                                vecMoveStep.x += TheCamera.m_fCameraAverageSpeed * 1.5f;
                                vecMoveStep.y += TheCamera.m_fCameraAverageSpeed * 1.5f;
                            }
                            else if (vecMoveStep.Magnitude() != 0.0f)
                            {
                                vecMoveStep.x += CGeneral::GetRandomNumberInRange(0.01f, 0.05f);
                                vecMoveStep.y += CGeneral::GetRandomNumberInRange(0.01f, 0.05f);
                            }
                        }
                    }

                    nWaterDropCount++;
                }

                if (vecPos.z <= 1.5f)
                    vecMoveStep.z = 0.0f;
            }

            if (psystem->m_Type == PARTICLE_HEATHAZE || psystem->m_Type == PARTICLE_HEATHAZE_IN_DIST)
            {
#ifdef FIX_BUGS
                int32_t nSinCosIndex = (int32_t(DEGTORAD((float)particle->m_nRotation) * float(SIN_COS_TABLE_SIZE) / TWOPI) + SIN_COS_TABLE_SIZE) % SIN_COS_TABLE_SIZE;
#else
                int32_t nSinCosIndex = int32_t(DEGTORAD((float)particle->m_nRotation) * float(SIN_COS_TABLE_SIZE) / TWOPI) % SIN_COS_TABLE_SIZE;
#endif
                vecMoveStep.x = Sin(nSinCosIndex);
                vecMoveStep.y = Sin(nSinCosIndex);

                if (psystem->m_Type == PARTICLE_HEATHAZE_IN_DIST)
                    particle->m_nRotation = int16_t((float)particle->m_nRotation + 0.75f);
                else
                    particle->m_nRotation = int16_t((float)particle->m_nRotation + 1.0f);
            }

            if (psystem->m_Type == PARTICLE_BEASTIE)
            {
#ifdef FIX_BUGS
                int32_t nSinCosIndex = (int32_t(DEGTORAD((float)particle->m_nRotation) * float(SIN_COS_TABLE_SIZE) / TWOPI) + SIN_COS_TABLE_SIZE) % SIN_COS_TABLE_SIZE;
#else
                int32_t nSinCosIndex = int32_t(DEGTORAD((float)particle->m_nRotation) * float(SIN_COS_TABLE_SIZE) / TWOPI) % SIN_COS_TABLE_SIZE;
#endif				
                particle->m_vecVelocity.x = 0.50f * Cos(nSinCosIndex);
                particle->m_vecVelocity.y = Cos(nSinCosIndex);
                particle->m_vecVelocity.z = 0.25f * Sin(nSinCosIndex);

                if (particle->m_vecVelocity.Magnitude() > 2.0f
                    || vecPos.z > 40.0f
                    || (TheCamera.GetPosition() - vecPos).Magnitude() < 60.0f
                    )
                {
                    bRemoveParticle = true;
                    continue;
                }
            }

            vecPos += vecMoveStep;

            if (psystem->m_Type == PARTICLE_FIREBALL) // todo: Fix heat haze
            {
                AddParticle(PARTICLE_HEATHAZE, particle->m_vecPosition, CVector(0.0f, 0.0f, 0.0f),
                    nullptr, particle->m_fSize * 5.0f);
            }

            if (psystem->m_Type == PARTICLE_GUNSMOKE2)
            {
                if (CTimer::m_FrameCounter & 10)
                {
#ifdef FIX_BUGS
                    if (FindPlayerPed() && FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_eWeaponType == WEAPON_MINIGUN)
#else
                    if (FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_eWeaponType == WEAPON_MINIGUN)
#endif
                    {
                        AddParticle(PARTICLE_HEATHAZE, particle->m_vecPosition, CVector(0.0f, 0.0f, 0.0f));
                    }
                }
            }

            if (CWeather::Wind > 0.0f)
            {
                if (vecMoveStep.Magnitude() != 0.0f)
                {
                    vecWind.x = CGeneral::GetRandomNumberInRange(0.75f, 1.25f) * -CWeather::Wind;
                    vecWind.y = CGeneral::GetRandomNumberInRange(0.75f, 1.25f) * -CWeather::Wind;
                    vecWind *= PARTICLE_WIND_TEST_SCALE * psystem->m_fWindFactor * CTimer::ms_fTimeStep;
                    particle->m_vecVelocity += vecWind;
                }
            }

            if (psystem->m_Type == PARTICLE_RAINDROP
                || psystem->m_Type == PARTICLE_RAINDROP_SMALL
                || psystem->m_Type == PARTICLE_RAIN_SPLASH
                || psystem->m_Type == PARTICLE_RAIN_SPLASH_BIGGROW
                || psystem->m_Type == PARTICLE_CAR_SPLASH
                || psystem->m_Type == PARTICLE_BOAT_SPLASH
                || psystem->m_Type == PARTICLE_RAINDROP_2D
                || psystem->m_Type == PARTICLE_SPLASH)
            {
                int32_t nMaxDrops = int32_t(6.0f * TheCamera.m_fCameraAverageSpeed + 1.0f);
                float fDistToCam = 0.0f;

                if (psystem->m_Type == PARTICLE_BOAT_SPLASH || psystem->m_Type == PARTICLE_CAR_SPLASH)
                {
                    if (vecPos.z + particle->m_fSize < 5.0f)
                    {
                        bRemoveParticle = true;
                        continue;
                    }

                    switch (TheCamera.GetLookDirection())
                    {
                    case 1:
                    case 2:
                    case 3:
                        nMaxDrops /= 2;
                        break;

                    default:
                        nMaxDrops = 0;
                        break;
                    }

                    fDistToCam = (TheCamera.GetPosition() - vecPos).Magnitude();
                }

                if (numWaterDropOnScreen < nMaxDrops && numWaterDropOnScreen < 63
                    && fDistToCam < 10.0f
                    && clearWaterDrop == false
                    && !CGame::currArea > 0)
                {
                    CVector vecWaterdropTarget
                    (
                        CGeneral::GetRandomNumberInRange(-0.25f, 0.25f),
                        CGeneral::GetRandomNumberInRange(0.1f, 0.75f),
                        -0.01f
                    );

                    CVector vecWaterdropPos;

                    if (TheCamera.m_fCameraAverageSpeed < 0.35f)
                        vecWaterdropPos.x = (float)CGeneral::GetRandomNumberInRange(50, int32_t(SCREEN_WIDTH) - 50);
                    else
                        vecWaterdropPos.x = (float)CGeneral::GetRandomNumberInRange(200, int32_t(SCREEN_WIDTH) - 200);

                    if (psystem->m_Type == PARTICLE_BOAT_SPLASH || psystem->m_Type == PARTICLE_CAR_SPLASH)
                        vecWaterdropPos.y = (float)CGeneral::GetRandomNumberInRange(SCREEN_HEIGHT / 2, SCREEN_HEIGHT);
                    else
                    {
                        if (TheCamera.m_fCameraAverageSpeed < 0.35f)
                            vecWaterdropPos.y = (float)CGeneral::GetRandomNumberInRange(0, int32_t(SCREEN_HEIGHT));
                        else
                            vecWaterdropPos.y = (float)CGeneral::GetRandomNumberInRange(150, int32_t(SCREEN_HEIGHT) - 200);
                    }

                    vecWaterdropPos.z = 2.0f;

                    if (AddParticle(PARTICLE_WATERDROP,
                        vecWaterdropPos,
                        vecWaterdropTarget,
                        nullptr,
                        CGeneral::GetRandomNumberInRange(0.1f, 0.15f),
                        0,
                        0,
                        GetRandomNumber() & 1,
                        0) != nullptr)
                    {
                        numWaterDropOnScreen++;
                    }
                }
            }

            if (CTimer::m_snTimeInMilliseconds > particle->m_nTimeWhenWillBeDestroyed || particle->m_nAlpha == 0)
            {
                bRemoveParticle = true;
                continue;
            }

            if (particle->m_nTimeWhenColorWillBeChanged != 0)
            {
                if (particle->m_nTimeWhenColorWillBeChanged > CTimer::m_snTimeInMilliseconds)
                {
                    float colorMul = 1.0f - float(particle->m_nTimeWhenColorWillBeChanged - CTimer::m_snTimeInMilliseconds) / float(psystem->m_ColorFadeTime);

                    particle->m_Color.red = Clamp(
                        psystem->m_RenderColouring.red + int32_t(float(psystem->m_FadeDestinationColor.red - psystem->m_RenderColouring.red) * colorMul),
                        0, 255);

                    particle->m_Color.green = Clamp(
                        psystem->m_RenderColouring.green + int32_t(float(psystem->m_FadeDestinationColor.green - psystem->m_RenderColouring.green) * colorMul),
                        0, 255);

                    particle->m_Color.blue = Clamp(
                        psystem->m_RenderColouring.blue + int32_t(float(psystem->m_FadeDestinationColor.blue - psystem->m_RenderColouring.blue) * colorMul),
                        0, 255);
                }
                else
                    RwRGBAAssign(&particle->m_Color, &psystem->m_FadeDestinationColor);

            }

            if (psystem->Flags & CLIPOUT2D)
            {
                if (particle->m_vecPosition.x < -10.0f || particle->m_vecPosition.x > SCREEN_WIDTH + 10.0f
                    || particle->m_vecPosition.y < -10.0f || particle->m_vecPosition.y > SCREEN_HEIGHT + 10.0f)
                {
                    bRemoveParticle = true;
                    continue;
                }
            }

            if (!(psystem->Flags & SCREEN_TRAIL))
            {
                float size;

                if (particle->m_fExpansionRate > 0.0f)
                {
                    float speed = max(vecWind.Magnitude(), vecMoveStep.Magnitude());

                    if (psystem->m_Type == PARTICLE_EXHAUST_FUMES || psystem->m_Type == PARTICLE_ENGINE_STEAM)
                        speed *= 2.0f;

                    if ((psystem->m_Type == PARTICLE_BOAT_SPLASH || psystem->m_Type == PARTICLE_CAR_SPLASH)
                        && particle->m_fSize > 1.2f)
                    {
                        size = particle->m_fSize - (1.0f + speed) * particle->m_fExpansionRate;
                        particle->m_vecVelocity.z -= 0.15f;
                    }
                    else
                        size = particle->m_fSize + (1.0f + speed) * particle->m_fExpansionRate;
                }
                else
                    size = particle->m_fSize + particle->m_fExpansionRate;

                if (psystem->m_Type == PARTICLE_WATERDROP)
                    size = (size - fabsf(vecMoveStep.x) * 0.000150000007f) + (fabsf(vecMoveStep.z) * 0.0500000007f); //TODO:

                if (size < 0.0f)
                {
                    bRemoveParticle = true;
                    continue;
                }

                particle->m_fSize = size;
            }

            switch (psystem->m_nFrictionDecceleration)
            {
            case 50:
                particle->m_vecVelocity *= fFricDeccel50;
                break;

            case 80:
                particle->m_vecVelocity *= fFricDeccel80;
                break;

            case 90:
                particle->m_vecVelocity *= fFricDeccel90;
                break;

            case 95:
                particle->m_vecVelocity *= fFricDeccel95;
                break;

            case 96:
                particle->m_vecVelocity *= fFricDeccel96;
                break;

            case 99:
                particle->m_vecVelocity *= fFricDeccel99;
                break;
            }

            if (psystem->m_fGravitationalAcceleration > 0.0f)
            {
                if (-50.0f * psystem->m_fGravitationalAcceleration < particle->m_vecVelocity.z)
                    particle->m_vecVelocity.z -= psystem->m_fGravitationalAcceleration * CTimer::ms_fTimeStep;

                if (psystem->Flags & ZCHECK_FIRST)
                {
                    if (particle->m_vecPosition.z < particle->m_fZGround)
                    {
                        switch (psystem->m_Type)
                        {
                        case PARTICLE_RAINDROP:
                        case PARTICLE_RAINDROP_SMALL:
                        {
                            bRemoveParticle = true;

                            if (GetRandomNumber() & 1)
                            {
                                AddParticle(PARTICLE_RAIN_SPLASH,
                                    CVector
                                    (
                                        particle->m_vecPosition.x,
                                        particle->m_vecPosition.y,
                                        0.05f + particle->m_fZGround
                                    ),
                                    CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, 0, 0, 0, 0);
                            }
                            else
                            {
                                AddParticle(PARTICLE_RAIN_SPLASHUP,
                                    CVector
                                    (
                                        particle->m_vecPosition.x,
                                        particle->m_vecPosition.y,
                                        0.05f + particle->m_fZGround
                                    ),
                                    CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, 0, 0, 0, 0);
                            }

                            continue;
                        }
                        break;

                        case PARTICLE_WHEEL_WATER:
                        {
                            bRemoveParticle = true;

                            int32_t randVal = GetRandomNumber();

                            if (randVal & 1)
                            {
                                if ((randVal % 5) == 0)
                                {
                                    AddParticle(PARTICLE_RAIN_SPLASH,
                                        CVector
                                        (
                                            particle->m_vecPosition.x,
                                            particle->m_vecPosition.y,
                                            0.05f + particle->m_fZGround
                                        ),
                                        CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, 0, 0, 0, 0);
                                }
                                else
                                {
                                    AddParticle(PARTICLE_RAIN_SPLASHUP,
                                        CVector
                                        (
                                            particle->m_vecPosition.x,
                                            particle->m_vecPosition.y,
                                            0.05f + particle->m_fZGround
                                        ),
                                        CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, 0, 0, 0, 0);
                                }

                            }
                            continue;
                        }
                        break;

                        case PARTICLE_BLOOD:
                        case PARTICLE_BLOOD_SMALL:
                        {
                            bRemoveParticle = true;

                            CVector vecPosn = particle->m_vecPosition;
                            vecPosn.z += 1.0f;

                            Randomizer++;
                            int32_t randVal = int32_t(Randomizer & 7);
                            if (randVal == 5)
                            {
                                CShadows::AddPermanentShadow(SHADOW_DEFAULT, gpBloodPoolTex, &vecPosn,
                                    0.1f, 0.0f, 0.0f, -0.1f,
                                    255,
                                    255, 0, 0,
                                    4.0f, (GetRandomNumber() & 4095) + 2000, 1.0f);
                            }
                            else if (randVal == 2)
                            {
                                CShadows::AddPermanentShadow(SHADOW_DEFAULT, gpBloodPoolTex, &vecPosn,
                                    0.2f, 0.0f, 0.0f, -0.2f,
                                    255,
                                    255, 0, 0,
                                    4.0f, (GetRandomNumber() & 4095) + 8000, 1.0f);
                            }
                            continue;
                        }
                        break;
                        default: break;
                        }
                    }
                }
                else if (psystem->Flags & ZCHECK_STEP)
                {
                    CColPoint point;
                    CEntity* entity;

                    if (CWorld::ProcessVerticalLine(particle->m_vecPosition, vecPos.z, point, entity,
                        true, true, false, false, true, false, nullptr))
                    {
                        if (vecPos.z <= point.m_vecPoint.z)
                        {
                            vecPos.z = point.m_vecPoint.z;
                            if (psystem->m_Type == PARTICLE_DEBRIS2 || psystem->m_Type == PARTICLE_GUNSHELL_BUMP1 || psystem->m_Type == PARTICLE_GUNSHELL_BUMP2)
                            {
                                particle->m_vecVelocity.x *= 0.8f;
                                particle->m_vecVelocity.y *= 0.8f;
                                particle->m_vecVelocity.z *= -0.4f;
                                if (particle->m_vecVelocity.z < 0.005f)
                                    particle->m_vecVelocity.z = 0.0f;
                            }
                        }
                    }
                }
                else if (psystem->Flags & ZCHECK_BUMP)
                {
                    if (particle->m_vecPosition.z < particle->m_fZGround)
                    {
                        switch (psystem->m_Type)
                        {
                        case PARTICLE_GUNSHELL_FIRST:
                        case PARTICLE_GUNSHELL:
                        {
                            bRemoveParticle = true;
                            if (IsShotgun) {
                                color = ShotgunShellsColor;
                            }
                            else {
                                color = color;
                            }
                            AddParticle(PARTICLE_GUNSHELL_BUMP1,
                                CVector
                                (
                                    particle->m_vecPosition.x,
                                    particle->m_vecPosition.y,
                                    0.05f + particle->m_fZGround
                                ),
                                CVector
                                (
                                    CGeneral::GetRandomNumberInRange(-0.02f, 0.02f),
                                    CGeneral::GetRandomNumberInRange(-0.02f, 0.02f),
                                    CGeneral::GetRandomNumberInRange(0.05f, 0.1f)
                                ),
                                nullptr,
                                particle->m_fSize, color, particle->m_nRotationStep, 0, 0, 0);
                            if (GunshellSounds) {
                                PlayGunshellSound(&AudioEngine, AE_GLASS_HIT_GROUND, &particle->m_vecPosition);
                            }
                            //AudioEngine.ReportMissionAudioEvent(AE_FRONTEND_BUY_CAR_MOD, &particle->m_vecPosition);
                            //AudioEngine.ReportMissionAudioEvent(eAudioEvents::AE_BULLET_HIT, &particle->m_vecPosition);
                            //PlayOneShotScriptObject(SCRIPT_SOUND_GUNSHELL_DROP, particle->m_vecPosition);
                        }
                        break;

                        case PARTICLE_GUNSHELL_BUMP1:
                        {
                            bRemoveParticle = true;
                            if (IsShotgun) {
                                color = ShotgunShellsColor;
                            }
                            else {
                                color = color;
                            }
                            AddParticle(PARTICLE_GUNSHELL_BUMP2,
                                CVector
                                (
                                    particle->m_vecPosition.x,
                                    particle->m_vecPosition.y,
                                    0.05f + particle->m_fZGround
                                ),
                                CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.03f, 0.06f)),
                                nullptr,
                                particle->m_fSize, color, 0, 0, 0, 0);
                            if (GunshellSounds) {
                                PlayGunshellSound(&AudioEngine, AE_GLASS_HIT_GROUND_SLOW, &particle->m_vecPosition);
                            }
                            //AudioEngine.m_pWeaponAudio.
                            //AudioEngine.ReportMissionAudioEvent(AE_FRONTEND_BUY_CAR_MOD, &particle->m_vecPosition);
                            //PlayOneShotScriptObject(SCRIPT_SOUND_GUNSHELL_DROP_SOFT, particle->m_vecPosition);
                        }
                        break;

                        case PARTICLE_GUNSHELL_BUMP2:
                        {
                            if (IsShotgun) {
                                color = ShotgunShellsColor;
                            }
                            else {
                                color = color;
                            }
                            bRemoveParticle = true;
                            continue;
                        }
                        break;
                        default:
                            AddParticle(psystem->m_Type,
                                CVector
                                (
                                    particle->m_vecPosition.x,
                                    particle->m_vecPosition.y,
                                    0.05f + particle->m_fZGround
                                ),
                                CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.03f, 0.06f)),
                                nullptr,
                                particle->m_fSize, color, 0, 0, 0, 0);
                            break;
                        }
                    }
                }
            }
            else
            {
                if (psystem->m_fGravitationalAcceleration < 0.0f)
                {
                    if (-5.0f * psystem->m_fGravitationalAcceleration > particle->m_vecVelocity.z)
                        particle->m_vecVelocity.z -= psystem->m_fGravitationalAcceleration * CTimer::ms_fTimeStep;
                }
                else
                {
                    if (psystem->Flags & ZCHECK_STEP)
                    {
                        CColPoint point;
                        CEntity* entity;

                        if (CWorld::ProcessVerticalLine(particle->m_vecPosition, vecPos.z, point, entity,
                            true, false, false, false, true, false, nullptr))
                        {
                            if (vecPos.z <= point.m_vecPoint.z)
                            {
                                vecPos.z = point.m_vecPoint.z;
                                if (psystem->m_Type == PARTICLE_HELI_ATTACK)
                                {
                                    bRemoveParticle = true;
                                    AddParticle(PARTICLE_STEAM, vecPos, CVector(0.0f, 0.0f, 0.05f), nullptr, 0.2f, 0, 0, 0, 0);
                                    continue;
                                }
                            }
                        }
                    }
                }
            }

            if (particle->m_nFadeToBlackTimer != 0)
            {
                particle->m_nColorIntensity = Clamp(particle->m_nColorIntensity - particle->m_nFadeToBlackTimer,
                    0, 255);
            }

            if (particle->m_nFadeAlphaTimer != 0)
            {
                particle->m_nAlpha = Clamp(particle->m_nAlpha - particle->m_nFadeAlphaTimer,
                    0, 255);
                if (particle->m_nAlpha == 0)
                {
                    bRemoveParticle = true;
                    continue;
                }
            }

            if (psystem->m_nZRotationAngleChangeAmount != 0)
            {
                if (particle->m_nZRotationTimer >= psystem->m_nZRotationChangeTime)
                {
                    particle->m_nZRotationTimer = 0;
                    particle->m_nCurrentZRotation += psystem->m_nZRotationAngleChangeAmount;
                }
                else
                    ++particle->m_nZRotationTimer;
            }

            if (psystem->m_fZRadiusChangeAmount != 0.0f)
            {
                if (particle->m_nZRadiusTimer >= psystem->m_nZRadiusChangeTime)
                {
                    particle->m_nZRadiusTimer = 0;
                    particle->m_fCurrentZRadius += psystem->m_fZRadiusChangeAmount;
                }
                else
                    ++particle->m_nZRadiusTimer;
            }

            if (psystem->m_nAnimationSpeed != 0)
            {
                if (particle->m_nAnimationSpeedTimer > psystem->m_nAnimationSpeed)
                {
                    particle->m_nAnimationSpeedTimer = 0;

                    if (++particle->m_nCurrentFrame > psystem->m_nFinalAnimationFrame)
                    {
                        if (psystem->Flags & CYCLE_ANIM)
                            particle->m_nCurrentFrame = psystem->m_nStartAnimationFrame;
                        else
                            --particle->m_nCurrentFrame;
                    }
                }
                else
                    ++particle->m_nAnimationSpeedTimer;
            }

            if (particle->m_nRotationStep != 0)
#ifdef FIX_BUGS
               particle->m_nRotation = CGeneral::LimitAngle(particle->m_nRotation + particle->m_nRotationStep);
#else
                particle->m_nRotation += particle->m_nRotationStep;
#endif

            if (particle->m_fCurrentZRadius != 0.0f)
            {
                int32_t nSinCosIndex = particle->m_nCurrentZRotation % SIN_COS_TABLE_SIZE;

                float fX = (Cos(nSinCosIndex) - Sin(nSinCosIndex)) * particle->m_fCurrentZRadius;

                float fY = (Sin(nSinCosIndex) + Cos(nSinCosIndex)) * particle->m_fCurrentZRadius;

                vecPos -= particle->m_vecParticleMovementOffset;

                vecPos += CVector(fX, fY, 0.0f);

                particle->m_vecParticleMovementOffset = CVector(fX, fY, 0.0f);
            }

            particle->m_vecPosition = vecPos;
        }
    }
}

void CParticle::Render()
{
    //PUSH_RENDERGROUP("CParticle::Render");

    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);
    RwRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

    CSprite2::InitSpriteBuffer2D();

    uint32_t flags = DRAW_OPAQUE;

    RwRaster* prevFrame = nullptr;

    for (int32_t i = 0; i < MAX_PARTICLES; i++)
    {
        tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[i];
        bool particleBanned = false;
        CParticle* particle = psystem->m_pParticles;
        RwRaster** frames = psystem->m_ppRaster;
        tParticleType type = psystem->m_Type;

        if (type == PARTICLE_ENGINE_SMOKE
            || type == PARTICLE_ENGINE_SMOKE2
            || type == PARTICLE_ENGINE_STEAM
            || type == PARTICLE_CARFLAME_SMOKE
            || type == PARTICLE_RUBBER_SMOKE
            || type == PARTICLE_BURNINGRUBBER_SMOKE
            || type == PARTICLE_EXHAUST_FUMES
            || type == PARTICLE_CARCOLLISION_DUST
            || type == PARTICLE_FERRY_CHIM_SMOKE
            || type == PARTICLE_HYDRANT_STEAM)
        {
            particleBanned = true;
        }

        if (particle)
        {
            if ((flags & DRAW_OPAQUE) != (psystem->Flags & DRAW_OPAQUE)
                || (flags & DRAW_DARK) != (psystem->Flags & DRAW_DARK))
            {
                CSprite2::FlushSpriteBuffer();

                if (psystem->Flags & DRAW_OPAQUE)
                {
                    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
                    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
                }
                else
                {
                    if (psystem->Flags & DRAW_DARK)
                        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
                    else
                        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);

                    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
                }

                flags = psystem->Flags;
            }

            if (frames != nullptr)
            {
                RwRaster* curFrame = *frames;
                if (curFrame != prevFrame)
                {
                    CSprite2::FlushSpriteBuffer();
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)curFrame);
                    prevFrame = curFrame;
                }
            }
        }

        while (particle != nullptr)
        {
            bool canDraw = true;

            if (particle->m_nAlpha == 0)
                canDraw = false;

            if (canDraw && psystem->m_nFinalAnimationFrame != 0 && frames != nullptr)
            {
                RwRaster* curFrame = frames[particle->m_nCurrentFrame];
                if (prevFrame != curFrame)
                {
                    CSprite2::FlushSpriteBuffer();
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)curFrame);
                    prevFrame = curFrame;
                }
            }

            if (canDraw && psystem->Flags & DRAWTOP2D)
            {
                float screenZ;
#ifdef FIX_BUGS
                bool zIsZero = true;
                if (particle->m_vecPosition.z != 0.0f) {
#endif
                    screenZ = (particle->m_vecPosition.z - CDraw::ms_fNearClipZ)
                        * (RwIm2DGetFarScreenZ() - RwIm2DGetNearScreenZ())
                        * CDraw::ms_fFarClipZ
                        / ((CDraw::ms_fFarClipZ - CDraw::ms_fNearClipZ) * particle->m_vecPosition.z)
                        + RwIm2DGetNearScreenZ();
#ifdef FIX_BUGS
                    zIsZero = false;
                }
#endif

                float stretchTexW;
                float stretchTexH;

                if (i == PARTICLE_RAINDROP || i == PARTICLE_RAINDROP_SMALL || i == PARTICLE_RAINDROP_2D)
                {
                    stretchTexW = CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.x * (float)particle->m_nCurrentFrame + 63.0f;
                    stretchTexH = CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.y * (float)particle->m_nCurrentFrame + 63.0f;
                }
                else
                {
                    stretchTexW = CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.x + 63.0f;
                    stretchTexH = CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.y + 63.0f;
                }

#ifdef FIX_BUGS
                if (!zIsZero) {
#endif

                    if (i == PARTICLE_WATERDROP)
                    {
                        int32_t timeLeft = (particle->m_nTimeWhenWillBeDestroyed - CTimer::m_snTimeInMilliseconds) / particle->m_nTimeWhenWillBeDestroyed;

                        stretchTexH += (1.0f - (float)timeLeft) * psystem->m_vecTextureStretch.y;

                        RwRect rect;

                        rect.x = int32_t(particle->m_vecPosition.x - SCREEN_STRETCH_X(particle->m_fSize * stretchTexW));
                        rect.y = int32_t(particle->m_vecPosition.y - SCREEN_STRETCH_Y(particle->m_fSize * stretchTexH));
                        rect.w = int32_t(particle->m_vecPosition.x + SCREEN_STRETCH_X(particle->m_fSize * stretchTexW));
                        rect.h = int32_t(particle->m_vecPosition.y + SCREEN_STRETCH_Y(particle->m_fSize * stretchTexH));

                        FxType fxtype;

                        if (particle->m_nCurrentFrame != 0)
                            fxtype = FXTYPE_WATER2;
                        else
                            fxtype = FXTYPE_WATER1;

                        if (particle->m_nCurrentFrame != 0)
                            BlurType = FXTYPE_WATER2;
                        else
                            BlurType = FXTYPE_WATER1;
                        if (WaterDrops) {
                            CMBlur::AddRenderFx(Scene.m_pCamera, &rect, screenZ, fxtype);
                        }

                        canDraw = false;
                    }

                    if (i == PARTICLE_BLOODDROP)
                    {
                        int32_t timeLeft = (particle->m_nTimeWhenWillBeDestroyed - CTimer::m_snTimeInMilliseconds) / particle->m_nTimeWhenWillBeDestroyed;

                        stretchTexH += (1.0f + (float)timeLeft) * psystem->m_vecTextureStretch.y;
                        stretchTexW += (1.0f - (float)timeLeft) * psystem->m_vecTextureStretch.x;

                        RwRect rect;
                        rect.x = int32_t(particle->m_vecPosition.x - SCREEN_STRETCH_X(particle->m_fSize * stretchTexW));
                        rect.y = int32_t(particle->m_vecPosition.y - SCREEN_STRETCH_Y(particle->m_fSize * stretchTexH));
                        rect.w = int32_t(particle->m_vecPosition.x + SCREEN_STRETCH_X(particle->m_fSize * stretchTexW));
                        rect.h = int32_t(particle->m_vecPosition.y + SCREEN_STRETCH_Y(particle->m_fSize * stretchTexH));

                        FxType fxtype;

                        if (particle->m_nCurrentFrame)
                            fxtype = FXTYPE_BLOOD2;
                        else
                            fxtype = FXTYPE_BLOOD1;

                        if (particle->m_nCurrentFrame)
                            BlurType = FXTYPE_BLOOD2;
                        else
                            BlurType = FXTYPE_BLOOD1;
                        if (BloodDrops) {
                            CMBlur::AddRenderFx(Scene.m_pCamera, &rect, screenZ, fxtype);
                        }
                        canDraw = false;
                    }

                    if (i == PARTICLE_HEATHAZE_IN_DIST)
                    {
                        RwRect rect;

                        rect.x = int32_t(particle->m_vecPosition.x - SCREEN_STRETCH_X(particle->m_fSize * stretchTexW));
                        rect.y = int32_t(particle->m_vecPosition.y - SCREEN_STRETCH_Y(particle->m_fSize * stretchTexH * 0.15f));
                        rect.w = int32_t(particle->m_vecPosition.x + SCREEN_STRETCH_X(particle->m_fSize * stretchTexW));
                        rect.h = int32_t(particle->m_vecPosition.y + SCREEN_STRETCH_Y(particle->m_fSize * stretchTexH * 0.15f));

                        CMBlur::AddRenderFx(Scene.m_pCamera, &rect, screenZ, FXTYPE_HEATHAZE);

                        canDraw = false;
                    }

                    if (i == PARTICLE_HEATHAZE)
                    {
                        RwRect rect;

                        switch (TheCamera.GetLookDirection())
                        {
                        case 1:
                            rect.x = int32_t(particle->m_vecPosition.x - SCREEN_STRETCH_X(particle->m_fSize * psystem->m_vecTextureStretch.x * 2.0f));
                            rect.y = int32_t(particle->m_vecPosition.y - SCREEN_STRETCH_Y(particle->m_fSize * psystem->m_vecTextureStretch.y));
                            rect.w = int32_t(particle->m_vecPosition.x - SCREEN_STRETCH_X(particle->m_fSize * psystem->m_vecTextureStretch.x));
                            rect.h = int32_t(particle->m_vecPosition.y + SCREEN_STRETCH_Y(particle->m_fSize * psystem->m_vecTextureStretch.y));

                            break;

                        case 2:
                            rect.x = int32_t(particle->m_vecPosition.x + SCREEN_STRETCH_X(particle->m_fSize * psystem->m_vecTextureStretch.x));
                            rect.y = int32_t(particle->m_vecPosition.y - SCREEN_STRETCH_Y(particle->m_fSize * psystem->m_vecTextureStretch.y));
                            rect.w = int32_t(particle->m_vecPosition.x + SCREEN_STRETCH_X(particle->m_fSize * psystem->m_vecTextureStretch.x * 4.0f));
                            rect.h = int32_t(particle->m_vecPosition.y + SCREEN_STRETCH_Y(particle->m_fSize * psystem->m_vecTextureStretch.y));

                            break;

                        default:
                            rect.x = int32_t(particle->m_vecPosition.x - SCREEN_STRETCH_X(particle->m_fSize * psystem->m_vecTextureStretch.x));
                            rect.y = int32_t(particle->m_vecPosition.y - SCREEN_STRETCH_Y(particle->m_fSize * psystem->m_vecTextureStretch.y));
                            rect.w = int32_t(particle->m_vecPosition.x + SCREEN_STRETCH_X(particle->m_fSize * psystem->m_vecTextureStretch.x));
                            rect.h = int32_t(particle->m_vecPosition.y + SCREEN_STRETCH_Y(particle->m_fSize * psystem->m_vecTextureStretch.y));

                            break;
                        }

                        CMBlur::AddRenderFx(Scene.m_pCamera, &rect, screenZ, FXTYPE_HEATHAZE);

                        canDraw = false;
                    }
#ifdef FIX_BUGS
                }
                if (!(zIsZero && (i == PARTICLE_WATERDROP || i == PARTICLE_BLOODDROP || i == PARTICLE_HEATHAZE_IN_DIST || i == PARTICLE_HEATHAZE)))
#endif				
                    if (canDraw)
                    {
                        if (particle->m_nRotation != 0)
                        {
                            CSprite2::RenderBufferedOneXLUSprite2D_Rotate_Dimension(
                                particle->m_vecPosition.x,
                                particle->m_vecPosition.y,
                                particle->m_fSize * stretchTexW,
                                particle->m_fSize * stretchTexH,
                                particle->m_Color,
                                particle->m_nColorIntensity,
                                DEGTORAD((float)particle->m_nRotation),
                                particle->m_nAlpha);
                        }
                        else
                        {
                            CSprite2::RenderBufferedOneXLUSprite2D(
                                particle->m_vecPosition.x,
                                particle->m_vecPosition.y,
                                particle->m_fSize * stretchTexW,
                                particle->m_fSize * stretchTexH,
                                particle->m_Color,
                                particle->m_nColorIntensity,
                                particle->m_nAlpha);
                        }
                    }

                canDraw = false;
            }

            if (canDraw)
            {
                CVector coors;
                float w;
                float h;

                if (CSprite2::CalcScreenCoors(particle->m_vecPosition, coors, w, h, true, lim))
                {

                    if (i == PARTICLE_ENGINE_STEAM
                        || i == PARTICLE_ENGINE_SMOKE
                        || i == PARTICLE_ENGINE_SMOKE2
                        || i == PARTICLE_CARFLAME_SMOKE
                        || i == PARTICLE_CARCOLLISION_DUST
                        || i == PARTICLE_EXHAUST_FUMES
                        || i == PARTICLE_RUBBER_SMOKE
                        || i == PARTICLE_BURNINGRUBBER_SMOKE 
                        || i == PARTICLE_FERRY_CHIM_SMOKE
                        || i == PARTICLE_HYDRANT_STEAM)
                    {
                        switch (TheCamera.GetLookDirection())
                        {
                        case 1:
                        case 2:
                            w += CGeneral::GetRandomNumberInRange(1.0f, 7.5f) * psystem->m_vecTextureStretch.x;
                            h += CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.y;
                            break;

                        default:
                            w += CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.x;
                            h += CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.y;
                            break;
                        }
                    }
                    else if (i == PARTICLE_WATER_HYDRANT)
                    {
                        int32_t timeLeft = (particle->m_nTimeWhenWillBeDestroyed - CTimer::m_snTimeInMilliseconds) / particle->m_nTimeWhenWillBeDestroyed;

                        w += (1.0f - (float)timeLeft) * psystem->m_vecTextureStretch.x;
                        h += (1.0f - (float)timeLeft) * psystem->m_vecTextureStretch.y;
                    }
                    else if (i == PARTICLE_FLYERS)
                    {
                        w += psystem->m_vecTextureStretch.x;
                        h += psystem->m_vecTextureStretch.y;

                        w = max(w, 12.0f);
                        h = max(h, 12.0f);
                    }
                    else
                    {
                        w += CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.x;
                        h += CGeneral::GetRandomNumberInRange(0.1f, 1.0f) * psystem->m_vecTextureStretch.y;
                    }

                    if (i == PARTICLE_WATER_HYDRANT
                        || (!particleBanned || SCREEN_WIDTH * fParticleScaleLimit >= w)
                        && SCREEN_HEIGHT * fParticleScaleLimit >= h)
                    {
                        if (i == PARTICLE_WATER_HYDRANT)
                        {
                            RwRect rect;

                            if (w > 0.0f)
                            {
                                rect.x = int32_t(coors.x - SCREEN_STRETCH_X(particle->m_fSize * w));
                                rect.w = int32_t(coors.x + SCREEN_STRETCH_X(particle->m_fSize * w));
                            }
                            else
                            {
                                rect.w = int32_t(coors.x - SCREEN_STRETCH_X(particle->m_fSize * w));
                                rect.x = int32_t(coors.x + SCREEN_STRETCH_X(particle->m_fSize * w));
                            }

                            if (h > 0.0f)
                            {
                                rect.y = int32_t(coors.y - SCREEN_STRETCH_Y(particle->m_fSize * h));
                                rect.h = int32_t(coors.y + SCREEN_STRETCH_Y(particle->m_fSize * h));
                            }
                            else
                            {
                                rect.h = int32_t(coors.y - SCREEN_STRETCH_Y(particle->m_fSize * h));
                                rect.y = int32_t(coors.y + SCREEN_STRETCH_Y(particle->m_fSize * h));
                            }

                            float screenZ = (coors.z - CDraw::ms_fNearClipZ)
                                * (RwIm2DGetFarScreenZ() - RwIm2DGetNearScreenZ()) * CDraw::ms_fFarClipZ
                                / ((CDraw::ms_fFarClipZ - CDraw::ms_fNearClipZ) * coors.z) + RwIm2DGetNearScreenZ();

                            CMBlur::AddRenderFx(Scene.m_pCamera, &rect, screenZ, FXTYPE_SPLASH1);
                        }
                        else
                        {
                            if (particle->m_nRotation != 0 && i != PARTICLE_BEASTIE)
                            {
                                CSprite2::RenderBufferedOneXLUSprite_Rotate_Dimension(coors.x, coors.y, coors.z,
                                    particle->m_fSize * w, particle->m_fSize * h,
                                    particle->m_Color.red,
                                    particle->m_Color.green,
                                    particle->m_Color.blue,
                                    particle->m_nColorIntensity,
                                    1.0f / coors.z,
                                    DEGTORAD((float)particle->m_nRotation),
                                    particle->m_nAlpha);
                            }
                            else if (psystem->Flags & SCREEN_TRAIL)
                            {
                                float fRotation;
                                float fTrailLength;

                                if (particle->m_fZGround == 0.0f)
                                {
                                    fTrailLength = 0.0f;
                                    fRotation = 0.0f;
                                }
                                else
                                {
                                    CVector2D vecDist
                                    (
                                        coors.x - particle->m_fZGround,
                                        coors.y - particle->m_fExpansionRate
                                    );

                                    float fDist = vecDist.Magnitude();

                                    fTrailLength = fDist;

                                    float fRot = asinf(vecDist.x / fDist);

                                    fRotation = fRot;

                                    if (vecDist.y < 0.0f)
                                        fRotation = -1.0f * fRot + DEGTORAD(180.0f);
                                    fRotation = RADTODEG(fRotation);
                                    if (fRotation < 0.0f)
                                        fRotation += 360.0f;
                                    float fSpeed = particle->m_vecVelocity.Magnitude();

                                    float fNewTrailLength = fSpeed * CTimer::ms_fTimeStep * w * 2.0f;

                                    if (fDist > fNewTrailLength)
                                        fTrailLength = fNewTrailLength;
                                }

                                CSprite2::RenderBufferedOneXLUSprite_Rotate_Dimension(coors.x, coors.y, coors.z,
                                    particle->m_fSize * w,
                                    particle->m_fSize * h + fTrailLength * psystem->m_fTrailLengthMultiplier,
                                    particle->m_Color.red,
                                    particle->m_Color.green,
                                    particle->m_Color.blue,
                                    particle->m_nColorIntensity,
                                    1.0f / coors.z,
                                    fRotation,
                                    particle->m_nAlpha);

                                particle->m_fZGround = coors.x;				// WTF ? (coors.x)
                                particle->m_fExpansionRate = coors.y;		// WTF ?
                            }
                            else if (psystem->Flags & SPEED_TRAIL)
                            {
                                CVector vecPrevPos = particle->m_vecPosition - particle->m_vecVelocity;
                                float fRotation;
                                float fTrailLength;
                                CVector vecScreenPosition;

                                if (CSprite2::CalcScreenCoors(vecPrevPos, vecScreenPosition, fTrailLength, fRotation, true, lim))
                                {
                                    CVector2D vecDist
                                    (
                                        coors.x - vecScreenPosition.x,
                                        coors.y - vecScreenPosition.y
                                    );

                                    float fDist = vecDist.Magnitude();

                                    fTrailLength = fDist;

                                    float fRot = asinf(vecDist.x / fDist);

                                    fRotation = fRot;

                                    if (vecDist.y < 0.0f)
                                        fRotation = -1.0f * fRot + DEGTORAD(180.0f);

                                    fRotation = RADTODEG(fRotation);
                                    if (fRotation < 0.0f)
                                        fRotation += 360.0f;
                                }
                                else
                                {
                                    fRotation = 0.0f;
                                    fTrailLength = 0.0f;
                                }

                                CSprite2::RenderBufferedOneXLUSprite_Rotate_Dimension(coors.x, coors.y, coors.z,
                                    particle->m_fSize * w,
                                    particle->m_fSize * h + fTrailLength * psystem->m_fTrailLengthMultiplier,
                                    particle->m_Color.red,
                                    particle->m_Color.green,
                                    particle->m_Color.blue,
                                    particle->m_nColorIntensity,
                                    1.0f / coors.z,
                                    fRotation,
                                    particle->m_nAlpha);
                            }
                            else if (psystem->Flags & VERT_TRAIL)
                            {
                                float fTrailLength = fabsf(particle->m_vecVelocity.z * 10.0f);

                                CSprite2::RenderBufferedOneXLUSprite(coors.x, coors.y, coors.z,
                                    particle->m_fSize * w,
                                    (particle->m_fSize + fTrailLength * psystem->m_fTrailLengthMultiplier) * h,
                                    particle->m_Color.red,
                                    particle->m_Color.green,
                                    particle->m_Color.blue,
                                    particle->m_nColorIntensity,
                                    1.0f / coors.z,
                                    particle->m_nAlpha);
                            }
                            else if (i == PARTICLE_RAINDROP_SMALL)
                            {
                                CSprite2::RenderBufferedOneXLUSprite(coors.x, coors.y, coors.z,
                                    particle->m_fSize * w * 0.05f,
                                    particle->m_fSize * h,
                                    particle->m_Color.red,
                                    particle->m_Color.green,
                                    particle->m_Color.blue,
                                    particle->m_nColorIntensity,
                                    1.0f / coors.z,
                                    particle->m_nAlpha);
                            }
                            /*else if ( i == PARTICLE_BOAT_WAKE )*/
                            else if (i == PARTICLE_BOAT_WAKE)
                            {
                                CSprite2::RenderBufferedOneXLUSprite(coors.x, coors.y, coors.z,
                                    particle->m_fSize * w,
                                    psystem->m_fDefaultInitialRadius * h,
                                    particle->m_Color.red,
                                    particle->m_Color.green,
                                    particle->m_Color.blue,
                                    particle->m_nColorIntensity,
                                    1.0f / coors.z,
                                    particle->m_nAlpha);
                            }
                            else
                            {
                                CSprite2::RenderBufferedOneXLUSprite(coors.x, coors.y, coors.z,
                                    particle->m_fSize * w,
                                    particle->m_fSize * h,
                                    particle->m_Color.red,
                                    particle->m_Color.green,
                                    particle->m_Color.blue,
                                    particle->m_nColorIntensity,
                                    1.0f / coors.z,
                                    particle->m_nAlpha);
                            }
                        }
                    }
                }
            }

            particle = particle->m_pNext;
        }

        CSprite2::FlushSpriteBuffer();

    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

    //POP_RENDERGROUP();
}

void CParticle::RemovePSystem(tParticleType type)
{
    tParticleSystemData* psystemdata = &mod_ParticleSystemManager.m_aParticles[type];

    for (CParticle* particle = psystemdata->m_pParticles; particle; particle = psystemdata->m_pParticles)
        RemoveParticle(particle, nullptr, psystemdata);
    log("Particle system %s was removed", psystemdata->m_aName);
    debug("Particle system %s was removed", psystemdata->m_aName);
}

void CParticle::RemoveParticle(CParticle* pParticle, CParticle* pPrevParticle, tParticleSystemData* pPSystemData)
{
    if (pPSystemData->m_Type == PARTICLE_WATERDROP)
        --numWaterDropOnScreen;

    if (pPrevParticle)
        pPrevParticle->m_pNext = pParticle->m_pNext;
    else
        pPSystemData->m_pParticles = pParticle->m_pNext;

    pParticle->m_pNext = m_pUnusedListHead;
    m_pUnusedListHead = pParticle;
}

void CParticle::AddJetExplosion(CVector const& vecPos, float fPower, float fSize)
{
    RwRGBA color(240, 240, 240, 255);

    if (fPower < 1.0f)
        fPower = 1.0f;

    CVector vecRandOffset
    (
        CGeneral::GetRandomNumberInRange(-0.4f, 0.4f),
        CGeneral::GetRandomNumberInRange(-0.4f, 0.4f),
        CGeneral::GetRandomNumberInRange(0.1f, 0.3f)
    );

    vecRandOffset *= 2.0f;

    CVector vecStepPos = vecPos;

    for (int32_t i = 0; i < int32_t(fPower * 4.0f); i++)
    {
        AddParticle(PARTICLE_EXPLOSION_MFAST,
            vecStepPos,
            CVector
            (
                CGeneral::GetRandomNumberInRange(-0.02f, 0.02f),
                CGeneral::GetRandomNumberInRange(-0.02f, 0.02f),
                CGeneral::GetRandomNumberInRange(-0.02f, 0.0f)
            ),
            nullptr,
            fSize, color, 0, 0, 0, 0);

        AddParticle(PARTICLE_EXPLOSION_MFAST,
            vecStepPos,
            CVector
            (
                CGeneral::GetRandomNumberInRange(-0.04f, 0.04f),
                CGeneral::GetRandomNumberInRange(-0.04f, 0.04f),
                CGeneral::GetRandomNumberInRange(0.0f, 0.07f)
            ),
            nullptr,
            fSize, color, 0, 0, 0, 0);

        AddParticle(PARTICLE_EXPLOSION_MFAST,
            vecStepPos,
            CVector
            (
                CGeneral::GetRandomNumberInRange(-0.04f, 0.04f),
                CGeneral::GetRandomNumberInRange(-0.04f, 0.04f),
                CGeneral::GetRandomNumberInRange(0.0f, 0.07f)
            ),
            nullptr,
            fSize, color, 0, 0, 0, 0);

        vecStepPos += vecRandOffset;
    }
}
CMatrix Matrix::Inverted() const {
    CMatrix o;

    // Transpose rotation
    o.right = CVector{ right.x, GetForward().x, up.x };
    o.GetForward() = CVector{ right.y, GetForward().y, up.y };
    o.up = CVector{ right.z, GetForward().z, up.z };

    // Transform translation using the calculated rotation matrix
    o.pos = -((Vec)((Matrix)o).TransformVector(pos));

    return o;
}
CMatrix& Invert(CMatrix& in, CMatrix& out)
{
    out = ((Matrix)in).Inverted();
    return out;
}

CMatrix Invert(const CMatrix& in)
{
    return ((Matrix)in).Inverted();
}
void CParticle::AddYardieDoorSmoke(CVector const& vecPos, CMatrix const& matMatrix)
{
    RwRGBA color(0, 0, 0, 0);

    CMatrix invMat(Invert(matMatrix));

    CVector vecBasePos = matMatrix * (invMat * vecPos + CVector(0.0f, -1.0f, 0.5f));

    for (int32_t i = 0; i < 5; i++)
    {
        CVector pos = vecBasePos;

        pos.x += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
        pos.y += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);

        AddParticle(PARTICLE_CARCOLLISION_DUST,
            pos,
            CVector(0.0f, 0.0f, 0.0f),
            nullptr,
            0.3f, color, 0, 0, 0, 0);
    }
}

void CParticle::CalWindDir(CVector* vecDirIn, CVector* vecDirOut)
{
    vecDirOut->x = (Cos(128) * vecDirIn->x) + (Sin(128) * vecDirIn->y);

    vecDirOut->x = (Cos(128) * vecDirIn->x) + (Sin(128) * vecDirIn->y) * CWeather::Wind;
    vecDirOut->y = (Sin(128) * vecDirIn->x) - (Cos(128) * vecDirIn->y) * CWeather::Wind;
}

void CParticle::HandleShipsAtHorizonStuff()
{
    tParticleSystemData* psystemdata = &mod_ParticleSystemManager.m_aParticles[PARTICLE_SHIP_SIDE];

    for (CParticle* particle = psystemdata->m_pParticles; particle; particle = particle->m_pNext)
    {
        if (CTimer::m_snTimeInMilliseconds > particle->m_nTimeWhenWillBeDestroyed - 32000
            && CTimer::m_snTimeInMilliseconds < particle->m_nTimeWhenWillBeDestroyed - 22000)
        {
            particle->m_nAlpha = min(particle->m_nAlpha + 1, 96);
        }
        if (CTimer::m_snTimeInMilliseconds > particle->m_nTimeWhenWillBeDestroyed - 10000)
            particle->m_nFadeAlphaTimer = 1;
    }
}

void HandleWaterSparks()
{
    tParticleSystemData* psystemdata = &mod_ParticleSystemManager.m_aParticles[PARTICLE_WATER_SPARK];

    for (CParticle* particle = psystemdata->m_pParticles; particle; particle = particle->m_pNext)
    {
        float intens = 0.8f * 1.0f * CWeather::SunGlare;
        int r = (intens * (CTimeCycle::m_CurrentColours.m_nSunCoronaRed + 2 * 255) / 3.0f);
        int g = (intens * (CTimeCycle::m_CurrentColours.m_nSunCoronaGreen + 2 * 255) / 3.0f);
        int b = (intens * (CTimeCycle::m_CurrentColours.m_nSunCoronaBlue + 2 * 255) / 3.0f);
        RwRGBA color = { r, g, b, 255 };

        particle->m_Color = color;
    }
}


void CParticle::HandleShootableBirdsStuff(CEntity* entity, CVector const& camPos)
{
    float fHeadingRad = entity->GetForward().Heading();
    float fHeading = RADTODEG(fHeadingRad);
    float fBirdAngle = Cos(DEGTORAD(1.5f));

    tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[PARTICLE_BIRD_FRONT];
    CParticle* particle = psystem->m_pParticles;
    CParticle* prevParticle = NULL;
    bool bRemoveParticle;
    if (particle) {
        for (; particle != NULL; _Next(particle, prevParticle, psystem, bRemoveParticle))
        {
            bRemoveParticle = false;

            CVector2D vecPos(particle->m_vecPosition.x, particle->m_vecPosition.y);
            CVector2D vecCamPos(camPos.x, camPos.y);

            CVector2D vecDist = vecPos - vecCamPos;
            static_cast<Vec2D>(vecDist).Normalise();

            float fHead = DEGTORAD(fHeading);

            CVector2D vecDir(-Sin(fHead), Cos(fHead));
            static_cast<Vec2D>(vecDir).Normalise();

            float fDot = DotProduct2D(vecDir, vecDist);

            if (fDot > 0.0f && fDot > fBirdAngle)
            {
                if (static_cast<Vec>(((camPos - particle->m_vecPosition))).MagnitudeSqr() < 40000.0f)
                {
                    //CStats::SeagullsKilled++;

                    bRemoveParticle = true;

                    for (int32_t i = 0; i < 8; i++)
                    {
                        CParticle* pBirdDerbis = AddParticle(PARTICLE_BIRD_DEBRIS,
                            particle->m_vecPosition,
                            CVector
                            (
                                CGeneral::GetRandomNumberInRange(-3.0f, 3.0f),
                                CGeneral::GetRandomNumberInRange(-3.0f, 3.0f),
                                CGeneral::GetRandomNumberInRange(-3.0f, 3.0f)
                            ),
                            NULL,
                            0.3f,
                            particle->m_Color,
                            CGeneral::GetRandomNumberInRange(20, 40),
                            0,
                            GetRandomNumber() & 3,
                            200);
                        if (pBirdDerbis)
                            pBirdDerbis->m_nAlpha = particle->m_nAlpha;
                    }
                }
            }
        }
    }
}
void CParticle::HandleShootableBirdsStuff2(CEntity* entity, const CVector* pointA, const CVector* pointB) //CVector const& camPos)
{
    // TRACE("START BIRBS");
    // float fBirdAngle = Cos(DEGTORAD(1.5f));

    tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[PARTICLE_BIRD_FRONT];
    CParticle* particle = psystem->m_pParticles;
    CParticle* prevParticle = nullptr;
    bool bRemoveParticle;
    if (particle) {
        for (; particle != nullptr; _Next(particle, prevParticle, psystem, bRemoveParticle))
        {
            bRemoveParticle = false;

            // CVector2D vecPos(particle->m_vecPosition.x, particle->m_vecPosition.y);
           //  CColLine line{ *pointA, *pointB };
           //  CColSphere sphere{ particle->m_vecPosition, 0.5f };
            CColPoint cp{};
            CEntity* ent{};
            // ((CSphere)sphere).Set(0.5f, particle->m_vecPosition);
            if (CWorld::ProcessLineOfSight(entity->GetPosition(), particle->m_vecPosition, cp, ent, true, true, true, true, false, false, false, false)) {

                bRemoveParticle = true;

                for (int32_t i = 0; i < 8; i++)
                {
                    CParticle* pBirdDerbis = AddParticle(PARTICLE_BIRD_DEBRIS,
                        particle->m_vecPosition,
                        CVector
                        (
                            CGeneral::GetRandomNumberInRange(-3.0f, 3.0f),
                            CGeneral::GetRandomNumberInRange(-3.0f, 3.0f),
                            CGeneral::GetRandomNumberInRange(-3.0f, 3.0f)
                        ),
                        nullptr,
                        0.3f,
                        particle->m_Color,
                        CGeneral::GetRandomNumberInRange(20, 40),
                        0,
                        GetRandomNumber() & 3,
                        200);
                    log("The bird was shot :(");
                    debug("The bird was shot :(");
                    if (pBirdDerbis) {
                        pBirdDerbis->m_nAlpha = particle->m_nAlpha;
                    }
                    // TRACE("END BIRBS");
                     //}
                     //}
                 //}
                }
            }
        }
    }
}

/*void
CEntity::AddSteamsFromGround(CVector* unused)
{
    int i, n;
    C2dEffect* effect;
    CVector pos;

    n = CModelInfo::GetModelInfo(GetModelIndex())->GetNum2dEffects();
    for (i = 0; i < n; i++) {
        effect = CModelInfo::GetModelInfo(GetModelIndex())->Get2dEffect(i);
        if (effect->type != EFFECT_PARTICLE)
            continue;

        pos = GetMatrix() * effect->pos;
        switch (effect->particle.particleType) {
        case 0:
            CParticleObject::AddObject(POBJECT_PAVEMENT_STEAM, pos, effect->particle.dir, effect->particle.scale, false);
            break;
        case 1:
            CParticleObject::AddObject(POBJECT_WALL_STEAM, pos, effect->particle.dir, effect->particle.scale, false);
            break;
        case 2:
            CParticleObject::AddObject(POBJECT_DRY_ICE, pos, effect->particle.scale, false);
            break;
        case 3:
            CParticleObject::AddObject(POBJECT_SMALL_FIRE, pos, effect->particle.dir, effect->particle.scale, false);
            break;
        case 4:
            CParticleObject::AddObject(POBJECT_DARK_SMOKE, pos, effect->particle.dir, effect->particle.scale, false);
            break;
        case 5:
            CParticleObject::AddObject(POBJECT_WATER_FOUNTAIN_VERT, pos, effect->particle.dir, effect->particle.scale, false);
            break;
        case 6:
            CParticleObject::AddObject(POBJECT_WATER_FOUNTAIN_HORIZ, pos, effect->particle.dir, effect->particle.scale, false);
            break;
        }
    }
}*/
void
RenderSeaBirds()
{
    CVector cur_pos = TheCamera.GetPosition();

    if (!CCullZones::CamNoRain()
        && !CCullZones::PlayerNoRain() && !CGame::currArea > 0
        && (CWeather::NewWeatherType == WEATHER_SUNNY_LA
            || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_LA
            || CWeather::NewWeatherType == WEATHER_SUNNY_SF
            || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_SF
            || CWeather::NewWeatherType == WEATHER_SUNNY_VEGAS
            || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_VEGAS
            || CWeather::NewWeatherType == WEATHER_SUNNY_COUNTRYSIDE
            || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_COUNTRYSIDE
            || CWeather::NewWeatherType == WEATHER_SUNNY_DESERT
            || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_DESERT))
        /* && CClock::ms_nGameClockHours > 6 && CClock::ms_nGameClockHours < 20)*/
    {
        static CVector prev_pos(0.0f, 0.0f, 0.0f);
        static CVector prev_front(0.0f, 0.0f, 0.0f);
        static int32_t timecounter;

        if (fabsf(prev_pos.x - cur_pos.x) + fabsf(prev_pos.y - cur_pos.y) + fabsf(prev_pos.z - cur_pos.z) > 1.5f)
        {
            prev_pos = cur_pos;
            timecounter = CTimer::m_snTimeInMilliseconds;
        }
        else if ((CTimer::m_snTimeInMilliseconds - timecounter) > 5000)
        {
            static int32_t birdgenTime = 0;

            if ((CTimer::m_snTimeInMilliseconds - birdgenTime) > 1000)
            {
                birdgenTime = CTimer::m_snTimeInMilliseconds;

                CVector vecPos = cur_pos;

                float fAngle = CGeneral::GetRandomNumberInRange(90.0f, 150.0f);

                uint16_t nSinCosIdx = GetRandomNumber() % (CParticle::SIN_COS_TABLE_SIZE - 1);

                float fCos = CParticle::Cos(nSinCosIdx);
                float fSin = CParticle::Sin(nSinCosIdx);

                vecPos.x += (fCos - fSin) * fAngle;
                vecPos.y += (fSin + fCos) * fAngle;
                vecPos.z += CGeneral::GetRandomNumberInRange(10.0f, 30.0f);

                CVector vecDir(CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
                    CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
                    0.0f);

                CParticle::AddParticle(PARTICLE_BIRD_FRONT, vecPos, vecDir, nullptr, 0.0f, 0, 0, 0, 0);
            }
        }
    }
}

bool
IsLocationOutOfWorldBounds_WS(CVector const& vecPosn, int nOffset)
{
    int32_t x = int32_t((vecPosn.x / 50.0f) + 48.0f);
    int32_t y = int32_t((vecPosn.y / 50.0f) + 40.0f);

    return x < nOffset || x >= 80 - nOffset || y < nOffset || y >= 80 - nOffset;
}


void
RenderShipsOnHorizon()
{
#ifdef FIX_BUGS
    CVector cur_pos = FindPlayerCoors(-1);
#else
    CVector cur_pos = FindPlayerPed()->GetPosition();
#endif

    static CVector prev_pos(0.0f, 0.0f, 0.0f);
    static CVector prev_front(0.0f, 0.0f, 0.0f);
    static int32_t timecounter;

    if (fabsf(prev_pos.x - cur_pos.x) + fabsf(prev_pos.y - cur_pos.y) + fabsf(prev_pos.z - cur_pos.z) > 1.5f)
    {
        prev_pos = cur_pos;
        timecounter = CTimer::m_snTimeInMilliseconds;
    }
    else if ((CTimer::m_snTimeInMilliseconds - timecounter) > 5000)
    {
        static int32_t shipgenTime = 0;

        if ((CTimer::m_snTimeInMilliseconds - shipgenTime) > 4000)
        {
            shipgenTime = CTimer::m_snTimeInMilliseconds;

            CVector vecPos = cur_pos;

            float fAngle = CGeneral::GetRandomNumberInRange(450.0f, 750.0f);

            uint16_t nSinCosIdx = GetRandomNumber() % (CParticle::SIN_COS_TABLE_SIZE - 1);

            float fCos = CParticle::Cos(nSinCosIdx);
            float fSin = CParticle::Sin(nSinCosIdx);

            vecPos.x += (fCos - fSin) * fAngle;
            vecPos.y += (fSin + fCos) * fAngle;

            float fLevelNoWaves;

            if (CWaterLevel::GetWaterLevelNoWaves(vecPos.x, vecPos.y, vecPos.z, &fLevelNoWaves, nullptr, nullptr))
            {
                if (IsLocationOutOfWorldBounds_WS(vecPos, 1))
                {
                    vecPos.z = fLevelNoWaves + 9.5f;

                    CVector vecDir
                    (
                        CGeneral::GetRandomNumberInRange(-0.1f, 0.1f),
                        0.0f,
                        0.0f
                    );

                    CParticle::AddParticle(PARTICLE_SHIP_SIDE, vecPos, vecDir,
                        nullptr, 0.0f, 0, 0, GetRandomNumber() & 7, 0);
                }
            }
        }
    }
}

static int startStreamAfterRain;
int32_t StreamAfterRainTimer;
void AddStreamAfterRain() // Steam after & during rain
{
    if (CClock::ms_nGameClockHours > 6 && CClock::ms_nGameClockHours < 18) {
        RwRGBA colour = { 255, 255, 255, 24 };
        CVector pos = TheCamera.m_pTargetEntity ? TheCamera.m_pTargetEntity->GetPosition() : TheCamera.GetPosition();
        bool foundGround;
        float groundZ = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z, &foundGround, nullptr) + 0.2f;
        if (!foundGround)
            groundZ = pos.z + 0.75f;
        for (int i = 0; i < 20; i++) {
            float dist = (GetRandomNumber() & 0xFF) / 255.0f +
                CGeneral::GetRandomNumberInRange(-10.0f, 30.0f);
            float angle;
            uint8_t rnd = GetRandomNumber();
            if (rnd & 1)
                angle = (GetRandomNumber() & 0x7F) / 128.0f * TWOPI;
            else
                angle = TheCamera.m_fOrientation + (rnd - 128) / 160.0f;
            pos.x = TheCamera.GetPosition().x + dist * sinf(angle);
            pos.y = TheCamera.GetPosition().y + dist * cosf(angle);
            pos.z = groundZ;
            CParticle::AddParticle(PARTICLE_GROUND_STEAM, pos, CVector(0.0f, 0.0f, 0.015f), nullptr, 0.0f, colour);
        }
    }
    else {
        startStreamAfterRain = 0;
        StreamAfterRainTimer = 800;
    }
}

void AddSplashesDuringHurricane()
{
    RwRGBA colour = { 255, 255, 255, 32 };
    CVector pos = TheCamera.m_pTargetEntity ? TheCamera.m_pTargetEntity->GetPosition() : TheCamera.GetPosition();
    bool foundGround;
    float groundZ = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z, &foundGround, nullptr) + 0.1f;
    if (!foundGround)
        groundZ = pos.z + 0.5f;
    for (int i = 0; i < 20; i++) {
        float dist = (GetRandomNumber() & 0xFF) / 255.0f +
            CGeneral::GetRandomNumberInRange(-10.0f, 30.0f);
        float angle;
        uint8_t rnd = GetRandomNumber();
        if (rnd & 1)
            angle = (GetRandomNumber() & 0x7F) / 128.0f * TWOPI;
        else
            angle = TheCamera.m_fOrientation + (rnd - 128) / 160.0f;
        pos.x = TheCamera.GetPosition().x + dist * sinf(angle);
        pos.y = TheCamera.GetPosition().y + dist * cosf(angle);
        pos.z = groundZ;
        if (foundGround)
            CParticle::AddParticle(PARTICLE_GROUND_STEAM, pos, CVector(-0.002f, -0.002f, 0.015f), nullptr, 0.0f, colour);
    }
}

void AddHeatHaze()
{
    if (TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode == MODE_TOPDOWN ||
        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode == MODE_TOP_DOWN_PED)
        return;
    CVector pos;
    pos.x = SCREEN_WIDTH * 0.5f;
    if (TheCamera.GetLookingForwardFirstPerson())
        pos.y = CGeneral::GetRandomNumberInRange(SCREEN_HEIGHT * 0.25f, SCREEN_HEIGHT * 0.9f);
    else
        pos.y = CGeneral::GetRandomNumberInRange(SCREEN_HEIGHT * 0.4f, SCREEN_HEIGHT * 0.9f);
    pos.z = 100.0f;
    CParticle::AddParticle(PARTICLE_HEATHAZE_IN_DIST, pos, CVector(0.0f, 0.0f, 0.0f));
}

void AddSandStormParticles() {
    if (CGame::currArea > 0)
        return;
    CVector position = TheCamera.GetPosition();
    position.x += TheCamera.m_mCameraMatrix.GetForward().x * 10.0f;
    position.y += TheCamera.m_mCameraMatrix.GetForward().y * 10.0f;

    position.x += CGeneral::GetRandomNumberInRange(0.0f, 40.0f) - 20.0f;
    position.y += CGeneral::GetRandomNumberInRange(0.0f, 40.0f) - 20.0f;
    position.z += CGeneral::GetRandomNumberInRange(0.0f, 7.00f) - 2.00f;
    CVector vecDir
    (
        CWeather::WindDir.x,
        CWeather::WindDir.y,
        CWeather::WindDir.z
    );
    CVector velocity = CWeather::WindDir * 25.0f;
    RwRGBA color = { 67, 65, 55, 255 };
    CParticle::AddParticle(PARTICLE_SAND, position, vecDir, nullptr, 4.5f, color);
    //g_fx.m_pPrtSand2->AddParticle(&position, &velocity, 0.0f, &prtInfo, -1.0f, 1.2f, 0.6f, 0);
}

void RenderOneRainStreak(CVector pos, CVector unused, int intensity, bool scale, float distance)
{
    static float RandomTex;
    static float RandomTexX;
    static float RandomTexY;
    TempBufferRenderIndexList[TempBufferIndicesStored + 0] = TempBufferVerticesStored + 0;
    TempBufferRenderIndexList[TempBufferIndicesStored + 1] = TempBufferVerticesStored + 2;
    TempBufferRenderIndexList[TempBufferIndicesStored + 2] = TempBufferVerticesStored + 1;
    TempBufferRenderIndexList[TempBufferIndicesStored + 3] = TempBufferVerticesStored + 0;
    TempBufferRenderIndexList[TempBufferIndicesStored + 4] = TempBufferVerticesStored + 3;
    TempBufferRenderIndexList[TempBufferIndicesStored + 5] = TempBufferVerticesStored + 2;
    TempBufferRenderIndexList[TempBufferIndicesStored + 6] = TempBufferVerticesStored + 1;
    TempBufferRenderIndexList[TempBufferIndicesStored + 7] = TempBufferVerticesStored + 2;
    TempBufferRenderIndexList[TempBufferIndicesStored + 8] = TempBufferVerticesStored + 4;
    TempBufferRenderIndexList[TempBufferIndicesStored + 9] = TempBufferVerticesStored + 2;
    TempBufferRenderIndexList[TempBufferIndicesStored + 10] = TempBufferVerticesStored + 3;
    TempBufferRenderIndexList[TempBufferIndicesStored + 11] = TempBufferVerticesStored + 4;
    RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 0], 0, 0, 0, 0);
    RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 0], pos.x + 11.0f * TheCamera.GetUp().x, pos.y + 11.0f * TheCamera.GetUp().y, pos.z + 11.0f * TheCamera.GetUp().z);
    RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 1], 0, 0, 0, 0);
    RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 1], pos.x - 9.0f * TheCamera.GetRight().x, pos.y - 9.0f * TheCamera.GetRight().y, pos.z - 9.0f * TheCamera.GetRight().z);
    RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 2], RAIN_COLOUR_R * intensity / 256, RAIN_COLOUR_G * intensity / 256, RAIN_COLOUR_B * intensity / 256, RAIN_ALPHA);
    RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 2], pos.x, pos.y, pos.z);
    RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 3], 0, 0, 0, 0);
    RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 3], pos.x + 9.0f * TheCamera.GetRight().x, pos.y + 9.0f * TheCamera.GetRight().y, pos.z + 9.0f * TheCamera.GetRight().z);
    RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 4], 0, 0, 0, 0);
    RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 4], pos.x - 11.0f * TheCamera.GetUp().x, pos.y - 11.0f * TheCamera.GetUp().y, pos.z - 11.0f * TheCamera.GetUp().z);
    float u = STREAK_U;
    float v = STREAK_V;
    if (scale) {
        u *= LARGE_STREAK_COEFFICIENT;
        v *= LARGE_STREAK_COEFFICIENT;
    }
    float distance_coefficient;
    if (distance < STREAK_MIN_DISTANCE)
        distance_coefficient = 1.0f;
    else if (distance > STREAK_MAX_DISTANCE)
        distance_coefficient = 0.5f;
    else
        distance_coefficient = 1.0f - 0.5f * (distance - STREAK_MIN_DISTANCE) / (STREAK_MAX_DISTANCE - STREAK_MIN_DISTANCE);
    u *= distance_coefficient;
    v *= distance_coefficient;
    if (!CTimer::m_UserPause || !CTimer::m_CodePause) {
        if (GTAIIIStyleRain) {
            RandomTex = ((GetRandomNumber() & 255) - 128) * 0.01f;

            RandomTexX = (GetRandomNumber() & 127) * 0.01f;

            RandomTexY = (GetRandomNumber() & 127) * 0.01f;
        }
        else {
            RandomTex = 0.0f;

            RandomTexX = 0.0f;

            RandomTexY = 0.0f;
        }
    }
    RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 0], 0.5f * u - RandomTex + RandomTexX);
    RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 0], -v * 0.5f + RandomTexY);
    RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 1], RandomTexX);
    RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 1], RandomTexY);
    RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 2], 0.5f * u + RandomTexX);
    RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 2], RandomTexY);
    RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 3], u + RandomTexX);
    RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 3], RandomTexY);
    RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 4], 0.5f * u + RandomTex + RandomTexX);
    RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 5], 0.5f * v + RandomTexY);
    TempBufferIndicesStored += 12;
    TempBufferVerticesStored += 5;
}


void RenderRainStreaks(void)
{
    if (CTimer::m_UserPause || CTimer::m_CodePause || CGame::currArea > 0)
        return;
    int base_intensity = (64.0f - CTimeCycle::m_FogReduction) / 64.0f * int(255 * CWeather::Rain);
    if (base_intensity == 0)
        return;
    if (GTAIIIStyleRain) {
        if (TheCamera.m_fCameraAverageSpeed > 1.75f)
            return;
    }
    else {
        // Nothing, since VC doesn't have this check
    }
    TempBufferIndicesStored = 0;
    TempBufferVerticesStored = 0;
    for (int i = 0; i < NUM_RAIN_STREAKS; i++) {
        if (Streaks[i].timer) {
            float secondsElapsed = (CTimer::m_snTimeInMilliseconds - Streaks[i].timer) / 1024.0f;
            if (secondsElapsed > STREAK_LIFETIME)
                Streaks[i].timer = 0;
            else {
                int intensity;
                if (GTAIIIStyleRain) {
                    if (secondsElapsed < STREAK_INTEROLATION_TIME)
                        intensity = base_intensity * 0.5f * secondsElapsed / STREAK_INTEROLATION_TIME;
                    else if (secondsElapsed > (STREAK_LIFETIME - STREAK_INTEROLATION_TIME))
                        intensity = (STREAK_LIFETIME - secondsElapsed) * 0.5f * base_intensity / STREAK_INTEROLATION_TIME;
                    else
                        intensity = base_intensity * 0.5f;
                }
                else {
                    if (secondsElapsed < STREAK_INTEROLATION_TIME)
                        intensity = base_intensity * 0.25f * secondsElapsed / STREAK_INTEROLATION_TIME;
                    else if (secondsElapsed > (STREAK_LIFETIME - STREAK_INTEROLATION_TIME))
                        intensity = (STREAK_LIFETIME - secondsElapsed) * 0.25f * base_intensity / STREAK_INTEROLATION_TIME;
                    else
                        intensity = base_intensity * 0.25f;
                }
                CVector dir = Streaks[i].direction;
                dir.Normalise();
                CVector pos = Streaks[i].position + secondsElapsed * Streaks[i].direction;
                RenderOneRainStreak(pos, dir, intensity, false, (pos - TheCamera.GetPosition()).Magnitude());
#ifndef FIX_BUGS // remove useless code
                if (secondsElapsed > 1.0f && secondsElapsed < STREAK_LIFETIME - 1.0f) {
                    GetRandomNumber(), GetRandomNumber();
                }
#endif
            }
        }
        else if ((GetRandomNumber() & 0xF00) == 0) {
            // 1/16 probability
            if (GTAIIIStyleRain) {
                Streaks[i].direction = CVector(4.0f, 4.0f, -4.0f); // CVector(0.0f, 0.0f, -12.0f);
            }
            else {
                Streaks[i].direction = CVector(0.0f, 0.0f, -12.0f);
            }
            Streaks[i].position = 6.0f * TheCamera.GetForward() + TheCamera.GetPosition() + CVector(-1.8f * Streaks[i].direction.x, -1.8f * Streaks[i].direction.y, 8.0f);
            if (!CCutsceneMgr::ms_running) {
                Streaks[i].position.x += 2.0f * FindPlayerSpeed(-1).x * 60.0f;
                Streaks[i].position.y += 2.0f * FindPlayerSpeed(-1).y * 60.0f;
            }
            else
                Streaks[i].position += (TheCamera.GetPosition() - TheCamera.m_vecRealPreviousCameraPosition) * 20.0f;
            if (GTAIIIStyleRain) {
                Streaks[i].position.x += ((GetRandomNumber() & 255) - 128) * 0.08f;
                Streaks[i].position.y += ((GetRandomNumber() & 255) - 128) * 0.08f;
            }
            else {
                Streaks[i].position.x += ((GetRandomNumber() & 255) - 128) * 0.04f;
                Streaks[i].position.y += ((GetRandomNumber() & 255) - 128) * 0.04f;
            }
            Streaks[i].timer = CTimer::m_snTimeInMilliseconds;
        }
    }
    if (TempBufferIndicesStored) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
        RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void*)rwFOGTYPELINEAR);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(gpRainDropTex));
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
        if (RwIm3DTransform(TempBufferRenderVertices, TempBufferVerticesStored, nullptr, 1))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, TempBufferRenderIndexList, TempBufferIndicesStored);
            RwIm3DEnd();
        }
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
    }
    TempBufferVerticesStored = 0;
    TempBufferIndicesStored = 0;
}

void AddRain()
{
    if (CCullZones::CamNoRain() || CCullZones::PlayerNoRain() || CGame::currArea > 0)
        return;
    if (TheCamera.GetLookingLRBFirstPerson()) {
        CVehicle* pVehicle = FindPlayerVehicle(-1, false);
        if (pVehicle && pVehicle->CarHasRoof()) {
            CParticle::RemovePSystem(PARTICLE_RAINDROP_2D);
            return;
        }
    }

    if (CWeather::Rain > 0.0f) {
        startStreamAfterRain = 1;
        StreamAfterRainTimer = 800;
    }
    else if (startStreamAfterRain) {
        if (StreamAfterRainTimer > 0 && SteamAfterRain) {
            AddStreamAfterRain();
            StreamAfterRainTimer--;
        }
        else {
            startStreamAfterRain = 0;
            StreamAfterRainTimer = 800;
        }
    }

    if (CWeather::Rain > 0.5f && SteamDuringRain)
        AddSplashesDuringHurricane();

    if (CWeather::Rain <= 0.1f)
        return;
    static RwRGBA colour;
    int numDrops = 5.0f * CWeather::Rain;
    int numSplashes = 2.0f * CWeather::Rain;
    CVector pos, dir;
    for (int i = 0; i < numDrops; i++) {
        pos.x = CGeneral::GetRandomNumberInRange(0, (int)SCREEN_WIDTH);
        pos.y = CGeneral::GetRandomNumberInRange(0, (int)SCREEN_HEIGHT / 5);
        pos.z = 0.0f;
        dir.x = 0.0f;
        dir.y = CGeneral::GetRandomNumberInRange(30.0f, 40.0f);
        dir.z = 0.0f;
        CParticle::AddParticle(PARTICLE_RAINDROP_2D, pos, dir, nullptr, CGeneral::GetRandomNumberInRange(0.1f, 0.75f));

        pos.x = CGeneral::GetRandomNumberInRange(0, (int)SCREEN_WIDTH);
        pos.y = CGeneral::GetRandomNumberInRange((int)SCREEN_HEIGHT / 5, (int)SCREEN_HEIGHT / 2);
        pos.z = 0.0f;
        dir.x = 0.0f;
        dir.y = CGeneral::GetRandomNumberInRange(30.0f, 40.0f);
        dir.z = 0.0f;
        CParticle::AddParticle(PARTICLE_RAINDROP_2D, pos, dir, nullptr, CGeneral::GetRandomNumberInRange(0.1f, 0.75f));

        pos.x = CGeneral::GetRandomNumberInRange(0, (int)SCREEN_WIDTH);
        pos.y = 0.0f;
        pos.z = 0.0f;
        dir.x = 0.0f;
        dir.y = CGeneral::GetRandomNumberInRange(30.0f, 40.0f);
        dir.z = 0.0f;
        CParticle::AddParticle(PARTICLE_RAINDROP_2D, pos, dir, nullptr, CGeneral::GetRandomNumberInRange(0.1f, 0.75f));

        float dist = CGeneral::GetRandomNumberInRange(0.0f, max(10.0f * CWeather::Rain, 40.0f) / 2.0f);
        float angle;
        uint8_t rnd = GetRandomNumber();
        if (rnd & 1)
            angle = (GetRandomNumber() & 0x7F) / 128.0f * TWOPI;
        else
            angle = TheCamera.m_fOrientation + (rnd - 128) / 160.0f;
        pos.x = TheCamera.GetPosition().x + dist * sinf(angle);
        pos.y = TheCamera.GetPosition().y + dist * cosf(angle);
        pos.z = 0.0f;
        CColPoint point;
        CEntity* ent;
        if (CWorld::ProcessVerticalLine(pos + CVector(0.0f, 0.0f, 40.0f), -40.0f, point, ent, true, false, false, false, true, false, nullptr)) {
            pos.z = point.m_vecPoint.z;
            for (int j = 0; j < numSplashes + 15; j++) {
                CVector pos2 = pos;
                pos2.x += CGeneral::GetRandomNumberInRange(-15.0f, 15.0f);
                pos2.y += CGeneral::GetRandomNumberInRange(-15.0f, 15.0f);
                if (GetRandomNumber() & 1)
                    CParticle::AddParticle(PARTICLE_RAIN_SPLASH, pos2, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, colour);
                else
                    CParticle::AddParticle(PARTICLE_RAIN_SPLASHUP, pos2, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f, colour);
            }
        }
    }
}
void AddBeastie()
{
    if (FindPlayerVehicle(-1, false) || CTimer::m_FrameCounter % 10 || (GetRandomNumber() & 5) == 0)
        return;
    CVector pos = TheCamera.GetPosition();
    float dist = CGeneral::GetRandomNumberInRange(90.0f, 60.0f);
    int angle = GetRandomNumber() % CParticle::SIN_COS_TABLE_SIZE;
    float c = CParticle::m_CosTable[angle];
    float s = CParticle::m_SinTable[angle];
    pos.x += dist * (c - s);
    pos.y += dist * (c + s);
    pos.z += CGeneral::GetRandomNumberInRange(7.5f, 30.0f);
    CParticle::AddParticle(PARTICLE_BEASTIE, pos, CVector(0.0f, 0.0f, 0.0f));
}

void RenderWaterSparks()
{
    CVector pos = FindPlayerCoors(-1);
    if (CWeather::SunGlare > 0.1f) {
        for (int32_t i = 0; i < 3; i++) {
            float waterLevel;
            if (CWaterLevel::GetWaterLevelNoWaves(pos.x, pos.y, pos.z, &waterLevel, nullptr, nullptr)) {
                float dist = (GetRandomNumber() & 0xFF) / 255.0f + CGeneral::GetRandomNumberInRange(-10.0f, 30.0f);
                float angle;
                int32_t rnd = GetRandomNumber();
                if (rnd & 1)
                    angle = (GetRandomNumber() & 0x7F) / 128.0f * TWOPI;
                else
                    angle = TheCamera.m_fOrientation + (rnd - 128) / 160.0f;
                pos.x = TheCamera.GetPosition().x + dist * CParticle::Sin(angle);
                pos.y = TheCamera.GetPosition().y + dist * CParticle::Cos(angle);
                int32_t randval = GetRandomNumber();
                pos.z = waterLevel;
                for (int32_t j = 0; j < 7; j++)
                {
                    {
                        switch ((i + j + randval) & 3)
                        {
                        case 0:
                        {
                            CVector vecPos(
                                pos.x,
                                pos.y,
                                pos.z + 0.12f
                            );
                            vecPos -= 0.05f * TheCamera.GetForward();

                            CParticle::AddParticle(PARTICLE_WATER_SPARK,
                                vecPos,
                                CVector(0.0f, 0.0f, 0.0f),
                                NULL,
                                0.0f,
                                15,
                                CGeneral::GetRandomNumberInRange(-90, 90),
                                0,
                                0);
                        }
                        break;

                        case 1:
                        {
                            CVector vecPos(
                                pos.x,
                                pos.y,
                                pos.z + 0.12f
                            );
                            vecPos -= 0.05f * TheCamera.GetForward();

                            CParticle::AddParticle(PARTICLE_WATER_SPARK,
                                vecPos,
                                CVector(0.0f, 0.0f, 0.0f),
                                NULL,
                                0.0f,
                                15,
                                CGeneral::GetRandomNumberInRange(-90, 90),
                                0,
                                0);
                        }
                        break;

                        case 2:
                        {
                            CVector vecPos(
                                pos.x,
                                pos.y,
                                pos.z + 0.12f
                            );
                            vecPos -= 0.05f * TheCamera.GetForward();

                            CParticle::AddParticle(PARTICLE_WATER_SPARK,
                                vecPos,
                                CVector(0.0f, 0.0f, 0.0f),
                                NULL,
                                0.0f,
                                15,
                                CGeneral::GetRandomNumberInRange(-90, 90),
                                0,
                                0);
                        }
                        break;

                        case 3:
                        {
                            CVector vecPos(
                                pos.x,
                                pos.y,
                                pos.z + 0.12f
                            );
                            vecPos -= 0.05f * TheCamera.GetForward();

                            CParticle::AddParticle(PARTICLE_WATER_SPARK,
                                vecPos,
                                CVector(0.0f, 0.0f, 0.0f),
                                NULL,
                                0.0f,
                                15,
                                CGeneral::GetRandomNumberInRange(-90, 90),
                                0,
                                0);
                        }
                        break;
                        }
                    }
                }
            }
        }
    }
}



void
CEnt::AddSteamsFromGround(CVector* unused)
{
    int i, n;
    C2dEffect* effect;
    CVector pos;

    n = CModelInfo::GetModelInfo(m_nModelIndex)->m_nNum2dEffects;
    for (i = 0; i < n; i++) {
        effect = CModelInfo::GetModelInfo(m_nModelIndex)->Get2dEffect(i);
        if (effect->m_nType != EFFECT_PARTICLE)
            continue;
        CVector flamevel;
        pos = *GetMatrix() * effect->m_vecPosn;
        if (strcmp(effect->particle.m_szName, "vent") == 0 || strcmp(effect->particle.m_szName, "vent2") == 0) {
            // Add wall steam object
            CParticleObject::AddObject(POBJECT_WALL_STEAM, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        }
        else if (strcmp(effect->particle.m_szName, "water_fountain") == 0) {
            // Add water fountain object
            CParticleObject::AddObject(POBJECT_WATER_FOUNTAIN_VERT, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        }
    }
    /*switch (effect->m_nType) {
    case 0:
        //CParticle::AddParticle(PARTICLE_STEAM_NY, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 1.0f);
        CParticleObject::AddObject(POBJECT_PAVEMENT_STEAM, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        break;
    case 1:
        //CParticle::AddParticle(PARTICLE_STEAM_NY, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 1.0f);
        CParticleObject::AddObject(POBJECT_WALL_STEAM, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        break;
    case 2:
        //CParticle::AddParticle(PARTICLE_SMOKE, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 1.0f);
        CParticleObject::AddObject(POBJECT_DRY_ICE, pos, 1.0f, false);
        break;
    case 3:
        //flamevel.x = 1.0f;
        //flamevel.y = 1.0f;
        //flamevel.z = CGeneral::GetRandomNumberInRange(0.0125f * 1.0f, 0.1f * 1.0f);
        //CParticle::AddParticle(PARTICLE_FLAME, pos, flamevel, nullptr, 1.0f);
      //  log("Flame added here: X:%4.0f Y:%4.0f Z:%4.0f", pos.x, pos.y, pos.z);
       // debug("Flame added here: X:%4.0f Y:%4.0f Z:%4.0f", pos.x, pos.y, pos.z);
        CParticleObject::AddObject(POBJECT_SMALL_FIRE, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        break;
    case 4:
        //CParticle::AddParticle(PARTICLE_STEAM_NY, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 1.0f, RwRGBA(16, 16, 16, 255));
        CParticleObject::AddObject(POBJECT_DARK_SMOKE, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        break;
    case 5:
        //CParticle::AddParticle(PARTICLE_WATER_HYDRANT, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 1.0f);
        CParticleObject::AddObject(POBJECT_WATER_FOUNTAIN_VERT, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        break;
    case 6:
        //CParticle::AddParticle(PARTICLE_CAR_SPLASH, pos, CVector(0.0f, 0.0f, 0.0f), nullptr, 1.0f);
        CParticleObject::AddObject(POBJECT_WATER_FOUNTAIN_HORIZ, pos, CVector(0.0f, 0.0f, 0.0f), 1.0f, false);
        break;
    }
}*/
}
inline void
AddSteamsFromGround(CPtrList& list)
{
    CPtrNode* pNode = list.GetNode();
    while (pNode) {
        ((CEnt*)pNode->pItem)->AddSteamsFromGround(nullptr);
        pNode = pNode->pNext;
    }
}
void
AddParticles(void)
{
    for (int32_t y = 0; y < MAX_SECTORS_Y; y++) {
        for (int32_t x = 0; x < MAX_SECTORS_X; x++) {
            CSector* pSector = GetSector(x, y);
            AddSteamsFromGround(pSector->m_buildings);
            AddSteamsFromGround(pSector->m_dummies);
        }
    }
}

// 0x6B0690
void Heli::DoHeliDustEffect(float timeConstMult, float fxMaxZMult) {
    int i;
    float angle, radius{}, ground{};
    CColPoint point;
    CEntity* entity;
    uint8_t r, g, b;
    // log("WheelSpeed[1]: %f", m_fWheelSpeed[1]);
    // log("RotorZ: %4.0f SecondRotor: %4.0f", m_fRotorZ, m_fSecondRotorZ);
    bool isHeliAndRotorsRotating = (m_nVehicleSubClass == VEHICLE_HELI && m_fWheelSpeed[1] > 0.1f);
    bool isPlane = (m_nVehicleSubClass == VEHICLE_PLANE);

    if (isHeliAndRotorsRotating || isPlane) {
        if (m_pDriver) {
            radius = (GetPosition().z - m_pDriver->GetPosition().z - 10.0f - 1.0f) * 0.3f + 10.0f;
            ground = m_pDriver->GetPosition().z;
        }
        int rnd = max(16.0f - 4.0f * CTimer::ms_fTimeStep, 2.0f);
        uint8_t surface = SURFACE_TARMAC;
        int frm = CTimer::m_FrameCounter & 7;
        float testLowZ = ground - 10.0f;
        float dustSize = 0.0f;
        float baseSize = 1.0f;
        float offset = 1.0f;	// when heli is tilted
        float particleZ = -101.0f;
        int n = 0;
        static int time = 0;
        if (m_nModelIndex == MODEL_RCGOBLIN || m_nModelIndex == MODEL_RCRAIDER) {
            radius = 3.0f;
            dustSize = 0.04f;
            baseSize = 0.07f;
            offset = 0.3f;
        }

        CVector heliPos = GetPosition();

        if (m_nType == ENTITY_TYPE_VEHICLE) {
            heliPos.x -= (heliPos.z - ground) * GetUp().x * offset * 0.5f;
            heliPos.y -= (heliPos.z - ground) * GetUp().y * offset * 0.5f;
        }

        float steamSize = 0.25f * radius * baseSize;
        float splashSize = 0.3f * radius * baseSize;

        i = 0;
        for (i = 0; i < 32 + rnd; i++) {
            angle = i * TWOPI / 32.0f;
            CVector pos(radius * cosf(angle), radius * sinf(angle), 0.0f);
            CVector dir = CVector(pos.x, pos.y, 1.0f) * 0.01f;
            pos += heliPos;
            if (i < 32 && i == 4 * frm) {
                if (CWorld::ProcessVerticalLine(pos, testLowZ, point, entity, true, false, false, false, true, false, NULL)) {
                    n = rnd;
                    particleZ = point.m_vecPoint.z;
                    surface = point.m_nSurfaceTypeB;
                }
                else
                    n = 0;

                float waterLevel = 0.0f;
                if (CWaterLevel::GetWaterLevelNoWaves(pos.x, pos.y, pos.z, &waterLevel, nullptr, nullptr) && waterLevel > particleZ) {
                    surface = SURFACE_WATER_SHALLOW;
                    n = rnd;
                    particleZ = waterLevel;
                }
            }

            if (n) {
                pos.z = particleZ;
                if (surface == SURFACE_WATER_SHALLOW) {
                    float red = (0.3 * CTimeCycle::m_CurrentColours.m_fDirectionalRed + CTimeCycle::GetAmbientRed_Obj()) * 255.0f / 4.0f;
                    float green = (0.3 * CTimeCycle::m_CurrentColours.m_fDirectionalGreen + CTimeCycle::GetAmbientGreen_Obj()) * 255.0f / 4.0f;
                    float blue = (0.3 * CTimeCycle::m_CurrentColours.m_fDirectionalBlue + CTimeCycle::GetAmbientBlue_Obj()) * 255.0f / 4.0f;
                    r = Clamp(red, 0.0f, 255.0f);
                    g = Clamp(green, 0.0f, 255.0f);
                    b = Clamp(blue, 0.0f, 255.0f);
                    RwRGBA col1 = { r, g, b, (RwUInt8)CGeneral::GetRandomNumberInRange(8, 32) };
                    RwRGBA col2 = { 255, 255, 255, 32 };

                    if (n & 1)
                        CParticle::AddParticle(PARTICLE_STEAM_NY_SLOWMOTION, pos, dir, NULL, steamSize, col2);
                    else
                        CParticle::AddParticle(PARTICLE_CAR_SPLASH, pos, dir, NULL, splashSize, col1,
                            CGeneral::GetRandomNumberInRange(0.0f, 10.0f),
                            CGeneral::GetRandomNumberInRange(0.0f, 90.0f), 1);
                }
                else {
                    switch (surface) {
                    default:
                    case SURFACE_TARMAC:
                        r = 10;
                        g = 10;
                        b = 10;
                        break;
                    case SURFACE_GRASS_MEDIUM_DRY:
                        r = 10;
                        g = 10;
                        b = 3;
                        break;
                    case SURFACE_GRAVEL:
                        r = 10;
                        g = 8;
                        b = 7;
                        break;
                    case SURFACE_MUD_DRY:
                        r = 10;
                        g = 6;
                        b = 3;
                        break;
                    case SURFACE_SAND_MEDIUM:
                    case SURFACE_SAND_ARID:
                    case SURFACE_SAND_COMPACT:
                    case SURFACE_SAND_DEEP:
                    case SURFACE_SAND_MORE:
                    case SURFACE_SAND_BEACH:
                        r = 10;
                        g = 10;
                        b = 7;
                        break;
                    }

                    RwRGBA col = { r, g, b, 32 };
                    if (heliPos.z - pos.z < 20.0f)
                        CParticle::AddParticle(PARTICLE_HELI_DUST, pos, dir, NULL, dustSize, col);
                }

                n--;
            }
        }
    }
}

void AudioEnginee::ReportCollision(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, CVector& point, CVector* normal, float fCollisionImpact1, float fCollisionImpact2, bool playOnlyOneShotCollisionSound, bool unknown) {
    plugin::CallMethod<0x506EB0, CAudioEngine*, CEntity*, CEntity*, eSurfaceType, eSurfaceType, CVector&, CVector*, float, float, bool, bool>(this, entity1, entity2, surf1, surf2, point, normal, fCollisionImpact1, fCollisionImpact2, playOnlyOneShotCollisionSound, unknown);
}
CColModel* PedModelInfo::AnimatePedColModelSkinned(RpClump* clump) {
    return plugin::CallMethodAndReturn<CColModel*, 0x4C6F70, RpClump*>(clump);
}

// 0x6D7C90
void Veh::InflictDamage(CEntity* damager, eWeaponType weapon, float intensity, CVector coords) {
    ((void(__thiscall*)(CVehicle*, CEntity*, eWeaponType, float, CVector))0x6D7C90)(this, damager, weapon, intensity, coords);
}

void
HeliDustGenerate(CEntity* heli, float radius, float ground, int rnd)
{
    int i;
    float angle;
    CColPoint point;
    CEntity* entity;
    uint8_t r, g, b;
    if (heli) {

        uint8_t surface = SURFACE_TARMAC;
        int frm = CTimer::m_FrameCounter & 7;
        float testLowZ = ground - 10.0f;
        float dustSize = 0.0f;
        float baseSize = 1.0f;
        float offset = 1.0f;	// when heli is tilted
        float particleZ = -101.0f;
        int n = 0;

        if (heli->m_nModelIndex == MODEL_RCGOBLIN || heli->m_nModelIndex == MODEL_RCRAIDER) {
            radius = 3.0f;
            dustSize = 0.04f;
            baseSize = 0.07f;
            offset = 0.3f;
        }

        CVector heliPos = heli->GetPosition();

        if (heli->m_nType == ENTITY_TYPE_VEHICLE && ((CVehicle*)heli)->m_nVehicleSubClass == VEHICLE_AUTOMOBILE) {
            heliPos.x -= (heliPos.z - ground) * heli->GetUp().x * offset * 0.5f;
            heliPos.y -= (heliPos.z - ground) * heli->GetUp().y * offset * 0.5f;
        }

        float steamSize = 0.25f * radius * baseSize;
        float splashSize = 0.3f * radius * baseSize;

        i = 0;
        for (i = 0; i < 32 + rnd; i++) {
            angle = i * TWOPI / 32.0f;
            CVector pos(radius * cosf(angle), radius * sinf(angle), 0.0f);
            CVector dir = CVector(pos.x, pos.y, 1.0f) * 0.01f;
            pos += heliPos;
            if (i < 32 && i == 4 * frm) {
                if (CWorld::ProcessVerticalLine(pos, testLowZ, point, entity, true, false, false, false, true, false, nullptr)) {
                    n = rnd;
                    particleZ = point.m_vecPoint.z;
                    surface = point.m_nSurfaceTypeB;
                }
                else
                    n = 0;

                float waterLevel = 0.0f;
                if (CWaterLevel::GetWaterLevelNoWaves(pos.x, pos.y, pos.z, &waterLevel, nullptr, nullptr) && waterLevel > particleZ) {
                    surface = SURFACE_WATER_SHALLOW;
                    n = rnd;
                    particleZ = waterLevel;
                }
            }

            if (n) {
                pos.z = particleZ;
                if (surface == eSurfaceType::SURFACE_WATER_SHALLOW) {
                    float red = (0.3 * CTimeCycle::m_CurrentColours.m_fDirectionalRed + CTimeCycle::GetAmbientRed_Obj()) * 255.0f / 4.0f;
                    float green = (0.3 * CTimeCycle::m_CurrentColours.m_fDirectionalGreen + CTimeCycle::GetAmbientGreen_Obj()) * 255.0f / 4.0f;
                    float blue = (0.3 * CTimeCycle::m_CurrentColours.m_fDirectionalBlue + CTimeCycle::GetAmbientBlue_Obj()) * 255.0f / 4.0f;
                    r = Clamp(red, 0.0f, 255.0f);
                    g = Clamp(green, 0.0f, 255.0f);
                    b = Clamp(blue, 0.0f, 255.0f);
                    RwRGBA col1 = { r, g, b, (RwUInt8)CGeneral::GetRandomNumberInRange(8, 32) };
                    RwRGBA col2 = { 255, 255, 255, 32 };

                    if (n & 1)
                        CParticle::AddParticle(PARTICLE_STEAM_NY_SLOWMOTION, pos, dir, nullptr, steamSize, col2);
                    else
                        CParticle::AddParticle(PARTICLE_CAR_SPLASH, pos, dir, nullptr, splashSize, col1,
                            CGeneral::GetRandomNumberInRange(0.0f, 10.0f),
                            CGeneral::GetRandomNumberInRange(0.0f, 90.0f), 1);
                }
                else {
                    switch (surface) {
                    default:
                    case SURFACE_TARMAC:
                        r = 10;
                        g = 10;
                        b = 10;
                        break;
                    case SURFACE_GRASS_MEDIUM_DRY:
                        r = 10;
                        g = 10;
                        b = 3;
                        break;
                    case SURFACE_GRAVEL:
                        r = 10;
                        g = 8;
                        b = 7;
                        break;
                    case SURFACE_MUD_DRY:
                        r = 10;
                        g = 6;
                        b = 3;
                        break;
                    case SURFACE_SAND_MEDIUM:
                    case SURFACE_SAND_ARID:
                    case SURFACE_SAND_COMPACT:
                    case SURFACE_SAND_DEEP:
                    case SURFACE_SAND_MORE:
                    case SURFACE_SAND_BEACH:
                        r = 10;
                        g = 10;
                        b = 7;
                        break;
                    }
                    RwRGBA col = { r, g, b, 32 };
                    // if (heliPos.z - pos.z < 20.0f)
                    if (heliPos.z < 30.0f)
                        CParticle::AddParticle(PARTICLE_HELI_DUST, pos, dir, nullptr, dustSize, col);
                }

                n--;
            }
        }
    }
}
void
GenerateFlameThrowerParticles(CVector pos, CVector dir)
{
    dir *= 0.7f;
    CParticle::AddParticle(PARTICLE_FIREBALL, pos, dir);

    dir *= 0.7f;
    CParticle::AddParticle(PARTICLE_FIREBALL, pos, dir);

    dir *= 0.7f;
    CParticle::AddParticle(PARTICLE_FIREBALL, pos, dir);

    dir *= 0.7f;
    CParticle::AddParticle(PARTICLE_FIREBALL, pos, dir);

    dir *= 0.7f;
    CParticle::AddParticle(PARTICLE_FIREBALL, pos, dir);
}

CWeaponInfo*
GetInfo(CPed* ped)
{
    CWeaponInfo* info = CWeaponInfo::GetWeaponInfo(ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType, ped->GetWeaponSkill());
    assert(info != NULL);
    return info;
}

bool
FireAreaEffect(CPed* shooter, CVector* fireSource)
{

    CWeaponInfo* info = GetInfo(shooter);
    auto weaptype = FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_eWeaponType;
    float heading = RADTODEG(shooter->GetForward().Heading());

    CVector source;
    CVector target;
    CVector dir;

    if (shooter == FindPlayerPed())
    {
#ifdef FREE_CAM
        if (CCamera::bFreeCam) {
            CPlayerPed* shooterPed = (CPlayerPed*)shooter;
            Find3rdPersonCamTargetVectorFromCachedVectors(info->m_fRange, *fireSource, source, target, shooterPed->m_cachedCamSource, shooterPed->m_cachedCamFront, shooterPed->m_cachedCamUp);
        }
        else
#endif
        {
            TheCamera.Find3rdPersonCamTargetVector(info->m_fWeaponRange, *fireSource, &source, &target);
        }
        float norm = (1.0f / info->m_fWeaponRange);
        dir = (target - source) * norm;
    }
    else
    {
        float angle = DEGTORAD(heading);
        dir = CVector(-sinf(angle) * 0.5f, cosf(angle) * 0.5f, 0.0f);
        target = *fireSource + dir;
    }
    CParticle::HandleShootableBirdsStuff(shooter, *fireSource);
    //GenerateFlameThrowerParticles(*fireSource, dir);
    return true;
}

void CWeap::AddGunshell(CPed* creator, CVector& position, const CVector2D& direction, float size)
{
    ASSERT(creator != NULL);

    if (creator == NULL)
        return;

    tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[PARTICLE_GUNSHELL_BUMP1];
    tParticleSystemData* psystem2 = &mod_ParticleSystemManager.m_aParticles[PARTICLE_GUNSHELL_BUMP2];
    CParticle* part = psystem->m_pParticles;
    CParticle* part2 = psystem2->m_pParticles;
    CVector dir(direction.x * 0.05f, direction.y * 0.05f, CGeneral::GetRandomNumberInRange(0.02f, 0.08f));

    static CVector prevEntityPosition(0.0f, 0.0f, 0.0f);
    CVector entityPosition = creator->GetPosition();
    RwRGBA color{};
    CVector diff = entityPosition - prevEntityPosition;
    auto WeaponType = creator->m_aWeapons[creator->m_nActiveWeaponSlot].m_eWeaponType;
    auto WeaponInfo = GetInfo(creator);
    if (size > 1.0f) size = 1.0f;

    // Checking if it's a shotgun, if it is, set IsShotgun to true. Also set gunshell size for other weapons.
    switch (WeaponType) {
    case WEAPON_SHOTGUN:
    case WEAPON_SPAS12:
    case WEAPON_SAWNOFF:
        IsShotgun = true;
        break;
    case WEAPON_MICRO_UZI:
    case WEAPON_TEC9:
    case WEAPON_MP5:
        size = 0.015f;
        IsShotgun = false;
        break;
    case WEAPON_AK47:
    case WEAPON_M4:
    case WEAPON_MINIGUN:
        size = 0.02f;
        IsShotgun = false;
        break;
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
    case WEAPON_DESERT_EAGLE:
    case WEAPON_SNIPERRIFLE:
        size = 0.015f;
        IsShotgun = false;
        break;
    default:
        IsShotgun = false;
        break;
    }
    // Custom weapons support
    switch (WeaponInfo->m_nAnimToPlay) {
    case 19:
    case 20:
        if (WeaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT)
            size = 0.025f;
        break;
    case 45:
        if (WeaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT)
            size = 0.015f;
        break;
    case 25:
    case 26:
        if (WeaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT)
            size = 0.02f;
        break;
    case 23:
    case 24:
        if (WeaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT)
            size = 0.015f;
        break;
    case 21:
    case 22:
    case 16:
    case 17:
        if (WeaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT)
            size = 0.02f;
        break;
    default:
        if (WeaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT)
            size = 0.02f;
        break;
    }
    // Setting red color, if it's a shotgun
    if (IsShotgun) {
        color.red = 255;
        color.green = 0;
        color.blue = 0;
        color.alpha = 255;
    }
    else {
        color.red = 0;
        color.green = 0;
        color.blue = 0;
        color.alpha = 0;
    }

    if (fabsf(diff.x) + fabsf(diff.y) + fabsf(diff.z) > 1.5f)
    {
        prevEntityPosition = entityPosition;

        CParticle::AddParticle(PARTICLE_GUNSHELL_FIRST,
            position, dir, NULL, size, color, CGeneral::GetRandomNumberInRange(-20.0f, 20.0f));
    }
    else
    {
        CParticle::AddParticle(PARTICLE_GUNSHELL,
            position, dir, NULL, size, color, CGeneral::GetRandomNumberInRange(-20.0f, 20.0f));
    }
}

void
AddGunFlashBigGuns(CVector start, CVector end)
{
    CPointLights::AddLight(0,
        start, CVector(0.0f, 0.0f, 0.0f), 5.0f,
        1.0f, 0.8f, 0.0f, 0, false, nullptr);
    CVector gunflashPos = start;
    CVector shootVec = end - start;

    // Wtf did you do there R*?
    shootVec.Normalise();
    CVector2D ahead = shootVec;
    ((Vec2D)ahead).Normalise();
    tParticleType Type, Type2;
    Type = PARTICLE_GUNFLASH_NOANIM;
    Type2 = PARTICLE_GUNFLASH;
    /*if (VCSMuzzleFlash) {
        Type = PARTICLE_GUNFLASH_NOANIM_VCS;
        Type2 = PARTICLE_GUNFLASH_VCS;
    }
    else
    {
        Type = PARTICLE_GUNFLASH_NOANIM;
        Type2 = PARTICLE_GUNFLASH;
    }*/
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.08f);
    gunflashPos += CVector(0.06f * ahead.x, 0.06f * ahead.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.06f);
    gunflashPos += CVector(0.06f * ahead.x, 0.06f * ahead.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.06f);

    gunflashPos = start;
    gunflashPos += CVector(-0.1f * ahead.x, -0.1f * ahead.y, 0.0f);
    gunflashPos.z += 0.04f;
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
    gunflashPos.z += 0.04f;
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);
    gunflashPos.z += 0.03f;
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);

    gunflashPos = start;
    gunflashPos += CVector(-0.1f * ahead.x, -0.1f * ahead.y, 0.0f);
    gunflashPos.z -= 0.04f;
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
    gunflashPos.z -= 0.04f;
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);
    gunflashPos.z -= 0.03f;
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);

    CVector offset = CrossProduct(CVector(ahead.x, ahead.y, 0.0f), CVector(0.0f, 0.0f, 5.0f));
    offset.Normalise();

    gunflashPos = start;
    gunflashPos += CVector(-0.1f * ahead.x, -0.1f * ahead.y, 0.0f);
    gunflashPos += CVector(0.06f * offset.x, 0.06f * offset.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
    gunflashPos += CVector(0.04f * offset.x, 0.04f * offset.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.03f);
    gunflashPos += CVector(0.03f * offset.x, 0.03f * offset.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);

    gunflashPos = start;
    gunflashPos += CVector(-0.1f * ahead.x, -0.1f * ahead.y, 0.0f);
    gunflashPos -= CVector(0.06f * offset.x, 0.06f * offset.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
    gunflashPos -= CVector(0.04f * offset.x, 0.04f * offset.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.03f);
    gunflashPos -= CVector(0.03f * offset.x, 0.03f * offset.y, 0.0f);
    CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);

    CVector gunsmokePos = start;
    float rnd = CGeneral::GetRandomNumberInRange(0.05f, 0.25f);
    CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * rnd, ahead.y * rnd, 0.0f));
}

bool DoGunFlash(CPed* shooter, CVector* fireSource, bool LeftHand) {
    CVector source = *fireSource;
    CVector target = source;
    //float heading = CGeneral::GetAngleBetweenPoints(source.x, source.y, target.x, target.y);
    //float angle = DEGTORAD(heading);
    float heading = RADTODEG(shooter->GetForward().Heading());
    float angle = DEGTORAD(heading);
    CVector2D ahead(-sinf(angle), cosf(angle));
    ((Vec2D)ahead).Normalise();
    CVector dir = CVector(-sinf(angle) * 0.5f, cosf(angle) * 0.5f, 0.0f);
    target = *fireSource + dir;
    CVector vel = shooter->m_vecMoveSpeed;
    CVector gunsmokePos = *fireSource;
    CVector gunflashPos = *fireSource;
    CVector2D rotOffset(-sinf(angle), cosf(angle));
    ((Vec2D)rotOffset).Normalise();
    tParticleType Type, Type2;
    auto Task = shooter->m_pIntelligence->GetTaskUseGun();
    Type = PARTICLE_GUNFLASH_NOANIM;
    Type2 = PARTICLE_GUNFLASH;
    /*if (VCSMuzzleFlash) {
        Type = PARTICLE_GUNFLASH_NOANIM_VCS;
        Type2 = PARTICLE_GUNFLASH_VCS;
    }
    else
    {
        Type = PARTICLE_GUNFLASH_NOANIM;
        Type2 = PARTICLE_GUNFLASH;
    }*/
    auto WeaponInfo = GetInfo(shooter);
    CVector offset = WeaponInfo->m_vecFireOffset;
    auto weapType = shooter->m_aWeapons[shooter->m_nActiveWeaponSlot].m_eWeaponType;
    if (LeftHand) {
            // offset.x *= 0.40f;
            // offset.z *= 1.20f;
        if (!InitializedWeapon[weapType]) {
            return false;
        }
            offset.x = OffsetX[weapType];
            offset.y = OffsetY[weapType];
            offset.z = OffsetZ[weapType];
            log("Second gunflsh offset X for weapon type %d is %f", weapType, OffsetX[weapType]);
            log("Second gunflsh offset Y for weapon type %d is %f", weapType, OffsetY[weapType]);
            log("Second gunflsh offset Z for weapon type %d is %f", weapType, OffsetZ[weapType]);
    }
    float rnd = 0.0f;
    bool shooterMoving = (fabsf(shooter->m_vecMoveSpeed.x) > 0.0f && fabsf(shooter->m_vecMoveSpeed.y) > 0.0f);
    bool weaponFound = false;  // Is the weapon type found or not
    switch (weapType) {
    case WEAPON_SNIPERRIFLE:
    case WEAPON_COUNTRYRIFLE:
    case WEAPON_DESERT_EAGLE:
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
        weaponFound = true;
        if (shooterMoving) {
            gunflashPos += CVector(1.5f * vel.x, 1.5f * vel.y, 0.0f);
        }
        if (LeftHand) {
            gunflashPos += CVector(offset.x, offset.y, offset.z);
        }
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.06f);
        gunflashPos += CVector(0.06f * ahead.x, 0.06f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
        gunflashPos += CVector(0.04f * ahead.x, 0.04f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);

        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * 0.10f, ahead.y * 0.10f, 0.0f), nullptr, 0.005f);
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * 0.15f, ahead.y * 0.15f, 0.0f), nullptr, 0.015f);
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * 0.20f, ahead.y * 0.20f, 0.0f), nullptr, 0.025f);
        break;
    case WEAPON_MICRO_UZI:
    case WEAPON_TEC9:
    case WEAPON_MP5:
        weaponFound = true;
        if (shooterMoving) {
            gunflashPos += CVector(1.5f * vel.x, 1.5f * vel.y, 0.0f);
        }
        if (LeftHand) {
            gunflashPos += CVector(offset.x, offset.y, offset.z);
        }

        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.07f);
        gunflashPos += CVector(0.06f * ahead.x, 0.06f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.05f);
        gunflashPos += CVector(0.04f * ahead.x, 0.04f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
        gunflashPos += CVector(0.04f * ahead.x, 0.04f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.03f);
        gunflashPos += CVector(0.03f * ahead.x, 0.03f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.03f);
        gunflashPos += CVector(0.03f * ahead.x, 0.03f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);
        gunflashPos += CVector(0.02f * ahead.x, 0.02f * ahead.y, 0.0f);
        CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.01f);
        rnd = CGeneral::GetRandomNumberInRange(0.05f, 0.25f);
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * rnd, ahead.y * rnd, 0.0f));
        break;
    case WEAPON_SPAS12:
    case WEAPON_SHOTGUN:
    case WEAPON_SAWNOFF:
        weaponFound = true;
        if (shooterMoving) {
            gunflashPos += CVector(1.5f * vel.x, 1.5f * vel.y, 0.0f);
        }

        if (LeftHand) {
            gunflashPos += CVector(offset.x, offset.y, offset.z);
        }

        gunflashPos += CVector(rotOffset.x * 0.1f, rotOffset.y * 0.1f, 0.0f);
        CParticle::AddParticle(Type2, gunflashPos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f);
        gunflashPos += CVector(rotOffset.x * 0.1f, rotOffset.y * 0.1f, 0.0f);
        CParticle::AddParticle(Type2, gunflashPos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.15f);
        gunflashPos += CVector(rotOffset.x * 0.1f, rotOffset.y * 0.1f, 0.0f);
        CParticle::AddParticle(Type2, gunflashPos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.2f);
        CParticle::AddParticle(Type2, *fireSource, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f);

        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.10f, rotOffset.y * 0.10f, 0.0f), NULL, 0.1f);
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.15f, rotOffset.y * 0.15f, 0.0f), NULL, 0.1f);
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.20f, rotOffset.y * 0.20f, 0.0f), NULL, 0.1f);
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.25f, rotOffset.y * 0.25f, 0.0f), NULL, 0.1f);
        break;
    case WEAPON_AK47:
    case WEAPON_M4:
    case WEAPON_MINIGUN:
        weaponFound = true;
        AddGunFlashBigGuns(*fireSource, target);
        break;
        default:
            // log("[ERROR]: No support for custom weapons lol get rekt lmao sucka\n");
           //  error("No support for custom weapons lol get rekt lmao sucka\n"); // Nah :)
             // Add flash & smoke for unknown weapons anyway
            weaponFound = false;
           // CParticle::AddParticle(Type2, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.0f);
           // CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.10f, rotOffset.y * 0.10f, 0.0f), nullptr, 0.1f);
            break;
            // Better version down below
    }

    // Custom weapons support
    if (!weaponFound && Task) {
        switch (Task->m_pAnim->m_nAnimGroup) {
        case ANIM_GROUP_RIFLE:
        case ANIM_GROUP_RIFLEBAD:
            AddGunFlashBigGuns(*fireSource, target);
            break;
        case ANIM_GROUP_COLT45:
        case ANIM_GROUP_COLT45PRO:
        case ANIM_GROUP_COLT_COP:
        case ANIM_GROUP_PYTHON:
        case ANIM_GROUP_PYTHONBAD:
        case ANIM_GROUP_SILENCED:
            if (shooterMoving) {
                gunflashPos += CVector(1.5f * vel.x, 1.5f * vel.y, 0.0f);
            }

            if (LeftHand) {
                gunflashPos += CVector(offset.x, offset.y, offset.z);
            }

            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.06f);
            gunflashPos += CVector(0.06f * ahead.x, 0.06f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
            gunflashPos += CVector(0.04f * ahead.x, 0.04f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);

            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * 0.10f, ahead.y * 0.10f, 0.0f), nullptr, 0.005f);
            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * 0.15f, ahead.y * 0.15f, 0.0f), nullptr, 0.015f);
            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * 0.20f, ahead.y * 0.20f, 0.0f), nullptr, 0.025f);
            break;
        case ANIM_GROUP_UZI:
        case ANIM_GROUP_UZIBAD:
        case ANIM_GROUP_SNIPER:
            if (shooterMoving) {
                gunflashPos += CVector(1.5f * vel.x, 1.5f * vel.y, 0.0f);
            }

            if (LeftHand) {
                gunflashPos += CVector(offset.x, offset.y, offset.z);
            }

            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.07f);
            gunflashPos += CVector(0.06f * ahead.x, 0.06f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.05f);
            gunflashPos += CVector(0.04f * ahead.x, 0.04f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.04f);
            gunflashPos += CVector(0.04f * ahead.x, 0.04f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.03f);
            gunflashPos += CVector(0.03f * ahead.x, 0.03f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.03f);
            gunflashPos += CVector(0.03f * ahead.x, 0.03f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.02f);
            gunflashPos += CVector(0.02f * ahead.x, 0.02f * ahead.y, 0.0f);
            CParticle::AddParticle(Type, gunflashPos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.01f);
            rnd = CGeneral::GetRandomNumberInRange(0.05f, 0.25f);
            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(ahead.x * rnd, ahead.y * rnd, 0.0f));
            break;

        case ANIM_GROUP_BUDDY:
        case ANIM_GROUP_BUDDYBAD:
        case ANIM_GROUP_SAWNOFF:
        case ANIM_GROUP_SAWNOFFPRO:
        case ANIM_GROUP_SHOTGUN:
        case ANIM_GROUP_SHOTGUNBAD:
            if (shooterMoving) {
                gunflashPos += CVector(1.5f * vel.x, 1.5f * vel.y, 0.0f);
            }

            if (LeftHand) {
                gunflashPos += CVector(offset.x, offset.y, offset.z);
            }

            gunflashPos += CVector(rotOffset.x * 0.1f, rotOffset.y * 0.1f, 0.0f);
            CParticle::AddParticle(Type2, gunflashPos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f);
            gunflashPos += CVector(rotOffset.x * 0.1f, rotOffset.y * 0.1f, 0.0f);
            CParticle::AddParticle(Type2, gunflashPos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.15f);
            gunflashPos += CVector(rotOffset.x * 0.1f, rotOffset.y * 0.1f, 0.0f);
            CParticle::AddParticle(Type2, gunflashPos, CVector(0.0f, 0.0f, 0.0f), NULL, 0.2f);
            CParticle::AddParticle(Type2, *fireSource, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f);

            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.10f, rotOffset.y * 0.10f, 0.0f), NULL, 0.1f);
            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.15f, rotOffset.y * 0.15f, 0.0f), NULL, 0.1f);
            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.20f, rotOffset.y * 0.20f, 0.0f), NULL, 0.1f);
            CParticle::AddParticle(PARTICLE_GUNSMOKE2, gunsmokePos, CVector(rotOffset.x * 0.25f, rotOffset.y * 0.25f, 0.0f), NULL, 0.1f);
            break;
        }
    }
    return true;
}

RwRGBA RGBA::ToRwRGBA() const {
    return { r, g, b, a };
}

void Auto::dmgDrawCarCollidingParticles(const CVector& position, float force, eWeaponType weapon) {
    int i, n;

    if (!GetIsOnScreen())
        return;

    // FindPlayerSpeed() unused
    RwRGBA color = reinterpret_cast<RwRGBA&>(CVehicleModelInfo::ms_vehicleColourTable[m_nPrimaryColor]);
    n = (int)force / 20;

    for (i = 0; i < ((n + 4) & 0x1F); i++)
        CParticle::AddParticle(PARTICLE_SPARK_SMALL, position,
            CVector(CGeneral::GetRandomNumberInRange(-0.1f, 0.1f),
                CGeneral::GetRandomNumberInRange(-0.1f, 0.1f),
                0.006f));

    for (i = 0; i < n + 2; i++)
        CParticle::AddParticle(PARTICLE_CARCOLLISION_DUST,
            CVector(CGeneral::GetRandomNumberInRange(-1.2f, 1.2f) + position.x,
                CGeneral::GetRandomNumberInRange(-1.2f, 1.2f) + position.y,
                position.z),
            CVector(0.0f, 0.0f, 0.0f), NULL, 0.5f);

    n = (int)force / 50 + 1;
    for (i = 0; i < n; i++)
        CParticle::AddParticle(PARTICLE_CAR_DEBRIS, position,
            CVector(CGeneral::GetRandomNumberInRange(-0.25f, 0.25f),
                CGeneral::GetRandomNumberInRange(-0.25f, 0.25f),
                CGeneral::GetRandomNumberInRange(0.1f, 0.25f)),
            NULL,
            CGeneral::GetRandomNumberInRange(0.02f, 0.08f),
            color,
            CGeneral::GetRandomNumberInRange(-40, 40),
            0,
            CGeneral::GetRandomNumberInRange(0, 4));
}

// 0x6DE240
void Veh::AddExhaustParticles() {
    if (m_bOffscreen)
        return;

    float dist = DistanceBetweenPoints(GetPosition(), TheCamera.GetPosition());
    if (dist > 256.0f || dist > 64.0f && !((CTimer::m_FrameCounter + m_nModelIndex) & 1))
    {
        return;
    }

    auto mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(m_nModelIndex);
    CVector firstExhaustPos = mi->m_pVehicleStruct->m_avDummyPos[6];
    CVector secondExhaustPos = firstExhaustPos;
    secondExhaustPos.x *= -1.0f;
    CMatrix entityMatrix(*m_matrix);
    CVector dir1, dir2;
    dir1.z = 0.0f;
    dir2.z = 0.0f;
    float fwdSpeed = DotProduct(m_vecMoveSpeed, GetForward()) * 180.0f;
    if (fwdSpeed < 10.0f) {
        CVector steerFwd(-sinf(m_fSteerAngle), cosf(m_fSteerAngle), 0.0f);
        steerFwd = Multiply3x3(*GetMatrix(), steerFwd);
        float r = CGeneral::GetRandomNumberInRange(-0.06f, -0.03f);
        dir1.x = steerFwd.x * r;
        dir1.y = steerFwd.y * r;
    }
    else {
        dir1.x = m_vecMoveSpeed.x;
        dir1.y = m_vecMoveSpeed.y;
    }
    bool bHasDoubleExhaust = m_pHandlingData->m_bDoubleExhaust;
    if (m_nVehicleSubClass == VEHICLE_BIKE) {
        entityMatrix = *GetMatrix();
        switch (m_nModelIndex) {
        case MODEL_FCR900:
            if (m_anExtras[0] == 1 || m_anExtras[0] == 2)
                bHasDoubleExhaust = true;
            break;
        case MODEL_NRG500:
            if (!m_anExtras[0] || m_anExtras[0] == 1)
                secondExhaustPos = mi->m_pVehicleStruct->m_avDummyPos[11];
            break;
        case MODEL_BF400:
            if (m_anExtras[0] == 2)
                bHasDoubleExhaust = true;
            break;
        }
    }

    if (firstExhaustPos == 0.0f) {
        return;
    }

    CVector vecParticleVelocity;
    if (DotProduct(GetForward(), m_vecMoveSpeed) >= 0.05f) {
        vecParticleVelocity = m_vecMoveSpeed * 30.0f;
    }
    else {
        static float randomFactor = CGeneral::GetRandomNumberInRange(-1.8f, -0.9f);
        vecParticleVelocity = randomFactor * GetForward();
    }

    firstExhaustPos = static_cast<Matrix>(entityMatrix).TransformPoint(firstExhaustPos);
    bool bFirstExhaustSubmergedInWater = false;
    bool bSecondExhaustSubmergedInWater = false;
    float pLevel = 0.0f;
    if (m_nPhysicalFlags.bTouchingWater && CWaterLevel::GetWaterLevelNoWaves(firstExhaustPos.x, firstExhaustPos.y, firstExhaustPos.z, &pLevel, NULL, NULL) &&
        pLevel >= firstExhaustPos.z) {
        bFirstExhaustSubmergedInWater = true;
    }

    if (bHasDoubleExhaust) {
        secondExhaustPos = static_cast<Matrix>(entityMatrix).TransformPoint(secondExhaustPos);
        if (m_nPhysicalFlags.bTouchingWater && CWaterLevel::GetWaterLevelNoWaves(secondExhaustPos.x, secondExhaustPos.y, secondExhaustPos.z, &pLevel, NULL, NULL) &&
            pLevel >= secondExhaustPos.z) {
            bSecondExhaustSubmergedInWater = true;
        }
    }

    if (CGeneral::GetRandomNumberInRange(1.0f, 3.0f) * (m_fGasPedal + 1.1f) <= 2.5f)
        return;
    auto parttype = PARTICLE_EXHAUST_FUMES;
    CParticle* particle = CParticle::m_pUnusedListHead;
    CParticle* firstExhaustFxSystem = particle;
    CParticle* secondExhaustFxSystem = particle;
    float fMoveSpeed = m_vecMoveSpeed.Magnitude() * 0.5f;
    float particleAlpha = 0.0f;
    float size = 0.0f;
    if (0.25f - fMoveSpeed >= 0.0f) {
        particleAlpha = 0.25f - fMoveSpeed;
    }
    float fLife = max(0.2f - fMoveSpeed, 0.0f);
    if (bFirstExhaustSubmergedInWater) {
        firstExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
        //firstExhaustFxSystem->m_fSize = 0.05f;
        size = 0.05f;
        parttype = PARTICLE_WATER;
    }
    else {
        size = 0.0f;
    }
    CParticle::AddParticle(parttype, firstExhaustPos, dir1, nullptr, size, 0, -1.0, 0, fLife);
    if (bHasDoubleExhaust) {
        if (bSecondExhaustSubmergedInWater) {
            firstExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
            // firstExhaustFxSystem->m_fSize = 0.05f;
            size = 0.05f;
            parttype = PARTICLE_WATER;
        }
        else {
            size = 0.0f;
        }
        CParticle::AddParticle(parttype, secondExhaustPos, dir1, nullptr, size, 0, -1.0, 0, fLife);
    }

    if (m_fGasPedal > 0.5f && m_nCurrentGear < 3) {
        if (GetRandomNumber() % 2) {
            if (bFirstExhaustSubmergedInWater) {
                secondExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
                // secondExhaustFxSystem->m_fSize = 0.05f;
                size = 0.05f;
                parttype = PARTICLE_WATER;
            }
            else {
                size = 0.0f;
            }

            CParticle::AddParticle(parttype, firstExhaustPos, dir1, nullptr, size, 0, -1.0, 0, fLife);
        }
        else if (bHasDoubleExhaust) {

            if (bSecondExhaustSubmergedInWater) {
                secondExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
                // secondExhaustFxSystem->m_fSize = 0.05f;
                size = 0.05f;
                parttype = PARTICLE_WATER;
            }
            else {
                size = 0.0f;
            }

            CParticle::AddParticle(parttype, secondExhaustPos, dir1, nullptr, size, 0, -1.0, 0, fLife);
        }
    }
    // Diesel smoke
    static int previousGear = 0;

    if (m_pHandlingData->m_transmissionData.m_nEngineType == 'D' && m_fGasPedal > 0.3f && DieselVehiclesBlackSmoke) {
        // If the current gear is greater than previous gear
        if (m_nCurrentGear > previousGear && m_nCurrentGear <= 5) {
            // Add diesel smoke particles!
            if (!bFirstExhaustSubmergedInWater) {
                CParticle::AddParticle(PARTICLE_ENGINE_SMOKE, firstExhaustPos, dir1, nullptr, 0.01f, 0, -1.0, 0, fLife);
            }
            // If the vehicle has a second exhaust, add smoke for it too! Make sure it's not in the water too..
            if (bHasDoubleExhaust && !bSecondExhaustSubmergedInWater) {
                CParticle::AddParticle(PARTICLE_ENGINE_SMOKE, secondExhaustPos, dir1, nullptr, 0.01f, 0, -1.0, 0, fLife);
            }
        }

        // Update the variable
        previousGear = m_nCurrentGear;
    }
    // Fire on exhaust
    static float fumesLimit = 2.0f;
    if (FireOnExhaust) {
        if (m_nVehicleFlags.bEngineOn && !(m_pHandlingData->m_bNoExhaust) && fwdSpeed < 130.0f) {
            if (((Vec)firstExhaustPos) != CVector(0.0f, 0.0f, 0.0f) || ((Vec)secondExhaustPos) != CVector(0.0f, 0.0f, 0.0f)) {
                if (CGeneral::GetRandomNumberInRange(1.0f, 3.0f) * (m_fGasPedal + 1.1f) > fumesLimit) {
                    for (int i = 0; i < 4; i++) {
                        if (!InitializedVehicle[m_nModelIndex]) {
                            continue;  // Do not add fire exhaust if the vehicle is not initialized in INI file
                        }
                        if (/*m_nModelIndex == ModelIndex*/ ShouldHaveFireExhaust[m_nModelIndex] && i == 1 && m_fGasPedal > 0.9f) {
                            if (m_nCurrentGear == 1 || m_nCurrentGear == 3 && (CTimer::m_snTimeInMilliseconds % 1500) > 750) {
                                if (!bFirstExhaustSubmergedInWater) {
                                    CParticle::AddParticle(PARTICLE_FIREBALL, firstExhaustPos, dir1, NULL, 0.05f, 0, 0, 2, 200);
                                    CParticle::AddParticle(PARTICLE_FIREBALL, firstExhaustPos, dir1, NULL, 0.05f, 0, 0, 2, 200);
                                }
                                if (bHasDoubleExhaust && !bSecondExhaustSubmergedInWater) {
                                    CParticle::AddParticle(PARTICLE_FIREBALL, secondExhaustPos, dir1, NULL, 0.05f, 0, 0, 2, 200);
                                    CParticle::AddParticle(PARTICLE_FIREBALL, secondExhaustPos, dir1, NULL, 0.05f, 0, 0, 2, 200);
                                }
                            }
                            if (m_nStatus == STATUS_PLAYER && (CTimer::m_FrameCounter & 3) == 0 &&
                                CWeather::Rain == 0.0f && i == 0) {
                                CVector camDist = GetPosition() - TheCamera.GetPosition();
                                if (DotProduct(GetForward(), camDist) > 0.0f ||
                                    TheCamera.GetLookDirection() == 1 ||
                                    TheCamera.GetLookDirection() == 2) {
                                    if (!bFirstExhaustSubmergedInWater) {
                                        CParticle::AddParticle(PARTICLE_HEATHAZE, firstExhaustPos, CVector(0.0f, 0.0f, 0.0f));
                                    }
                                    if (bHasDoubleExhaust && !bSecondExhaustSubmergedInWater)
                                        CParticle::AddParticle(PARTICLE_HEATHAZE, secondExhaustPos, CVector(0.0f, 0.0f, 0.0f));
                                    if (!bFirstExhaustSubmergedInWater) {
                                        CParticle::AddParticle(PARTICLE_HEATHAZE, firstExhaustPos, CVector(0.0f, 0.0f, 0.0f));
                                    }
                                    if (bHasDoubleExhaust && !bSecondExhaustSubmergedInWater)
                                        CParticle::AddParticle(PARTICLE_HEATHAZE, secondExhaustPos, CVector(0.0f, 0.0f, 0.0f));
                                }
                            }
                            else {
                                i = 99;
                            }
                        }
                    }
                }
            }
        }
    }
}


void Veh::DoBoatSplashes(float Damping) {
    //return plugin::CallMethod<0x6DD130, CVehicle*, float>(this, fWaterDamping);
    CVector p0, p1;
    auto X_MULT = 0.7f;
    auto Z_MULT = 0.0f;
    const CVector& colMin = GetColModel()->m_boundBox.m_vecMin;
    const CVector& colMax = GetColModel()->m_boundBox.m_vecMax;
    p0 = { colMin.x * X_MULT, colMax.y / 2.0f, colMin.z * Z_MULT };
    p0 = ((Matrix*)m_matrix)->TransformPoint(p0);
    p1 = { colMax.x * X_MULT, colMax.y / 2.0f, colMin.z * Z_MULT };
    p1 = ((Matrix*)m_matrix)->TransformPoint(p1);
    RwUInt8 r, g, b;
    RwRGBA dropColor = { 0, 0, 0, 0 };
    RwRGBA splashColor, jetColor;
    r = 127.5f * (CTimeCycle::GetAmbientRed_Obj() + 0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalRed);
    g = 127.5f * (CTimeCycle::GetAmbientGreen_Obj() + 0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalGreen);
    b = 127.5f * (CTimeCycle::GetAmbientBlue_Obj() + 0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalBlue);
    r = Clamp(r, 0, 255);
    g = Clamp(g, 0, 255);
    b = Clamp(b, 0, 255);
    splashColor.red = r;
    splashColor.green = g;
    splashColor.blue = b;
    splashColor.alpha = CGeneral::GetRandomNumberInRange(160, 196);

    r = 229.5f * (CTimeCycle::GetAmbientRed() + 0.85f * CTimeCycle::m_CurrentColours.m_fDirectionalRed);
    g = 229.5f * (CTimeCycle::GetAmbientGreen() + 0.85f * CTimeCycle::m_CurrentColours.m_fDirectionalGreen);
    b = 229.5f * (CTimeCycle::GetAmbientBlue() + 0.85f * CTimeCycle::m_CurrentColours.m_fDirectionalBlue);
    r = Clamp(r, 0, 255);
    g = Clamp(g, 0, 255);
    b = Clamp(b, 0, 255);
    jetColor.red = r;
    jetColor.green = g;
    jetColor.blue = b;
    jetColor.alpha = CGeneral::GetRandomNumberInRange(196, 228);
    float speed = m_vecMoveSpeed.Magnitude();
    if (speed > 0.05f && GetUp().x > 0.0f && !TheCamera.GetLookingForwardFirstPerson() && IsVisible() &&
        (m_autoPilot.m_nCarMission != MISSION_CRUISE || (CTimer::m_FrameCounter & 2) == 0)) {
        CVector splashPos, splashDir;
        float splashSize, front, waterLevel;

        switch (m_nModelIndex) {
        case MODEL_SQUALO:
            splashSize = speed;
            front = 0.75f * GetColModel()->m_boundBox.m_vecMax.y;
            splashDir = -0.125f * m_vecMoveSpeed;
            splashDir.z += 0.15f * speed;
            splashDir += 0.25f * speed * GetRight();
            splashPos = GetPosition() + 0.5f * GetRight() + front * GetForward();
            splashPos.z += 0.5f;
            break;
        case MODEL_REEFER:
            splashSize = speed;
            front = 0.75f * GetColModel()->m_boundBox.m_vecMax.y;
            splashDir = -0.5f * m_vecMoveSpeed;
            splashDir.z += 0.15f * speed;
            splashDir += 0.5f * speed * GetRight();
            splashPos = GetPosition() + 1.3f * GetRight() + front * GetForward();
            break;
        case MODEL_COASTG:
            splashSize = 0.25f * speed;
            front = 0.8f * GetColModel()->m_boundBox.m_vecMax.y;
            splashDir = 0.165f * m_vecMoveSpeed;
            splashDir.z += 0.25f * speed;
            splashDir += 0.15f * speed * GetRight();
            splashPos = GetPosition() + 0.65f * GetRight() + front * GetForward();
            splashPos.z += 0.5f;
            break;
        case MODEL_DINGHY:
            splashSize = 0.25f * speed;
            front = 0.9f * GetColModel()->m_boundBox.m_vecMax.y;
            splashDir = 0.35f * m_vecMoveSpeed;
            splashDir.z += 0.25f * speed;
            splashDir += 0.25f * speed * GetRight();
            splashPos = GetPosition() + 0.6f * GetRight() + front * GetForward();
            splashPos.z += 0.5f;
            break;
        default:
            splashSize = speed;
            front = 0.9f * GetColModel()->m_boundBox.m_vecMax.y;
            splashDir = -0.5f * m_vecMoveSpeed;
            splashDir.z += 0.25f * speed;
            splashDir += 0.35f * speed * GetRight();
            splashPos = GetPosition() + 0.5f * GetRight() + front * GetForward();
            break;
        }
        if (splashSize > 0.75f) splashSize = 0.75f;
        if (m_autoPilot.m_nCarMission == MISSION_CRUISE)
            splashDir *= 1.5f;
        static float lifeMult = 1000.0f;
        static float lifeBase = 300.0f;
        splashDir.z += 0.0003f;
        CWaterLevel::GetWaterLevelNoWaves(splashPos.x, splashPos.y, splashPos.z, &waterLevel, nil, nil);
        if (splashPos.z - waterLevel < 3.0f &&
            CVisibilityPlugins::GetDistanceSquaredFromCamera((RwV3d*)&splashPos) < SQR(70.0f * TheCamera.m_fGenerationDistMultiplier)) {
            splashPos.z = waterLevel + 0.1f;
            CParticle::AddParticle(PARTICLE_CAR_SPLASH, p0, 0.75f * splashDir, nil, splashSize + 0.1f, splashColor,
                CGeneral::GetRandomNumberInRange(0.0f, 10.0f), CGeneral::GetRandomNumberInRange(0.0f, 90.0f),
                1, lifeBase + splashDir.z * lifeMult);
            CParticle::AddParticle(PARTICLE_BOAT_SPLASH, p0, splashDir, nil, splashSize, jetColor,
                CGeneral::GetRandomNumberInRange(0.0f, 0.4f), CGeneral::GetRandomNumberInRange(0.0f, 45.0f),
                0, lifeBase + splashDir.z * lifeMult);
        }

        switch (m_nModelIndex) {
        case MODEL_SQUALO:
            splashDir = -0.125f * m_vecMoveSpeed;
            splashDir.z += 0.15f * speed;
            splashDir -= 0.25f * speed * GetRight();
            splashPos = GetPosition() - 0.5f * GetRight() + front * GetForward();
            splashPos.z += 0.5f;
            break;
        case MODEL_REEFER:
            splashDir = -0.5f * m_vecMoveSpeed;
            splashDir.z += 0.15f * speed;
            splashDir -= 0.5f * speed * GetRight();
            splashPos = GetPosition() - 1.3f * GetRight() + front * GetForward();
            break;
        case MODEL_COASTG:
            splashDir = 0.165f * m_vecMoveSpeed;
            splashDir.z += 0.25f * speed;
            splashDir -= 0.15f * speed * GetRight();
            splashPos = GetPosition() - 0.65f * GetRight() + front * GetForward();
            splashPos.z += 0.5f;
            break;
        case MODEL_DINGHY:
            splashDir = 0.35f * m_vecMoveSpeed;
            splashDir.z += 0.25f * speed;
            splashDir -= 0.25f * speed * GetRight();
            splashPos = GetPosition() - 0.6f * GetRight() + front * GetForward();
            splashPos.z += 0.5f;
            break;
        default:
            splashDir = -0.5f * m_vecMoveSpeed;
            splashDir.z += 0.25f * speed;
            splashDir -= 0.35f * speed * GetRight();
            splashPos = GetPosition() - 0.5f * GetRight() + front * GetForward();
            break;
        }
        if (m_autoPilot.m_nCarMission == MISSION_CRUISE)
            splashDir *= 1.5f;
        splashDir.z += 0.0003f;
        CWaterLevel::GetWaterLevelNoWaves(splashPos.x, splashPos.y, splashPos.z, &waterLevel, nil, nil);
        if (splashPos.z - waterLevel < 3.0f &&
            CVisibilityPlugins::GetDistanceSquaredFromCamera((RwV3d*)&splashPos) < SQR(70.0f * TheCamera.m_fGenerationDistMultiplier)) {
            splashPos.z = waterLevel + 0.1f;
            CParticle::AddParticle(PARTICLE_CAR_SPLASH, p1, 0.75f * splashDir, nil, splashSize + 0.1f, splashColor,
                CGeneral::GetRandomNumberInRange(0.0f, 10.0f), CGeneral::GetRandomNumberInRange(0.0f, 90.0f),
                1, lifeBase + splashDir.z * lifeMult);
            CParticle::AddParticle(PARTICLE_BOAT_SPLASH, p1, splashDir, nil, splashSize, jetColor,
                CGeneral::GetRandomNumberInRange(0.0f, 0.4f), CGeneral::GetRandomNumberInRange(0.0f, 45.0f),
                0, lifeBase + splashDir.z * lifeMult);
        }
    }
   /* const auto speedDist = ((Vec)m_vecMoveSpeed).MagnitudeSqr();
    if (speedDist <= 0.0025f || GetUp().z <= 0.0f || TheCamera.GetLookingForwardFirstPerson() || !IsVisible()) {
        return;
    }

    if (m_autoPilot.m_nCarMission == MISSION_CRUISE && (CTimer::m_FrameCounter & 2) != 0) {
        return;
    }

    auto vec = GetPosition() - TheCamera.GetPosition(); // -> DistanceBetweenPoints2D()
    vec.z = 0.0f;
    auto dist = vec.Magnitude();
    if (dist >= 80.0f)
        return;

    auto v9 = std::sqrt(speedDist) * 0.075f * Damping;
    if (m_nModelIndex == MODEL_SKIMMER) {
        v9 = min(v9 * 3.0f, 0.5f);
    }
    else if (v9 > 1.0f) {
        v9 = 1.0f;
    }

    if (v9 <= 0.15f) {
        return;
    }

    auto v48 = v9 * 0.75f;
    if (m_autoPilot.m_nCarMission == MISSION_CRUISE) {
        auto v10 = v48 + v48;
        if (v10 >= 1.0f)
            v48 = 1.0f;
        else
            v48 = v10;
    }

    auto alpha0 = std::min(v48 * 128.0f, 64.0f);
    if (dist > 50.0f) {
        alpha0 *= (80.0f - dist) * 0.033f;
    }
    CVector p0, p1;
    // FxPrtMult_c particleData(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    auto alpha = alpha0 / 255.0f;
    RwRGBA color;
    color.alpha = alpha >= 1.0f ? 1.0f : alpha;

    auto v12 = v48 * 10.0f;
    auto size = v12 + 0.75f * 0.1f;
    float m_fsize = size >= 1.0f ? 1.0f : size;

    auto life = CGeneral::GetRandomNumberInRange(0.8f, 1.2f) * (v48 + v48 + 0.3f) * 0.2f;
    int32_t m_fLife = life >= 1.0f ? 1.0f : life;

    const CVector& colMin = GetColModel()->m_boundBox.m_vecMin;
    const CVector& colMax = GetColModel()->m_boundBox.m_vecMax;

    auto X_MULT = 0.7f;
    auto Z_MULT = 0.0f;
    if (m_nModelIndex == MODEL_SKIMMER) {
        X_MULT = 0.25f; // 0x8D3678
        Z_MULT = 0.85f; // 0x8D3674
    }

    auto baseVel = CVector{ -GetForward().x, -GetRight().y, -GetRight().z };
    CVector dir = 0.5f * m_vecMoveSpeed;
    dir *= v12;
    p0 = { colMin.x * X_MULT, colMax.y / 2.0f, colMin.z * Z_MULT };
    p0 = ((Matrix*)m_matrix)->TransformPoint(p0);
    auto vel0 = baseVel * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel0 -= GetRight() * CGeneral::GetRandomNumberInRange(0.3f, 0.7f); // minus
    vel0 += GetUp() * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel0 *= v12;
    CParticle::AddParticle(PARTICLE_BOAT_SPLASH, p0, dir, NULL, m_fsize);
    //  g_fx.m_BoatSplash->AddParticle(&p0, &vel0, 0.0f, &particleData, -1.0f, 1.2f, 0.6f, false);

    p1 = { colMax.x * X_MULT, colMax.y / 2.0f, colMin.z * Z_MULT };
    p1 = ((Matrix*)m_matrix)->TransformPoint(p1);
    auto vel1 = baseVel * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel1 += GetRight() * CGeneral::GetRandomNumberInRange(0.3f, 0.7f);  // plus
    vel1 += GetUp() * CGeneral::GetRandomNumberInRange(0.8f, 1.2f);
    vel1 *= v12;
    CParticle::AddParticle(PARTICLE_BOAT_SPLASH, p1, dir, NULL, m_fsize);
    // g_fx.m_BoatSplash->AddParticle(&p1, &vel1, 0.0f, &particleData, -1.0f, 1.2f, 0.6f, false);*/
}

int32_t
Auto::AddWheelDirtAndWater(CColPoint* colpoint, uint32_t belowEffectSpeed)
{
    int i;
    CVector dir;
    static RwRGBA grassCol = { 8, 24, 8, 255 };
    static RwRGBA gravelCol = { 64, 64, 64, 255 };
    static RwRGBA mudCol = { 64, 32, 16, 255 };
    static RwRGBA sandCol = { 170, 165, 140, 255 };
    static RwRGBA waterCol = { 48, 48, 64, 0 };

    //if (!belowEffectSpeed &&
     //   colpoint->m_nSurfaceTypeB !=
     //  SURFACE_SAND_ARID && colpoint->m_nSurfaceTypeB != SURFACE_SAND_BEACH)
      //  return 0;
    switch (colpoint->m_nSurfaceTypeB) {
    case SURFACE_GRASS_MEDIUM_LUSH:
    case SURFACE_GRASS_LONG_DRY:
    case SURFACE_GRASS_MEDIUM_DRY:
    case SURFACE_GRASS_SHORT_DRY:
    case SURFACE_GRASS_SHORT_LUSH:
    case SURFACE_GRASS_LONG_LUSH:
        //  debug("Grass");
        dir.x = -0.05f * m_vecMoveSpeed.x;
        dir.y = -0.05f * m_vecMoveSpeed.y;
        for (i = 0; i < 4; i++) {
            dir.z = CGeneral::GetRandomNumberInRange(0.03f, 0.06f);
            CParticle::AddParticle(PARTICLE_SAND, colpoint->m_vecPoint, dir, NULL,
                CGeneral::GetRandomNumberInRange(0.02f, 0.1f), grassCol);
        }
        break;
        return 0;
    case SURFACE_GRAVEL:
        dir.x = -0.05f * m_vecMoveSpeed.x;
        dir.y = -0.05f * m_vecMoveSpeed.y;
        for (i = 0; i < 4; i++) {
            dir.z = CGeneral::GetRandomNumberInRange(0.03f, 0.06f);
            CParticle::AddParticle(PARTICLE_WHEEL_DIRT, colpoint->m_vecPoint, dir, NULL,
                CGeneral::GetRandomNumberInRange(0.05f, 0.09f), gravelCol);
        }
        return 1;
    case SURFACE_MUD_DRY:
        dir.x = -0.05f * m_vecMoveSpeed.x;
        dir.y = -0.05f * m_vecMoveSpeed.y;
        for (i = 0; i < 4; i++) {
            dir.z = CGeneral::GetRandomNumberInRange(0.03f, 0.06f);
            CParticle::AddParticle(PARTICLE_WHEEL_DIRT, colpoint->m_vecPoint, dir, NULL,
                CGeneral::GetRandomNumberInRange(0.02f, 0.06f), mudCol);
        }
        return 0;
    case SURFACE_SAND_ARID:
    case SURFACE_SAND_BEACH:
        if (CTimer::m_FrameCounter & 2 ||
            CWeather::WetRoads > 0.0f && CGeneral::GetRandomNumberInRange(CWeather::WetRoads, 1.01f) > 0.5f)
            return 0;
        dir.x = 0.5f * m_vecMoveSpeed.x;
        dir.y = 0.5f * m_vecMoveSpeed.y;
        for (i = 0; i < 1; i++) {
            dir.z = CGeneral::GetRandomNumberInRange(0.02f, 0.055f);
            CParticle::AddParticle(PARTICLE_SAND, colpoint->m_vecPoint, dir, NULL,
                2.0f * m_vecMoveSpeed.Magnitude(), sandCol);
        }
        return 0;
    default:
        if (CWeather::WetRoads > 0.01f && m_vecMoveSpeed.Magnitude() > 0.1f && !CCullZones::CamNoRain() && !CCullZones::PlayerNoRain()) {
            if (CTimer::m_FrameCounter & 1)
                CParticle::AddParticle(
                    PARTICLE_WATERSPRAY,
                    colpoint->m_vecPoint + CVector(0.0f, 0.0f, 0.25f + 0.25f),
                    CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.005f, 0.04f)),
                    NULL,
                    CGeneral::GetRandomNumberInRange(0.1f, 0.5f), waterCol);
            return 0;
        }
        return 1;
    }
}

// Rain on roof
void Veh::AddWaterSplashParticles() {
    if (!IsPointInSphere(GetPosition(), TheCamera.GetPosition(), 10.f)) {
        return;
    }
    float fwdSpeed = DotProduct(m_vecMoveSpeed, GetForward()) * 180.0f;
    if (!CCullZones::CamNoRain() && !CCullZones::PlayerNoRain() &&
        fabsf(fwdSpeed) < 20.0f && CWeather::Rain > 0.02f) {
        CColModel* colModel = GetColModel();

        for (int i = 0; i < colModel->m_pColData->m_nNumTriangles; i++) {
            CVector p1, p2, p3, c;

            colModel->m_pColData->GetTrianglePoint(p1, colModel->m_pColData->m_pTriangles[i].m_nVertA);
            p1 = *GetMatrix() * p1;
            colModel->m_pColData->GetTrianglePoint(p2, colModel->m_pColData->m_pTriangles[i].m_nVertB);
            p2 = *GetMatrix() * p2;
            colModel->m_pColData->GetTrianglePoint(p3, colModel->m_pColData->m_pTriangles[i].m_nVertC);
            p3 = *GetMatrix() * p3;
            c = (p1 + p2 + p3) / 3.0f;

            int n = 6.0f * CWeather::Rain;
            for (int j = 0; j <= n; j++)
                CParticle::AddParticle(PARTICLE_RAIN_SPLASHUP,
                    c + CVector(CGeneral::GetRandomNumberInRange(-0.4f, 0.4f), CGeneral::GetRandomNumberInRange(-0.4f, 0.4f), 0.0f),
                    CVector(0.0f, 0.0f, 0.0f),
                    nullptr, 0.0f, 0, 0, GetRandomNumber() & 1);
        }
    }
}

void CWeap::DoWeaponEffect(CVector origin, CVector dir) {
    tParticleType fxName{};
    RwRGBA color{};
    switch (m_eWeaponType) {
    case eWeaponType::WEAPON_FTHROWER:	   fxName = ParticleEXFire ? PARTICLE_FIREBALL_PARTICLEEX : PARTICLE_FIREBALL; break;
    case eWeaponType::WEAPON_EXTINGUISHER: fxName = PARTICLE_STEAM;   break;
    case eWeaponType::WEAPON_SPRAYCAN:     fxName = PARTICLE_PAINT_SMOKE; color = { 0,255,0,255 };     break;
    default:                               CParticle::RemovePSystem(fxName); color = { 0,0,0,0 };      return;
    }
    dir /= 3.5f; // Make particles go slower
    CParticle::AddParticle(fxName, origin, dir, nullptr, 0.0f, color);
}

bool ShouldSnow() {
    // Condition: snow appears when the weather is cloudy, player is not in a interior, player & camera not in rain zones,
    // not underwater,
    // and when there is no rain (bScriptsForceRain == false && CWeather::Rain == 0.0f) and the current weather is not a sandstorm.
   /*tParticleSystemData* psystemdata = &mod_ParticleSystemManager.m_aParticles[PARTICLE_SNOW];

    CParticle* particle = psystemdata->m_pParticles;
    float waterlevel = 0.0f;
    if (particle) {
        if (CWaterLevel::GetWaterLevelNoWaves(particle->m_vecPosition.x, particle->m_vecPosition.y, particle->m_vecPosition.z, &waterlevel, nullptr, nullptr) && particle->m_vecPosition.z < waterlevel) {
            if (particle->m_vecPosition.z < waterlevel) {
                particle->RemoveParticle(particle, nullptr, psystemdata);
            }
        }
    }*/
    // Additional conditions:
    const bool notInSandstorm = (CWeather::Sandstorm == 0.0f); // Not a sandstorm
    const bool notRaining = !CWeather::bScriptsForceRain && CWeather::Rain == 0.0f; // Not raining
    // The main condition for the appearance of snow:
    const bool shouldSnow =
        (CWeather::NewWeatherType == WEATHER_CLOUDY_COUNTRYSIDE ||
            CWeather::NewWeatherType == WEATHER_CLOUDY_LA ||
            CWeather::NewWeatherType == WEATHER_CLOUDY_SF ||
            CWeather::NewWeatherType == WEATHER_CLOUDY_VEGAS ||
            CWeather::OldWeatherType == WEATHER_CLOUDY_COUNTRYSIDE ||
            CWeather::OldWeatherType == WEATHER_CLOUDY_LA ||
            CWeather::OldWeatherType == WEATHER_CLOUDY_SF ||
            CWeather::OldWeatherType == WEATHER_CLOUDY_VEGAS)
        &&
        notInSandstorm
        &&
        notRaining
        &&
        !CCullZones::CamNoRain()
        && !CCullZones::PlayerNoRain() && !CGame::currArea > 0 && !CWeather::UnderWaterness > 0.0f;

    return shouldSnow;
}

// Snow!
void AddSnow() {
    if (Snow) {
        if (ShouldSnow()) {
            for (int i = 0; i < SnowFlakes; i++)
            {
                CVector random_pos = FindPlayerEntity(-1)->TransformFromObjectSpace(CVector(CGeneral::GetRandomNumberInRange(-350.0f, 350.0f), CGeneral::GetRandomNumberInRange(-350.0f, 350.0f), CGeneral::GetRandomNumberInRange(-30.0f, 60.0f)));
                CVector random_2pos = FindPlayerEntity(-1)->TransformFromObjectSpace(CVector(CGeneral::GetRandomNumberInRange(-350.0f, 350.0f), CGeneral::GetRandomNumberInRange(-350.0f, 350.0f), CGeneral::GetRandomNumberInRange(-30.0f, 60.0f)));
                CParticle::AddParticlesAlongLine(PARTICLE_SNOW, random_pos, random_2pos, CVector(CWeather::WindDir.x, CWeather::WindDir.y, CWeather::WindDir.z), 20.0f, NULL, 0.1f, 0, 0, 0, 8000);
                random_pos = FindPlayerEntity(-1)->TransformFromObjectSpace(CVector(CGeneral::GetRandomNumberInRange(-50.0f, 50.0f), CGeneral::GetRandomNumberInRange(-50.0f, 50.0f), CGeneral::GetRandomNumberInRange(-30.0f, 60.0f)));
                random_2pos = FindPlayerEntity(-1)->TransformFromObjectSpace(CVector(CGeneral::GetRandomNumberInRange(-50.0f, 50.0f), CGeneral::GetRandomNumberInRange(-50.0f, 50.0f), CGeneral::GetRandomNumberInRange(-30.0f, 60.0f)));
                CParticle::AddParticlesAlongLine(PARTICLE_SNOW, random_pos, random_2pos, CVector(CWeather::WindDir.x, CWeather::WindDir.y, CWeather::WindDir.z), 10.0f, NULL, 0.1f, 0, 0, 0, 8000);
            }
        }

    }
}

uint8_t m_harvesterParticleCounter;
int32_t& MI_HARVESTERBODYPART1 = *(int32_t*)0x8CD6E4;
int32_t& MI_HARVESTERBODYPART2 = *(int32_t*)0x8CD6E8;
int32_t& MI_HARVESTERBODYPART3 = *(int32_t*)0x8CD6EC;
int32_t& MI_HARVESTERBODYPART4 = *(int32_t*)0x8CD6F0;
int32_t& MI_GRASSHOUSE = *(int32_t*)0x8CD6F4;
int32_t& MI_GRASSPLANT = *(int32_t*)0x8CD6F8;

void VehAudio::AddAudioEvent(eAudioEvents audioevent, float volume) {
    return plugin::CallMethod<0x4F6420, CAEVehicleAudioEntity*, eAudioEvents, float>(this, audioevent, volume);
}

void FireAudio::AddAudioEvent(eAudioEvents audioevent, CVector* pos) {
    return plugin::CallMethod<0x4DD3C0, CAEFireAudioEntity*, eAudioEvents, CVector*>(this, audioevent, pos);
}

void Auto::ProcessHarvester()
{
    if (m_nStatus != STATUS_PLAYER)
        return;

    CStreaming::m_bStreamHarvesterModelsThisFrame = true;
    if (m_vecMoveSpeed.Magnitude2D() > 0.01f) {
        if ((CTimer::m_FrameCounter & 1) != 0)
        {
            for (int32_t i = CPools::ms_pPedPool->m_nSize - 1; i >= 0; i--) {
                CPed* ped = CPools::ms_pPedPool->GetAt(i);
                if (!ped)
                    continue;

                if (ped->IsPlayer())
                    continue;

                CVector distance = ped->GetPosition() - GetPosition();
                float distanceForward = DotProduct(distance, GetForward());
                if (distanceForward > 4.0f && distanceForward < 5.0f) {
                    float distanceRight = std::abs(DotProduct(distance, GetRight()));
                    if (distanceRight < 4.0f) {
                        float distanceUp = std::abs(DotProduct(distance, GetUp()));
                        if (distanceUp < 4.0f) {
                            ped->FlagToDestroyWhenNextProcessed();
                            m_harvesterParticleCounter = 20;
                            static_cast<VehAudio>(m_vehicleAudio).AddAudioEvent(AE_BODY_HARVEST, 0.0f);
                        }
                    }
                }
            }
        }
        else
        {
            for (int32_t i = CPools::ms_pObjectPool->m_nSize - 1; i >= 0; i--) {
                CObject* object = CPools::ms_pObjectPool->GetAt(i);
                if (!object)
                    continue;

                if (m_nModelIndex != MI_GRASSHOUSE && m_nModelIndex != MI_GRASSPLANT)
                    continue;

                CVector distance1 = object->GetPosition() - GetPosition();
                float distanceForward1 = DotProduct(distance1, GetForward());
                if (distanceForward1 > 4.0f && distanceForward1 < 5.0f) {
                    float distanceRight1 = std::abs(DotProduct(distance1, GetRight()));
                    if (distanceRight1 < 4.0f) {
                        float distanceUp1 = std::abs(DotProduct(distance1, GetUp()));
                        if (distanceUp1 < 4.0f)
                            object->ObjectDamage(99999.0f, nullptr, nullptr, this, (eWeaponType)0x32);
                    }
                }
            }
        }
    }

    if (!m_harvesterParticleCounter)
        return;

    CVector pos = reinterpret_cast<Matrix*>(m_matrix)->TransformPoint(CVector(-1.2f, -3.8f, 1.5f));
    CVector velocity = GetForward() * -0.1f;
    velocity.x += CGeneral::GetRandomNumberInRange(0.05f, -0.05f);
    velocity.y += CGeneral::GetRandomNumberInRange(0.05f, -0.05f);
    int32_t bodyPartModelId = -1;
    switch (m_harvesterParticleCounter - 1) { // todo: remove -1 and refactor?
    case 0:
    case 6:
        bodyPartModelId = MI_HARVESTERBODYPART1;
        break;
    case 1:
    case 3:
        bodyPartModelId = MI_HARVESTERBODYPART2;
        break;
    case 2:
        bodyPartModelId = MI_HARVESTERBODYPART3;
        break;
    case 5:
        bodyPartModelId = MI_HARVESTERBODYPART4;
        break;
    }

    if (bodyPartModelId >= 0) {
        CObject* limb = CObject::Create(bodyPartModelId);
        CPlaceable::SetMatrix(*m_matrix);
        limb->SetPosn(pos);
        limb->m_vecMoveSpeed = velocity;
        limb->m_vecTurnSpeed = CVector{
            CGeneral::GetRandomNumberInRange(0.12f, -0.04f),
            CGeneral::GetRandomNumberInRange(0.12f, -0.04f),
            CGeneral::GetRandomNumberInRange(0.12f, -0.04f)
        };
        limb->m_nObjectType = OBJECT_TEMPORARY;
        limb->UpdateRwMatrix();
        limb->UpdateRwFrame();
        limb->SetIsStatic(false);
        //CObject::nNoTempObjects++;
        CWorld::Add(limb);
    }
    m_harvesterParticleCounter--;
    if (m_harvesterParticleCounter % 3 == 0 && BloodnGore) {
        // Gory asf
        CParticle::AddParticle(PARTICLE_TEST, pos, velocity);
        CParticle::AddParticle(PARTICLE_BLOOD_SPURT, pos, velocity);
        CParticle::AddParticle(PARTICLE_BLOOD, pos, velocity);
        CParticle::AddParticle(PARTICLE_BLOOD_SMALL, pos, velocity);
        if (BloodDrops) {
            CVector dropDir(CGeneral::GetRandomNumberInRange(-0.15f, 0.15f), CGeneral::GetRandomNumberInRange(0.1f, 0.35f), 0.f);
            CVector dropPos(CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_X(50.0f), SCREEN_STRETCH_FROM_RIGHT(50.0f)),
                CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_Y(50.0f), SCREEN_STRETCH_FROM_BOTTOM(50.0f)), 1.f);
            CParticle::AddParticle(PARTICLE_BLOODDROP, dropPos, dropDir, NULL, CGeneral::GetRandomNumberInRange(0.1f, 0.15f),
                RwRGBA(0, 0, 0, 0), 0, 0, GetRandomNumber() & 1, 0);
        }
    }
}
void Obj::DoBurnEffect() const {
    const auto& box = CModelInfo::GetModelInfo(m_nModelIndex)->m_pColModel->m_boundBox;
    const auto& vecSize = box.m_vecMax - box.m_vecMin;
    const auto nUsedSize = static_cast<int32_t>(vecSize.x * vecSize.y * vecSize.z * m_fBurnDamage / 20.0F);
    if (nUsedSize <= 0)
        return;

    for (auto i = 0; i < nUsedSize; ++i) {
        const auto fRandX = CGeneral::GetRandomNumberInRange(box.m_vecMin.x, box.m_vecMax.x);
        const auto fRandY = CGeneral::GetRandomNumberInRange(box.m_vecMin.y, box.m_vecMax.y);
        const auto fRandZ = CGeneral::GetRandomNumberInRange(box.m_vecMin.z, box.m_vecMax.z);
        auto vecParticlePos = reinterpret_cast<Matrix*>(m_matrix)->TransformPoint(CVector(fRandX, fRandY, fRandZ));

        auto vecVelocity = CVector(0.0F, 0.0F, 0.02F);
        CParticleObject::AddObject(POBJECT_SMOKE_TRAIL, vecParticlePos, vecVelocity, 0.55f, 350, RwRGBA(0, 0, 0, 0), true);
    }
}
auto Firev::GetFireParticleNameForStrength() const {
    if (m_fStrength > 1.0f)
        return (m_fStrength > 2.0f) ? PARTICLE_CARFLAME : PARTICLE_FLAME;
    else
        return PARTICLE_CARFLAME;
};

// 0x539360
void Firev::CreateFxSysForStrength(const CVector& point, RwMatrix* matrix) {
    //DestroyFx();
    CParticle::AddParticle(GetFireParticleNameForStrength(), point,
        CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.0125f, 0.1f) * m_fStrength),
        0, m_fStrength, 0, 0, 0, 0);

    //  GetRandomNumber(); GetRandomNumber(); GetRandomNumber(); /* unsure why these three rands are called */

    CParticle::AddParticle(PARTICLE_CARFLAME_SMOKE, point,
        CVector(0.0f, 0.0f, 0.0f), 0, 0.0f, 0, 0, 0, 0);
    //g_fxMan.CreateFxSystem(GetFireParticleNameForStrength(), const_cast<CVector*>(&point), matrix, true); // TODO: Make CreateFxSys take const CVector&
    //if (m_pFxSystem)
        //m_pFxSystem->Play();
}

CVector RandomVec1(float min, float max)
{
    const auto Get = [=] { return CGeneral::GetRandomNumberInRange(min, max); };
    return { Get(), Get(), Get() };
}
void test(CWeapon* weapon, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int arg5) {
    if (!victim)
        return;
    CParticle::AddParticle(PARTICLE_TEST, *startPoint, CVector(0.0f, 0.0f, 0.0f));
    debug("SUS");
}

plugin::ThiscallEvent <plugin::AddressList<0x53EB0D, plugin::H_CALL>, plugin::PRIORITY_AFTER, plugin::ArgPickN<CCamera*, 0>, void(CCamera*)> Idle;
// 0x59E9B0
/*bool AddBreak(CObject* object, CVector* vecPos, float fVelocityRand, bool bSmash) {
    return CallMethodAndReturn<bool, 0x59E9B0, CObject*, CVector*, float, bool>(object, vecPos, fVelocityRand, bSmash);
}
int32_t& MI_IMY_SHASH_WALL = *(int32_t*)0x8CD748;
void Obj::ObjectDamage(float damage, const CVector* fxOrigin, const CVector* fxDirection, CEntity* damager, eWeaponType weaponType) {
    if (!m_bUsesCollision)
        return;

    if (weaponType == 55 && damager && damager->m_nType == ENTITY_TYPE_VEHICLE)
        weaponType == 50;

    if (!CanPhysicalBeDamaged(weaponType, nullptr))
        return;

    m_fHealth -= damage * m_pObjectInfo->m_fColDamageMultiplier;
    m_fHealth = max(0.0F, m_fHealth);

    if (!m_nColDamageEffect || m_nPhysicalFlags.bInvulnerable && damager != FindPlayerPed() && damager != FindPlayerVehicle(-1, false))
        return;

    // Big Smoke crack palace wall break checks
    if (m_nModelIndex == MI_IMY_SHASH_WALL) {
        if (!damager)
            return;

        if (damager->m_nType == ENTITY_TYPE_PED) {
            auto* ped = ((CPed*)damager);
            if (!ped->m_nPedFlags.bInVehicle || !ped->m_pVehicle || ped->m_pVehicle->m_nModelIndex != MODEL_SWATVAN)
                return;
        }
        else if (damager->m_nType == ENTITY_TYPE_VEHICLE) {
            if (damager->m_nModelIndex != MODEL_SWATVAN)
                return;
        }
        else
            return;
    }

    if (damager && damager->m_nModelIndex == MODEL_FORKLIFT)
        return;

    m_nLastWeaponDamage = weaponType;
    bool bWasDestroyed = false;

    if (damage * m_pObjectInfo->m_fColDamageMultiplier > 150.0F || m_fHealth == 0.0F) {
        switch (m_nColDamageEffect) {
        case 1:
            if (!m_bRenderDamaged) {
                bWasDestroyed = true;
                DeleteRwObject();
            }
            m_bRenderDamaged = true;
            break;

        case 20:
            m_bUsesCollision = false;
            m_bIsVisible = false;
            if (!m_bIsStatic)
                CPhysical::RemoveFromMovingList();

            m_bIsStatic = true;
            m_nPhysicalFlags.bExplosionProof = true;
            m_vecMoveSpeed = CVector();
            m_vecTurnSpeed = CVector();
            DeleteRwObject();
            bWasDestroyed = true;
            break;

        case 21:
            if (m_bRenderDamaged) {
                m_bUsesCollision = false;
                m_bIsVisible = false;
                if (!m_bIsStatic) {
                   RemoveFromMovingList();
                }

                m_bIsStatic = true;
                m_nPhysicalFlags.bExplosionProof = true;
                m_vecMoveSpeed = CVector();
                m_vecTurnSpeed = CVector();
                DeleteRwObject();
                bWasDestroyed = true;
            }
            else {
                DeleteRwObject();
                m_bRenderDamaged = true;
            }
            break;

        case 200:
        case 202: {
            const auto bJustFaces = damage * m_pObjectInfo->m_fColDamageMultiplier > m_pObjectInfo->m_fSmashMultiplier * 150.0F;
            AddBreak(this, &m_pObjectInfo->m_vecBreakVelocity, m_pObjectInfo->m_fBreakVelocityRand, bJustFaces);

            m_bUsesCollision = false;
            m_bIsVisible = false;
            if (!m_bIsStatic))
                RemoveFromMovingList();

            m_bIsStatic = true;
            m_nPhysicalFlags.bExplosionProof = true;
            m_vecMoveSpeed = CVector();
            m_vecTurnSpeed = CVector();
            m_nObjectFlags.bIsBroken = true;
            DeleteRwObject();
            bWasDestroyed = true;
            break;
        }
        }

        if (!m_bUsesCollision && !m_bIsVisible) {
            m_fHealth = 0.0F;
        }
    }

    bool bExploded = false;
    if (bWasDestroyed) {
        if (TryToExplode()) {
            bExploded = true;
        }
        AudioEngine.ReportObjectDestruction(this); // todo: Add check ``if bExploded``?
    }

    // Particle creation
    if (bWasDestroyed && bExploded)
        return;

    switch (m_pObjectInfo->m_nFxType) {
    case eObjectFxType::NO_FX:
        return;
    case eObjectFxType::PLAY_ON_HIT_DESTROYED:
        break;
    case eObjectFxType::PLAY_ON_DESTROYED:
        if (bWasDestroyed) {
            break;
        }
        return;
    case eObjectFxType::PLAY_ON_HIT:
        if (damage > 30.0f) {
            break;
        }
        return;
    default:
        return;
    }

    if (m_pObjectInfo->m_vFxOffset.x < -500.0F) {
        if (!fxOrigin)
            return;

        RwMatrix particleMat;
        g_fx.CreateMatFromVec(&particleMat, fxOrigin, fxDirection);
        auto* fxSystem = g_fxMan.CreateFxSystem(m_pObjectInfo->m_pFxSystemBP, particleMat, nullptr, false);
        if (fxSystem)
            fxSystem->PlayAndKill();

        return;
    }

    auto vecPoint = reinterpret_cast<Matrix*>(m_matrix)->TransformPoint(m_pObjectInfo->m_vFxOffset);
    vecPoint += GetPosition();
    auto fxSystem = g_fxMan.CreateFxSystem(m_pObjectInfo->m_pFxSystem, vecPoint, nullptr, false);
    if (fxSystem)
        fxSystem->PlayAndKill();

}*/

static void __fastcall MyDoBulletImpact(CWeapon* weapon, int i, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int a7)
{
    log("fastcall impact");
    if (BloodnGore) {
        eWeaponType weaptype;
        uint8_t bloodAmount = 8;

        if (victim && owner) {
            CPed* pedVictim = ((CPed*)victim);
            CPed* pedOwner = ((CPed*)owner);

            weaptype = pedOwner->m_aWeapons[pedOwner->m_nActiveWeaponSlot].m_eWeaponType;
            CVector dir = (colPoint->m_vecPoint - pedVictim->GetPosition()) * 0.01f;
            dir.z = 0.01f;
            bool isHeavy = weaptype >= WEAPON_GOLFCLUB && weaptype <= WEAPON_KATANA;

            // If it's a ped, and NOT bullet proof...
            if (pedVictim->m_nType == ENTITY_TYPE_PED && !pedVictim->m_nPhysicalFlags.bBulletProof) {
                if (weaptype == WEAPON_MINIGUN) {
                    CParticle::AddParticle(PARTICLE_TEST, colPoint->m_vecPoint, CVector(0.f, 0.f, 0.f), NULL, 0.f, 0, 0, 0, 0);
                    CParticle::AddParticle(PARTICLE_TEST, colPoint->m_vecPoint + CVector(0.2f, -0.2f, 0.f), CVector(0.f, 0.f, 0.f), NULL, 0.f, 0, 0, 0, 0);
                    CParticle::AddParticle(PARTICLE_TEST, colPoint->m_vecPoint + CVector(-0.2f, 0.2f, 0.f), CVector(0.f, 0.f, 0.f), NULL, 0.f, 0, 0, 0, 0);
                }
                else {
                    for (int32_t i = 0; i < 8; i++) {
                        if (pedVictim->GetIsOnScreen())
                        {
                            CParticle::AddParticle(PARTICLE_BLOOD_SMALL, colPoint->m_vecPoint + CVector(0.0f, 0.0f, 0.5f), dir);
                        }
                    }
                }
                if (isHeavy)
                {
                    dir.x += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                    dir.y += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                    CParticle::AddParticle(PARTICLE_BLOOD, colPoint->m_vecPoint, dir);

                    dir.x += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                    dir.y += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                    CParticle::AddParticle(PARTICLE_BLOOD, colPoint->m_vecPoint, dir);
                }
                if (weaptype == WEAPON_SHOTGUN || weaptype == WEAPON_SPAS12 || weaptype == WEAPON_SAWNOFF) {
                    bloodAmount = 32;
                    debug("Shotgun");
                    log("Shotgun");
                }
                else {
                    bloodAmount = 8;
                    debug("Not shotgun");
                    log("Not shotgun");
                }

                for (uint8_t i = 0; i < bloodAmount; i++) {
                    CParticle::AddParticle(PARTICLE_BLOOD_SMALL, colPoint->m_vecPoint, dir);
                }
            }
        }
    }

    if (BulletImpactParticles) {
        if (victim && owner) {
            if (victim->m_nType == ENTITY_TYPE_BUILDING || victim->m_nType == ENTITY_TYPE_VEHICLE || victim->m_nType == ENTITY_TYPE_OBJECT) {
                {
                    for (int32_t i = 0; i < 16; i++) {
                        CParticle::AddParticle(PARTICLE_SPARK, colPoint->m_vecPoint, colPoint->m_vecNormal * 0.05f);
                    }

                    CVector smokePos = colPoint->m_vecPoint;
                    smokePos.x += CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
                    smokePos.y += CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
                    smokePos.z += CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);

                    CParticle::AddParticle(PARTICLE_BULLETHIT_SMOKE, smokePos, CVector(0.0f, 0.0f, 0.0f));
                }
            }
        }
    }
    weapon->DoBulletImpact(owner, victim, startPoint, endPoint, colPoint, a7);
}

// Instead of noping particles, which some of them has sounds (and to keep skygfx droplets),
// you just set their alpha to 0
static void __fastcall FxInfo(FxPrtMult_c* infa,
    float r,
    float g,
    float b,
    float a,
    float size,
    float a7,
    float lifetime) {
    FxPrtMult_c info(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    infa = &info;
}


static void __fastcall MyWeaponFire(CWeapon* weapon, int,
    CPed* owner,
    CVector* vecOrigin,
    CVector* vecEffectPosn,
    CEntity* targetEntity,
    CVector* vecTarget,
    CVector* arg_14)
{
        DoGunFlash(owner, vecEffectPosn, false);
        weapon->Fire(owner, vecOrigin, vecEffectPosn, targetEntity, vecTarget, arg_14);
}

static void __fastcall MyTriggerGunflash(Fx_c* fx, int, CEntity* entity, CVector& origin, CVector& target, bool doGunflash)
{
    if (entity) {
        CPed* ped = reinterpret_cast<CPed*>(entity);
        auto Task = ped->m_pIntelligence->GetTaskUseGun();
        if (Task) {
        bool LeftHand = Task->bLefttHand;
        if (doGunflash) {
            DoGunFlash(ped, &origin, LeftHand);
        }
       }
    }
}
void __fastcall DoDriveByGunflash(CPed* driver, int, int, bool leftHand) {
    CVector pos;
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    DoGunFlash(driver, &pos, leftHand);
}

RwRGBA FoamColour(255, 255, 255, 255);
uint32_t CSpecialParticleStuff::BoatFromStart; // Typo?

void
CSpecialParticleStuff::CreateFoamAroundObject(CMatrix* pMatrix, float innerFw, float innerRg, float innerUp, int32_t particles)
{
    float outerFw = innerFw + 5.0f;
    float outerRg = innerRg + 5.0f;
    float outerUp = innerUp + 5.0f;
    for (int attempts = 0; particles > 0 && attempts < 1000; attempts++) {
        CVector pos;
        int rnd = GetRandomNumber();
        pos.x = (int8_t)(rnd - 128) * innerFw / 110.0f;
        pos.y = (int8_t)((rnd >> 8) - 128) * innerFw / 110.0f;
        pos.z = 0.0f;
        if (DotProduct2D(pos, TheCamera.GetForward()) >= 0)
            continue;
        // was there any point in adding it here?
        pos += pMatrix->GetPosition();
        pos.z = 2.0f;
        float fw = fabsf(DotProduct(pMatrix->GetForward(), pos - pMatrix->GetPosition()));
        if (fw >= outerFw)
            continue;
        float rg = fabsf(DotProduct(pMatrix->GetRight(), pos - pMatrix->GetPosition()));
        if (rg >= outerRg)
            continue;
        float up = fabsf(DotProduct(pMatrix->GetUp(), pos - pMatrix->GetPosition()));
        if (up >= outerUp)
            continue;
        if (fw > innerFw || rg > innerRg || up > innerUp) {
            CParticle::AddParticle(PARTICLE_STEAM2, pos, CVector(0.0f, 0.0f, 0.0f), NULL, 4.0f, FoamColour, 1, 0, 0, 0);
            particles--;
        }
    }
}

void
CSpecialParticleStuff::StartBoatFoamAnimation()
{
    BoatFromStart = CTimer::m_snTimeInMilliseconds;
}

void
CSpecialParticleStuff::UpdateBoatFoamAnimation(CMatrix* pMatrix)
{
    static int32_t FrameInAnimation = 0;
    static float X, Y, Z, dX, dY, dZ;
    CreateFoamAroundObject(pMatrix, 107.0f, 24.1f, 30.5f, 2);
    uint32_t prev = CTimer::m_snPreviousTimeInMilliseconds;
    uint32_t cur = CTimer::m_snTimeInMilliseconds;
    if (FrameInAnimation != 0) {
        X += dX;
        Y += dY;
        Z += dZ;
        CVector pos = *pMatrix * CVector(X, Y, Z);
        CParticle::AddParticle(PARTICLE_STEAM_NY, pos, CVector(0.0f, 0.0f, 0.0f),
            NULL, FrameInAnimation * 0.5f + 2.0f, FoamColour, 1, 0, 0, 0);
        if (++FrameInAnimation > 15)
            FrameInAnimation = 0;
    }
    if ((cur & 0x3FF) < (prev & 0x3FF)) {
        FrameInAnimation = 1;
        int rnd = GetRandomNumber();
        X = (int8_t)(rnd - 128) * 0.2f;
        Y = (int8_t)((rnd >> 8) - 128) * 0.2f;
        Z = 10.0f;
        rnd = GetRandomNumber();
        dX = (int8_t)(rnd - 128) * 0.02f;
        dY = (int8_t)((rnd >> 8) - 128) * 0.02f;
        dZ = 2.0f;
    }
}
#ifdef GTA_SCENE_EDIT
#include "CHud.h"
#include <CTheScripts.h>
#include <CTheScripts.h>
#include <Buoyancy.h>
#include <FallingGlassPane.h>
void
Cam::Process_Editor(const CVector&, float, float, float)
{
    static float Speed = 0.0f;
    CVector TargetCoors;

    if (m_bResetStatics) {
        m_vecSource = CVector(796.0f, -937.0f, 40.0f);
        m_pCamTargetEntity = NULL;
    }
    m_bResetStatics = false;

    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.9f);
    // m_fFOV = m_fFOV;
   //  m_fTrueAlpha += DEGTORAD(CPad::GetPad(1)->GetLeftStickY()) / 50.0f;
    // m_fTrueBeta += DEGTORAD(CPad::GetPad(1)->leftstick * 1.5f) / 19.0f;

    if (m_pCamTargetEntity && CSceneEdit::m_bCameraFollowActor) {
        TargetCoors = m_pCamTargetEntity->GetPosition();
    }
    else if (CSceneEdit::m_bRecording) {
        TargetCoors.x = m_vecSource.x + cosf(m_fTrueAlpha) * sinf(m_fTrueBeta) * 7.0f;
        TargetCoors.y = m_vecSource.y + cosf(m_fTrueAlpha) * cosf(m_fTrueBeta) * 7.0f;
        TargetCoors.z = m_vecSource.z + sinf(m_fTrueAlpha) * 7.0f;
    }
    else
        TargetCoors = CSceneEdit::m_vecCamHeading + m_vecSource;
    CSceneEdit::m_vecCurrentPosition = TargetCoors;
    CSceneEdit::m_vecCamHeading = TargetCoors - m_vecSource;

    if (m_fTrueAlpha > DEGTORAD(89.5f)) m_fTrueAlpha = DEGTORAD(89.5f);
    else if (m_fTrueAlpha < DEGTORAD(-89.5f)) m_fTrueAlpha = DEGTORAD(-89.5f);

    if (CPad::GetPad(1)->NewMouseControllerState.lmb)
        Speed += 0.1f;
    else if (CPad::GetPad(1)->NewMouseControllerState.rmb)
        Speed -= 0.1f;
    else
        Speed = 0.0f;
    if (Speed > 70.0f) Speed = 70.0f;
    if (Speed < -70.0f) Speed = -70.0f;

    m_vecFront = TargetCoors - m_vecSource;
    m_vecFront.Normalise();
    m_vecSource = m_vecSource + m_vecFront * Speed;

    if (m_vecSource.z < -450.0f)
        m_vecSource.z = -450.0f;

    if (CPad::GetPad(1)->SprintJustDown()) {
        if (FindPlayerVehicle(-1, false))
            FindPlayerVehicle(-1, false)->Teleport(m_vecSource, false);
        else
            CWorld::Players[CWorld::PlayerInFocus].m_pPed->SetPosn(m_vecSource);

    }

    // stay inside sectors
   /*while (CWorld::GetSectorX(Source.x) > NUMSECTORS_X - 5.0f)
        Source.x -= 1.0f;
    while (CWorld::GetSectorX(Source.x) < 5.0f)
        Source.x += 1.0f;
    while (CWorld::GetSectorY(Source.y) > NUMSECTORS_X - 5.0f)
        Source.y -= 1.0f;
    while (CWorld::GetSectorY(Source.y) < 5.0f)
        Source.y += 1.0f;
    GetVectorsReadyForRW();*/

    if (CPad::GetPad(1)->NewMouseControllerState.lmb)
        CShadows::StoreShadowToBeRendered(SHADOW_ADDITIVE, gpShadowExplosionTex, &m_vecSource,
            12.0f, 0.0f, 0.0f, -12.0f,
            128, 128, 128, 128, 1000.0f, false, 1.0f, NULL, false);

    if (CHud::m_Wants_To_Draw_Hud) {
        char str[256];
        sprintf(str, "CamX: %f CamY: %f  CamZ:  %f", m_vecSource.x, m_vecSource.y, m_vecSource.z);
        sprintf(str, "Frontx: %f, Fronty: %f, Frontz: %f ", m_vecFront.x, m_vecFront.y, m_vecFront.z);
        sprintf(str, "Look@: %f, Look@: %f, Look@: %f ", m_vecFront.x + m_vecSource.x, m_vecFront.y + m_vecSource.y, m_vecFront.z + m_vecSource.z);
    }
}
#endif
/*float& m_fWaterFXStartUnderWaterness = *(float*)0x8D514C; // 0.535f;
// 0x716C90
void Clouds::MovingFogRender() {

    if (MovingFog_GetFXIntensity() == 0.f || !CGame::CanSeeOutSideFromCurrArea() && FindPlayerPed()->m_nAreaCode != 0)
        return;

    // Adjust fog intensity
    {
        const float step = CTimer::ms_fTimeStep / 300.f;
        if (CCullZones::CamNoRain() && CCullZones::PlayerNoRain())
            CurrentFogIntensity = max(CurrentFogIntensity - step, 0.f); // Decreasing [towards 0]
        else
            CurrentFogIntensity = min(CurrentFogIntensity + step, 1.f); // Increasing [towards 1]

        if (CWeather::UnderWaterness >= m_fWaterFXStartUnderWaterness) {
            CurrentFogIntensity = 0.f;
            return;
        }

        if (CurrentFogIntensity == 0.f) {
            return;
        }
    }

    CVector camUp = TheCamera.m_matrix->GetUp(), camRight = TheCamera.m_matrix->GetRight();

  //  CPostEffects::ImmediateModeRenderStatesStore();
  //  CPostEffects::ImmediateModeRenderStatesSet();
   // ImmediateModeRenderStatesStore();
   // ImmediateModeRenderStatesSet();

  //  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
   // RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCloudMaskTex)));
   // RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    const int32_t red = min(CTimeCycle::m_CurrentColours.m_nSkyBottomRed + 132, 255);
    const int32_t green = min(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen + 132, 255);
    const int32_t blue = min(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue + 132, 255);

    int32_t numVerts = 0;

    for (auto fogIdx = 0u; fogIdx < MAX_MOVING_FOG; fogIdx++) {
        if (!ms_mf.m_bFogSlots[fogIdx])
            continue;

        const auto& pos = ms_mf.m_vecPosn[fogIdx];
        const float   halfSize = ms_mf.m_fSize[fogIdx] / 2.f;
        const CVector fogUp = camUp * halfSize;
        const CVector fogRight = camRight * halfSize;

        // Original code used a switch case, we're going to use a lookup table to make it nicer. (And faster)
        const struct { CVector pos; RwTexCoords uv; } corners[]{
            { pos + fogRight + fogUp,  {0.f, 0.f} }, // Top right
            { pos + fogRight - fogUp,  {1.f, 0.f} }, // Bottom right
            { pos - fogRight - fogUp,  {1.f, 1.f} }, // Bottom left
            { pos - fogRight + fogUp,  {0.f, 1.f} }, // Top left
        };

        const auto alpha = static_cast<int32_t>(MovingFog_GetFXIntensity() * ms_mf.m_fIntensity[fogIdx] * CurrentFogIntensity);

           // auto& vert = TempBufferVertices.m_3d[numVerts++];
          //  RwV3dAssign(RwIm3DVertexGetPos(&vert), &corner.pos);
           // RwIm3DVertexSetRGBA(&vert, red, green, blue, alpha);
           // RwIm3DVertexSetU(&vert, corner.uv.u);
           // RwIm3DVertexSetV(&vert, corner.uv.v);
            CParticle::AddParticle(PARTICLE_STEAM2, pos, CVector(ms_mf.m_vecWind.x, ms_mf.m_vecWind.y, ms_mf.m_vecWind.z), NULL, halfSize, RwRGBA(red,green,blue,alpha));
            // Flush buffer if it's getting full
            //if (numVerts == TOTAL_TEMP_BUFFER_3DVERTICES - 2) {
             //   RenderVertices();
           // }

    }

   // ImmediateModeRenderStatesReStore();
    MovingFog_Update();
}*/
RwRGBA SmokeColor = { 0, 0, 0, 0 };

// Nuclear explosion attempt
void CreateNuclearExplosion(CVector position) {
    // Bright flash
    for (int i = 0; i < 10; ++i) {
        CParticle::AddParticle(PARTICLE_GUNFLASH, position, CVector(0.0f, 0.0f, 0.0f), nullptr, 4.0f + i * 0.1f);
    }

    // Fire ball
    for (int i = 0; i < 50; ++i) {
        float angle = CGeneral::GetRandomNumberInRange(0.0f, 2.0f * M_PI);
        float distance = CGeneral::GetRandomNumberInRange(0.0f, 5.0f);
        CVector offset(cos(angle) * distance, sin(angle) * distance, CGeneral::GetRandomNumberInRange(0.0f, 3.0f));
        CVector velocity(cos(angle) * 2.0f, sin(angle) * 2.0f, 1.0f + CGeneral::GetRandomNumberInRange(0.0f, 2.0f));
        CParticle::AddParticle(PARTICLE_EXPLOSION_LARGE, position + offset, velocity, nullptr, 3.0f);
    }

    // Uprising smoke
    for (int i = 0; i < 100; ++i) {
        float angle = CGeneral::GetRandomNumberInRange(0.0f, 2.0f * M_PI);
        float distance = CGeneral::GetRandomNumberInRange(0.0f, 10.0f);
        CVector offset(cos(angle) * distance, sin(angle) * distance, CGeneral::GetRandomNumberInRange(0.0f, 5.0f));
        CVector velocity(cos(angle) * 0.5f, sin(angle) * 0.5f, 1.0f + CGeneral::GetRandomNumberInRange(0.0f, 2.0f));
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, position + offset, velocity, nullptr, 5.0f);
    }

    // The mushroom
    for (int i = 0; i < 150; ++i) {
        float angle = CGeneral::GetRandomNumberInRange(0.0f, 2.0f * M_PI);
        float distance = CGeneral::GetRandomNumberInRange(5.0f, 15.0f);
        float height = CGeneral::GetRandomNumberInRange(10.0f, 20.0f);
        CVector offset(cos(angle) * distance, sin(angle) * distance, height);
        CVector velocity(cos(angle) * 0.2f, sin(angle) * 0.2f, 0.5f);
        CParticle::AddParticle(PARTICLE_GUNSMOKE2, position + offset, velocity, nullptr, 7.0f);
    }

    // Smoke
    for (int i = 0; i < 200; ++i) {
        float angle = CGeneral::GetRandomNumberInRange(0.0f, 2.0f * M_PI);
        float distance = CGeneral::GetRandomNumberInRange(10.0f, 20.0f);
        float height = CGeneral::GetRandomNumberInRange(0.0f, 25.0f);
        CVector offset(cos(angle) * distance, sin(angle) * distance, height);
        CVector velocity(cos(angle) * 0.1f, sin(angle) * 0.1f, 0.2f);
        CParticle::AddParticle(PARTICLE_SMOKE, position + offset, velocity, nullptr, 10.0f);
    }
}
// 0x6A3BD0
/*void Auto::DoNitroEffect(float power) {
    if (m_bOffscreen)
        return;

    float dist = DistanceBetweenPoints(GetPosition(), TheCamera.GetPosition());
    if (dist > 256.0f || dist > 64.0f && !((CTimer::m_FrameCounter + m_nModelIndex) & 1))
    {
        return;
    }

    auto mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(m_nModelIndex);
    CVector firstExhaustPos = mi->m_pVehicleStruct->m_avDummyPos[6];
    CVector secondExhaustPos = firstExhaustPos;
    secondExhaustPos.x *= -1.0f;
    CMatrix entityMatrix(*m_matrix);
    CVector dir1, dir2;
    dir1.z = 0.0f;
    dir2.z = 0.0f;
    float fwdSpeed = DotProduct(m_vecMoveSpeed, GetForward()) * 180.0f;
    if (fwdSpeed < 10.0f) {
        CVector steerFwd(-sinf(m_fSteerAngle), cosf(m_fSteerAngle), 0.0f);
        steerFwd = Multiply3x3(*GetMatrix(), steerFwd);
        float r = CGeneral::GetRandomNumberInRange(-0.06f, -0.03f);
        dir1.x = steerFwd.x * r;
        dir1.y = steerFwd.y * r;
    }
    else {
        dir1.x = m_vecMoveSpeed.x;
        dir1.y = m_vecMoveSpeed.y;
    }
    bool bHasDoubleExhaust = m_pHandlingData->m_bDoubleExhaust;

    if (firstExhaustPos == 0.0f) {
        return;
    }

    CVector vecParticleVelocity;
    if (DotProduct(GetForward(), m_vecMoveSpeed) >= 0.05f) {
        vecParticleVelocity = m_vecMoveSpeed * 30.0f;
    }
    else {
        static float randomFactor = CGeneral::GetRandomNumberInRange(-1.8f, -0.9f);
        vecParticleVelocity = randomFactor * GetForward();
    }

    firstExhaustPos = static_cast<Matrix>(entityMatrix).TransformPoint(firstExhaustPos);
    bool bFirstExhaustSubmergedInWater = false;
    bool bSecondExhaustSubmergedInWater = false;
    float pLevel = 0.0f;
    if (m_nPhysicalFlags.bTouchingWater && CWaterLevel::GetWaterLevelNoWaves(firstExhaustPos.x, firstExhaustPos.y, firstExhaustPos.z, &pLevel, NULL, NULL) &&
        pLevel >= firstExhaustPos.z) {
        bFirstExhaustSubmergedInWater = true;
    }

    if (bHasDoubleExhaust) {
        secondExhaustPos = static_cast<Matrix>(entityMatrix).TransformPoint(secondExhaustPos);
        if (m_nPhysicalFlags.bTouchingWater && CWaterLevel::GetWaterLevelNoWaves(secondExhaustPos.x, secondExhaustPos.y, secondExhaustPos.z, &pLevel, NULL, NULL) &&
            pLevel >= secondExhaustPos.z) {
            bSecondExhaustSubmergedInWater = true;
        }
    }

    if (CGeneral::GetRandomNumberInRange(1.0f, 3.0f) * (m_fGasPedal + 1.1f) <= 2.5f)
        return;
    float fMoveSpeed = m_vecMoveSpeed.Magnitude() * 0.5f;
    float size = fabs(power);
    if (size > 0.1f) size = 0.1f;
    log("DoNitroEffect: power %f", size);
    auto parttype = PARTICLE_ENGINE_SMOKE;
   // RwRGBA color(0,0,255);
    RwRGBA color(0, 0, 255);
  //  firstExhaustFxSystem->m_Color.blue = 255;
   // secondExhaustFxSystem->m_Color.blue = 255;
    float particleAlpha = 0.0f;
    if (0.25f - fMoveSpeed >= 0.0f) {
        particleAlpha = 0.25f - fMoveSpeed;
    }
    float fLife = max(0.2f - fMoveSpeed, 0.0f);
    if (!bFirstExhaustSubmergedInWater) {
     //   firstExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
        CParticle::AddParticle(parttype, firstExhaustPos, dir1, nullptr, size, color, -1.0, 0, fLife);
    }
    if (bHasDoubleExhaust) {
        if (!bSecondExhaustSubmergedInWater) {
      //      firstExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
            CParticle::AddParticle(parttype, secondExhaustPos, dir1, nullptr, size, color, -1.0, 0, fLife);
        }
 
    }

    if (m_fGasPedal > 0.5f && m_nCurrentGear < 3) {
        if (GetRandomNumber() % 2) {
            if (!bFirstExhaustSubmergedInWater) {
        //        secondExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
         CParticle::AddParticle(parttype, firstExhaustPos, dir1, nullptr, size, color, -1.0, 0, fLife);
            }

           
        }
        else if (bHasDoubleExhaust) {

            if (!bSecondExhaustSubmergedInWater) {
           //     secondExhaustFxSystem->m_Color.alpha = particleAlpha * 0.5f;
            CParticle::AddParticle(parttype, secondExhaustPos, dir1, nullptr, size, color, -1.0, 0, fLife);
            }

           
        }
    }
}

// 0x6A3E60
void Auto::StopNitroEffect() {
        CParticle::RemovePSystem(PARTICLE_ENGINE_SMOKE);
}*/

void Auto::AddDamagedVehicleParticles()
{
    if (m_nVehicleSubClass == VEHICLE_PLANE)
        return;
    if (this == FindPlayerVehicle(-1, false) && TheCamera.GetLookingForwardFirstPerson())
        return;
    if (m_nPhysicalFlags.bSubmergedInWater || m_fHealth >= 650.0f || m_fHealth < 250.0f)
        return;
    CDamageManager Damage = m_damageManager;

    uint8 engineStatus = Damage.GetEngineStatus();

    CVector vecDAMAGE_ENGINE_POS_SMALL(-0.1f, -0.1f, 0.0f);
    CVector vecDAMAGE_ENGINE_POS_BIG(-0.5f, -0.3f, 0.0f);
    CVector direction = 0.5f * m_vecMoveSpeed;
    CVector damagePos = ((CVehicleModelInfo*)CModelInfo::GetModelInfo(m_nModelIndex))->m_pVehicleStruct->m_avDummyPos[VCIIIDamagedVehicleSmokePos ? 0 : 7];

    switch (Damage.GetDoorStatus(0)) {
    case DAMSTATE_OK:
    case DAMSTATE_OPENED:
        // Bonnet is still there, smoke comes out at the edge
        damagePos += VCIIIDamagedVehicleSmokePos ? vecDAMAGE_ENGINE_POS_SMALL : CVector(0.0f, 0.0f, 0.0f);
        break;
    case DAMSTATE_DAMAGED:
    case DAMSTATE_OPENED_DAMAGED:
        // Bonnet is gone, smoke comes out at the engine
        damagePos += VCIIIDamagedVehicleSmokePos ? vecDAMAGE_ENGINE_POS_BIG : CVector(0.0f, 0.0f, 0.0f);
        break;
    }

    if (m_nModelIndex == MODEL_BFINJECT)
        damagePos = CVector(0.3f, -1.5f, -0.1f);

    damagePos = *GetMatrix() * damagePos;
    damagePos.z += 0.15f;
    if (m_fHealth < 320.0f) {
            CParticle::AddParticle(PARTICLE_ENGINE_SMOKE2, damagePos, 0.2f * direction);
    }
    else if (m_fHealth < 390.0f) {
            CParticle::AddParticle(PARTICLE_ENGINE_STEAM, damagePos, direction);
            CParticle::AddParticle(PARTICLE_ENGINE_SMOKE, damagePos, 0.3f * direction);
    }
    else if (m_fHealth < 650.0f) {
            direction.z += 0.05f;
            CParticle::AddParticle(PARTICLE_ENGINE_STEAM, damagePos, direction, nil, 0.1f);
    }
}

static void __fastcall TriggerWaterHydrant(Fx_c* fx, int, CVector* pos) {
    CParticleObject::AddObject(POBJECT_FIRE_HYDRANT, *pos - CVector(0.0f, 0.0f, 0.5f), true);
}

static void __fastcall MyAddGlass(Fx_c* fx, int, CVector& pos, RwRGBA& color, float scale, int32_t amount) {
        CVector dir;

        RwRGBA colorVC = { 255, 255, 255, 255 };

            static int32 nFrameGen = 0;

            for (int32 i = 0; i < 4; i++)
            {
                dir.x = CGeneral::GetRandomNumberInRange(-0.35f, 0.35f);
                dir.y = CGeneral::GetRandomNumberInRange(-0.35f, 0.35f);
                dir.z = CGeneral::GetRandomNumberInRange(0.05f, 0.20f);

                CParticle::AddParticle(PARTICLE_CAR_DEBRIS,
                    pos,
                    dir,
                    nil,
                    CGeneral::GetRandomNumberInRange(0.02f, 0.2f),
                    colorVC,
                    CGeneral::GetRandomNumberInRange(-40, 40),
                    0,
                    ++nFrameGen & 3,
                    500);
            }
}
// 0x4AAC90
/*void FxSys::DoFxAudio(CVector pos) {
    constexpr struct data { tParticleType hash; eAudioEvents event; } mapping[] = {
        { PARTICLE_CARFLAME,           AE_FIRE               },
        { PARTICLE_WATER_HYDRANT,  AE_FIRE_HYDRANT       },
    };
  //  tParticleSystemData* psystem{};
  //  CParticle* particle{};
   // for (int32_t i = 0; i < 94; i++) {
      auto  psystem = &mod_ParticleSystemManager.m_aParticles[PARTICLE_CARFLAME];
      auto  particle = psystem->m_pParticles;
   // }
    data TheData;
  //  pos = particle->m_vecPosition;
        if (particle && psystem->m_Type == TheData.hash) {
         //   pos = particle->m_vecPosition;
            ((FireAudio*)GetFireAudio())->AddAudioEvent(TheData.event, &pos);
        }
    //}
}*/

void FxSys::DoFxAudio(CVector pos) {
    struct AudioEvent {
        tParticleType ParticleType;
        eAudioEvents eventID;
    };

    const std::array<AudioEvent, 5> audioEvents = { {
        {PARTICLE_CARFLAME, AE_FIRE_CAR},
        {PARTICLE_FLAME, AE_FIRE},
        {PARTICLE_CARFLAME_MOVING, AE_FIRE_CAR},
        {PARTICLE_WATER_HYDRANT, AE_FIRE_HYDRANT},
        {PARTICLE_SPLASH, AE_FIRE_HYDRANT},
    } };

    for (const auto& event : audioEvents) {
        auto psystem = &mod_ParticleSystemManager.m_aParticles[event.ParticleType];
        auto particle = psystem->m_pParticles;
        particle->AddParticle(event.ParticleType, pos, CVector(0.0f, 0.0f, 0.0f));
        if (particle && psystem->m_Type == event.ParticleType) {
            pos = particle->m_vecPosition;
            static_cast<FireAudio*>(GetFireAudio())->AddAudioEvent(event.eventID, &pos);
            break;
        }
    }
}

static void __fastcall MyImpactFx(Fx_c* fx, int, CVector& posn, CVector& direction, int32_t bulletFxType, int32_t amount, float arg4) {
                {
                    for (int32_t i = 0; i < 16; i++) {
                        CParticle::AddParticle(PARTICLE_SPARK, posn, direction * 0.05f);
                    }

                    CVector smokePos = posn;
                    smokePos.x += CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
                    smokePos.y += CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
                    smokePos.z += CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);

                    CParticle::AddParticle(PARTICLE_BULLETHIT_SMOKE, smokePos, CVector(0.0f, 0.0f, 0.0f));
                }
    fx->AddBulletImpact(posn, direction, bulletFxType, amount, arg4);
}

class CParticleVC {
public:
    CParticleVC() {
        Events::initGameEvent += []() {
           CDebug::DebugInitTextBuffer();
#ifdef GTA_SCENE_EDIT
            CSceneEdit::Initialise();
#endif
            Memory::InjectHook(0x71D700, CMBlur::MotionBlurRender, PATCH_JUMP);
            Memory::InjectHook(0x71D710, CMBlur::MotionBlurOpen, PATCH_JUMP);
            Memory::InjectHook(0x71D720, CMBlur::MotionBlurClose, PATCH_JUMP);
            Memory::InjectHook(0x71D730, CMBlur::SetDrunkBlur, PATCH_JUMP);
            Memory::InjectHook(0x71D780, CMBlur::ClearDrunkBlur, PATCH_JUMP);
            CMBlur::MotionBlurOpen(TheCamera.m_pRwCamera);
            };

        Events::initRwEvent += []() {
            CParticle::Initialise();
            log("Injecting patches...");;
          //  patch::RedirectCall(0x7424CB, MyFireSniper);
         //   Memory::InjectHook(0x4AAC90, &FxSys::DoFxAudio, PATCH_JUMP);
            if (BulletImpactParticles) {
                patch::RedirectCall(0x73CD92, MyDoBulletImpact);
                patch::RedirectCall(0x741199, MyDoBulletImpact);
                patch::RedirectCall(0x7411DF, MyDoBulletImpact);
                patch::RedirectCall(0x7412DF, MyDoBulletImpact);
                patch::RedirectCall(0x741E30, MyDoBulletImpact);
                patch::RedirectCall(0x736545, MyImpactFx);
                patch::RedirectCall(0x49F4C1, FxInfo);
                patch::RedirectCall(0x49F0DD, FxInfo);
                patch::RedirectCall(0x49F59A, FxInfo);
            }
            if (MissileSmoke) {
                patch::RedirectCall(0x738C9D, FxInfo);
            }
            if (WheelParticles) {
                patch::RedirectCall(0x4A026B, FxInfo);
                patch::RedirectCall(0x4A04BB, FxInfo);
                patch::RedirectCall(0x4A0ABD, FxInfo);
                patch::RedirectCall(0x4A070D, FxInfo);
                patch::RedirectCall(0x49F374, FxInfo);
                patch::RedirectCall(0x6DF1E5, FxInfo);
                patch::RedirectCall(0x6DEF77, FxInfo);
                patch::RedirectCall(0x6DED41, FxInfo);
            }
            if (PunchImpactParticles) {
                patch::RedirectCall(0x49F6E3, FxInfo);
            }
            if (BloodnGore && (BloodnGore && BulletImpactParticles)) {
                patch::RedirectCall(0x49EB86, FxInfo);
            }
            if (WoodImpactParticles) {
                patch::RedirectCall(0x49EE84, FxInfo);
            }
            if (PlaneSmokeParticles) {
                patch::RedirectCall(0x6C93FB, FxInfo);
                patch::RedirectCall(0x6CBF1B, FxInfo);
                patch::RedirectCall(0x6CA97D, FxInfo);
                patch::RedirectCall(0x6CAA09, FxInfo);
            }
            // Fire extinguish particles
            if (FiråExtinguishParticles) {
                patch::RedirectCall(0x539612, FxInfo);
            }
            // Water cannon
            if (WaterCannînParticles) {
                patch::RedirectCall(0x729354, FxInfo);
                patch::RedirectCall(0x729A5A, FxInfo);
            }
            // Some water particles
            if (WaterParticles) {
                cBuoyancy::InjectHooks();
                patch::RedirectCall(0x6C3630, FxInfo);
                patch::RedirectCall(0x6C39BF, FxInfo);
                patch::RedirectCall(0x5E7543, FxInfo);
                //patch::RedirectCall(0x5E7415, FxInfo);
                patch::RedirectCall(0x68AD9F, FxInfo);
            }
            if (FootDustParticles) {
                patch::RedirectCall(0x5E3835, FxInfo);
            }
            if (FootSplashesParticles) {
                patch::RedirectCall(0x5E36A5, FxInfo);
            }
            if (MuzzleFlashnSmoke) {
                Memory::InjectHook(0x48ED94, &MyTriggerGunflash, PATCH_CALL);
                Memory::InjectHook(0x740DA7, &MyTriggerGunflash, PATCH_CALL);
                patch::RedirectCall(0x628328, MyWeaponFire);
              //  patch::RedirectCall(0x61ECCD, MyWeaponFire);
             //   patch::RedirectJump(0x61ECA7, MyWeaponFire);
          //    Memory::InjectHook(0x61ECCD, &MyWeaponFire, PATCH_CALL);
                //AE_FIRE_HYDRANT;
             //   patch::RedirectCall(0x5678F2, TriggerWaterHydrant);
            //    patch::RedirectCall(0x567902, TriggerWaterHydrant);
             //   Memory::InjectHook(0x742299, &DoDriveByGunflash, PATCH_CALL);
                //patch::RedirectJump(0x4A0DE0, MyTriggerGunflash);
                patch::Nop(0x61ECE6, 0x61ECFA - 0x61ECE6); // CPed::DoGunFlash [CTaskSimpleUseGun::FireGun]
                //   injector::MakeNOP(0x4A0F38, 5, true);
            }
            // Memory::InjectHook(0x716C90, &Clouds::MovingFogRender, PATCH_JUMP);
          //   Memory::InjectHook(0x6DD130, &Veh::DoBoatSplashes, PATCH_JUMP);
             // Memory::InjectHook(0x4A0D70, TriggerWaterHydrant);
              //AddParticles();
              //patch::RedirectCall(0x742299, DoDriveByGunflash);
              //patch::RedirectJump(0x4A0DE0, DoGunFlash);
              // Disable some of the SA particles, to replace them with VC ones
              //memset((void*)0x6AB344, 0x90, 5); // Exhaust particles (Automobile)
              //if (FlamethrowerFlame) {
              //	memset((void*)0x73E6AF, 0x90, 4); // Flamethrower flame
              //}
              /*//memset((void*)0x737239, 0x90, 5); // Tiny explosion
              memset((void*)0x736F5E, 0x90, 5); // Medium Explosion
              memset((void*)0x737234, 0x90, 2); // Tiny explosion
              memset((void*)0x736CE0, 0x90, 5); // Small explosion
              memset((void*)0x73719C, 0x90, 5); // Small explosion
              memset((void*)0x737115, 0x90, 5); // Large explosion
              memset((void*)0x736FCD, 0x90, 5); // Large explosion*/
              //memset((void*)0x6BD3FF, 0x90, 5); // Exhaust particles (Bike)
            if (DamagedEngineSmoke) {
           //     memset((void*)0x6AB34B, 0x90, 5); // Damaged engine smoke (Car)
            //    memset((void*)0x6BD40A, 0x90, 5); // Damaged engine smoke (Bike)
                Memory::InjectHook(0x6D2A80, &Auto::AddDamagedVehicleParticles, PATCH_JUMP);
            }
            // if (FireWhenAboutToExplode) {
                 //Memory::InjectHook(0x6A7090, &Auto::ProcessCarOnFireAndExplode, PATCH_JUMP);
             //    memset((void*)0x6A7542, 0x90, 6); // Fire when car is about to explode
                 //memset((void*)0x6F1E41, 0x90, 5); // Fire when boat is about to explode
           //  }
          //   Memory::Patch<float>(0x4A06F5, 0.0f);
         //    Memory::Nop(0x49F498, 5);
           //  injector::MakeNOP(0x73BF24, 5);
            // memset((void*)0x73BF24, 0x90, 5);
           //  injector::MakeNOP(0x73C04B, 5);
             //memset((void*)0x6DB6C1, 0x90, 5); // Sparks
             //injector::MakeNOP(0x6BBCB0, 6); // Fire when bike is about to explode
             //memset((void*)0x61ECF5, 0x90, 6); // Fire when bike is about to explode
             //patch::Nop(0x6BBCA9, 6);
             //memset((void*)0x6B3B93, 0x90, 5); // Car explosion 1
             //memset((void*)0x6B3B67, 0x90, 2); // Car explosion 2
             // memset((void*)0x4A0F38, 0x90, 5); // Gunflash
             // memset((void*)0x4A0F67, 0x90, 5); // Gunsmoke
             //Memory::Nop(0x61ECF5, 2); // Remove default gunflashes
            //  memset((void*)0x4A0F38, 0x90, 5);
             // Replace gunshells
            if (Gunshells) {
                Memory::InjectHook(0x73A3E0, &CWeap::AddGunshell, PATCH_JUMP);
            }
            // Replace sandstorm particles
            if (SandStorm) {
                Memory::InjectHook(0x72A820, &AddSandStormParticles, PATCH_JUMP);
            }
            // Replace rain
            if (GTAIIIVCRainEnable) {
                Memory::InjectHook(0x72A9A0, &AddRain, PATCH_JUMP);
                Memory::InjectHook(0x72AF70, &RenderRainStreaks, PATCH_JUMP);
            }
            // Replace damage particles
            if (dmgParticles) {
                Memory::InjectHook(0x6A6DC0, &Auto::dmgDrawCarCollidingParticles, PATCH_JUMP);
            }
            // Replace exhaust smoke
            if (ExhaustSmoke) {
                Memory::InjectHook(0x6DE240, &Veh::AddExhaustParticles, PATCH_JUMP);
            }
            // Replace friction sparks (I know it's bad to replace the whole function, just for one minor thing, sorry) NO LONGER NEEDED SINCE SPARKS ARE CREATED EVERYWHERE THEY CALLED
           // if (FrictionParticles) {
            //    Memory::InjectHook(0x5454C0, &Physical::ApplyFriction, PATCH_JUMP);
           // }
            // Replace dirt/water and other wheel particles
            // Memory::InjectHook(0x6D2D50, &Auto::AddWheelDirtAndWater, PATCH_JUMP);
            // Replace rain splash particles
            if (RainOnRoofParticles) {
                Memory::InjectHook(0x6DDF60, &Veh::AddWaterSplashParticles, PATCH_JUMP);
            }
            // Weapon effects such as: Flamethrower flame, extinguisher smoke, spray can
            if (WeaponEffects) {
                Memory::InjectHook(0x73E690, &CWeap::DoWeaponEffect, PATCH_JUMP);
            }
            // Heli dust effect
            if (HeliDust) {
                Memory::InjectHook(0x6B0690, &Heli::DoHeliDustEffect, PATCH_JUMP);
            }
            // Teargas smoke
            // Memory::InjectHook(0x738B20, &ProjectileInfo::Update, PATCH_JUMP);
            //Memory::InjectHook(0x61EB10, &FireGun, PATCH_JUMP);
            //Memory::Patch(0x5DF3E6, 0);
            // Harvester particles, when you run people over
            if (HarvesterParticles && (HarvesterParticles && BloodnGore)) {
                Memory::InjectHook(0x6A9680, &Auto::ProcessHarvester, PATCH_JUMP);
            }
            // Object burn effect
            if (ObjectBurnParticles) {
                Memory::InjectHook(0x59FB50, &Obj::DoBurnEffect, PATCH_JUMP);
            }
            //CWaterCannon::InjectHooks();
            //Memory::InjectHook(0x49EB00, &AddBlood, PATCH_CALL);
            // Foot particles
            //Memory::InjectHook(0x5E3630, &particleProduceFootSplash, PATCH_JUMP);
            //Memory::InjectHook(0x5E37C0, &particleProduceFootDust, PATCH_JUMP);
            //Memory::InjectHook(0x4A1150, &particleProduceFootSplash2, PATCH_JUMP);
            //Memory::InjectHook(0x4A1070, &TriggerWaterSplash, PATCH_JUMP);
            //Memory::InjectHook(0x4A1150, &TriggerFootSplash, PATCH_JUMP);
            //Memory::InjectHook(0x4A10E0, &TriggerBulletSplash, PATCH_JUMP);
            //Memory::InjectHook(0x73AAC0, &CWeap::FireSniper, PATCH_JUMP);
            //Memory::InjectHook(0x73B300, &CWeap::ProcessLineOfSight, PATCH_JUMP);
            // Fire particles
            //Memory::InjectHook(0x539360, &Firev::CreateFxSysForStrength, PATCH_JUMP);
            if (IsPluginInstalled("IMFX.asi")) {
                log("IMFX = true\n");
                IMFX = true;
            }
            else
            {
                log("IMFX = false\n");
                IMFX = false;
            }
            if (!IMFX && MuzzleFlashnSmoke) {
                patch::Nop(0x73306D, 9); // Remove default gunflashes
                patch::Nop(0x7330FF, 9); // Remove default gunflashes
                //    patch::Nop(0x740DA7, 5); // Remove default gunflashes
                patch::SetUShort(0x5DF425, 0xE990); // Remove default gunflashes
            }
            /*if (NitroParticles) {
                Memory::InjectHook(0x6A3BD0, &Auto::DoNitroEffect, PATCH_JUMP);
                Memory::InjectHook(0x6A3E60, &Auto::StopNitroEffect, PATCH_JUMP);
            }*/
            if (ShatteredGlassParticles) {
               // CFallingGlassPane::InjectHooks();
                patch::RedirectCall(0x49F9C7, FxInfo);
                patch::RedirectCall(0x71ABEB, MyAddGlass);
                patch::RedirectCall(0x71AC24, MyAddGlass);
                patch::RedirectCall(0x71AC5D, MyAddGlass);
                patch::RedirectCall(0x71AC96, MyAddGlass);
            }
            log("Finished injecting patches!");
            //  plugin::patch::RedirectCall(0x6A71C8, HK_CreateFxSystem);
         //     wuzipatch1();
            };

        // CameraSize += [](RwCamera* camera, RwRect* rect, float viewWindow, float aspectRatio) {
           //  CMBlur::MotionBlurOpen(camera);
        // };

        Events::reInitGameEvent += []() {
            CParticle::ReloadConfig();
            CParticle::ReloadTXD();
            CParticleObject::RemoveAllExpireableParticleObjects();
            // CPlane2::InitPlanes();
            };

        Events::gameProcessEvent += []() {
            CParticle::Update();
            //  UpdateCheckHandler();
            //  CPlane2::UpdatePlanes();          
#ifdef GTA_SCENE_EDIT
            CSceneEdit::Update();
            if (CSceneEdit::m_bEditOn) {
                TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode = MODE_EDITOR;
            }
            if (TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode == MODE_EDITOR) {
                ((Cam)TheCamera.m_aCams[TheCamera.m_nActiveCam]).Process_Editor(TheCamera.m_aCams[TheCamera.m_nActiveCam].m_pCamTargetEntity->GetPosition(), CGeneral::GetATanOfXY(TheCamera.m_aCams[TheCamera.m_nActiveCam].m_pCamTargetEntity->GetForward().x, TheCamera.m_aCams[TheCamera.m_nActiveCam].m_pCamTargetEntity->GetForward().y), 0.0f, 0.0f);
            }
#endif
            if (ParticlesReview) {
                CVector vecDir;
                //CParticle::AddYardieDoorSmoke(FindPlayerCoors(-1), *FindPlayerPed()->GetMatrix());
                // If player have a vehicle right now, use vehicles velocity, otherwise, use players
                if (FindPlayerVehicle(-1, false)) {
                    vecDir = 0.85f * FindPlayerVehicle(-1, false)->m_vecMoveSpeed;
                }
                else {
                    vecDir = FindPlayerSpeed(-1);
                }
                CParticle::AddParticle((tParticleType)particles,
                    FindPlayerCoors(-1),
                    //CVector(2486.120605f, -1670.179932f, 13.335947f),
                    vecDir);
            }
            if (ParticleObjectsReview) {
                CParticleObject::AddObject(particleobjects, FindPlayerCoors(-1), true);
            }
            // Detect changes in variables
            if (nParticleCreationInterval != nParticleCreationIntervalnew) {
                log("Value of nParticleCreationInterval was changed. New value is: %d", nParticleCreationInterval);
                debug("Value of nParticleCreationInterval was changed. New value is: %d", nParticleCreationInterval);
                nParticleCreationIntervalnew = nParticleCreationInterval;
            }
            if (fParticleScaleLimit != fParticleScaleLimitnew) {
                log("Value of fParticleScaleLimit was changed. New value is: %4.0f", fParticleScaleLimit);
                debug("Value of fParticleScaleLimit was changed. New value is: %4.0f", fParticleScaleLimit);
                fParticleScaleLimitnew = fParticleScaleLimit;
            }
            if (PARTICLE_WIND_TEST_SCALE != PARTICLE_WIND_TEST_SCALE_NEW) {
                log("Value of PARTICLE_WIND_TEST_SCALE was changed. New value is: %4.0f", PARTICLE_WIND_TEST_SCALE);
                debug("Value of PARTICLE_WIND_TEST_SCALE was changed. New value is: %4.0f", PARTICLE_WIND_TEST_SCALE);
                PARTICLE_WIND_TEST_SCALE_NEW = PARTICLE_WIND_TEST_SCALE;
            }

            if (Snow) {
                AddSnow();
            }
            for (CProjectileInfo* info = gaProjectileInfo; info < &gaProjectileInfo[MAX_PROJECTILE_INFOS]; info++) {
                float tearGasOffset = -0.0f;
                if (info->m_nWeaponType == WEAPON_TEARGAS && TearGasSmoke) {
                    CParticle::AddParticle(PARTICLE_TEARGAS, info->m_vecLastPosn, CVector(0.2f, tearGasOffset, 0.0f), 0, 0.0f, 0, 0, 0, 0);
                    CParticle::AddParticle(PARTICLE_TEARGAS, info->m_vecLastPosn, CVector(-0.2f, tearGasOffset, 0.0f), 0, 0.0f, 0, 0, 0, 0);
                    CParticle::AddParticle(PARTICLE_TEARGAS, info->m_vecLastPosn, CVector(tearGasOffset, tearGasOffset, 0.0f), 0, 0.0f, 0, 0, 0, 0);
                }
            }
            if (ShipsOnHorizon) {
                CParticle::HandleShipsAtHorizonStuff();
                RenderShipsOnHorizon();
            }
            if (SeaBirds) {
                RenderSeaBirds();
            }
            if (WaterSparks) {
                RenderWaterSparks();
                HandleWaterSparks();
            }
            //AddHeatHaze(); // No
          //  if (GTAIIIVCRainEnable) {
            //    AddRain();
            //    RenderRainStreaks();
           // }

            /*if ((CWeather::NewWeatherType == WEATHER_SUNNY_LA
                || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_LA
                || CWeather::NewWeatherType == WEATHER_SUNNY_SF
                || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_SF
                || CWeather::NewWeatherType == WEATHER_SUNNY_VEGAS
                || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_VEGAS
                || CWeather::NewWeatherType == WEATHER_SUNNY_COUNTRYSIDE
                || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_COUNTRYSIDE
                || CWeather::NewWeatherType == WEATHER_SUNNY_DESERT
                || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_DESERT) &&
                !CGame::currArea > 0 && !CCutsceneMgr::ms_running && (CTimer::m_FrameCounter & 7) == 0) {*/
                /*#ifdef FIX_BUGS
                                        if (FindPlayerPed() && (!FindPlayerPed()->m_nPedFlags.bIsInTheAir || (FindPlayerPed()->m_nPedFlags.bIsInTheAir && FindPlayerPed()->GetPosition().z < 7.5f &&
                                            CClock::ms_nGameClockHours > 6 && CClock::ms_nGameClockHours < 18)))
                #else
                                        if (!FindPlayerPed()->CheckIfInTheAir() || (FindPlayerPed()->CheckIfInTheAir() && FindPlayerPed()->GetPosition().z < 7.5f &&
                                            CClock::GetHours() > 6 && CClock::GetHours() < 18))
                #endif
                                            AddHeatHaze();
                                    }*/
            if (SandStorm) {
                if (CWeather::Sandstorm > 0.3f) {
                    AddSandStormParticles();
                }
            }
            if (Beasties) {
                if (CWeather::NewWeatherType == WEATHER_SUNNY_LA
                    || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_LA
                    || CWeather::NewWeatherType == WEATHER_SUNNY_SF
                    || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_SF
                    || CWeather::NewWeatherType == WEATHER_SUNNY_VEGAS
                    || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_VEGAS
                    || CWeather::NewWeatherType == WEATHER_SUNNY_COUNTRYSIDE
                    || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_COUNTRYSIDE
                    || CWeather::NewWeatherType == WEATHER_SUNNY_DESERT
                    || CWeather::NewWeatherType == WEATHER_EXTRASUNNY_DESERT
                    || CWeather::OldWeatherType == WEATHER_SUNNY_LA
                    || CWeather::OldWeatherType == WEATHER_EXTRASUNNY_LA
                    || CWeather::OldWeatherType == WEATHER_SUNNY_SF
                    || CWeather::OldWeatherType == WEATHER_EXTRASUNNY_SF
                    || CWeather::OldWeatherType == WEATHER_SUNNY_VEGAS
                    || CWeather::OldWeatherType == WEATHER_EXTRASUNNY_VEGAS
                    || CWeather::OldWeatherType == WEATHER_SUNNY_COUNTRYSIDE
                    || CWeather::OldWeatherType == WEATHER_EXTRASUNNY_COUNTRYSIDE
                    || CWeather::OldWeatherType == WEATHER_SUNNY_DESERT
                    || CWeather::OldWeatherType == WEATHER_EXTRASUNNY_DESERT && !CGame::currArea > 0 && !CCutsceneMgr::ms_running)
                {
                    AddBeastie();
                }
            }

            static int time = 0;
            if (TestExplosions && CTimer::m_snTimeInMilliseconds - time > 2000) {
                time = CTimer::m_snTimeInMilliseconds;
                // CVector pos;
                // pos.x = 2486.120605f;
                // pos.y = -1670.179932f;
                // pos.z = 13.335947f;
                 //CreateNuclearExplosion(pos);
                CExplosionVC::AddExplosion(nullptr, FindPlayerPed(), (eExplosionTypeVC)explosiontype, CVector(2486.120605f, -1670.179932f, 13.335947f), 5, 1, 1.5f, 0);
            }
            RwRGBA dropColor = { 0, 0, 0, 0 };
            if (TheCamera.GetLookingForwardFirstPerson() && FindPlayerVehicle(-1, false) && FindPlayerVehicle(-1, false)->m_nVehicleSubClass == VEHICLE_BOAT) {
                CVector dropPos;
                CVector dropDir(CGeneral::GetRandomNumberInRange(-0.25f, 0.25f), CGeneral::GetRandomNumberInRange(1.0f, 0.75f), 0.0f);
                int frm = GetRandomNumber() & 1;
                if (TheCamera.m_fCameraAverageSpeed < 0.35f) {
                    dropPos.x = CGeneral::GetRandomNumberInRange(50, (int)SCREEN_WIDTH - 50);
                    dropPos.y = CGeneral::GetRandomNumberInRange(50, (int)SCREEN_HEIGHT - 50);
                }
                else {
                    dropPos.x = CGeneral::GetRandomNumberInRange(200, (int)SCREEN_WIDTH - 200);
                    dropPos.y = CGeneral::GetRandomNumberInRange(150, (int)SCREEN_HEIGHT - 150);
                }
                dropPos.z = 1.0f;

                if (TheCamera.m_fCameraAverageSpeed > 0.35f) {
                    if ((int)SCREEN_WIDTH / 2 < dropPos.x)
                        dropPos.x += CGeneral::GetRandomNumberInRange(0.35f, TheCamera.m_fCameraAverageSpeed) * 7.5f;
                    else
                        dropPos.x -= CGeneral::GetRandomNumberInRange(0.35f, TheCamera.m_fCameraAverageSpeed) * 7.5f;

                    if ((int)SCREEN_HEIGHT / 2 < dropPos.y)
                        dropPos.y += CGeneral::GetRandomNumberInRange(0.35f, TheCamera.m_fCameraAverageSpeed) * 7.5f;
                    else
                        dropPos.y -= CGeneral::GetRandomNumberInRange(0.35f, TheCamera.m_fCameraAverageSpeed) * 7.5f;
                }
                if (CParticle::AddParticle(PARTICLE_WATERDROP, dropPos, dropDir, nullptr,
                    CGeneral::GetRandomNumberInRange(0.1f, 0.15f), dropColor, 0, 0, frm))
                    numWaterDropOnScreen++;
            }

            };
           /*Events::pedRenderEvent += [](CPed* ped) {
               auto weaptype = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType;
               auto info = GetInfo(ped);
               CVector firePos(0.0f, 0.0f, 0.6f);
               firePos = *ped->GetMatrix() * firePos;
               CEntity* nearStatic = (CObject*)CWorld::TestSphereAgainstWorld(firePos, info->m_fRadius, nil, true, false, false, true, false, false);
               if (nearStatic && weaptype == WEAPON_CHAINSAW)
               {
                   debug("Near static");
                   for (int i = 0; i < 4; i++) {
                       CParticle::AddParticle(PARTICLE_SPARK_SMALL, gaTempSphereColPoints[0].m_vecPoint, CVector(0.0f, 0.0f, 0.3f), 0, 0.0f, 0, 0, 0, 0);
                       CParticle::AddParticle(PARTICLE_SPARK, gaTempSphereColPoints[0].m_vecPoint, 0.1f * gaTempSphereColPoints[0].m_vecNormal, 0, 0.0f, 0, 0, 0, 0);
                   }
               }
               int sound{};
               Command<0x0AC1>(plugin::paths::GetPluginDirRelativePathA((char*)"fireVC.wav"), &sound);

               Command<0x0AC2>(sound, ped->GetPosition().x, ped->GetPosition().y, ped->GetPosition().z);
               Command<0x0AAD>(sound, 1);
                };*/
            // BASS_3DVECTOR pos = BASS_3DVECTOR(ped->GetPosition().x, ped->GetPosition().y, ped->GetPosition().z);
            // BASS_Set3DPosition(&pos, &BASS_3DVECTOR(0.0f, 0.0f, 0.0f), &BASS_3DVECTOR(0.0f, 0.0f, 0.0f), &BASS_3DVECTOR(0.0f, 0.0f, 0.0f));
            // BASS_Apply3D();
            // if (!ped->IsPlayer() && !trackIsPlaying)
            // playTrackBASS(PLUGIN_PATH((char*)"fireVC.wav"));
            
            // SetSoundPosition(ped->GetPosition().x, ped->GetPosition().y, ped->GetPosition().z);
       //  };
         /* Events::pedRenderEvent += [](CPed* ped) {
              auto weaponState = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_nState;
              auto weaponType = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType;
              auto weaponInfo2 = CWeaponInfo::GetWeaponInfo(weaponType, ped->GetWeaponSkill());
              auto usingGun = ped->m_pIntelligence->GetTaskUseGun();



              if (ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType == WEAPON_FTHROWER) {
                  auto weaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_FTHROWER, eWeaponSkill::WEAPSKILL_STD);
                  if (ped->m_pIntelligence->GetTaskUseGun()) {
                      if (weaponState == WEAPONSTATE_FIRING) {
                          CVector firePos(-0.25f, 1.15f, -0.15f);
                          firePos = *ped->GetMatrix() * firePos;
                          FireAreaEffect(ped, &firePos);
                      }
                  }
              }




              CVector fireSource(0.15f, 1.15f, 0.35f);
              fireSource = *ped->GetMatrix() * fireSource;

              float heading = RADTODEG(ped->GetForward().Heading());
              float angle = DEGTORAD(heading);
                  CVector target;
                  CVector dir;
                  float heading = RADTODEG(ped->GetForward().Heading());
                  float angle = DEGTORAD(heading);
                  CVector fireSource(0.15f, 1.15f, 0.35f);
                  fireSource = *ped->GetMatrix() * fireSource;
                  dir = CVector(-sinf(angle) * 0.5f, cosf(angle) * 0.5f, 0.0f);
                  target = fireSource + dir;
                  CSphere* sphere = &ped->GetColModel()->m_boundSphere;
                  CWeaponInfo* info = GetInfo(ped);
                  CVector collisionDist = ped->GetPosition() + sphere->m_vecCenter - fireSource;
                  CVector bloodPos = fireSource + (collisionDist * 0.7f);
                  /*CVector newDir(dir);
                  newDir.z += 0.2f;
                  CParticle::AddParticle(PARTICLE_BLOOD_SMALL, bloodPos, newDir);
                  CParticle::AddParticle(PARTICLE_BLOOD, bloodPos, newDir);
                  newDir.z = dir.z + 0.1f;
                  CParticle::AddParticle(PARTICLE_BLOOD, bloodPos, newDir);
                  newDir.x = 0.0f;
                  newDir.y = 0.0f;
                  newDir.z = 0.01f;
                  CParticle::AddParticle(PARTICLE_DEBRIS2, bloodPos, newDir);
                  CVector dropDir(CGeneral::GetRandomNumberInRange(-0.15f, 0.15f), CGeneral::GetRandomNumberInRange(0.1f, 0.35f), 0.f);
                  CVector dropPos(CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_X(50.0f), SCREEN_STRETCH_FROM_RIGHT(50.0f)),
                      CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_Y(50.0f), SCREEN_STRETCH_FROM_BOTTOM(50.0f)), 1.f);
                  CParticle::AddParticle(PARTICLE_BLOODDROP, dropPos, dropDir, nullptr, CGeneral::GetRandomNumberInRange(0.1f, 0.15f),
                      RwRGBA(0, 0, 0, 0), 0, 0, GetRandomNumber() & 1, 0);
                  (fireSource).x += -sinf(angle) * 0.15f;
                  (fireSource).y += cosf(angle) * 0.15f;
              auto cam = TheCamera.m_aCams[TheCamera.m_nActiveCam];
              if (FindPlayerPed() && ped && ped->m_pWeaponObject && ped->m_pWeaponObject->parent &&
                  ((RwFrame*)ped->m_pWeaponObject->parent)->child) {
                  CVector muzzleflashPos = RwFrameGetLTM(((RwFrame*)ped->m_pWeaponObject->parent)->child)->pos;
                  if (ped->m_pIntelligence->GetTaskUseGun()) {
                      if (ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_nState == WEAPONSTATE_FIRING) {
                          //DoGunFlash(ped, &muzzleflashPos);
                          // Handling shootable birbs
                          auto source = TheCamera.m_aCams[TheCamera.m_nActiveCam];
                          tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[PARTICLE_BIRD_FRONT];
                          CParticle* particle = psystem->m_pParticles;
                          CColPoint* cp{};
                          CEntity* ent{};
                          const auto camOriginPos = 0.5f * source.m_vecSource;
                          const auto camTargetPos = camOriginPos + source.m_vecFront * 3.f;
                          if (SeaBirds) { // Are they enabled?
                              //HandleGunShot(&particle->m_vecPosition, &particle->m_vecPosition);
                              //CWeap::ProcessLineOfSight(camOriginPos, camTargetPos, *cp, ent, weaponType, ent, true, true, true, true, true, true, true);
                              //CParticle::HandleShootableBirdsStuff(ped, &start, &particle->m_vecPosition);
                          }
                      }
                  }
              }
          };*/
        if (DebugMenuLoad()) {
            static const char* particleTypes[] = {
            "PARTICLE_SPARK",
            "PARTICLE_SPARK_SMALL",
            "PARTICLE_WATER_SPARK",
            "PARTICLE_WHEEL_DIRT",
            "PARTICLE_SAND",
            "PARTICLE_WHEEL_WATER",
            "PARTICLE_BLOOD",
            "PARTICLE_BLOOD_SMALL",
            "PARTICLE_BLOOD_SPURT",
            "PARTICLE_DEBRIS",
            "PARTICLE_DEBRIS2",
            "PARTICLE_FLYERS",
            "PARTICLE_WATER",
            "PARTICLE_FLAME",
            "PARTICLE_FIREBALL",
            "PARTICLE_GUNFLASH",
            "PARTICLE_GUNFLASH_NOANIM",
            "PARTICLE_GUNSMOKE",
            "PARTICLE_GUNSMOKE2",
            "PARTICLE_CIGARETTE_SMOKE",
            "PARTICLE_SMOKE",
            "PARTICLE_SMOKE_SLOWMOTION",
            "PARTICLE_DRY_ICE",
            "PARTICLE_TEARGAS",
            "PARTICLE_GARAGEPAINT_SPRAY",
            "PARTICLE_SHARD",
            "PARTICLE_SPLASH",
            "PARTICLE_CARFLAME",
            "PARTICLE_STEAM",
            "PARTICLE_STEAM2",
            "PARTICLE_STEAM_NY",
            "PARTICLE_STEAM_NY_SLOWMOTION",
            "PARTICLE_GROUND_STEAM",
            "PARTICLE_ENGINE_STEAM",
            "PARTICLE_RAINDROP",
            "PARTICLE_RAINDROP_SMALL",
            "PARTICLE_RAIN_SPLASH",
            "PARTICLE_RAIN_SPLASH_BIGGROW",
            "PARTICLE_RAIN_SPLASHUP",
            "PARTICLE_WATERSPRAY",
            "PARTICLE_WATERDROP",
            "PARTICLE_BLOODDROP",
            "PARTICLE_EXPLOSION_MEDIUM",
            "PARTICLE_EXPLOSION_LARGE",
            "PARTICLE_EXPLOSION_MFAST",
            "PARTICLE_EXPLOSION_LFAST",
            "PARTICLE_CAR_SPLASH",
            "PARTICLE_BOAT_SPLASH",
            "PARTICLE_BOAT_THRUSTJET",
            "PARTICLE_WATER_HYDRANT",
            "PARTICLE_WATER_CANNON",
            "PARTICLE_EXTINGUISH_STEAM",
            "PARTICLE_PED_SPLASH",
            "PARTICLE_PEDFOOT_DUST",
            "PARTICLE_CAR_DUST",
            "PARTICLE_HELI_DUST",
            "PARTICLE_HELI_ATTACK",
            "PARTICLE_ENGINE_SMOKE",
            "PARTICLE_ENGINE_SMOKE2",
            "PARTICLE_CARFLAME_SMOKE",
            "PARTICLE_FIREBALL_SMOKE",
            "PARTICLE_PAINT_SMOKE",
            "PARTICLE_TREE_LEAVES",
            "PARTICLE_CARCOLLISION_DUST",
            "PARTICLE_CAR_DEBRIS",
            "PARTICLE_BIRD_DEBRIS",
            "PARTICLE_HELI_DEBRIS",
            "PARTICLE_EXHAUST_FUMES",
            "PARTICLE_RUBBER_SMOKE",
            "PARTICLE_BURNINGRUBBER_SMOKE",
            "PARTICLE_BULLETHIT_SMOKE",
            "PARTICLE_GUNSHELL_FIRST",
            "PARTICLE_GUNSHELL",
            "PARTICLE_GUNSHELL_BUMP1",
            "PARTICLE_GUNSHELL_BUMP2",
            "PARTICLE_ROCKET_SMOKE",
            "PARTICLE_TEST",
            "PARTICLE_BIRD_FRONT",
            "PARTICLE_SHIP_SIDE",
            "PARTICLE_BEASTIE",
            "PARTICLE_RAINDROP_2D",
            "PARTICLE_HEATHAZE",
            "PARTICLE_HEATHAZE_IN_DIST",
            "PARTICLE_SNOW",
            "PARTICLE_BOAT_WAKE",
            "PARTICLE_FERRY_CHIM_SMOKE",
            "PARTICLE_MULTIPLAYER_HIT",
            "PARTICLE_HYDRANT_STEAM",
            "PARTICLE_GUNFLASH_VCS",
            "PARTICLE_GUNFLASH_NOANIM_VCS",
            "PARTICLE_EXPLOSION_MASSIVE",
            "PARTICLE_EXPLOSION_PARTICLEEX",
            "PARTICLE_CARFLAME_MOVING",
            "PARTICLE_FIREBALL_PARTICLEEX"
            };
            static const char* particleObjectTypes[] = {
            "POBJECT_PAVEMENT_STEAM",
            "POBJECT_PAVEMENT_STEAM_SLOWMOTION",
            "POBJECT_WALL_STEAM",
            "POBJECT_WALL_STEAM_SLOWMOTION",
            "POBJECT_DARK_SMOKE",
            "POBJECT_FIRE_HYDRANT",
            "POBJECT_CAR_WATER_SPLASH",
            "POBJECT_PED_WATER_SPLASH",
            "POBJECT_SPLASHES_AROUND",
            "POBJECT_SMALL_FIRE",
            "POBJECT_BIG_FIRE",
            "POBJECT_DRY_ICE",
            "POBJECT_DRY_ICE_SLOWMOTION",
            "POBJECT_WATER_FOUNTAIN_VERT",
            "POBJECT_WATER_FOUNTAIN_HORIZ",
            "POBJECT_FIRE_TRAIL",
            "POBJECT_SMOKE_TRAIL",
            "POBJECT_FIREBALL_AND_SMOKE",
            "POBJECT_ROCKET_TRAIL",
            "POBJECT_EXPLOSION_ONCE",
            "POBJECT_CATALINAS_GUNFLASH",
            "POBJECT_CATALINAS_SHOTGUNFLASH"
            };
            DebugMenuAddVar("Particles", "Particles scale limit", &fParticleScaleLimit, nullptr, 0.1f, 0.0f, 50.0f);
            DebugMenuAddVar("Particles", "Drunkness", &CMBlur::Drunkness, nullptr, 0.1f, 0.0f, 10.0f);
         //   DebugMenuAddVar("Particles", "Second Gunflash OffsetX", &OffsetX[weapType], nullptr, 0.1f, 0.0f, 50.0f);
         //   DebugMenuAddVar("Particles", "Second Gunflash OffsetY", &OffsetY[weapType], nullptr, 0.1f, 0.0f, 50.0f);
        //    DebugMenuAddVar("Particles", "Second Gunflash OffsetZ", &OffsetZ[weapType], nullptr, 0.1f, 0.0f, 50.0f);
            DebugMenuAddInt32("Particles", "Particles testing", &particles, nullptr, 1, PARTICLE_FIRST - 0, MAX_PARTICLES - 1, particleTypes);
            DebugMenuAddInt32("Particles", "Particle objects testing", &particleobjects, nullptr, 1, 0, 21, particleObjectTypes);
            DebugMenuAddInt32("Particles", "Explosions testing", &explosiontype, nullptr, 1, 0, 18, nullptr);
            DebugMenuAddInt32("Particles", "Particles creation interval", &nParticleCreationInterval, nullptr, 1, 0, 20, nullptr);
            DebugMenuAddCmd("Particles", "Reload particle.cfg & .ini", CParticle::ReloadConfig);
            DebugMenuAddCmd("Particles", "Reload particleVC.txd", CParticle::ReloadTXD);
            //  DebugMenuAddCmd("Particles", "Check for updates", CheckUpdates);
            DebugMenuAddCmd("Particles", "Remove all particle objects", CParticleObject::RemoveAllParticleObjects);
#ifdef GTA_SCENE_EDIT
            DebugMenuAddVarBool8("Debug", "Scene Edit on", (int8_t*)&CSceneEdit::m_bEditOn, NULL);
#endif
            //	DebugMenuAddInt32("Particles", "Rainbow", (int32_t*)&rainbow, nullptr, 0.1, 0.0, 1.0, nullptr);
                //DebugMenuAddVarBool8("Particles", "Toggle trails", (int8_t*)&CMBlur::BlurOn, nullptr);
            DebugMenuAddVarBool8("Particles", "Toggle particles logging", (int8_t*)&Logging, nullptr);
            // DebugMenuAddVar("Particles", "Drunkness", &CMBlur::Drunkness, nullptr, 0.1f, 0.0f, 1.0f);
           //  DebugMenuAddVarBool8("Particles", "Toggle trails", (int8_t*)&CMBlur::BlurOn, nullptr);
            DebugMenuAddVarBool8("Particles", "Show particles debug stuff", (int8_t*)&gbDebugStuffInRelease, nullptr);
            DebugMenuAddVarBool8("Particles", "Explosions on Grove St. to test (switch with Explosions testing)", (int8_t*)&TestExplosions, nullptr);
            DebugMenuAddVarBool8("Particles", "Show particles where player is to test (switch with Particles testing)", (int8_t*)&ParticlesReview, nullptr);
            DebugMenuAddVarBool8("Particles", "Show particle objects where player is to test (switch with Particle objects testing)", (int8_t*)&ParticleObjectsReview, nullptr);
        }
        Events::drawHudEvent += []() {
            CDebug::DisplayScreenStrings();	// custom
            CDebug::DebugDisplayTextBuffer();
#ifdef GTA_SCENE_EDIT
            if (CSceneEdit::m_bEditOn) {
                CSceneEdit::Draw();
            }
#endif
            };
        RenderEffectsEvent += []()
            {
                CParticle::Render();
                CMBlur::RenderMotionBlur();
            };
        /*renderBlurEvent += [](CCamera* camera) {
            CMBlur::RenderMotionBlur();
            };*/
        /*heliRenderEvent += [](CHeli* heli) {
            // Heli dust
            if (heli->m_pDustParticle && HeliDust) {
                float radius = (heli->GetPosition().z - FindPlayerCoors(-1).z - 10.0f - 1.0f) * 0.3f + 10.0f;
                HeliDustGenerate(heli, radius, FindPlayerCoors(-1).z, max(16.0f - 4.0f * CTimer::ms_fTimeStep, 2.0f));
            }
        };*/

        Events::vehicleRenderEvent += [](CVehicle* veh) {
            if (veh->m_nVehicleSubClass == VEHICLE_BMX || veh->m_nVehicleSubClass == VEHICLE_PLANE) // Why would a bicycle have smoke, fire, sparks, and other particles?
                return;

            float fwdSpeed = DotProduct(veh->m_vecMoveSpeed, veh->GetForward()) * 180.0f;
            auto Damage = reinterpret_cast<CAutomobile*>(veh)->m_damageManager;
            auto automobile = reinterpret_cast<CAutomobile*>(veh);
            auto plane = reinterpret_cast<CPlane*>(veh);
            int i{};
            int drawParticles = fabsf(fwdSpeed) < 90.0f;
            for (i = 0; i < 4; i++) {
                //  if (fabsf(fwdSpeed) > 5.0f) {
                ((Auto*)automobile)->AddWheelDirtAndWater(&automobile->m_wheelColPoint[i], 0);
                //  }
            }
            //((Veh*)veh)->DoBoatSplashes(1.0f);

              // Rain on roof (now a separate function)
              /*if (!CCullZones::CamNoRain() && !CCullZones::PlayerNoRain() &&
                  fabsf(fwdSpeed) < 20.0f && CWeather::Rain > 0.02f) {
                  CColModel* colModel = veh->GetColModel();

                  for (int i = 0; i < colModel->m_pColData->m_nNumTriangles; i++) {
                      CVector p1, p2, p3, c;

                      colModel->m_pColData->GetTrianglePoint(p1, colModel->m_pColData->m_pTriangles[i].m_nVertA);
                      p1 = *veh->GetMatrix() * p1;
                      colModel->m_pColData->GetTrianglePoint(p2, colModel->m_pColData->m_pTriangles[i].m_nVertB);
                      p2 = *veh->GetMatrix() * p2;
                      colModel->m_pColData->GetTrianglePoint(p3, colModel->m_pColData->m_pTriangles[i].m_nVertC);
                      p3 = *veh->GetMatrix() * p3;
                      c = (p1 + p2 + p3) / 3.0f;

                      int n = 6.0f * CWeather::Rain;
                      for (int j = 0; j <= n; j++)
                          CParticle::AddParticle(PARTICLE_RAIN_SPLASHUP,
                              c + CVector(CGeneral::GetRandomNumberInRange(-0.4f, 0.4f), CGeneral::GetRandomNumberInRange(-0.4f, 0.4f), 0.0f),
                              CVector(0.0f, 0.0f, 0.0f),
                              nullptr, 0.0f, 0, 0, GetRandomNumber() & 1);
                  }
              }*/
            if (veh->m_fHealth < 250.0f && veh->m_nStatus != STATUS_WRECKED && FireWhenAboutToExplode && (!veh->m_nPhysicalFlags.bTouchingWater || !veh->m_nPhysicalFlags.bSubmergedInWater)) {
                CVector damagePos = ((CVehicleModelInfo*)CModelInfo::GetModelInfo(veh->m_nModelIndex))->m_pVehicleStruct->m_avDummyPos[7];
                damagePos = *veh->GetMatrix() * damagePos;
                damagePos.z += 0.15f;
                //	auto PlayFireSound = (void(__cdecl*)(CAEFireAudioEntity * AudioEntity, int eventId, CVector * posn))0x4DD3C0;
                // Car is on fire
                tParticleType ParticleToUse = PARTICLE_CARFLAME;
                if (ParticleEXFire) {
                    ParticleToUse = PARTICLE_CARFLAME_MOVING;
                }
                else {
                    ParticleToUse = PARTICLE_CARFLAME;
                }
                    CParticle::AddParticle(ParticleToUse, damagePos,
                        CVector(0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.01125f, 0.09f)),
                        nullptr, 0.63f);
                
                     //if (!trackIsPlaying) {
                     //	playTrackBASS(PLUGIN_PATH((char*)"fireVC.wav"));
                     //	setVolume(0.3f);
                     //}
                     //veh->m_pFireParticle->DoFxAudio(veh->GetPosition());
                     //static_cast<FireAudio*>(veh->m_pFireParticle->GetFireAudio())->AddAudioEvent(AE_FIRE, veh->GetPosition());
                     //auto ent = veh->m_pFireParticle->GetFireAudio();
                     //PlayFireSound(ent, 133, &veh->GetPosition());


                CVector coors = damagePos;
                coors.x += CGeneral::GetRandomNumberInRange(-0.5625f, 0.5625f),
                    coors.y += CGeneral::GetRandomNumberInRange(-0.5625f, 0.5625f),
                    coors.z += CGeneral::GetRandomNumberInRange(0.5625f, 2.25f);
                CParticle::AddParticle(PARTICLE_CARFLAME_SMOKE, coors, CVector(0.0f, 0.0f, 0.0f));


                CParticle::AddParticle(PARTICLE_ENGINE_SMOKE2, damagePos, CVector(0.0f, 0.0f, 0.0f), nullptr, 0.5f);
            }
            
            //for (i = 0; i < 4; i++) {
                /*	bool rearSkidding = false;
                    int drawParticles = fabsf(fwdSpeed) < 90.0f;
                    if (Damage.m_anWheelsStatus[WHEEL_REAR_LEFT] == 2 ||
                        Damage.m_anWheelsStatus[WHEEL_REAR_RIGHT] == 2)
                        rearSkidding = true;*/
                        //	if (bAlreadySkidding) {
                        //		CParticle::AddParticle(PARTICLE_BURNINGRUBBER_SMOKE,
                        //			automobile->m_wheelColPoint[i].m_vecPoint + CVector(0.0f, 0.0f, 0.25f),
                        //			CVector(0.0f, 0.0f, 0.05f));
                        //	}
                        //}
                                /*tWheelState wheelstate = automobile->m_aWheelState[i];
                                    switch (wheelstate) {
                                    case 1:
                                        if (automobile->AddWheelDirtAndWater(automobile->m_wheelColPoint[i], drawParticles, 0, 0)) {
                                            CParticle::AddParticle(PARTICLE_BURNINGRUBBER_SMOKE,
                                                automobile->m_wheelColPoint[i].m_vecPoint + CVector(0.0f, 0.0f, 0.25f),
                                                CVector(0.0f, 0.0f, 0.0f));

                                            CParticle::AddParticle(PARTICLE_BURNINGRUBBER_SMOKE,
                                                automobile->m_wheelColPoint[i].m_vecPoint + CVector(0.0f, 0.0f, 0.25f),
                                                CVector(0.0f, 0.0f, 0.05f));
                                        }

                                        CParticle::AddParticle(PARTICLE_RUBBER_SMOKE,
                                            automobile->m_wheelColPoint[i].m_vecPoint + CVector(0.0f, 0.0f, 0.25f),
                                            CVector(0.0f, 0.0f, 0.0f));
                                        break;

                                    case 2:
                                        if (i == WHEEL_REAR_LEFT || i == WHEEL_REAR_RIGHT || rearSkidding) {
                                            // same as below

                                            if (fabsf(fwdSpeed) > 5.0f) {

                                                CParticle::AddParticle(PARTICLE_RUBBER_SMOKE,
                                                    automobile->m_wheelColPoint[i].m_vecPoint + CVector(0.0f, 0.0f, 0.25f),
                                                    CVector(0.0f, 0.0f, 0.0f));
                                            }
                                        }
                                        break;

                                    case 3:
                                        if (fabsf(fwdSpeed) > 5.0f) {

                                            CParticle::AddParticle(PARTICLE_RUBBER_SMOKE,
                                                automobile->m_wheelColPoint[i].m_vecPoint + CVector(0.0f, 0.0f, 0.25f),
                                                CVector(0.0f, 0.0f, 0.0f));
                                        }
                                        break;

                                    default:
                                        break;
                                    }
                            }*/

                            // Sparks for friction of burst wheels
            for (int i = 0; i < 4; i++) {
                if (Damage.GetWheelStatus(i) == 1) {
                    static float speedSq;
                    speedSq = static_cast<Vec>(veh->m_vecMoveSpeed).MagnitudeSqr();
                    if (speedSq > SQR(0.1f) &&
                        automobile->m_wheelColPoint[i].m_nSurfaceTypeB != SURFACE_GRASS_MEDIUM_DRY &&
                        automobile->m_wheelColPoint[i].m_nSurfaceTypeB != SURFACE_MUD_DRY &&
                        automobile->m_wheelColPoint[i].m_nSurfaceTypeB != SURFACE_P_SAND &&
                        automobile->m_wheelColPoint[i].m_nSurfaceTypeB != SURFACE_SAND_BEACH &&
                        automobile->m_wheelColPoint[i].m_nSurfaceTypeB != SURFACE_P_UNDERWATERLUSH && WheelParticles) {
                        CVector normalSpeed = automobile->m_wheelColPoint[i].m_vecNormal * DotProduct(automobile->m_wheelColPoint[i].m_vecNormal, veh->m_vecMoveSpeed);
                        CVector frictionSpeed = veh->m_vecMoveSpeed - normalSpeed;
                        if (i == WHEEL_FRONT_LEFT || i == WHEEL_REAR_LEFT)
                            frictionSpeed -= 0.05f * veh->GetRight();
                        else
                            frictionSpeed += 0.05f * veh->GetRight();
                        CVector unusedRight = 0.15f * veh->GetRight();
                        CVector sparkDir = 0.25f * frictionSpeed;
                        CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->m_wheelColPoint[i].m_vecPoint, sparkDir);

                        if (speedSq > 0.04f)
                            CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->m_wheelColPoint[i].m_vecPoint, sparkDir);
                        if (speedSq > 0.16f) {
                            CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->m_wheelColPoint[i].m_vecPoint, sparkDir);
                            CParticle::AddParticle(PARTICLE_SPARK_SMALL, automobile->m_wheelColPoint[i].m_vecPoint, sparkDir);
                        }
                    }
                }
            }

            // Exhaust smoke
            /*CVehicleModelInfo* mi = (CVehicleModelInfo*)CModelInfo::GetModelInfo(veh->m_nModelIndex);
            if (veh->m_nVehicleFlags.bEngineOn && !(veh->m_pHandlingData->m_bNoExhaust) && fwdSpeed < 130.0f) {
                CVector exhaustPos = mi->m_pVehicleStruct->m_avDummyPos[6];
                CVector exhaustPos2 = mi->m_pVehicleStruct->m_avDummyPos[11];
                CVector pos1, pos2, dir1, dir2;
                if (static_cast<Vec>(exhaustPos) != CVector(0.0f, 0.0f, 0.0f)) {
                    dir1.z = 0.0f;
                    dir2.z = 0.0f;
                    if (fwdSpeed < 10.0f) {
                        CVector steerFwd(-sinf(veh->m_fSteerAngle), cosf(veh->m_fSteerAngle), 0.0f);
                        steerFwd = Multiply3x3(*veh->GetMatrix(), steerFwd);
                        float r = CGeneral::GetRandomNumberInRange(-0.06f, -0.03f);
                        dir1.x = steerFwd.x * r;
                        dir1.y = steerFwd.y * r;
                    }
                    else {
                        dir1.x = veh->m_vecMoveSpeed.x;
                        dir1.y = veh->m_vecMoveSpeed.y;
                    }

                    pos1 = *veh->GetMatrix() * exhaustPos;
                    if (veh->m_pHandlingData->m_bDoubleExhaust) {
                        pos2 = exhaustPos;
                        pos2.x = -pos2.x;
                        pos2 = *veh->GetMatrix() * pos2;
                        dir2 = dir1;
                    }

                    static float fumesLimit = 2.0f;
                    if (CGeneral::GetRandomNumberInRange(1.0f, 3.0f) * (veh->m_fGasPedal + 1.1f) > fumesLimit)
                        //for (i = 0; i < 4; i++) {
                            CParticle::AddParticle(PARTICLE_EXHAUST_FUMES, pos1, dir1);
                            if (veh->m_pHandlingData->m_bDoubleExhaust)
                                CParticle::AddParticle(PARTICLE_EXHAUST_FUMES, pos2, dir2);


                            static float extraFumesLimit = 0.5f;
                            if (veh->m_fGasPedal > extraFumesLimit && veh->m_nCurrentGear < 3) {
                                if (GetRandomNumber() & 1)
                                    CParticle::AddParticle(PARTICLE_EXHAUST_FUMES, pos1, dir1);
                                else if (veh->m_pHandlingData->m_bDoubleExhaust)
                                    CParticle::AddParticle(PARTICLE_EXHAUST_FUMES, pos2, dir2);
                            }*/

                            /*if (veh->m_nStatus == STATUS_PLAYER && (CTimer::m_FrameCounter & 3) == 0 &&
                                CWeather::Rain == 0.0f && i == 0) {
                                CVector camDist = veh->GetPosition() - TheCamera.GetPosition();
                                if (DotProduct(veh->GetForward(), camDist) > 0.0f ||
                                    TheCamera.GetLookDirection() == 1 ||
                                    TheCamera.GetLookDirection() == 2) {
                                    CParticle::AddParticle(PARTICLE_HEATHAZE, pos1, CVector(0.0f, 0.0f, 0.0f));
                                    if (veh->m_pHandlingData->m_bDoubleExhaust)
                                        CParticle::AddParticle(PARTICLE_HEATHAZE, pos2, CVector(0.0f, 0.0f, 0.0f));

                                    CParticle::AddParticle(PARTICLE_HEATHAZE, pos1, CVector(0.0f, 0.0f, 0.0f));
                                    if (veh->m_pHandlingData->m_bDoubleExhaust)
                                        CParticle::AddParticle(PARTICLE_HEATHAZE, pos2, CVector(0.0f, 0.0f, 0.0f));
                                }
                            }*/
                            //	}


                    //}
            static RwRGBA splashCol = { 155, 155, 185, 196 };
            static RwRGBA smokeCol = { 255, 255, 255, 255 };

            /*for (int i = 0; i < 4; i++) {
                if (automobile->m_wheelColPoint[i].m_nSurfaceTypeB == SURFACE_WATER_SHALLOW) {
                    CVector pos = automobile->m_wheelColPoint[i].m_vecPoint + 0.3f * veh->GetUp() - veh->GetPosition();
                    CVector vSpeed = veh->GetSpeed(pos);
                    vSpeed.z = 0.0f;
                    float fSpeed = static_cast<Vec>(vSpeed).MagnitudeSqr();
                    if (fSpeed > sq(0.05f)) {
                        fSpeed = Sqrt(fSpeed);

                        float size = min((fSpeed < 0.15f ? 0.25f : 0.75f) * fSpeed, 0.6f);
                        CVector right = 0.2f * fSpeed * veh->GetRight() + 0.2f * vSpeed;

                        CParticle::AddParticle(PARTICLE_PED_SPLASH,
                            pos + veh->GetPosition(), -0.5f * right,
                            nullptr, size, splashCol,
                            CGeneral::GetRandomNumberInRange(0.0f, 10.0f),
                            CGeneral::GetRandomNumberInRange(0.0f, 90.0f), 1, 0);

                        CParticle::AddParticle(PARTICLE_RUBBER_SMOKE,
                            pos + veh->GetPosition(), -0.6f * right,
                            nullptr, size, smokeCol, 0, 0, 0, 0);
                    }
                }
            }*/

            /*if (!veh->m_nPhysicalFlags.bSubmergedInWater && veh->m_nStatus != STATUS_WRECKED) {
                if (DamagedEngineSmoke) {
                    CVector direction = fSpeedMult[5] * veh->m_vecMoveSpeed;
                    CVector damagePos = ((CVehicleModelInfo*)CModelInfo::GetModelInfo(veh->m_nModelIndex))->m_pVehicleStruct->m_avDummyPos[7];
                    if (veh->m_nModelIndex == MODEL_BFINJECT)
                        damagePos = CVector(0.3f, -1.5f, -0.1f);
                    else if (veh->m_nModelIndex == MODEL_CADDY)
                        damagePos = CVector(0.6f, -1.0f, -0.25f);
                    else if (veh->m_pHandlingData->m_bIsHeli) {
                        damagePos.x = 0.4f * veh->GetColModel()->m_boundBox.m_vecMax.x;
                        damagePos.y = 0.2f * veh->GetColModel()->m_boundBox.m_vecMin.y;
                        damagePos.z = 0.3f * veh->GetColModel()->m_boundBox.m_vecMax.z;
                    }
                    else
                        damagePos.z += fDamagePosSpeedShift * (veh->GetColModel()->m_boundBox.m_vecMax.z - damagePos.z) * DotProduct(veh->GetForward(), veh->m_vecMoveSpeed);
                    damagePos = *veh->GetMatrix() * damagePos;
                    damagePos.z += 0.15f;
                    bool electric = veh->m_pHandlingData->m_transmissionData.m_nEngineType == 'E';

                    if (electric && veh->m_fHealth < 320.0f && veh->m_fHealth > 1.0f) {
                        direction = 0.85f * veh->m_vecMoveSpeed;
                        direction += veh->GetRight() * CGeneral::GetRandomNumberInRange(0.0f, 0.04f) * (1.0f - 2.0f * veh->m_vecMoveSpeed.Magnitude());
                        direction.z += 0.001f;
                        int n = (GetRandomNumber() & 7) + 2;
                        for (int i = 0; i < n; i++)
                            CParticle::AddParticle(PARTICLE_SPARK_SMALL, damagePos, direction);
                        if (((CTimer::m_FrameCounter + veh->m_nRandomSeed) & 7) == 0)
                            CParticle::AddParticle(PARTICLE_ENGINE_SMOKE2, damagePos, 0.8f * veh->m_vecMoveSpeed, nullptr, 0.1f, 0, 0, 0, 1000);
                    }
                    else if (electric && veh->m_fHealth < 460.0f) {
                        direction = 0.85f * veh->m_vecMoveSpeed;
                        direction += veh->GetRight() * CGeneral::GetRandomNumberInRange(0.0f, 0.04f) * (1.0f - 2.0f * veh->m_vecMoveSpeed.Magnitude());
                        direction.z += 0.001f;
                        int n = (GetRandomNumber() & 3) + 1;
                        for (int i = 0; i < n; i++)
                            CParticle::AddParticle(PARTICLE_SPARK_SMALL, damagePos, direction);
                        if (((CTimer::m_FrameCounter + veh->m_nRandomSeed) & 0xF) == 0)
                            CParticle::AddParticle(PARTICLE_ENGINE_SMOKE, damagePos, 0.8f * veh->m_vecMoveSpeed, nullptr, 0.1f, 0, 0, 0, 1000);
                    }
                    else if (veh->m_fHealth < 320.0f) {
                        CParticle::AddParticle(PARTICLE_ENGINE_SMOKE2, damagePos, fSpeedMult[0] * direction);
                    }
                    else if (veh->m_fHealth < 390.0f) {
                        CParticle::AddParticle(PARTICLE_ENGINE_STEAM, damagePos, fSpeedMult[1] * direction);
                        CParticle::AddParticle(PARTICLE_ENGINE_SMOKE, damagePos, fSpeedMult[2] * direction);
                    }
                    else if (veh->m_fHealth < 650.0f) {
                        CParticle::AddParticle(PARTICLE_ENGINE_STEAM, damagePos, fSpeedMult[3] * direction, nullptr, 0.63f);
                    }
                    else {
                        int rnd = CTimer::m_FrameCounter + veh->m_nRandomSeed;
                        if (rnd < 10 ||
                            rnd < 70 && rnd > 25 ||
                            rnd < 160 && rnd > 100 ||
                            rnd < 200 && rnd > 175 ||
                            rnd > 235)
                            return;
                        direction.z += 0.05f * max(1.0f - 1.6f * veh->m_vecMoveSpeed.Magnitude(), 0.0f);
                        if (electric) {
                            direction = 0.85f * veh->m_vecMoveSpeed;
                            direction += veh->GetRight() * CGeneral::GetRandomNumberInRange(0.0f, 0.04f) * (1.0f - 2.0f * veh->m_vecMoveSpeed.Magnitude());
                            direction.z += 0.001f;
                            int n = (GetRandomNumber() & 2) + 2;
                            for (int i = 0; i < n; i++)
                                CParticle::AddParticle(PARTICLE_SPARK_SMALL, damagePos, direction);
                            if (((CTimer::m_FrameCounter + veh->m_nRandomSeed) & 0xF) == 0)
                                CParticle::AddParticle(PARTICLE_ENGINE_SMOKE, damagePos, 0.8f * veh->m_vecMoveSpeed, nullptr, 0.1f, 0, 0, 0, 1000);
                        }
                        else {
                            if (TheCamera.GetLookDirection() != 3)
                                CParticle::AddParticle(PARTICLE_ENGINE_STEAM, damagePos, direction);
                            else if (((CTimer::m_FrameCounter + veh->m_nRandomSeed) & 1) == 0)
                                CParticle::AddParticle(PARTICLE_ENGINE_STEAM, damagePos, fSpeedMult[4] * veh->m_vecMoveSpeed);
                        }
                    }
                }
            }*/

            };
        shutdownGameEvent += []()
            {
                CParticleObject::RemoveAllExpireableParticleObjects();
                CParticle::Shutdown();
                CMBlur::MotionBlurClose();
             //   CParticleObject::SaveParticle(work_buff, (uint*)sizeof(work_buff));
                // CPlane2::Shutdown();
            };
        TyreBurstEvent += [](CVector& position, CVector& direction) {
            for (int32_t i = 0; i < 4; i++) {
                CParticle::AddParticle(PARTICLE_BULLETHIT_SMOKE, position, CVector(0.0f, 0.0f, 0.0f));
            }
            };

        HeliBloodEvent += [](Fx_c* fx, CVector* a2, CVector* a3, int amount, float a5) {
            for (int i = 0; i < 16; i++) {
                if (BloodnGore) {
                    CParticle::AddParticle(PARTICLE_BLOOD_SMALL, *a2, CVector(0.0f, 0.0f, 1.0f) * 0.01f);
                    CParticle::AddParticle(PARTICLE_TEST, *a2, CVector(0.0f, 0.0f, 0.02f), NULL, 0.1f);
                    CParticle::AddParticle(PARTICLE_TEST, *a2 + CVector(0.0f, 0.0f, 0.2f), CVector(0.0f, 0.0f, -0.01f), NULL, 0.1f);
                }
            }
            };

        HeadShotEvent += [](Fx_c* fx, CVector* a2, CVector* a3, int amount, float a5) {
            if (BloodnGore) {
                for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++) {
                    CPed* ped = CPools::ms_pPedPool->GetAt(i);
                    if (ped) {
                        if ((CTimer::m_FrameCounter % 7) > 3) {
                            CVector bloodDir(0.0f, 0.0f, 0.0f);
                            bloodDir = 0.1f * ped->GetUp();
                            for (int i = 0; i < 4; i++) {
                                CParticle::AddParticle(PARTICLE_BLOOD_SPURT, *a2, bloodDir, NULL, 0.0f, 0, 0, 0, 0);
                            }
                        }

                    }
                }
            }
            };
        MeleeWeaponEvent += [](CTaskSimpleFight* task,
            CPed* creator,
            CPed* ped,
            CVector* a4,
            CVector* a5,
            __int16 a6) {
                if (BloodnGore) {
                    eWeaponType weaptype;
                    CWeaponInfo* info;
                    CVector newDir;
                    CVector collisionDist;
                    CColModel* victimPedCol = ped->GetColModel();
                    CSphere* sphere = &victimPedCol->m_boundSphere;
                    //  a5->Normalise();
                    *a5 /= 2.0f;
                    collisionDist = sphere->m_vecCenter - *a5;
                    //  collisionDist.Normalise();
                    collisionDist /= 2.0f;
                    CVector dir = collisionDist * RecipSqrt(1.0f, 10.0f * static_cast<Vec>(collisionDist).MagnitudeSqr());
                    dir /= 2.0f;
                    weaptype = creator->m_aWeapons[creator->m_nActiveWeaponSlot].m_eWeaponType;
                    bool isHeavy = weaptype >= WEAPON_GOLFCLUB && weaptype <= WEAPON_KATANA;
                    info = GetInfo(creator);
                    if (weaptype == WEAPON_CHAINSAW ||
                        task->m_pAnim->m_nAnimGroup == ANIM_GROUP_CSAW_1 ||
                        task->m_pAnim->m_nAnimGroup == ANIM_GROUP_PLAYERCSAW ||
                        task->m_pAnim->m_nAnimGroup == ANIM_GROUP_PLAYERCSAWF ||
                        task->m_pAnim->m_nAnimGroup == ANIM_GROUP_PLAYERCSAWM)
                    {
                        if (ped->m_ePedState != PEDSTATE_DEAD && !((CTimer::m_FrameCounter + 17) & 1)
                            || ped->m_ePedState == PEDSTATE_DEAD && !((CTimer::m_FrameCounter + 17) & 3))
                        {
                            CParticle::AddParticle(PARTICLE_TEST, *a4, CVector(0.0f, 0.0f, 0.0f), NULL, 0.2f);
                        }

                        if (isHeavy)
                        {
                            CVector bloodPos = *a4 + (collisionDist * 0.7f);
                            dir.x += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                            dir.y += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                            CParticle::AddParticle(PARTICLE_BLOOD, bloodPos, dir);

                            dir.x += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                            dir.y += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                            CParticle::AddParticle(PARTICLE_BLOOD, bloodPos, dir);
                        }
                        newDir = dir;
                        newDir /= 2.0f;
                        newDir.z += 0.2f;
                        CParticle::AddParticle(PARTICLE_BLOOD_SMALL, *a4, newDir);
                        for (int i = 0; i < 7; i++) {
                            CParticle::AddParticle(PARTICLE_BLOOD_SMALL, *a4, newDir);
                            CParticle::AddParticle(PARTICLE_BLOOD_SPURT, *a4, newDir);
                        }
                        CParticle::AddParticle(PARTICLE_BLOOD, *a4, newDir);
                        newDir.z = dir.z + 0.1f;
                        CParticle::AddParticle(PARTICLE_BLOOD, *a4, newDir);
                        newDir.x = 0.0f;
                        newDir.y = 0.0f;
                        newDir.z = 0.01f;
                        CParticle::AddParticle(PARTICLE_DEBRIS2, *a4, newDir);
                        //CParticle::AddParticle(PARTICLE_TEST, *a2, CVector(0.0f, 0.0f, 0.0f));
                        CVector dropDir(CGeneral::GetRandomNumberInRange(-0.15f, 0.15f), CGeneral::GetRandomNumberInRange(0.1f, 0.35f), 0.f);
                        CVector dropPos(CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_X(50.0f), SCREEN_STRETCH_FROM_RIGHT(50.0f)),
                            CGeneral::GetRandomNumberInRange(SCREEN_STRETCH_Y(50.0f), SCREEN_STRETCH_FROM_BOTTOM(50.0f)), 1.f);
                        CParticle::AddParticle(PARTICLE_BLOODDROP, dropPos, dropDir, NULL, CGeneral::GetRandomNumberInRange(0.1f, 0.15f),
                            RwRGBA(0, 0, 0, 0), 0, 0, GetRandomNumber() & 1, 0);
                        debug("BloodEvent");
                    }
                    if (weaptype == WEAPON_KNIFE || weaptype == WEAPON_KATANA || task->m_pAnim->m_nAnimGroup == 39) {
                        for (int i = 0; i < 3; i++) {
                            CParticle::AddParticle(PARTICLE_BLOOD_SPURT, *a4, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, 0, 0, 0, 0);
                            CParticle::AddParticle(PARTICLE_BLOOD_SPURT, *a4, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, 0, 0, 0, 0);
                            CParticle::AddParticle(PARTICLE_BLOOD_SPURT, *a4, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, 0, 0, 0, 0);
                        }
                        if (!(GetRandomNumber() & 3)) {
                            CParticle::AddParticle(PARTICLE_TEST, *a4, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, 0, 0, 0, 0);
                        }
                        for (int i = 0; i < 4; i++) {
                            CParticle::AddParticle(PARTICLE_BLOOD, *a4, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, 0, 0, 0, 0);
                        }
                    }
                    for (int i = 0; i < 4; ++i) {
                        CVector bloodPos = *a4 - 0.2f * ped->GetForward();
                        CParticle::AddParticle(PARTICLE_BLOOD, bloodPos, dir, NULL, 0.0f, 0, 0, 0, 0);
                    }
                }
            };
        // Old method, lame, moved to DoBulletImpact
       /* BloodEvent += [](Fx_c* fx, CVector* a2, CVector* a3, int amount, float a5) {
            if (BloodnGore) {
                eWeaponType weaptype;
                CWeaponInfo* info;
                uint8_t bloodAmount = 1;
                for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++) {
                    CPed* ped = CPools::ms_pPedPool->GetAt(i);
                    if (ped) {
                        weaptype = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType;
                        info = GetInfo(ped);
                        CCam cam = TheCamera.m_aCams[TheCamera.m_nActiveCam];
                        *a3 /= 3.0f;
                        a3->Normalise();
                        CVector collisionDist;
                        CColModel* victimPedCol = ped->GetColModel();
                        CSphere* sphere = &victimPedCol->m_boundSphere;
                        collisionDist = sphere->m_vecCenter - *a3;
                        collisionDist.Normalise();
                        collisionDist /= 3.0f;
                        CVector dir = collisionDist * RecipSqrt(1.0f, 10.0f * static_cast<Vec>(collisionDist).MagnitudeSqr());
                        dir /= 3.0f;
                        auto usingGun = ped->m_pIntelligence->GetTaskUseGun();
                        bool isHeavy = weaptype >= WEAPON_GOLFCLUB && weaptype <= WEAPON_KATANA;

                        if (usingGun) {
                                if (weaptype == WEAPON_MINIGUN)
                                {
                                    CParticle::AddParticle(PARTICLE_TEST, *a2, CVector(0.f, 0.f, 0.f), NULL, 0.f, 0, 0, 0, 0);
                                    CParticle::AddParticle(PARTICLE_TEST, *a2, CVector(0.f, 0.f, 0.f), NULL, 0.f, 0, 0, 0, 0);
                                    CParticle::AddParticle(PARTICLE_TEST, *a2, CVector(0.f, 0.f, 0.f), NULL, 0.f, 0, 0, 0, 0);
                                }
                                else
                                {
                                    for (int32_t i = 0; i < 4; i++) {
                                        CParticle::AddParticle(PARTICLE_BLOOD_SMALL, *a2 + CVector(0.0f, 0.0f, 0.5f), CVector(0.0f, 0.0f, 0.0f));
                                    }
                                }


                                if (weaptype == WEAPON_SHOTGUN || weaptype == WEAPON_SPAS12 || weaptype == WEAPON_SAWNOFF) {
                                    bloodAmount = 32;
                                    debug("Shotgun");
                                    log("Shotgun");
                                }
                                else
                                {
                                    bloodAmount = 4;
                                    debug("Not shotgun");
                                    log("Not shotgun");
                                }
                                for (uint8_t i = 0; i < bloodAmount; i++) {
                                    CParticle::AddParticle(PARTICLE_BLOOD_SMALL, *a2, dir);
                                }

                        }
                    }

                }
            }
        };*/

        /*FireInstantHit += [](CWeapon* weap,
             CEntity* firingEntity,
             CVector* posn,
             CVector* effectPosn,
             CEntity* targetEntity,
             CVector* target,
             CVector* posnForDriveBy,
             char a8,
             char additionalEffects) {
               //  if (MuzzleFlashnSmoke) {
                  //  (*effectPosn).x += -sinf(angle) * 0.15f;
                  //   (*effectPosn).y += cosf(angle) * 0.15f;
               //      DoGunFlash((CPed*)firingEntity, effectPosn);
                  //   debug("FireInstantHit");
                if (firingEntity) {
                    CParticle::HandleShootableBirdsStuff(firingEntity, *target);
                }
               //  }
         };*/

        FireFromCarEvent += [](CWeapon* weapon,
            CVector origin,
            CVector target,
            CVehicle* a4,
            CPed* owner) {
                if (MuzzleFlashnSmoke) {
                    DoGunFlash(owner, &origin, false);
                }
            };

        PunchImpactEvent += [](Fx_c* cock, CVector& pos, CVector& dir, int32_t a4) {
            if (PunchImpactParticles) {
                CParticle::AddParticle(PARTICLE_SMOKE_SLOWMOTION, pos, CVector(0.0f, 0.0f, 0.0f));
            }
            };
        WheelSandEvent += [](CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
            if (WheelParticles) {
                if (CTimer::m_FrameCounter & 2 ||
                    CWeather::WetRoads > 0.0f && CGeneral::GetRandomNumberInRange(CWeather::WetRoads, 1.01f) > 0.5f)
                    return;
                static RwRGBA sandCol = { 170, 165, 140, 255 };
                CVector dir;
                dir.x = 0.5f * vehicle->m_vecMoveSpeed.x;
                dir.y = 0.5f * vehicle->m_vecMoveSpeed.y;
                for (int i = 0; i < 1; i++) {
                    dir.z = CGeneral::GetRandomNumberInRange(0.02f, 0.055f);
                    CParticle::AddParticle(PARTICLE_SAND, pos, dir, NULL,
                        2.0f * vehicle->m_vecMoveSpeed.Magnitude(), sandCol);
                }
            }
            };

        WheelGrassEvent += [](CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
            if (WheelParticles) {
                static RwRGBA grassCol = { 8, 24, 8, 255 };
                CVector dir;
                dir.x = -0.05f * vehicle->m_vecMoveSpeed.x;
                dir.y = -0.05f * vehicle->m_vecMoveSpeed.y;
                for (int i = 0; i < 4; i++) {
                    dir.z = CGeneral::GetRandomNumberInRange(0.03f, 0.06f);
                    CParticle::AddParticle(PARTICLE_WHEEL_DIRT, pos, dir, NULL,
                        CGeneral::GetRandomNumberInRange(0.05f, 0.09f), grassCol);
                }
            }
            };

        WheelGravelEvent += [](CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
            if (WheelParticles) {
                static RwRGBA gravelCol = { 64, 64, 64, 255 };
                CVector dir;
                dir.x = -0.05f * vehicle->m_vecMoveSpeed.x;
                dir.y = -0.05f * vehicle->m_vecMoveSpeed.y;
                for (int i = 0; i < 4; i++) {
                    dir.z = CGeneral::GetRandomNumberInRange(0.03f, 0.06f);
                    CParticle::AddParticle(PARTICLE_WHEEL_DIRT, pos, dir, NULL,
                        CGeneral::GetRandomNumberInRange(0.05f, 0.09f), gravelCol);
                }
            }
            };

        WheelMudEvent += [](CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
            if (WheelParticles) {
                static RwRGBA mudCol = { 64, 32, 16, 255 };
                CVector dir;
                dir.x = -0.05f * vehicle->m_vecMoveSpeed.x;
                dir.y = -0.05f * vehicle->m_vecMoveSpeed.y;
                for (int i = 0; i < 4; i++) {
                    dir.z = CGeneral::GetRandomNumberInRange(0.03f, 0.06f);
                    CParticle::AddParticle(PARTICLE_WHEEL_DIRT, pos, dir, NULL,
                        CGeneral::GetRandomNumberInRange(0.05f, 0.09f), mudCol);
                }
            }
            };


        RocketSmokeEvent += [](void* fuckingbullshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (MissileSmoke) {
                    // CParticleObject::AddObject(POBJECT_ROCKET_TRAIL, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f), 0.15f, 450, RwRGBA(0, 0, 0, 0), true);
                    CParticle::AddParticlesAlongLine(PARTICLE_ROCKET_SMOKE, (CVector&)*position, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f), 0.7f, 0, 0, 0, 3000);
                }
            };

        StuntPlaneSmokeEvent += [](void* fuckingbullshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                auto vehicle = FindPlayerVehicle(-1, false);
                RwRGBA color;
                if (vehicle) {
                    switch (vehicle->m_nModelIndex) {
                    case MODEL_STUNT:
                        color.red = 255;
                        color.green = 0;
                        color.blue = 0;
                        color.alpha = 255;
                        break;
                    case MODEL_CROPDUST:
                        color.red = 255;
                        color.green = 255;
                        color.blue = 255;
                        color.alpha = 255;
                        break;
                    }
                    if (PlaneSmokeParticles) {
                        CParticleObject::AddObject(POBJECT_SMOKE_TRAIL, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f), 0.35f, 350, color, true);
                    }
                }
            };

        BurnoutEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (WheelParticles) {
                    CParticle::AddParticle(PARTICLE_BURNINGRUBBER_SMOKE,
                        (CVector)*position + CVector(0.0f, 0.0f, 0.25f),
                        CVector(0.0f, 0.0f, 0.0f));

                    CParticle::AddParticle(PARTICLE_BURNINGRUBBER_SMOKE,
                        (CVector)*position + CVector(0.0f, 0.0f, 0.25f),
                        CVector(0.0f, 0.0f, 0.05f));
                }
            };

        PlaneSmokeEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (PlaneSmokeParticles) {
                    CParticleObject::AddObject(POBJECT_SMOKE_TRAIL, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f), 0.25f, 350, RwRGBA(0, 0, 0, -1), 1);
                }
            };

        ExplodedPlaneSmokeEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (PlaneSmokeParticles) {
                    CParticleObject::AddObject(POBJECT_SMOKE_TRAIL, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f), 0.25f, 1000, RwRGBA(0, 0, 0, -1), 1);
                }
            };

        AddSparksEvent += [](RwV3d* position,
            RwV3d* plsdontletthismodflop,
            float force,
            signed int count,
            float acrossX,
            float acrossY,
            float acrossZ,
            int blurIf0,
            float spread,
            float life) {
                if (BulletImpactParticles) {
                    for (int32_t i = 0; i < 4; i++)
                    {
                        CParticle::AddParticle(PARTICLE_SPARK_SMALL, (CVector&)*position, CVector(0.0f, 0.0f, 0.10f));
                        CParticle::AddParticle(PARTICLE_SPARK, (CVector&)*position, CVector(0.0f, 0.0f, 0.10f));
                    }
                }
            };
        FrictionSparksEvent += [](RwV3d* position,
            RwV3d* plsdontletthismodflop,
            float force,
            signed int count,
            float acrossX,
            float acrossY,
            float acrossZ,
            int blurIf0,
            float spread,
            float life) {
                CParticle::AddParticle(PARTICLE_SPARK_SMALL, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f));
            };

        PlanenHeliBlowUpEvent += [](CEntity* newVictim,
            CPed* newCreator,
            int type,
            CVector2D pos,
            float z,
            int time,
            char usesSound,
            float cameraShake,
            char isVisible) {
                unsigned char originalRed = CDraw::FadeRed;
                unsigned char originalGreen = CDraw::FadeGreen;
                unsigned char originalBlue = CDraw::FadeBlue;
                if (ExplosionsFlash) {
                    debug("Plane or heli blew up");
                    if (GetRandomNumber() & 3) {
                        TheCamera.SetFadeColour(255, 255, 255);
                        TheCamera.Fade(0.0f, 0);
                        TheCamera.ProcessFade();
                        TheCamera.Fade(1.0f, 1);
                    }
                    else {
                        TheCamera.SetFadeColour(200, 200, 200);
                        TheCamera.Fade(0.0f, 0);
                        TheCamera.ProcessFade();
                        TheCamera.Fade(1.0f, 1);
                    }
                }
            };




        SandBulletImpactEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (BulletImpactParticles) {
                    CParticle::AddParticle(PARTICLE_SAND, (CVector&)*position, CVector(CGeneral::GetRandomNumberInRange(0.0f, 0.3f), CGeneral::GetRandomNumberInRange(0.0f, 0.3f), CGeneral::GetRandomNumberInRange(0.05f, 0.15f)), NULL, 1.0f);
                }
            };

        FireExtinguishParticles += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (FiråExtinguishParticles) {
                    CParticle::AddParticle(PARTICLE_STEAM_NY_SLOWMOTION, (CVector&)*position, CVector(0.f, 0.f, 0.2f), NULL, 0.5f);
                    CParticle::AddParticle(PARTICLE_STEAM_NY_SLOWMOTION, (CVector&)*position, CVector(0.f, 0.f, 0.1f), NULL, 0.8f);
                }
            };

        WaterCannonParticles += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (WaterCannînParticles) {
                    CParticle::AddParticle(PARTICLE_STEAM_NY_SLOWMOTION, (CVector&)*position, CVector(0.f, 0.f, 0.05f), 0, 0.5f);
                    CParticle::AddParticle(PARTICLE_SPLASH, (CVector&)*position, CVector(0.f, 0.f, 0.0f), 0, 1.0f,
                        CGeneral::GetRandomNumberInRange(0.f, 10.f), CGeneral::GetRandomNumberInRange(0.f, 90.f), 1);
                }
            };


        /*WaterParticlesSplashEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (WaterParticles) {
                    CParticle::AddParticle(PARTICLE_CAR_SPLASH, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f), 0, 0.5f,
                        CGeneral::GetRandomNumberInRange(0.f, 10.f), CGeneral::GetRandomNumberInRange(0.f, 90.f), 1);
                }
        };

        /*WaterParticlesWakeEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (WaterParticles) {
                    RwRGBA waterSprayCol = { 48, 48, 64, 0 };
                    CParticle::AddParticle(PARTICLE_WATERSPRAY, (CVector&)*position, CVector(0.0f,0.0f,0.0f), NULL, 0.0f, waterSprayCol);
                }
        };*/

        FootSmokeEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (FootDustParticles) {
                    for (int i = 0; i < 4; ++i) {
                        CParticle::AddParticle(PARTICLE_PEDFOOT_DUST, (CVector&)*position, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, RwRGBA(0, 0, 0, 0), 0, 0, 0, 0);
                    }
                }
            };

        FootSplashEvent += [](void* pieceofshit,
            RwV3d* position,
            RwV3d* velocity,
            int a4,
            FxPrtMult_c* smoke,
            float a6,
            float a7,
            float a8,
            int a9) {
                if (FootSplashesParticles) {
                    CVector direction;
                    for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++) {
                        auto ped = CPools::ms_pPedPool->GetAt(i);
                        if (ped) {
                            direction = ped->GetForward() * -0.05f;
                        }
                    }
                    for (int i = 0; i < 4; i++) {
                        CVector adjustedPos = *(CVector*)position;
                        adjustedPos.x += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f);
                        adjustedPos.y += CGeneral::GetRandomNumberInRange(-0.1f, 0.1f);
                        CParticle::AddParticle(PARTICLE_RAIN_SPLASHUP, adjustedPos, direction, NULL, 0.15f, RwRGBA(32, 32, 32, 32), 0, 0, CGeneral::GetRandomNumber() & 1, 200);
                    }
                }
        };

        FireOneInstantRoundEvent += [](CVector* start, CVector* end, int intensity) {
            if (PoliceBoatGunParticles) {
                debug("FireOneInstantRoundEvent");
                if (WaterParticles) {
                    float waterLevel;
                    CVector particleDir;
                    particleDir.z = CGeneral::GetRandomNumberInRange(0.01f, 0.03f);

                    if (CWaterLevel::GetWaterLevelNoWaves(end->x, end->y, end->z + 10.0f, &waterLevel, NULL, NULL))
                    {
                        CParticle::AddParticle(PARTICLE_BOAT_SPLASH, CVector((*end).x, (*end).y, waterLevel), CVector(0.0f, 0.0f, 0.01f));
                    }
                }
                CParticle::AddParticle(PARTICLE_HELI_ATTACK, *start, ((*end) - (*start)) * 0.15f);
                CParticle::AddParticle(PARTICLE_GUNFLASH, *start, CVector(0.0f, 0.0f, 0.0f));
            }
        };

        // More chaos to riots
        RiotSmokeEvent += [](void* bullshit, char* name, RwV3d* point, RwMatrix* m, int flag) {
            debug("RiotSmoke");
            if (RiotExplosions) {
                CExplosionVC::AddExplosion(nullptr, nullptr, EXPLOSION_SMALL, (CVector&)*point, 3000, 1, 1.5f, 0);
                CParticle::AddJetExplosion((CVector&)*point, CGeneral::GetRandomNumberInRange(0.0f, 1.0f), CGeneral::GetRandomNumberInRange(0.0f, 10.0f));
                CParticleObject::AddObject(POBJECT_SMOKE_TRAIL, (CVector&)*point, CVector(0.0f, 0.0f, 0.6f), 2.5f, CGeneral::GetRandomNumberInRange(9000, 20000), RwRGBA(0, 0, 0, -1), 1);
            }
        };

        PedWaterSplashEvent += [](Fx_c* fx, CVector* pos) {
            CVector particlePos, particleDir, particlePos2;
            float pedSpeed{};
            bool Jump{};
            for (auto ped : CPools::ms_pPedPool) {

                if (ped) {
                    Jump = ped->m_ePedState == PEDSTATE_JUMP;
                    particlePos = ped->GetPosition() + ped->GetForward() * 0.3f;
                    particlePos.z -= 1.2f;
                    pedSpeed = CVector2D(ped->m_vecMoveSpeed).Magnitude();
                    particleDir = ped->m_vecMoveSpeed * -0.75f;
                    particlePos2 = ped->GetPosition();
                    particlePos2.z -= 0.1f;
                }
            }


            if (WaterParticles) {
                RwRGBA color;
                color.red = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalRed + CTimeCycle::GetAmbientRed()) * 127.5f;
                color.green = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalBlue + CTimeCycle::GetAmbientBlue()) * 127.5f;
                color.blue = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalGreen + CTimeCycle::GetAmbientGreen()) * 127.5f;
                color.alpha = CGeneral::GetRandomNumberInRange(48.0f, 96.0f);
                for (int i = 0; i < 4; i++) {
                    CVector position = *pos;
                    position.x += CGeneral::GetRandomNumberInRange(-0.75f, 0.75f);
                    position.y += CGeneral::GetRandomNumberInRange(-0.75f, 0.75f);
                    CParticle::AddParticle(PARTICLE_RAIN_SPLASH_BIGGROW, position, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, color, 0, 0, 0, 0);
                }
                CVector position = *pos;
                if (position.z >= 0.0f) {
                    position.z += 0.25f;
                    CParticleObject::AddObject(POBJECT_SPLASHES_AROUND, position, CVector(0.0f, 0.0f, 0.0f), 4.5f, 1500, RwRGBA(0, 0, 0, 0), true);
                }
                for (int i = 0; i < 3; i++) {
                    CParticleObject::AddObject(POBJECT_PED_WATER_SPLASH, *pos, CVector(0.0f, 0.0f, 0.1f), 0.0f, 200, color, true);
                }
                RwRGBA rubberSmokeColor(255, 255, 255, 196);
                if (pedSpeed > 0.03f && CTimer::m_FrameCounter % 2 == 0 && pedSpeed > 0.13f) {
                    float particleSize = pedSpeed * 2.0f;

                    if (particleSize < 0.25f)
                        particleSize = 0.25f;

                    if (particleSize > 0.75f)
                        particleSize = 0.75f;

                    particleDir.z = CGeneral::GetRandomNumberInRange(0.01f, 0.03f);
                    CParticle::AddParticle(PARTICLE_CAR_SPLASH, particlePos, particleDir, NULL, 0.5f * particleSize, RwRGBA(0, 0, 0, 0), 0, 0, 0, 0);
                    CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, particlePos, particleDir, NULL, particleSize, rubberSmokeColor, 0, 0, 0, 0);

                    if (Jump) {
                        CVector particleDir(0.0f, 0.075f, 0.0f);
                        CParticle::AddParticle(PARTICLE_CAR_SPLASH, particlePos2, particleDir, NULL, 0.005f, RwRGBA(0, 0, 0, 0), 0, 0, 0, 0);
                        particleDir.x += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                        particleDir.y += CGeneral::GetRandomNumberInRange(-0.05f, 0.05f);
                        particleDir.z -= CGeneral::GetRandomNumberInRange(0.025f, 0.05f);
                        CParticle::AddParticle(PARTICLE_RUBBER_SMOKE, particlePos2, particleDir, NULL, 0.5f, rubberSmokeColor, 0, 0, 0, 0);
                    }
                }
            }


        };

        VehicleWaterSplashEvent += [](Fx_c* fx, CVector* pos) {
            if (WaterParticles) {
                // for (auto veh : CPools::ms_pVehiclePool) {
                   //  if (veh) {
                RwRGBA color{};
                color.red = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalRed + CTimeCycle::GetAmbientRed()) * 127.5f;
                color.green = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalBlue + CTimeCycle::GetAmbientBlue()) * 127.5f;
                color.blue = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalGreen + CTimeCycle::GetAmbientGreen()) * 127.5f;
                color.alpha = CGeneral::GetRandomNumberInRange(48.0f, 96.0f);
                CVector position = *pos;
                static RwRGBA black;
                if (position.z >= 0.0f) {
                    position.z += 0.5f;
                    CParticleObject::AddObject(POBJECT_SPLASHES_AROUND,
                        position, CVector(0.0f, 0.0f, 0.0f), 6.5f, 2500, black, true);
                }
                for (int i = 0; i < 4; i++) {
                    CVector p = *pos;
                    p.x += CGeneral::GetRandomNumberInRange(-2.5f, 2.5f);
                    p.y += CGeneral::GetRandomNumberInRange(-2.5f, 2.5f);
                    CParticle::AddParticle(PARTICLE_RAIN_SPLASH_BIGGROW,
                        p, CVector(0.0f, 0.0f, 0.0f),
                        NULL, 0.0f, black, 0, 0, 0, 0);
                }
                CParticleObject::AddObject(POBJECT_CAR_WATER_SPLASH, *pos, CVector(0.0f, 0.0f, 0.0f), 0.0f, 75, color, true);
                // for (int i = 0; i < 4; i++) {
                static RwRGBA splashCol = { 155, 155, 185, 196 };
                static RwRGBA smokeCol = { 255, 255, 255, 255 };
                //  auto automobile = (CAutomobile*)veh;
                 // if (automobile) {
                     // CVector pos = automobile->m_wheelColPoint[i].m_vecPoint + 0.3f * veh->GetUp() - veh->GetPosition();
                     // CVector vSpeed = veh->GetSpeed(pos);
                     // vSpeed.z = 0.0f;
                     // float fSpeed = ((Vec)vSpeed).MagnitudeSqr();
                    //  if (fSpeed > sq(0.05f)) {
                      //    fSpeed = Sqrt(fSpeed);

                float size = 0.75f;

                CParticle::AddParticle(PARTICLE_PED_SPLASH,
                    *pos, CVector(0.0f, 0.0f, 0.0f),
                    NULL, size, splashCol,
                    CGeneral::GetRandomNumberInRange(0.0f, 10.0f),
                    CGeneral::GetRandomNumberInRange(0.0f, 90.0f), 1, 0);

                CParticle::AddParticle(PARTICLE_RUBBER_SMOKE,
                    *pos, CVector(0.0f, 0.0f, 0.0f),
                    NULL, size, smokeCol, 0, 0, 0, 0);
                // }
            // }
       //  }
  //   }
// }
// }
            }
        };

        BulletSplashEvent += [](void* bullshit, char* name, RwV3d* point, RwMatrix* m, int flag) {
            debug("BulletSplashEvent");
            if (WaterParticles) {
                CParticleObject::AddObject(POBJECT_PED_WATER_SPLASH, (CVector&)*point, CVector(0.0f, 0.0f, 0.1f), 0.01f, 150, RwRGBA(0, 0, 0, 0), true);
                // CParticle::AddParticle(PARTICLE_SPLASH, (CVector&)*point, CVector(0.0f, 0.0f, 0.1f));
            }
        };
        /*TearGasSmokeEvent += [](void* UselessCocksucker, char* name, RwV3d* point, RwMatrix* m, int flag) {
             debug("FX: %s", name);
             float tearGasOffset = -0.0f;
             CParticle::AddParticle(PARTICLE_TEARGAS, (CVector&)*point, CVector(0.2f, tearGasOffset, 0.0f), 0, 0.0f, 0, 0, 0, 0);
             CParticle::AddParticle(PARTICLE_TEARGAS, (CVector&)*point, CVector(-0.2f, tearGasOffset, 0.0f), 0, 0.0f, 0, 0, 0, 0);
             CParticle::AddParticle(PARTICLE_TEARGAS, (CVector&)*point, CVector(tearGasOffset, tearGasOffset, 0.0f), 0, 0.0f, 0, 0, 0, 0);
         };*/

        AddWoodEvent += [](CVector& position, CVector& velocity, int a3, float a7) {
            if (WoodImpactParticles) {
                CParticle::AddParticle(PARTICLE_DEBRIS2, position, CVector(0.0f, 0.0f, 0.0f), NULL, 0.0f, RwRGBA(150, 75, 0, 255));
            }
        };

        /*ObjectExplodeParticles += [](FxManager_c* fx,
            FxSystemBP_c* fxSystem,
            RwV3d* point,
            RwMatrixTag* matrix,
            char flag) {
                CVector vecDir(
                    CGeneral::GetRandomNumberInRange(-0.35f, 0.35f),
                    CGeneral::GetRandomNumberInRange(-0.35f, 0.35f),
                    CGeneral::GetRandomNumberInRange(0.10f, 0.15f) + 0.0002f
                );
                static int8_t nFrameGen = 0;
                static RwRGBA secondParticleColors[4] = {
    RwRGBA(0xA0, 0x60, 0x60, 0xFF),
    RwRGBA(0x60, 0xA0, 0x60, 0xFF),
    RwRGBA(0x60, 0x60, 0xA0, 0xFF),
    RwRGBA(0xA0, 0xA0, 0xA0, 0xFF)
                };
                ++nFrameGen;
                vecDir *= 0.5f;
                RwRGBA& secondParticleColor = secondParticleColors[nFrameGen & 3];
                int32 nSecondRotationSpeed = CGeneral::GetRandomNumberInRange(-40, 40);
                log("Object exploded and created particles!");
                debug("Object exploded and created particles!");
                for (int32_t i = 0; i < 16; i++) {
                 //   CParticle::AddParticle(PARTICLE_DEBRIS, (CVector&)point, vecDir);
                    CParticle::AddParticle(PARTICLE_DEBRIS, (CVector&)point, vecDir, nil, 0.1f, secondParticleColor, nSecondRotationSpeed, 0, 1, 0);
                }
            };

        ObjectExplodeParticles2 += [](void* shit, int a2, RwMatrix* a3, RwMatrix* a4, char a5) {
            for (int32_t i = 0; i < 16; i++) {
                CParticle::AddParticle(PARTICLE_DEBRIS, (CVector&)a4->pos, CVector(0.0f, 0.0f, 0.0f));
            }
            };*/
    }

} _cParticleVC;

// Exported function to add particles through cleo
extern "C" void __declspec(dllexport) AddParticle(tParticleType type, float x, float y, float z, float xdir, float ydir, float zdir, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan)
{
    CVector vecPos(
        x,
        y,
        z
    );

    CVector vecDir(
        xdir,
        ydir,
        zdir
    );

    RwRGBA color(r, g, b, a);

    CParticle* particle = CParticle::AddParticle(type, vecPos, vecDir, NULL, fSize, color, 0, 0, 0, LifeSpan);
    CTheScripts::AddScriptEffectSystem((FxSystem_c*)particle);
    log("Added particle through cleo with type %d pos: x:%4.0f y:%4.0f z:%4.0f direction: x:%4.0f y:%4.0f z:%4.0f and size:%4.0f", type, x, y, z, xdir, ydir, zdir, fSize);
    debug("Added particle through cleo with type %d pos: x:%4.0f y:%4.0f z:%4.0f direction: x:%4.0f y:%4.0f z:%4.0f and size:%4.0f", type, x, y, z, xdir, ydir, zdir, fSize);
    return;
}
// Attach the particle to a vehicle
/*extern "C" void __declspec(dllexport) AddParticleAttachedToAVehicle(tParticleType type, float xoffset, float yoffset, float zoffset, float xdir, float ydir, float zdir, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan, int VehicleHandle)
{
    CVector vecPos(
        xoffset,
        yoffset,
        zoffset
    );

        CVehicle* vehicle = CPools::ms_pVehiclePool->GetAtRef(VehicleHandle);
        if (vehicle) {
            // Get the matrix
            CMatrix* vehicleMatrix = vehicle->GetMatrix();
            // Get the pos
            CVector vecBasePos = vehicleMatrix->GetPosition();

            // Apply offset (If any)
            if (!((Vec)vecPos).IsZero()) {
                vecBasePos += Multiply3x3(*vehicleMatrix, vecPos);
            }
            else {
                log("The offset is zero (Attach particle to a vehicle opcode)");
            }

            CVector vecDir(
                xdir,
                ydir,
                zdir
            );

            RwRGBA color(r, g, b, a);

            CParticle* particle = CParticle::AddParticle(type, vecBasePos, vecDir, NULL, fSize, color, 0, 0, 0, LifeSpan);
            CTheScripts::AddScriptEffectSystem((FxSystem_c*)particle);
        }
        else {
            debug("Attach particle to a vehicle opcode: Vehicle doesn't exist!");
            log("Attach particle to a vehicle opcode: Vehicle doesn't exist!");
        }
    log("Added particle through cleo that is attached to a vehicle with type %d pos(offset): x:%4.0f y:%4.0f z:%4.0f direction: x:%4.0f y:%4.0f z:%4.0f  size:%4.0f and vehicle handle: %d", type, xoffset, yoffset, zoffset, xdir, ydir, zdir, fSize, VehicleHandle);
    debug("Added particle through cleo that is attached to a vehicle with type %d pos(offset): x:%4.0f y:%4.0f z:%4.0f direction: x:%4.0f y:%4.0f z:%4.0f  size:%4.0f and vehicle handle: %d", type, xoffset, yoffset, zoffset, xdir, ydir, zdir, fSize, VehicleHandle);
    return;
}

// Attach the particle to a object
extern "C" void __declspec(dllexport) AddParticleAttachedToAObject(tParticleType type, float xoffset, float yoffset, float zoffset, float xdir, float ydir, float zdir, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan, int ObjectHandle)
{
    CVector vecPos(
        xoffset,
        yoffset,
        zoffset
    );

        CObject* pObject = CPools::ms_pObjectPool->GetAtRef(ObjectHandle);
        if (pObject) {
            // Get the matrix
            CMatrix* objectMatrix = pObject->GetMatrix();

            // Get the pos
            CVector vecBasePos = objectMatrix->GetPosition();

            // Apply offset (If any)
            if (!((Vec)vecPos).IsZero()) {
                vecBasePos += Multiply3x3(*objectMatrix, vecPos);
            }
            else {
                log("The offset is zero (Attach particle to a object opcode)");
            }

            CVector vecDir(
                xdir,
                ydir,
                zdir
            );

            RwRGBA color(r, g, b, a);

            CParticle* particle = CParticle::AddParticle(type, vecBasePos, vecDir, NULL, fSize, color, 0, 0, 0, LifeSpan);
            CTheScripts::AddScriptEffectSystem((FxSystem_c*)particle);
        }
        else {
            debug("Attach particle to a object opcode: object doesn't exist!");
            log("Attach particle to a object opcode: object doesn't exist!");
        }
    log("Added particle through cleo that is attached to a object with type %d pos: x:%4.0f y:%4.0f z:%4.0f direction: x:%4.0f y:%4.0f z:%4.0f size:%4.0f and object handle: %d", type, xoffset, yoffset, zoffset, xdir, ydir, zdir, fSize, ObjectHandle);
    debug("Added particle through cleo that is attached to a object with type %d pos: x:%4.0f y:%4.0f z:%4.0f direction: x:%4.0f y:%4.0f z:%4.0f size:%4.0f and object handle: %d", type, xoffset, yoffset, zoffset, xdir, ydir, zdir, fSize, ObjectHandle);
    return;
}*/

// Add particle object (moving particles)
extern "C" void __declspec(dllexport) AddParticleObject(uint8_t ParticleObjectType, float x, float y, float z, float strengthX, float strengthY, float strengthZ, float scale, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, uint32_t durationInMs, bool remove)
{
    CVector vecPos(
        x,
        y,
        z
    );

    CVector vecStrength(
        strengthX,
        strengthY,
        strengthZ
    );

    CVector pos = vecPos;
    if (pos.z <= -100.0f)
        pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
    float size = max(0.0f, scale);
    eParticleObjectType type = (eParticleObjectType)ParticleObjectType;
    RwRGBA color;
    // Apply colors on any particle object, not just POBJECT_SMOKE_TRAIL
   // if (type == POBJECT_SMOKE_TRAIL) {
    color.alpha = a;
    color.red = r;
    color.green = g;
    color.blue = b;
    /*}
    else {
        color.alpha = color.red = color.blue = color.green = 0;
    }*/
    CVector target = vecStrength;
    CParticleObject* PObject = /*gParticleObjectPool.New(Handle);*/ CParticleObject::AddObject(type, pos, target, size, durationInMs, color, remove);
    
    //gParticleObjectPool.New(Handle);
    /*PObject->m_Type = type;
    PObject->SetPosn(pos);
    PObject->m_vecTarget = target;
    PObject->m_fSize = size;
    PObject->m_nRemoveTimer = durationInMs;
    PObject->m_Color = color;
    PObject->m_bRemove = remove;*/
   // gParticleObjectPool.SetIdAt(Handle, Handle);
  ////  gParticleObjectPool.CreateAtRef(Handle);
   // if (gParticleObjectPool.IsObjectValid(PObject)) {
    //    PObject->AddObject(type, pos, target, size, durationInMs, color, remove);
    //    log("CLEO Opcode Creat particle object: Created particle object with ID:%d, pool ID: %d name:%s", Handle, gParticleObjectPool.GetAtRef(Handle), CParticleObject::ParticleObjectTypeToString(type));
   // }
    CTheScripts::AddScriptEffectSystem((FxSystem_c*)PObject);
    return;
}

// Remove particle objects in area
extern "C" void __declspec(dllexport) RemoveParticleObjectsInArea(float x1, float y1, float z1, float x2, float y2, float z2)
{
    CParticleObject* tmp = CParticleObject::pCloseListHead;
    while (tmp) {
        CParticleObject* next = tmp->m_pNext;
        if (tmp->IsWithinArea(x1, y1, z1, x2, y2, z2))
            tmp->RemoveObject();
        CTheScripts::RemoveScriptEffectSystem((int)tmp);
        tmp = next;
    }

    tmp = CParticleObject::pFarListHead;
    while (tmp) {
        CParticleObject* next = tmp->m_pNext;
        if (tmp->IsWithinArea(x1, y1, z1, x2, y2, z2))
            tmp->RemoveObject();
        CTheScripts::RemoveScriptEffectSystem((int)tmp);
        tmp = next;
    }
    return;
}

// Remove all particle objects
extern "C" void __declspec(dllexport) RemoveAllParticleObjects()
{
    CParticleObject::RemoveAllParticleObjects();
    return;
}

// Remove all expireable particle objects
extern "C" void __declspec(dllexport) RemoveAllExpireableParticleObjects()
{
    CParticleObject::RemoveAllExpireableParticleObjects();
    return;
}


// Add particles along line
extern "C" void __declspec(dllexport) AddParticlesAlongLine(tParticleType type, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float fPower, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan)
{
    CVector vecPosStart(
        x1,
        y1,
        z1
    );

    CVector vecPosEnd(
        x2,
        y2,
        z2
    );

    CVector vecDir(
        x3,
        y3,
        z3
    );

    RwRGBA color(r, g, b, a);

    CParticle::AddParticlesAlongLine(type, vecPosStart, vecPosEnd, vecDir, fPower, NULL, fSize, color, 0, 0, 0, LifeSpan);
    return;
}

// Remove particle system
extern "C" void __declspec(dllexport) RemoveParticle(tParticleType type)
{
    /*tParticleSystemData* psystem = &mod_ParticleSystemManager.m_aParticles[type];
    CParticle* tmp = psystem->m_pParticles;
    while (tmp) {
        CParticle* next = tmp->m_pNext;
            CParticle::RemoveParticle(tmp, nullptr, psystem);
        tmp = next;
    }*/
    CParticle::RemovePSystem(type);
    CTheScripts::RemoveScriptEffectSystem(type);
    return;
}

// Remove particle object
/*extern "C" void __declspec(dllexport) RemovePObject(int obj)
{
    CParticleObject* object = gParticleObjectPool.GetAtRef(obj);
  //  if (object) {
    log("PObject handle to remove: %d name: %s", gParticleObjectPool.GetAtRef(obj), CParticleObject::ParticleObjectTypeToString(object->m_Type));
      //  object->RemoveObject();
    object->RemoveObject();
        log("Removed!");
  //  }
    return;
}*/

// Add jet explosion
extern "C" void __declspec(dllexport) AddJetExplosion(float x, float y, float z, float fPower, float fSize)
{
    CVector vecPos(
        x,
        y,
        z
    );

    CParticle::AddJetExplosion(vecPos, fPower, fSize);
    return;
}

// Add heli dust around object
extern "C" void __declspec(dllexport) AddHeliDust(int ObjectHandle, float DustRadius, int Density)
{
        CObject* pHeli = CPools::ms_pObjectPool->GetAtRef(ObjectHandle);
        if (pHeli) {
            bool found = false;
            float waterLevel = -1000.0f;
            CVector pos = pHeli->GetPosition();
            float radius = DustRadius;
            float ground = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z, &found, nullptr);
            if (!CWaterLevel::GetWaterLevelNoWaves(pos.x, pos.y, pos.z, &waterLevel, NULL, NULL)) {
                waterLevel = 0.0f;
            }
            if (waterLevel > ground) {
                ground = waterLevel;
            }
            if (Density > 8) {
                Density = 8;
            }
            HeliDustGenerate(pHeli, (pos.z - ground - 1.0f - radius) * 0.3 + radius, ground, Density);
        }
        else {
            debug("Add heli dust opcode: Object doesn't exist!");
            log("Add heli dust opcode: Object doesn't exist!");
        }
    return;
}

// Add big gun flash effect
extern "C" void __declspec(dllexport) AddBigGunFlash(float x1, float y1, float z1, float x2, float y2, float z2)
{
    CVector vecPosStart(
        x1,
        y1,
        z1
    );

    CVector vecPosEnd(
        x2,
        y2,
        z2
    );

    AddGunFlashBigGuns(vecPosStart, vecPosEnd);
    return;
}

// Initializes the boat foam animation
extern "C" void __declspec(dllexport) StartBoatFoamAnimation()
{
    CSpecialParticleStuff::StartBoatFoamAnimation();
    return;
}

// Update the boat foam animation
extern "C" void __declspec(dllexport) UpdateBoatFoamAnimation(int ObjectHandle)
{
        CObject* pObject = CPools::ms_pObjectPool->GetAtRef(ObjectHandle);
        if (pObject) {
            CSpecialParticleStuff::UpdateBoatFoamAnimation(pObject->GetMatrix());
        }
        else {
            debug("Update boat foam animation opcode: Object doesn't exist!");
            log("Update boat foam animation opcode: Object doesn't exist!");
        }
    return;
}

// Add yardie door smoke to an object
extern "C" void __declspec(dllexport) AddYardieDoorSmoke(CVector & vecPos, int ObjectHandle)
{
        CObject* pObject = CPools::ms_pObjectPool->GetAtRef(ObjectHandle);
        if (pObject) {
            CParticle::AddYardieDoorSmoke(vecPos, *pObject->GetMatrix());
        }
        else {
            debug("Add yardie door smoke opcode: Object doesn't exist!");
            log("Add yardie door smoke opcode: Object doesn't exist!");
        }
    return;
}

// Debug purposes
extern "C" void __declspec(dllexport) LogExport(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    Log(msg, args);
    va_end(args);
}