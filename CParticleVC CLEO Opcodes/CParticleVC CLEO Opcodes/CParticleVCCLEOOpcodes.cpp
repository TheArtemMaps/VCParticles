#include "plugin.h"
#include "CLEO.h"
#include "CKeyGen.h"
#include "../../CParticleVC/Utility.h"
#include <csignal>
#include "Exported functions.h"
#include "ParticleType.h"
#include <ParticleObject.h>
#include <CRunningScript.h>
#include <CDebug.h>
using namespace CLEO;
void ErrorWindowWithName(const char* WindowName, const char* msg, ...)
{
    va_list va;
    va_start(va, msg);
    vsprintf_s(re3_buff, re3_buffsize, msg, va);
    va_end(va);

    ::MessageBoxA(NULL, re3_buff, WindowName,
        MB_OK | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);

    raise(SIGABRT);
    _exit(3);
}

using namespace plugin;
#define CREATE_SINGLE_PARTICLE 0x1337
#define ADD_PARTICLES_ALONG_LINE 0x1338
#define ADD_JET_EXPLOSION 0x1335
#define REMOVE_PARTICLE_SYSTEM 0x1333
#define ADD_BIG_GUN_FLASH 0x1341
#define ADD_MOVING_PARTICLE_EFFECT 0x1342
#define REMOVE_PARTICLE_OBJECTS_IN_AREA 0x1343
#define REMOVE_ALL_EXPIRABLE_PARTICLE_OBJECTS 0x1344
#define REMOVE_ALL_PARTICLE_OBJECTS 0x1345
#define CREATE_DUST_EFFECT_FOR_CUTSCENE_HELI 0x1346
#define UPDATE_BOAT_FOAM_ANIMATION 0x1347
#define START_BOAT_FOAM_ANIMATION 0x1348
#define ADD_YARDIE_DOOR_SMOKE 0x1349
#define REMOVE_PARTICLE_OBJECT 0x1350
//#define ADD_PARTICLE_ATTACHED_TO_A_VEHICLE 0x1350
//#define ADD_PARTICLE_ATTACHED_TO_A_OBJECT 0x1351
// Adding particles with a string
tParticleType ParticleTypeFromString(const char* typeStr) {
    if (strcmp(typeStr, "SPARK") == 0) return PARTICLE_SPARK;
    if (strcmp(typeStr, "SPARK_SMALL") == 0) return PARTICLE_SPARK_SMALL;
    if (strcmp(typeStr, "WATER_SPARK") == 0) return PARTICLE_WATER_SPARK;
    if (strcmp(typeStr, "WHEEL_DIRT") == 0) return PARTICLE_WHEEL_DIRT;
    if (strcmp(typeStr, "SAND") == 0) return PARTICLE_SAND;
    if (strcmp(typeStr, "WHEEL_WATER") == 0) return PARTICLE_WHEEL_WATER;
    if (strcmp(typeStr, "BLOOD") == 0) return PARTICLE_BLOOD;
    if (strcmp(typeStr, "BLOOD_SMALL") == 0) return PARTICLE_BLOOD_SMALL;
    if (strcmp(typeStr, "BLOOD_SPURT") == 0) return PARTICLE_BLOOD_SPURT;
    if (strcmp(typeStr, "DEBRIS") == 0) return PARTICLE_DEBRIS;
    if (strcmp(typeStr, "DEBRIS2") == 0) return PARTICLE_DEBRIS2;
    if (strcmp(typeStr, "FLYERS") == 0) return PARTICLE_FLYERS;
    if (strcmp(typeStr, "WATER") == 0) return PARTICLE_WATER;
    if (strcmp(typeStr, "FLAME") == 0) return PARTICLE_FLAME;
    if (strcmp(typeStr, "FIREBALL") == 0) return PARTICLE_FIREBALL;
    if (strcmp(typeStr, "GUNFLASH") == 0) return PARTICLE_GUNFLASH;
    if (strcmp(typeStr, "GUNFLASH_NOANIM") == 0) return PARTICLE_GUNFLASH_NOANIM;
    if (strcmp(typeStr, "GUNSMOKE") == 0) return PARTICLE_GUNSMOKE;
    if (strcmp(typeStr, "GUNSMOKE2") == 0) return PARTICLE_GUNSMOKE2;
    if (strcmp(typeStr, "CIGARETTE_SMOKE") == 0) return PARTICLE_CIGARETTE_SMOKE;
    if (strcmp(typeStr, "SMOKE") == 0) return PARTICLE_SMOKE;
    if (strcmp(typeStr, "SMOKE_SLOWMOTION") == 0) return PARTICLE_SMOKE_SLOWMOTION;
    if (strcmp(typeStr, "DRY_ICE") == 0) return PARTICLE_DRY_ICE;
    if (strcmp(typeStr, "TEARGAS") == 0) return PARTICLE_TEARGAS;
    if (strcmp(typeStr, "GARAGEPAINT_SPRAY") == 0) return PARTICLE_GARAGEPAINT_SPRAY;
    if (strcmp(typeStr, "SHARD") == 0) return PARTICLE_SHARD;
    if (strcmp(typeStr, "SPLASH") == 0) return PARTICLE_SPLASH;
    if (strcmp(typeStr, "CARFLAME") == 0) return PARTICLE_CARFLAME;
    if (strcmp(typeStr, "STEAM") == 0) return PARTICLE_STEAM;
    if (strcmp(typeStr, "STEAM2") == 0) return PARTICLE_STEAM2;
    if (strcmp(typeStr, "STEAM_NY") == 0) return PARTICLE_STEAM_NY;
    if (strcmp(typeStr, "STEAM_NY_SLOWMOTION") == 0) return PARTICLE_STEAM_NY_SLOWMOTION;
    if (strcmp(typeStr, "GROUND_STEAM") == 0) return PARTICLE_GROUND_STEAM;
    if (strcmp(typeStr, "ENGINE_STEAM") == 0) return PARTICLE_ENGINE_STEAM;
    if (strcmp(typeStr, "RAINDROP") == 0) return PARTICLE_RAINDROP;
    if (strcmp(typeStr, "RAINDROP_SMALL") == 0) return PARTICLE_RAINDROP_SMALL;
    if (strcmp(typeStr, "RAIN_SPLASH") == 0) return PARTICLE_RAIN_SPLASH;
    if (strcmp(typeStr, "RAIN_SPLASH_BIGGROW") == 0) return PARTICLE_RAIN_SPLASH_BIGGROW;
    if (strcmp(typeStr, "RAIN_SPLASHUP") == 0) return PARTICLE_RAIN_SPLASHUP;
    if (strcmp(typeStr, "WATERSPRAY") == 0) return PARTICLE_WATERSPRAY;
    if (strcmp(typeStr, "WATERDROP") == 0) return PARTICLE_WATERDROP;
    if (strcmp(typeStr, "BLOODDROP") == 0) return PARTICLE_BLOODDROP;
    if (strcmp(typeStr, "EXPLOSION_MEDIUM") == 0) return PARTICLE_EXPLOSION_MEDIUM;
    if (strcmp(typeStr, "EXPLOSION_LARGE") == 0) return PARTICLE_EXPLOSION_LARGE;
    if (strcmp(typeStr, "EXPLOSION_MFAST") == 0) return PARTICLE_EXPLOSION_MFAST;
    if (strcmp(typeStr, "EXPLOSION_LFAST") == 0) return PARTICLE_EXPLOSION_LFAST;
    if (strcmp(typeStr, "CAR_SPLASH") == 0) return PARTICLE_CAR_SPLASH;
    if (strcmp(typeStr, "BOAT_SPLASH") == 0) return PARTICLE_BOAT_SPLASH;
    if (strcmp(typeStr, "BOAT_THRUSTJET") == 0) return PARTICLE_BOAT_THRUSTJET;
    if (strcmp(typeStr, "WATER_HYDRANT") == 0) return PARTICLE_WATER_HYDRANT;
    if (strcmp(typeStr, "WATER_CANNON") == 0) return PARTICLE_WATER_CANNON;
    if (strcmp(typeStr, "EXTINGUISH_STEAM") == 0) return PARTICLE_EXTINGUISH_STEAM;
    if (strcmp(typeStr, "PED_SPLASH") == 0) return PARTICLE_PED_SPLASH;
    if (strcmp(typeStr, "PEDFOOT_DUST") == 0) return PARTICLE_PEDFOOT_DUST;
    if (strcmp(typeStr, "CAR_DUST") == 0) return PARTICLE_CAR_DUST;
    if (strcmp(typeStr, "HELI_DUST") == 0) return PARTICLE_HELI_DUST;
    if (strcmp(typeStr, "HELI_ATTACK") == 0) return PARTICLE_HELI_ATTACK;
    if (strcmp(typeStr, "ENGINE_SMOKE") == 0) return PARTICLE_ENGINE_SMOKE;
    if (strcmp(typeStr, "ENGINE_SMOKE2") == 0) return PARTICLE_ENGINE_SMOKE2;
    if (strcmp(typeStr, "CARFLAME_SMOKE") == 0) return PARTICLE_CARFLAME_SMOKE;
    if (strcmp(typeStr, "FIREBALL_SMOKE") == 0) return PARTICLE_FIREBALL_SMOKE;
    if (strcmp(typeStr, "PAINT_SMOKE") == 0) return PARTICLE_PAINT_SMOKE;
    if (strcmp(typeStr, "TREE_LEAVES") == 0) return PARTICLE_TREE_LEAVES;
    if (strcmp(typeStr, "CARCOLLISION_DUST") == 0) return PARTICLE_CARCOLLISION_DUST;
    if (strcmp(typeStr, "CAR_DEBRIS") == 0) return PARTICLE_CAR_DEBRIS;
    if (strcmp(typeStr, "BIRD_DEBRIS") == 0) return PARTICLE_BIRD_DEBRIS;
    if (strcmp(typeStr, "HELI_DEBRIS") == 0) return PARTICLE_HELI_DEBRIS;
    if (strcmp(typeStr, "EXHAUST_FUMES") == 0) return PARTICLE_EXHAUST_FUMES;
    if (strcmp(typeStr, "RUBBER_SMOKE") == 0) return PARTICLE_RUBBER_SMOKE;
    if (strcmp(typeStr, "BURNINGRUBBER_SMOKE") == 0) return PARTICLE_BURNINGRUBBER_SMOKE;
    if (strcmp(typeStr, "BULLETHIT_SMOKE") == 0) return PARTICLE_BULLETHIT_SMOKE;
    if (strcmp(typeStr, "GUNSHELL_FIRST") == 0) return PARTICLE_GUNSHELL_FIRST;
    if (strcmp(typeStr, "GUNSHELL") == 0) return PARTICLE_GUNSHELL;
    if (strcmp(typeStr, "GUNSHELL_BUMP1") == 0) return PARTICLE_GUNSHELL_BUMP1;
    if (strcmp(typeStr, "GUNSHELL_BUMP2") == 0) return PARTICLE_GUNSHELL_BUMP2;
    if (strcmp(typeStr, "ROCKET_SMOKE") == 0) return PARTICLE_ROCKET_SMOKE;
    if (strcmp(typeStr, "TEST") == 0) return PARTICLE_TEST;
    if (strcmp(typeStr, "BIRD_FRONT") == 0) return PARTICLE_BIRD_FRONT;
    if (strcmp(typeStr, "SHIP_SIDE") == 0) return PARTICLE_SHIP_SIDE;
    if (strcmp(typeStr, "BEASTIE") == 0) return PARTICLE_BEASTIE;
    if (strcmp(typeStr, "RAINDROP_2D") == 0) return PARTICLE_RAINDROP_2D;
    if (strcmp(typeStr, "HEATHAZE") == 0) return PARTICLE_HEATHAZE;
    if (strcmp(typeStr, "HEATHAZE_IN_DIST") == 0) return PARTICLE_HEATHAZE_IN_DIST;
    if (strcmp(typeStr, "SNOW") == 0) return PARTICLE_SNOW;
    if (strcmp(typeStr, "BOAT_WAKE") == 0) return PARTICLE_BOAT_WAKE;
    if (strcmp(typeStr, "FERRY_CHIM_SMOKE") == 0) return PARTICLE_FERRY_CHIM_SMOKE;
    if (strcmp(typeStr, "MULTIPLAYER_HIT") == 0) return PARTICLE_MULTIPLAYER_HIT;
    if (strcmp(typeStr, "HYDRANT_STEAM") == 0) return PARTICLE_HYDRANT_STEAM;
    if (strcmp(typeStr, "GUNFLASH_VCS") == 0) return PARTICLE_GUNFLASH_VCS;
    if (strcmp(typeStr, "GUNFLASH_NOANIM_VCS") == 0) return PARTICLE_GUNFLASH_NOANIM_VCS;
    if (strcmp(typeStr, "EXPLOSION_MASSIVE") == 0) return PARTICLE_EXPLOSION_MASSIVE;
    if (strcmp(typeStr, "EXPLOSION_PARTICLEEX") == 0) return PARTICLE_EXPLOSION_PARTICLEEX;
    if (strcmp(typeStr, "CARFLAME_MOVING") == 0) return PARTICLE_CARFLAME_MOVING;
    if (strcmp(typeStr, "FIREBALL_PARTICLEEX") == 0) return PARTICLE_FIREBALL_PARTICLEEX;
    return (tParticleType)-1; // Return -1 for unknown
}


