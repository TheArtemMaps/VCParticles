#include "plugin.h"

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

void __cdecl RemovePObject(int PObjectHandle) {
    return ((void(__cdecl*)(int))GetExportedFunction("RemovePObject"))(PObjectHandle);
}

void __cdecl AddParticleObject(uint8_t ParticleObjectType, float x, float y, float z, float strengthX, float strengthY, float strengthZ, float scale, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, uint32_t durationInMs, bool remove) {
    return ((void(__cdecl*)(uint8_t, float, float, float, float, float, float, float, RwUInt8, RwUInt8, RwUInt8, RwUInt8, uint32_t, bool))GetExportedFunction("AddParticleObject"))(ParticleObjectType, x, y, z, strengthX, strengthY, strengthZ, scale, r, g, b, a, durationInMs, remove);
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

/*void __cdecl AddParticleAttachedToAObject(int type, float xoffset, float yoffset, float zoffset, float xdir, float ydir, float zdir, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan, int ObjectHandle) {
    return ((void(__cdecl*)(int, float, float, float, float, float, float, float, RwUInt8, RwUInt8, RwUInt8, RwUInt8, int32_t, int))GetExportedFunction("AddParticleAttachedToAObject"))(type, xoffset, yoffset, zoffset, xdir, ydir, zdir, fSize, r, g, b, a, LifeSpan, ObjectHandle);
}

void __cdecl AddParticleAttachedToAVehicle(int type, float xoffset, float yoffset, float zoffset, float xdir, float ydir, float zdir, float fSize, RwUInt8 r, RwUInt8 g, RwUInt8 b, RwUInt8 a, int32_t LifeSpan, int VehicleHandle) {
    return ((void(__cdecl*)(int, float, float, float, float, float, float, float, RwUInt8, RwUInt8, RwUInt8, RwUInt8, int32_t, int))GetExportedFunction("AddParticleAttachedToAVehicle"))(type, xoffset, yoffset, zoffset, xdir, ydir, zdir, fSize, r, g, b, a, LifeSpan, VehicleHandle);
}*/

void __cdecl LogExport(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    return ((void(__cdecl*)(const char*, ...))GetExportedFunction("LogExport"))(msg, args);
    va_end(args);
}

void __cdecl SetMotionBlurEx(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha, int32_t type) {
    return ((void(__cdecl*)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t))GetExportedFunction("SetMotionBlurEx"))(red, green, blue, alpha, type);
}