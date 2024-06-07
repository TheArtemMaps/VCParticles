#include "plugin.h"
#include "CLEO.h"
#include "CKeyGen.h"
#include "../../CParticleVC/Utility.h"
#include <csignal>
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
// Adding particles with a string
int ParticleTypeFromString(const char* typeStr) {
    if (strcmp(typeStr, "SPARK") == 0) return 0;
    if (strcmp(typeStr, "SPARK_SMALL") == 0) return 1;
    if (strcmp(typeStr, "WATER_SPARK") == 0) return 2;
    if (strcmp(typeStr, "WHEEL_DIRT") == 0) return 3;
    if (strcmp(typeStr, "SAND") == 0) return 4;
    if (strcmp(typeStr, "WHEEL_WATER") == 0) return 5;
    if (strcmp(typeStr, "BLOOD") == 0) return 6;
    if (strcmp(typeStr, "BLOOD_SMALL") == 0) return 7;
    if (strcmp(typeStr, "BLOOD_SPURT") == 0) return 8;
    if (strcmp(typeStr, "DEBRIS") == 0) return 9;
    if (strcmp(typeStr, "DEBRIS2") == 0) return 10;
    if (strcmp(typeStr, "FLYERS") == 0) return 11;
    if (strcmp(typeStr, "WATER") == 0) return 12;
    if (strcmp(typeStr, "FLAME") == 0) return 13;
    if (strcmp(typeStr, "FIREBALL") == 0) return 14;
    if (strcmp(typeStr, "GUNFLASH") == 0) return 15;
    if (strcmp(typeStr, "GUNFLASH_NOANIM") == 0) return 16;
    if (strcmp(typeStr, "GUNSMOKE") == 0) return 17;
    if (strcmp(typeStr, "GUNSMOKE2") == 0) return 18;
    if (strcmp(typeStr, "CIGARETTE_SMOKE") == 0) return 19;
    if (strcmp(typeStr, "SMOKE") == 0) return 20;
    if (strcmp(typeStr, "SMOKE_SLOWMOTION") == 0) return 21;
    if (strcmp(typeStr, "DRY_ICE") == 0) return 22;
    if (strcmp(typeStr, "TEARGAS") == 0) return 23;
    if (strcmp(typeStr, "GARAGEPAINT_SPRAY") == 0) return 24;
    if (strcmp(typeStr, "SHARD") == 0) return 25;
    if (strcmp(typeStr, "SPLASH") == 0) return 26;
    if (strcmp(typeStr, "CARFLAME") == 0) return 27;
    if (strcmp(typeStr, "STEAM") == 0) return 28;
    if (strcmp(typeStr, "STEAM2") == 0) return 29;
    if (strcmp(typeStr, "STEAM_NY") == 0) return 30;
    if (strcmp(typeStr, "STEAM_NY_SLOWMOTION") == 0) return 31;
    if (strcmp(typeStr, "GROUND_STEAM") == 0) return 32;
    if (strcmp(typeStr, "ENGINE_STEAM") == 0) return 33;
    if (strcmp(typeStr, "RAINDROP") == 0) return 34;
    if (strcmp(typeStr, "RAINDROP_SMALL") == 0) return 35;
    if (strcmp(typeStr, "RAIN_SPLASH") == 0) return 36;
    if (strcmp(typeStr, "RAIN_SPLASH_BIGGROW") == 0) return 37;
    if (strcmp(typeStr, "RAIN_SPLASHUP") == 0) return 38;
    if (strcmp(typeStr, "WATERSPRAY") == 0) return 39;
    if (strcmp(typeStr, "WATERDROP") == 0) return 40;
    if (strcmp(typeStr, "BLOODDROP") == 0) return 41;
    if (strcmp(typeStr, "EXPLOSION_MEDIUM") == 0) return 42;
    if (strcmp(typeStr, "EXPLOSION_LARGE") == 0) return 43;
    if (strcmp(typeStr, "EXPLOSION_MFAST") == 0) return 44;
    if (strcmp(typeStr, "EXPLOSION_LFAST") == 0) return 45;
    if (strcmp(typeStr, "CAR_SPLASH") == 0) return 46;
    if (strcmp(typeStr, "BOAT_SPLASH") == 0) return 47;
    if (strcmp(typeStr, "BOAT_THRUSTJET") == 0) return 48;
    if (strcmp(typeStr, "WATER_HYDRANT") == 0) return 49;
    if (strcmp(typeStr, "WATER_CANNON") == 0) return 50;
    if (strcmp(typeStr, "EXTINGUISH_STEAM") == 0) return 51;
    if (strcmp(typeStr, "PED_SPLASH") == 0) return 52;
    if (strcmp(typeStr, "PEDFOOT_DUST") == 0) return 53;
    if (strcmp(typeStr, "CAR_DUST") == 0) return 54;
    if (strcmp(typeStr, "HELI_DUST") == 0) return 55;
    if (strcmp(typeStr, "HELI_ATTACK") == 0) return 56;
    if (strcmp(typeStr, "ENGINE_SMOKE") == 0) return 57;
    if (strcmp(typeStr, "ENGINE_SMOKE2") == 0) return 58;
    if (strcmp(typeStr, "CARFLAME_SMOKE") == 0) return 59;
    if (strcmp(typeStr, "FIREBALL_SMOKE") == 0) return 60;
    if (strcmp(typeStr, "PAINT_SMOKE") == 0) return 61;
    if (strcmp(typeStr, "TREE_LEAVES") == 0) return 62;
    if (strcmp(typeStr, "CARCOLLISION_DUST") == 0) return 63;
    if (strcmp(typeStr, "CAR_DEBRIS") == 0) return 64;
    if (strcmp(typeStr, "BIRD_DEBRIS") == 0) return 65;
    if (strcmp(typeStr, "HELI_DEBRIS") == 0) return 66;
    if (strcmp(typeStr, "EXHAUST_FUMES") == 0) return 67;
    if (strcmp(typeStr, "RUBBER_SMOKE") == 0) return 68;
    if (strcmp(typeStr, "BURNINGRUBBER_SMOKE") == 0) return 69;
    if (strcmp(typeStr, "BULLETHIT_SMOKE") == 0) return 70;
    if (strcmp(typeStr, "GUNSHELL_FIRST") == 0) return 71;
    if (strcmp(typeStr, "GUNSHELL") == 0) return 72;
    if (strcmp(typeStr, "GUNSHELL_BUMP1") == 0) return 73;
    if (strcmp(typeStr, "GUNSHELL_BUMP2") == 0) return 74;
    if (strcmp(typeStr, "ROCKET_SMOKE") == 0) return 75;
    if (strcmp(typeStr, "TEST") == 0) return 76;
    if (strcmp(typeStr, "BIRD_FRONT") == 0) return 77;
    if (strcmp(typeStr, "SHIP_SIDE") == 0) return 78;
    if (strcmp(typeStr, "BEASTIE") == 0) return 79;
    if (strcmp(typeStr, "RAINDROP_2D") == 0) return 80;
    if (strcmp(typeStr, "HEATHAZE") == 0) return 81;
    if (strcmp(typeStr, "HEATHAZE_IN_DIST") == 0) return 82;
    if (strcmp(typeStr, "SNOW") == 0) return 83;
    if (strcmp(typeStr, "BOAT_WAKE") == 0) return 84;
    if (strcmp(typeStr, "FERRY_CHIM_SMOKE") == 0) return 85;
    if (strcmp(typeStr, "MULTIPLAYER_HIT") == 0) return 86;
    if (strcmp(typeStr, "HYDRANT_STEAM") == 0) return 87;
    return -1; // Return -1 for unknown
}