// Adding particle objects with a string
eParticleObjectType ParticleObjectTypeFromString(const char* typeStr) {
    if (strcmp(typeStr, "PAVEMENT_STEAM") == 0) return POBJECT_PAVEMENT_STEAM;
    if (strcmp(typeStr, "PAVEMENT_STEAM_SLOWMOTION") == 0) return POBJECT_PAVEMENT_STEAM_SLOWMOTION;
    if (strcmp(typeStr, "WALL_STEAM") == 0) return POBJECT_WALL_STEAM;
    if (strcmp(typeStr, "WALL_STEAM_SLOWMOTION") == 0) return POBJECT_WALL_STEAM_SLOWMOTION;
    if (strcmp(typeStr, "DARK_SMOKE") == 0) return POBJECT_DARK_SMOKE;
    if (strcmp(typeStr, "FIRE_HYDRANT") == 0) return POBJECT_FIRE_HYDRANT;
    if (strcmp(typeStr, "CAR_WATER_SPLASH") == 0) return POBJECT_CAR_WATER_SPLASH;
    if (strcmp(typeStr, "PED_WATER_SPLASH") == 0) return POBJECT_PED_WATER_SPLASH;
    if (strcmp(typeStr, "SPLASHES_AROUND") == 0) return POBJECT_SPLASHES_AROUND;
    if (strcmp(typeStr, "SMALL_FIRE") == 0) return POBJECT_SMALL_FIRE;
    if (strcmp(typeStr, "BIG_FIRE") == 0) return POBJECT_BIG_FIRE;
    if (strcmp(typeStr, "DRY_ICE") == 0) return POBJECT_DRY_ICE;
    if (strcmp(typeStr, "DRY_ICE_SLOWMOTION") == 0) return POBJECT_DRY_ICE_SLOWMOTION;
    if (strcmp(typeStr, "WATER_FOUNTAIN_VERT") == 0) return POBJECT_WATER_FOUNTAIN_VERT;
    if (strcmp(typeStr, "WATER_FOUNTAIN_HORIZ") == 0) return POBJECT_WATER_FOUNTAIN_HORIZ;
    if (strcmp(typeStr, "FIRE_TRAIL") == 0) return POBJECT_FIRE_TRAIL;
    if (strcmp(typeStr, "SMOKE_TRAIL") == 0) return POBJECT_SMOKE_TRAIL;
    if (strcmp(typeStr, "FIREBALL_AND_SMOKE") == 0) return POBJECT_FIREBALL_AND_SMOKE;
    if (strcmp(typeStr, "ROCKET_TRAIL") == 0) return POBJECT_ROCKET_TRAIL;
    if (strcmp(typeStr, "EXPLOSION_ONCE") == 0) return POBJECT_EXPLOSION_ONCE;
    if (strcmp(typeStr, "CATALINAS_GUNFLASH") == 0) return POBJECT_CATALINAS_GUNFLASH;
    if (strcmp(typeStr, "CATALINAS_SHOTGUNFLASH") == 0) return POBJECT_CATALINAS_SHOTGUNFLASH;
    return POBJECT_UNKNOWN; // Return POBJECT_UNKNOWN for unknown
}

void AddParticleOpcode(CScriptThread* thread) {
    char buf[500];
    auto typeStr = CLEO_ReadStringOpcodeParam(thread, buf, sizeof(buf));
    auto typeStr2 = ParticleTypeFromString(typeStr);
    if (typeStr2 == (tParticleType)-1) {
        LogExport("AddParticleOpcode Error: Unknown particle type input, not creating particle...");
    }
    float x = CLEO_GetFloatOpcodeParam(thread);
    float y = CLEO_GetFloatOpcodeParam(thread);
    float z = CLEO_GetFloatOpcodeParam(thread);
    float xdir = CLEO_GetFloatOpcodeParam(thread);
    float ydir = CLEO_GetFloatOpcodeParam(thread);
    float zdir = CLEO_GetFloatOpcodeParam(thread);
    float size = CLEO_GetFloatOpcodeParam(thread);
    RwUInt8 r = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 g = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 b = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 a = CLEO_GetIntOpcodeParam(thread);
    int32_t LifeSpan = CLEO_GetIntOpcodeParam(thread);
    if (typeStr2 != (tParticleType)-1) {
        AddParticle(typeStr2, x, y, z, xdir, ydir, zdir, size, r, g, b, a, LifeSpan);
    }
}

/*void AddParticleAttachedToAObjectOpcode(CScriptThread* thread) {
    char buf[500];
    auto typeStr = CLEO_ReadStringOpcodeParam(thread, buf, sizeof(buf));
    auto typeStr2 = ParticleTypeFromString(typeStr);
    float xoffset = CLEO_GetFloatOpcodeParam(thread);
    float yoffset = CLEO_GetFloatOpcodeParam(thread);
    float zoffset = CLEO_GetFloatOpcodeParam(thread);
    float xdir = CLEO_GetFloatOpcodeParam(thread);
    float ydir = CLEO_GetFloatOpcodeParam(thread);
    float zdir = CLEO_GetFloatOpcodeParam(thread);
    float size = CLEO_GetFloatOpcodeParam(thread);
    RwUInt8 r = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 g = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 b = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 a = CLEO_GetIntOpcodeParam(thread);
    int32_t LifeSpan = CLEO_GetIntOpcodeParam(thread);
    int ObjectHandle = CLEO_GetIntOpcodeParam(thread);
    AddParticleAttachedToAObject(typeStr2, xoffset, yoffset, zoffset, xdir, ydir, zdir, size, r, g, b, a, LifeSpan, ObjectHandle);
}

void AddParticleAttachedToAVehicleOpcode(CScriptThread* thread) {
    char buf[500];
    auto typeStr = CLEO_ReadStringOpcodeParam(thread, buf, sizeof(buf));
    auto typeStr2 = ParticleTypeFromString(typeStr);
    float xoffset = CLEO_GetFloatOpcodeParam(thread);
    float yoffset = CLEO_GetFloatOpcodeParam(thread);
    float zoffset = CLEO_GetFloatOpcodeParam(thread);
    float xdir = CLEO_GetFloatOpcodeParam(thread);
    float ydir = CLEO_GetFloatOpcodeParam(thread);
    float zdir = CLEO_GetFloatOpcodeParam(thread);
    float size = CLEO_GetFloatOpcodeParam(thread);
    RwUInt8 r = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 g = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 b = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 a = CLEO_GetIntOpcodeParam(thread);
    int32_t LifeSpan = CLEO_GetIntOpcodeParam(thread);
    int VehicleHandle = CLEO_GetIntOpcodeParam(thread);
    AddParticleAttachedToAVehicle(typeStr2, xoffset, yoffset, zoffset, xdir, ydir, zdir, size, r, g, b, a, LifeSpan, VehicleHandle);
}*/