// Adding particle objects with a string
int ParticleObjectTypeFromString(const char* typeStr) {
    if (strcmp(typeStr, "PAVEMENT_STEAM") == 0) return 0;
    if (strcmp(typeStr, "PAVEMENT_STEAM_SLOWMOTION") == 0) return 1;
    if (strcmp(typeStr, "WALL_STEAM") == 0) return 2;
    if (strcmp(typeStr, "WALL_STEAM_SLOWMOTION") == 0) return 3;
    if (strcmp(typeStr, "DARK_SMOKE") == 0) return 4;
    if (strcmp(typeStr, "FIRE_HYDRANT") == 0) return 5;
    if (strcmp(typeStr, "CAR_WATER_SPLASH") == 0) return 6;
    if (strcmp(typeStr, "PED_WATER_SPLASH") == 0) return 7;
    if (strcmp(typeStr, "SPLASHES_AROUND") == 0) return 8;
    if (strcmp(typeStr, "SMALL_FIRE") == 0) return 9;
    if (strcmp(typeStr, "BIG_FIRE") == 0) return 10;
    if (strcmp(typeStr, "DRY_ICE") == 0) return 11;
    if (strcmp(typeStr, "DRY_ICE_SLOWMOTION") == 0) return 12;
    if (strcmp(typeStr, "WATER_FOUNTAIN_VERT") == 0) return 13;
    if (strcmp(typeStr, "WATER_FOUNTAIN_HORIZ") == 0) return 14;
    if (strcmp(typeStr, "FIRE_TRAIL") == 0) return 15;
    if (strcmp(typeStr, "SMOKE_TRAIL") == 0) return 16;
    if (strcmp(typeStr, "FIREBALL_AND_SMOKE") == 0) return 17;
    if (strcmp(typeStr, "ROCKET_TRAIL") == 0) return 18;
    if (strcmp(typeStr, "EXPLOSION_ONCE") == 0) return 19;
    if (strcmp(typeStr, "CATALINAS_GUNFLASH") == 0) return 20;
    if (strcmp(typeStr, "CATALINAS_SHOTGUNFLASH") == 0) return 21;
    return -1; // Return -1 for unknown
}