void AddYardieDoorSmokeOpcode(CScriptThread* thread) {
    float x = CLEO_GetFloatOpcodeParam(thread);
    float y = CLEO_GetFloatOpcodeParam(thread);
    float z = CLEO_GetFloatOpcodeParam(thread);
    CVector vecPos(x, y, z);
    int ObjectHandle = CLEO_GetIntOpcodeParam(thread);
    AddYardieDoorSmoke(vecPos, ObjectHandle);
}

void AddHeliDustOpcode(CScriptThread* thread) {
    int ObjectHandle = CLEO_GetIntOpcodeParam(thread);
    float DustRadius = CLEO_GetFloatOpcodeParam(thread);
    int Density = CLEO_GetIntOpcodeParam(thread);
    AddHeliDust(ObjectHandle, DustRadius, Density);
}

void AddParticleObjectOpcode(CScriptThread* thread) {
    char buf[500];
    auto typeStr = CLEO_ReadStringOpcodeParam(thread, buf, sizeof(buf));
    auto typeStr2 = ParticleObjectTypeFromString(typeStr);
    if (typeStr2 == POBJECT_UNKNOWN) {
        LogExport("AddParticleObjectOpcode Error: Unknown particle object type input, not creating PObject...");
    }
    float x = CLEO_GetFloatOpcodeParam(thread);
    float y = CLEO_GetFloatOpcodeParam(thread);
    float z = CLEO_GetFloatOpcodeParam(thread);
    float strengthX = CLEO_GetFloatOpcodeParam(thread);
    float strengthY = CLEO_GetFloatOpcodeParam(thread);
    float strengthZ = CLEO_GetFloatOpcodeParam(thread);
    float scale = CLEO_GetFloatOpcodeParam(thread);
    RwUInt8 r = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 g = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 b = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 a = CLEO_GetIntOpcodeParam(thread);
    int durationInMs = CLEO_GetIntOpcodeParam(thread);
    bool remove = CLEO_GetIntOpcodeParam(thread);
    if (typeStr2 != POBJECT_UNKNOWN) {
        AddParticleObject(typeStr2, x, y, z, strengthX, strengthY, strengthZ, scale, r, g, b, a, durationInMs, remove);
    }
}

void RemoveParticleObjectsInAreaOpcode(CScriptThread* thread) {
    float x1 = CLEO_GetFloatOpcodeParam(thread);
    float y1 = CLEO_GetFloatOpcodeParam(thread);
    float z1 = CLEO_GetFloatOpcodeParam(thread);
    float x2 = CLEO_GetFloatOpcodeParam(thread);
    float y2 = CLEO_GetFloatOpcodeParam(thread);
    float z2 = CLEO_GetFloatOpcodeParam(thread);
    RemoveParticleObjectsInArea(x1, y1, z1, x2, y2, z2);
}

void StartBoatFoamAnimationOpcode(CScriptThread* thread) {
    StartBoatFoamAnimationEx();
}

void UpdateBoatFoamAnimationOpcode(CScriptThread* thread) {
    int ObjectHandle = CLEO_GetIntOpcodeParam(thread);
    UpdateBoatFoamAnimationEx(ObjectHandle);
}

void AddParticlesAlongLineOpcode(CScriptThread* thread) {
    char buf[500];
    auto typeStr = CLEO_ReadStringOpcodeParam(thread, buf, sizeof(buf));
    auto typeStr2 = ParticleTypeFromString(typeStr);
    float x1 = CLEO_GetFloatOpcodeParam(thread);
    float y1 = CLEO_GetFloatOpcodeParam(thread);
    float z1 = CLEO_GetFloatOpcodeParam(thread);
    float x2 = CLEO_GetFloatOpcodeParam(thread);
    float y2 = CLEO_GetFloatOpcodeParam(thread);
    float z2 = CLEO_GetFloatOpcodeParam(thread);
    float x3 = CLEO_GetFloatOpcodeParam(thread);
    float y3 = CLEO_GetFloatOpcodeParam(thread);
    float z3 = CLEO_GetFloatOpcodeParam(thread);
    float power = CLEO_GetFloatOpcodeParam(thread);
    float size = CLEO_GetFloatOpcodeParam(thread);
    RwUInt8 r = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 g = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 b = CLEO_GetIntOpcodeParam(thread);
    RwUInt8 a = CLEO_GetIntOpcodeParam(thread);
    int32_t LifeSpan = CLEO_GetIntOpcodeParam(thread);
    AddParticlesAlongLine(typeStr2, x1, y1, z1, x2, y2, z2, x3, y3, z3, power, size, r, g, b, a, LifeSpan);
}