const uint32_t GetExportedFunction(const char* szExportName)
{
    return reinterpret_cast<const uint32_t>(GetProcAddress(GetModuleHandleA("CParticleVC.SA.asi"), szExportName));
}
void __cdecl AddParticle(int type, float x, float y, float z, float xdir, float ydir, float zdir, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan) {
return ((void(__cdecl*)(int, float, float, float, float, float, float, float, RwUInt8, RwUInt8, RwUInt8, RwUInt8, int32_t))GetExportedFunction("AddParticle"))(type, x, y, z, xdir, ydir, zdir, fSize, r, g, b, a, LifeSpan);
}

void __cdecl AddParticlesAlongLine(int type, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float fPower, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan) {
    return ((void(__cdecl*)(int, float, float, float, float, float, float, float, float, float, float, float, RwUInt8, RwUInt8, RwUInt8, RwUInt8, int32_t))GetExportedFunction("AddParticlesAlongLine"))(type, x1, y1, z1, x2, y2, z2, x3, y3, z3, fPower, fSize, r, g, b, a, LifeSpan);
}

void __cdecl AddJetExplosion(float x, float y, float z, float fPower, float fSize) {
    return ((void(__cdecl*)(float, float, float, float, float))GetExportedFunction("AddJetExplosion"))(x, y, z, fPower, fSize);
}

void __cdecl AddBigGunFlash(float x1, float y1, float z1, float x2, float y2, float z2) {
    return ((void(__cdecl*)(float, float, float, float, float, float))GetExportedFunction("AddBigGunFlash"))(x1, y1, z1, x2, y2, z2);
}

void __cdecl RemovePSystem(int particlesystem) {
    return ((void(__cdecl*)(int))GetExportedFunction("RemoveParticle"))(particlesystem);
}

void __cdecl AddParticleObject(uint8_t ParticleObjectType, float x, float y, float z, float strengthX, float strengthY, float strengthZ, float scale, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int durationInMs, bool remove) {
    ((void(__cdecl*)(uint8_t, float, float, float, float, float, float, float, RwUInt8, RwUInt8, RwUInt8, RwUInt8, int, bool))GetExportedFunction("AddParticleObject"))(ParticleObjectType, x, y, z, strengthX, strengthY, strengthZ, scale, r, g, b, a, durationInMs, remove);
}

void __cdecl RemoveParticleObjectsInArea(float x1, float y1, float z1, float x2, float y2, float z2) {
    return ((void(__cdecl*)(float, float, float, float, float, float))GetExportedFunction("RemoveParticleObjectsInArea"))(x1, y1, z1, x2, y2, z2);
}

void __cdecl RemoveAllParticleObjects() {
    return ((void(__cdecl*)())GetExportedFunction("RemoveAllParticleObjects"))();
}

void __cdecl RemoveAllExpireableParticleObjects() {
    return ((void(__cdecl*)())GetExportedFunction("RemoveAllExpireableParticleObjects"))();
}

void __cdecl AddHeliDust(int ObjectHandle, float DustRadius, int Density) {
    return ((void(__cdecl*)(int, float, int))GetExportedFunction("AddHeliDust"))(ObjectHandle, DustRadius, Density);
}

void __cdecl StartBoatFoamAnimationEx() {
    return ((void(__cdecl*)())GetExportedFunction("StartBoatFoamAnimation"))();
}

void __cdecl UpdateBoatFoamAnimationEx(int ObjectHandle) {
    return ((void(__cdecl*)(int))GetExportedFunction("UpdateBoatFoamAnimation"))(ObjectHandle);
}

void __cdecl AddYardieDoorSmoke(CVector& vecPos, int ObjectHandle) {
    return ((void(__cdecl*)(CVector&, int))GetExportedFunction("AddYardieDoorSmoke"))(vecPos, ObjectHandle);
}


void AddParticleOpcode(CScriptThread* thread) {
    char buf[500];
    auto typeStr = CLEO_ReadStringOpcodeParam(thread, buf, sizeof(buf));
    auto typeStr2 = ParticleTypeFromString(typeStr);
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
    AddParticle(typeStr2, x, y, z, xdir, ydir, zdir, size, r, g, b, a, LifeSpan);
}

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
    AddParticleObject(typeStr2, x, y, z, strengthX, strengthY, strengthZ, scale, r, g, b, a, durationInMs, remove);
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
    }
} cParticleVCCLEOOpcodes;