void RemovePSystemOpcode(CScriptThread* thread) {
    char buf[500];
    auto typeStr = CLEO_ReadStringOpcodeParam(thread, buf, sizeof(buf));
    auto typeStr2 = ParticleTypeFromString(typeStr);
    RemovePSystem(typeStr2);
}

void RemovePObjectOpcode(CScriptThread* thread) {
    int PObjectHandle = CLEO_GetIntOpcodeParam(thread);
    RemovePObject(PObjectHandle);
}

void AddJetExplosionOpcode(CScriptThread* thread) {
    float x = CLEO_GetFloatOpcodeParam(thread);
    float y = CLEO_GetFloatOpcodeParam(thread);
    float z = CLEO_GetFloatOpcodeParam(thread);
    float power = CLEO_GetFloatOpcodeParam(thread);
    float size = CLEO_GetFloatOpcodeParam(thread);
    AddJetExplosion(x, y, z, power, size);
}

void AddBigGunFlashOpcode(CScriptThread* thread) {
    float x1 = CLEO_GetFloatOpcodeParam(thread);
    float y1 = CLEO_GetFloatOpcodeParam(thread);
    float z1 = CLEO_GetFloatOpcodeParam(thread);
    float x2 = CLEO_GetFloatOpcodeParam(thread);
    float y2 = CLEO_GetFloatOpcodeParam(thread);
    float z2 = CLEO_GetFloatOpcodeParam(thread);
    AddBigGunFlash(x1, y1, z1, x2, y2, z2);
}

void RemoveAllParticleObjectsOpcode(CScriptThread* thread) {
    RemoveAllParticleObjects();
}

void RemoveAllExpireableParticleObjectsOpcode(CScriptThread* thread) {
    RemoveAllExpireableParticleObjects();
}

class CParticleVCCLEOOpcodes {
public:
    static OpcodeResult WINAPI AddParticleMain(CScriptThread* thread) {
        AddParticleOpcode(thread);
        return OR_CONTINUE;
    }
    static OpcodeResult WINAPI AddParticlesAlongLineMain(CScriptThread* thread) {
         AddParticlesAlongLineOpcode(thread);
        return OR_CONTINUE;
    }
    static OpcodeResult WINAPI RemovePSystemMain(CScriptThread* thread) {
         RemovePSystemOpcode(thread);
        return OR_CONTINUE;
    }
    static OpcodeResult WINAPI AddJetExplosionMain(CScriptThread* thread) {
        AddJetExplosionOpcode(thread);
        return OR_CONTINUE;
    }
    static OpcodeResult WINAPI AddBigGunFlashMain(CScriptThread* thread) {
        AddBigGunFlashOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI AddParticleObjectMain(CScriptThread* thread) {
        AddParticleObjectOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI RemoveParticleObjectsInAreaMain(CScriptThread* thread) {
        RemoveParticleObjectsInAreaOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI RemoveAllParticleObjectsMain(CScriptThread* thread) {
        RemoveAllParticleObjectsOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI RemoveAllExpirableParticleObjectsMain(CScriptThread* thread) {
        RemoveAllExpireableParticleObjectsOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI AddHeliDustMain(CScriptThread* thread) {
        AddHeliDustOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI StartBoatFoamAnimationMain(CScriptThread* thread) {
        StartBoatFoamAnimationOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI UpdateBoatFoamAnimationMain(CScriptThread* thread) {
        UpdateBoatFoamAnimationOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI AddYardieDoorSmokeMain(CScriptThread* thread) {
        AddYardieDoorSmokeOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI RemovePObjectMain(CScriptThread* thread) {
        RemovePObjectOpcode(thread);
        return OR_CONTINUE;
    }

    /*static OpcodeResult WINAPI AddParticleAttachedToAObjectMain(CScriptThread* thread) {
        AddParticleAttachedToAObjectOpcode(thread);
        return OR_CONTINUE;
    }

    static OpcodeResult WINAPI AddParticleAttachedToAVehicleMain(CScriptThread* thread) {
        AddParticleAttachedToAVehicleOpcode(thread);
        return OR_CONTINUE;
    }*/

    CParticleVCCLEOOpcodes() {
        Events::initRwEvent += []() {
            if (!IsPluginInstalled("CParticleVC.SA.asi")) {
                ErrorWindowWithName(PLUGIN_FILENAME, "This CLEO plugin will only work with the VCParticles mod installed!");
            }
        };
        CLEO_RegisterOpcode(CREATE_SINGLE_PARTICLE, AddParticleMain); // 1337=13, add_particle %1d% x %2d% y %3d% z %4d% xdir %5d% ydir %6d% zdir %7d% size %8d% r %9d% g %10d% b %11d% a %12d% lifespan %13d%
        CLEO_RegisterOpcode(ADD_PARTICLES_ALONG_LINE, AddParticlesAlongLineMain); // 1338=17, add_particles_along_line %1d% x1 %2d% y1 %3d% z1 %4d% x2 %5d% y2 %6d% z2 %7d% x3 %8d% y3 %9d% z3 %10d% power %11d% size %12d% r %13d% g %14d% b %15d% a %16d% lifespan %17d%
        CLEO_RegisterOpcode(ADD_JET_EXPLOSION, AddJetExplosionMain); // 1335=5, add_jet_explosion x %1d% y %2d% z %3d% power %4d% size %5d%
        CLEO_RegisterOpcode(REMOVE_PARTICLE_SYSTEM, RemovePSystemMain); // 1333=1, remove_particle_system %1d%
        CLEO_RegisterOpcode(ADD_BIG_GUN_FLASH, AddBigGunFlashMain); // 1341=6, add_big_gun_flash xfrom %1d% yfrom %2d% zfrom %3d% xto %4d% yto %5d% zto %6d%
        CLEO_RegisterOpcode(ADD_MOVING_PARTICLE_EFFECT, AddParticleObjectMain); // 1342=14, add_moving_particle_effect %1d% x %2d% y %3d% z %4d% xdir %5d% ydir %6d% zdir %7d% scale %8d% r %9d% g %10d% b %11d% a %12d% duration %13d% remove %14d%
        CLEO_RegisterOpcode(REMOVE_PARTICLE_OBJECTS_IN_AREA, RemoveParticleObjectsInAreaMain); // 1343=6, remove_particle_objects_in_area x1 %1d% y1 %2d% z1 %3d% x2 %4d% y2 %5d% z1 %6d%
        CLEO_RegisterOpcode(REMOVE_ALL_EXPIRABLE_PARTICLE_OBJECTS, RemoveAllExpirableParticleObjectsMain); // 1344=0, remove_all_expirable_particle_objects
        CLEO_RegisterOpcode(REMOVE_ALL_PARTICLE_OBJECTS, RemoveAllParticleObjectsMain); // 1345=0, remove_all_particle_objects
        CLEO_RegisterOpcode(CREATE_DUST_EFFECT_FOR_CUTSCENE_HELI, AddHeliDustMain); // 1346=3, stir_ground_around_object %1d% radius %2d% density %3d%
        CLEO_RegisterOpcode(UPDATE_BOAT_FOAM_ANIMATION, UpdateBoatFoamAnimationMain); // 1347=1, update_boat %1d% foam_animation
        CLEO_RegisterOpcode(START_BOAT_FOAM_ANIMATION, StartBoatFoamAnimationMain); // 1348=0, start_boat_foam_animation
        CLEO_RegisterOpcode(ADD_YARDIE_DOOR_SMOKE, AddYardieDoorSmokeMain); // 1349=4, add_yardie_door_smoke_at x %1d% y %2d% z %3d% attach_to_object %4d%
      //  CLEO_RegisterOpcode(REMOVE_PARTICLE_OBJECT, RemovePObjectMain); // 1350=1, remove_particle_object %1d%
   //     CLEO_RegisterOpcode(ADD_PARTICLE_ATTACHED_TO_A_VEHICLE, AddParticleAttachedToAVehicleMain); // 1350=14, add_particle_attached_to_a_object %1d% xoffset %2d% yoffset %3d% zoffset %4d% xdir %5d% ydir %6d% zdir %7d% size %8d% r %9d% g %10d% b %11d% a %12d% lifespan %13d% object_handle %14d%
    //    CLEO_RegisterOpcode(ADD_PARTICLE_ATTACHED_TO_A_OBJECT, AddParticleAttachedToAObjectMain); // 1351=14, add_particle_attached_to_a_vehicle %1d% xoffset %2d% yoffset %3d% zoffset %4d% xdir %5d% ydir %6d% zdir %7d% size %8d% r %9d% g %10d% b %11d% a %12d% lifespan %13d% vehicle_handle %14d%
    }
} cParticleVCCLEOOpcodes;
