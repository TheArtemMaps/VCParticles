#include "plugin.h"

#include "Buoyancy.h"
#include "M:/My Projects/VCMarkersSA/VCMarkersSA/MemoryMgr.h"
#include "CParticleVC.h"
#include <game_sa/CGeneral.h>
#include "CTimeCycle.h"
#include "ParticleObject.h"
using namespace plugin;
cBuoyancy& mod_Buoyancy = *(cBuoyancy*)0xC1C890;
float& cBuoyancy::fPointVolMultiplier = *(float*)0x8D32C8;
CBuoyancyCalcStruct& cBuoyancy::calcStruct = *(CBuoyancyCalcStruct*)0xC1C858;
float(*cBuoyancy::afBoatVolumeDistributionSpeed)[3] = (float(*)[3])0x8D335C;
float(*cBuoyancy::afBoatVolumeDistributionDinghy)[3] = (float(*)[3])0x8D3338;
float(*cBuoyancy::afBoatVolumeDistributionSail)[3] = (float(*)[3])0x8D3314;
float(*cBuoyancy::afBoatVolumeDistribution)[3] = (float(*)[3])0x8D32CC;
float(*cBuoyancy::afBoatVolumeDistributionCat)[3] = (float(*)[3])0x8D3314; // Catamaran volume distribution, unused in game, as there is no matching vehicle

void cBuoyancy::InjectHooks()
{

  //  patch::RedirectJump(0x6C3EF0, ProcessBuoyancy);
  //  patch::RedirectJump(0x6C3030,ProcessBuoyancyBoat, 0x6C3030);
  //  patch::RedirectJump(0x6C3030,CalcBuoyancyForce, 0x6C2750);
  //  patch::RedirectJump(0x6C3030,PreCalcSetup, 0x6C2B90);
    Memory::InjectHook(0x6C34E0, &AddSplashParticles, PATCH_JUMP);
  //  patch::RedirectJump(0x6C3030,SimpleCalcBuoyancy, 0x6C3B00);
  //  patch::RedirectJump(0x6C3030,SimpleSumBuoyancyData, 0x6C2970);
   // patch::RedirectJump(0x6C3030,FindWaterLevel, 0x6C2810);
   // patch::RedirectJump(0x6C3030,FindWaterLevelNorm, 0x6C28C0);
}

// 0x6C3EF0
/*bool cBuoyancy::ProcessBuoyancy(CPhysical* entity, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) {
    CVector& entityPosition = entity->GetPosition();
    if (!CWaterLevel::GetWaterLevel(entityPosition, m_fWaterLevel, entity->physicalFlags.bTouchingWater)) {
        return false;
    }

    m_EntityMatrix = *entity->m_matrix;
    PreCalcSetup(entity, fBuoyancy);
    if (entity->IsPed()) {
        entity->GetColModel(); // for some reason, this is here?

        m_bInWater = true;
        m_fEntityWaterImmersion = (m_fWaterLevel - entityPosition.z + 1.0F) / 1.9F;
        if (m_fEntityWaterImmersion > 1.0F) { // clamp
            m_fEntityWaterImmersion = 1.0F;
        } else if (m_fEntityWaterImmersion < 0.0F) {
            m_fEntityWaterImmersion = 0.0F;
            m_bInWater = false;
        }

        m_vecTurnPoint = CVector{};
        if (m_fEntityWaterImmersion > 0.0F && m_fEntityWaterImmersion < 1.0F) {
            const CVector dir = -entity->GetForward();
            const CVector vec = CVector(0.0F, 0.0F, m_fWaterLevel - entityPosition.z);
            cBuoyancy::AddSplashParticles(entity, vec, vec, dir, true);
        }
    } else {
        SimpleCalcBuoyancy(entity);
    }

    bool bCalcBuoyancyForce = CalcBuoyancyForce(entity, vecBuoyancyTurnPoint, vecBuoyancyForce);
    if (m_bProcessingBoat || bCalcBuoyancyForce) {
        return true;
    }

    return false;
}

// 0x6C3030
bool cBuoyancy::ProcessBuoyancyBoat(CVehicle* vehicle, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce, bool bUnderwater) {
    const CVector& entityPosition = vehicle->GetPosition();
    if (!CWaterLevel::GetWaterLevel(entityPosition, m_fWaterLevel, vehicle->physicalFlags.bTouchingWater)) {
        return false;
    }

    m_EntityMatrix = *vehicle->m_matrix;
    PreCalcSetup(vehicle, fBuoyancy);
    float fBoundingHeight = m_vecBoundingMax.z - m_vecBoundingMin.z;
    auto fBoatHeightRatio = 1.0F / (fBoundingHeight * 9.0F);

    for (int32 iXMult = 0; iXMult < 3; ++iXMult) {
        for (int32 iYMult = 0; iYMult < 3; ++iYMult) {
            auto fCurrentX = m_vecBoundingMin.x + m_vecCenterOffset.x * iXMult;
            auto fCurrentY = m_vecBoundingMin.y + m_vecCenterOffset.y * iYMult;
            CVector vecCurPoint(fCurrentX, fCurrentY, 0.0F);

            auto vecWaveNormal = CVector(0.0F, 0.0F, 1.0F);
            tWaterLevel eState;
            FindWaterLevelNorm(m_vecInitialZPos, &vecCurPoint, &eState, &vecWaveNormal);

            auto fThird = 1.0F / 3.0F;
            vecWaveNormal.z += 2.0F;
            vecWaveNormal *= fThird;

            switch (vehicle->m_nModelIndex) {
            case MODEL_SQUALO:
            case MODEL_SPEEDER:
            case MODEL_JETMAX:
            case MODEL_LAUNCH:
                fPointVolMultiplier = afBoatVolumeDistributionSpeed[iYMult][iXMult];
                break;
            case MODEL_COASTG:
            case MODEL_DINGHY:
                fPointVolMultiplier = afBoatVolumeDistributionDinghy[iYMult][iXMult];
                break;
            case MODEL_MARQUIS:
                fPointVolMultiplier = afBoatVolumeDistributionSail[iYMult][iXMult];
                break;
            default:
                fPointVolMultiplier = afBoatVolumeDistribution[iYMult][iXMult];
                break;
            }

            if (eState == tWaterLevel::COMPLETELY_ABOVE_WATER)
                continue;

            auto fAddedDistToSurface = SimpleSumBuoyancyData(&vecCurPoint, eState);
            auto fCurBuoyancy = CTimer::GetTimeStep() * m_fBuoyancy;
            auto fPointContribution = fCurBuoyancy * (fAddedDistToSurface * fBoatHeightRatio);

            CVector vecOffset(fCurrentX, fCurrentY, 0.0F);
            CVector vecTransformedPos = vehicle->GetMatrix().TransformVector(vecOffset);

            CVector vecSpeedAtPoint = vehicle->GetSpeed(vecTransformedPos);
            auto handling = vehicle->m_pHandlingData;
            auto fWavePower = 1.0F - DotProduct(vecSpeedAtPoint, vecWaveNormal) * handling->m_fSuspensionDampingLevel;
            fWavePower = std::max(fWavePower, 0.0F);
            fWavePower *= fPointContribution;
            vecBuoyancyForce->z += fWavePower;

            if (!bUnderwater) {
                auto vecTurnForceAtPoint = vecWaveNormal * fWavePower;
                CVector vecAppliedForcePoint = m_EntityMatrix.TransformVector(vecCurPoint);
                vehicle->ApplyTurnForce(vecTurnForceAtPoint, vecAppliedForcePoint);
            }
        }
    }

    m_fEntityWaterImmersion *= fBoatHeightRatio;
    *vecBuoyancyTurnPoint = m_EntityMatrix.TransformVector(m_vecTurnPoint);

    if (m_bProcessingBoat)
        return true;

    return m_bInWater;
}

bool cBuoyancy::CalcBuoyancyForce(CPhysical* entity, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce)
{
    if (!m_bInWater)
        return false;

    *vecBuoyancyTurnPoint = m_EntityMatrix.TransformVector(m_vecTurnPoint);
    auto fCurrentBuoyancy = m_fEntityWaterImmersion * m_fBuoyancy * CTimer::GetTimeStep();
    vecBuoyancyForce->Set(0.0F, 0.0F, fCurrentBuoyancy);

    float fMoveForceZ = entity->m_fMass * entity->m_vecMoveSpeed.z;
    if (fMoveForceZ <= fCurrentBuoyancy * 4.0F)
        return true;

    float fBuoyancy = std::max(0.0F, fCurrentBuoyancy - fMoveForceZ);
    vecBuoyancyForce->z = fBuoyancy;
    return true;
}

// 0x6C2B90
void cBuoyancy::PreCalcSetup(CPhysical* entity, float fBuoyancy)
{
    CVehicle* vehicle = entity->AsVehicle();
    m_bProcessingBoat = entity->IsVehicle() && vehicle->IsBoat();
    auto cm = entity->GetColModel();
    m_vecBoundingMin = cm->m_boundBox.m_vecMin;
    m_vecBoundingMax = cm->m_boundBox.m_vecMax;

    if (!m_bProcessingBoat) {
        switch (entity->m_nModelIndex) {
        case MODEL_LEVIATHN: //417
            m_vecBoundingMin.y *= 0.4F;
            m_vecBoundingMax.y *= 1.15F;
            break;
        case MODEL_SKIMMER: //460
            m_vecBoundingMin.x *= 0.3F;
            m_vecBoundingMin.y = m_vecBoundingMax.y * -0.6F;
            m_vecBoundingMin.z -= 0.1F;
            m_vecBoundingMax.x *= 0.3F;
            m_vecBoundingMax.y *= 1.4F;
            break;
        default:
            if (entity->IsVehicle() && vehicle->IsSubHeli()) {
                m_vecBoundingMin.y = -m_vecBoundingMax.y;
                m_vecBoundingMax.z = m_vecBoundingMin.z * -1.1F;
                m_vecBoundingMin.z *= 0.85F;
            }
            break;
        }
    }
    else {
        switch (entity->m_nModelIndex) {
        case MODEL_SQUALO: //446
            m_vecBoundingMax.y *= 0.9F;
            m_vecBoundingMin.y *= 0.9F;
            break;
        case MODEL_SPEEDER: //452
            m_vecBoundingMax.y *= 1.25F;
            m_vecBoundingMin.y *= 0.83F;
            break;
        case MODEL_REEFER: //453
        case MODEL_JETMAX: //498
            m_vecBoundingMin.y *= 0.9F;
            break;
        case MODEL_TROPIC: //454
            m_vecBoundingMax.y *= 1.3F;
            m_vecBoundingMin.y *= 0.82F;
            m_vecBoundingMin.z -= 0.2F;
            break;
        case MODEL_COASTG: //472
            m_vecBoundingMax.y *= 1.1F;
            m_vecBoundingMin.y *= 0.9F;
            m_vecBoundingMin.z -= 0.3F;
            break;
        case MODEL_DINGHY: //473
            m_vecBoundingMax.y *= 1.3F;
            m_vecBoundingMin.y *= 0.9F;
            m_vecBoundingMin.z -= 0.2F;
            break;
        case MODEL_MARQUIS: //484
            m_vecBoundingMax.y *= 1.1F;
            m_vecBoundingMin.y *= 0.9F;
            break;
        case MODEL_LAUNCH: //595
            m_vecBoundingMax.y *= 1.25F;
            m_vecBoundingMin.y *= 0.8F;
            m_vecBoundingMin.z -= 0.1F;
            break;
        default:
            m_vecBoundingMax.y *= 1.05F;
            m_vecBoundingMin.y *= 0.9F;
            break;
        }
    }

    // Calculate center offset and "normalized" version of it
    // (it's not normalized vector, just the offset scaled by multiplicative inverse of it's longest component
    //  so the longest component is equal to 1.0F)
    m_vecCenterOffset = (m_vecBoundingMax - m_vecBoundingMin) * 0.5F;
    float fScale;
    if ((m_vecCenterOffset.z > m_vecCenterOffset.x) && (m_vecCenterOffset.z > m_vecCenterOffset.y))
        fScale = 1.0F / m_vecCenterOffset.z;
    else if ((m_vecCenterOffset.y > m_vecCenterOffset.x) && (m_vecCenterOffset.y > m_vecCenterOffset.z))
        fScale = 1.0F / m_vecCenterOffset.y;
    else
        fScale = 1.0F / m_vecCenterOffset.x;

    m_vecNormalizedCenterOffset = m_vecCenterOffset * fScale;
    m_fNumCheckedPoints = 1.0F;
    m_vecTurnPoint.Set(0.0F, 0.0F, 0.0F);
    m_bInWater = false;
    m_fEntityWaterImmersion = 0.0F;
    m_vecPos = entity->GetPosition();
    m_fWaterLevel += m_fUnkn2;
    m_vecInitialZPos.Set(0.0F, 0.0F, m_vecPos.z);
    m_fBuoyancy = fBuoyancy;
}*/
// 0x506F10
void AudioEnginee::ReportWaterSplash(CVector posn, float volume) {
     Call<0x506F10, CAudioEngine*, CVector, float>(this, posn, volume);
}

void AudioEnginee::ReportWaterSplash(CPhysical* physical, float volume, bool forcePlaySplashSound) {
    Call<0x506F00, CAudioEngine*, CPhysical*, float, bool>(this, physical, volume, forcePlaySplashSound);
}

void PedAudio::AddAudioEvent(eAudioEvents event, float volume, float speed, CPhysical* ped, uint8_t surfaceId, int32_t a7, uint32_t maxVol) {
      Call<0x4E2BB0, CAEPedAudioEntity*, eAudioEvents, float, float, CPhysical*, uint8_t, int32_t, uint32_t>(this, event, volume, speed, ped, surfaceId, a7, maxVol);
}

// 0x6C34E0
void cBuoyancy::AddSplashParticles(CPhysical* entity, CVector vecFrom, CVector vecTo, CVector vecSplashDir, uint8_t bReduceParticleSize)
{
    auto fDistBetweenPoints = DistanceBetweenPoints(vecFrom, vecTo);
    auto vecUsedSpeed = entity->m_vecMoveSpeed;
    auto fMoveSpeed = vecUsedSpeed.Magnitude();
    if (fMoveSpeed > 0.1F) {
        auto fMult = 1.0F / fMoveSpeed;
        vecUsedSpeed *= (fMult * 0.1F); // Get the speed in [0 : 0.1] range
        fMoveSpeed = 0.1F;
    }

    if (fMoveSpeed <= 0.05F)
        return;
    
    FxPrtMult_c curParticle(1.0F, 1.0F, 1.0F, 0.1F, 0.5F, 1.0F, 0.2F);
    if (bReduceParticleSize) {
        curParticle.m_fSize *= 0.5F;
        curParticle.m_fLife *= 0.5F;
    }
 
    auto vecVelocityModifier = vecUsedSpeed * CVector(0.0F, 0.0F, -1.0F) * 120.0F;

    auto iNumParticles = max(1, static_cast<int32_t>(fDistBetweenPoints + fDistBetweenPoints));
    for (int32_t iIter = 0; iIter < iNumParticles; ++iIter) {
        auto fCurrentProgress = static_cast<float>(iIter) / static_cast<float>(iNumParticles);
        auto vecCurPoint = lerp(vecFrom, vecTo, fCurrentProgress);
        auto vecTransformedPoint = reinterpret_cast<Matrix*>(entity->m_matrix)->TransformPoint(vecCurPoint);

        if (!entity->m_nType == ENTITY_TYPE_PED) {
            const auto& vecEntPos = entity->GetPosition();
            vecSplashDir = vecTransformedPoint - vecEntPos;
        }
        vecSplashDir.z = 0.0F;
        vecSplashDir.Normalise();
      //  RwRGBA color;
       // color.red = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalRed + CTimeCycle::GetAmbientRed()) * 127.5f;
       // color.green = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalBlue + CTimeCycle::GetAmbientBlue()) * 127.5f;
      //  color.blue = (0.5f * CTimeCycle::m_CurrentColours.m_fDirectionalGreen + CTimeCycle::GetAmbientGreen()) * 127.5f;
      //  color.alpha = CGeneral::GetRandomNumberInRange(48.0f, 96.0f);
        auto vecVelocity = vecSplashDir * fMoveSpeed;
        float fRand = CGeneral::GetRandomNumberInRange(0.0F, 0.5F);
        vecTransformedPoint += (vecSplashDir * fRand);
       // CParticleObject::AddObject(POBJECT_PED_WATER_SPLASH, vecTransformedPoint, CVector(0.0f, 0.0f, 0.0f), 0.0f, 50, color, true);
        CParticle::AddParticle(tParticleType::PARTICLE_PED_SPLASH, vecTransformedPoint, vecVelocity, NULL, 0.3f);
       // g_fx.m_WaterSplash->AddParticle(&vecTransformedPoint, &vecVelocity, 0.0F, &curParticle, -1.0F, 1.2F, 0.6F, 0);
    }

    if (entity->m_nType == ENTITY_TYPE_PED) {
        auto ped = ((CPed*)entity);
        auto swimTask = ped->m_pIntelligence->GetTaskSwim();
        if (!swimTask) {
           const auto& vecPedForward = entity->GetForward();
            auto fPedAngle = CGeneral::GetAngleBetweenPoints(vecPedForward.x, vecPedForward.y, 0.0F, 0.0F);
            fPedAngle = CGeneral::LimitAngle(fPedAngle) + 180.0F;
            curParticle = FxPrtMult_c(1.0F, 1.0F, 1.0F, 0.2F, 0.4F, 0.0F, 0.5F);
            auto vecPedVelocity = CVector(0.0F, 0.0F, 0.0F);
            auto vecPedParticlePos = entity->GetPosition() + (vecPedForward * 0.4F);
            if (ped->m_pPlayerData)
                vecPedParticlePos.z = ped->m_pPlayerData->m_fWaterHeight;
            else
                vecPedParticlePos.z += 0.5F;
           //((FireAudio)fireaudio).AddAudioEvent(eAudioEvents::AE_FIRE, ped->GetPosition());
            RwRGBA waterSprayCol = { 48, 48, 64, 0 };
            CParticle::AddParticle(PARTICLE_WATERSPRAY, vecPedParticlePos, -static_cast<Vec>(ped->m_vecMoveSpeed), NULL, 0.6f, waterSprayCol);
           // g_fx.m_Wake->AddParticle(&vecPedParticlePos, &vecPedVelocity, 0.0F, &curParticle, fPedAngle, 1.2F, 0.6F, 0);
          //  ((PedAudio)ped->m_pedAudio).AddAudioEvent(eAudioEvents::AE_PED_SWIM_WAKE, 0.0F, 1.0F, nullptr, 0, 0, 0);
        }
    }

   // ((AudioEnginee)AudioEngine).ReportWaterSplash(entity, -100.0F, false);
}

/*void cBuoyancy::SimpleCalcBuoyancy(CPhysical* entity)
{
    CVector vecAllPoints[3][3];

    for (int32 iXMult = 0; iXMult < 3; ++iXMult) {
        for (int32 iYMult = 0; iYMult < 3; ++iYMult) {
            auto& pCurVec = vecAllPoints[iXMult][iYMult];
            pCurVec.Set(0.0F, 0.0F, 0.0F);

            auto vecCurPoint = CVector();
            vecCurPoint.x = m_vecBoundingMin.x + (m_vecCenterOffset.x * iXMult);
            vecCurPoint.y = m_vecBoundingMin.y + (m_vecCenterOffset.y * iYMult);
            vecCurPoint.z = 0.0F;
            tWaterLevel eState;
            FindWaterLevel(m_vecInitialZPos, &vecCurPoint, &eState);
            cBuoyancy::fPointVolMultiplier = 1.0F;

            if (eState == tWaterLevel::COMPLETELY_ABOVE_WATER)
                pCurVec.z = m_vecBoundingMin.z;
            else {
                SimpleSumBuoyancyData(&vecCurPoint, eState);
                pCurVec = vecCurPoint;
            }
        }
    }

    auto fHeight = m_vecBoundingMax.z - m_vecBoundingMin.z;
    m_fEntityWaterImmersion = m_fEntityWaterImmersion / (9.0F * fHeight);
    auto fHalfHeightZ = m_vecBoundingMin.z + fHeight * 0.5F;

    for (int32 i = 0; i < 2; ++i) {
        auto fMinYHeight = vecAllPoints[i][0].z;            // 000      000     ^y
        auto fMinYHeight2 = vecAllPoints[i + 1][0].z;       // 000  ->  000     │   x
        if (fMinYHeight > fHalfHeightZ                      // xx0      0xx     └───>
            && fMinYHeight < m_vecBoundingMax.z
            && fMinYHeight2 > fHalfHeightZ
            && fMinYHeight2 < m_vecBoundingMax.z) {

            CVector vecDir(0.0F, -1.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[i][0], vecAllPoints[i + 1][0], vecDir, false);
        }

        auto fMaxYHeight = vecAllPoints[i][2].z;            // xx0      0xx     ^y
        auto fMaxYHeight2 = vecAllPoints[i + 1][2].z;       // 000  ->  000     │   x
        if (fMaxYHeight > fHalfHeightZ                      // 000      000     └───>
            && fMaxYHeight < m_vecBoundingMax.z
            && fMaxYHeight2 > fHalfHeightZ
            && fMaxYHeight2 < m_vecBoundingMax.z) {

            CVector vecDir(0.0F, 1.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[i][2], vecAllPoints[i + 1][2], vecDir, false);
        }
    }

    for (int32 i = 0; i < 2; ++i) {
        auto fMinXHeight = vecAllPoints[0][i].z;            // 000      x00     ^y
        auto fMinXHeight2 = vecAllPoints[0][i + 1].z;       // x00  ->  x00     │   x
        if (fMinXHeight > fHalfHeightZ                      // x00      000     └───>
            && fMinXHeight < m_vecBoundingMax.z
            && fMinXHeight2 > fHalfHeightZ
            && fMinXHeight2 < m_vecBoundingMax.z) {

            CVector vecDir(-1.0F, 0.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[0][i], vecAllPoints[0][i + 1], vecDir, false);
        }

        auto fMaxXHeight = vecAllPoints[2][i].z;            // 000      00x     ^y
        auto fMaxXHeight2 = vecAllPoints[2][i + 1].z;       // 00x  ->  00x     │   x
        if (fMaxXHeight > fHalfHeightZ                      // 00x      000     └───>
            && fMaxXHeight < m_vecBoundingMax.z
            && fMaxXHeight2 > fHalfHeightZ
            && fMaxXHeight2 < m_vecBoundingMax.z) {

            CVector vecDir(1.0F, 0.0F, 0.0F);
            AddSplashParticles(entity, vecAllPoints[i][2], vecAllPoints[i + 1][2], vecDir, false);
        }
    }
}

float cBuoyancy::SimpleSumBuoyancyData(CVector* vecWaterOffset, tWaterLevel ePointState)
{
    if (!cBuoyancy::calcStruct.bBuoyancyDataSummed)
        cBuoyancy::calcStruct.bBuoyancyDataSummed = true;

    auto fPointDistanceToWaterSurface = fabs(vecWaterOffset->z - m_vecBoundingMin.z) - (1.0F - cBuoyancy::fPointVolMultiplier);
    cBuoyancy::calcStruct.fAddedDistToWaterSurface = fPointDistanceToWaterSurface;
    if (fPointDistanceToWaterSurface < 0.0F)
        return 0.0F;

    if (m_bProcessingBoat)
        cBuoyancy::calcStruct.fAddedDistToWaterSurface = pow(cBuoyancy::calcStruct.fAddedDistToWaterSurface * cBuoyancy::fPointVolMultiplier, 2.0F);

    m_fEntityWaterImmersion += cBuoyancy::calcStruct.fAddedDistToWaterSurface;

    cBuoyancy::calcStruct.vecCurOffsetTurnPoint.x = vecWaterOffset->x * m_vecNormalizedCenterOffset.x;
    cBuoyancy::calcStruct.vecCurOffsetTurnPoint.y = vecWaterOffset->y * m_vecNormalizedCenterOffset.y;
    cBuoyancy::calcStruct.vecCurOffsetTurnPoint.z = (vecWaterOffset->z + m_vecBoundingMin.z) * m_vecNormalizedCenterOffset.z * 0.5F;

    if (m_bFlipUnknVector)
        cBuoyancy::calcStruct.vecCurOffsetTurnPoint *= -1.0F;

    // Rolling average calculation, averages the move force from all checked points
    cBuoyancy::calcStruct.fNewPointContribution = 1.0F / m_fNumCheckedPoints;
    cBuoyancy::calcStruct.fCurrentAverageContribution = 1.0F - cBuoyancy::calcStruct.fNewPointContribution;
    auto vecNewPointContribution = cBuoyancy::calcStruct.vecCurOffsetTurnPoint * cBuoyancy::calcStruct.fNewPointContribution * cBuoyancy::calcStruct.fAddedDistToWaterSurface;
    auto vecCurrentAverageContibution = m_vecTurnPoint * cBuoyancy::calcStruct.fCurrentAverageContribution;
    m_vecTurnPoint = vecNewPointContribution + vecCurrentAverageContibution;

    m_fNumCheckedPoints += 1.0F;
    m_bInWater = true;

    return cBuoyancy::calcStruct.fAddedDistToWaterSurface;
}

void cBuoyancy::FindWaterLevel(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState)
{
    CVector transformedPos = m_EntityMatrix.TransformVector(*outVecOffset);
    auto vecWorldPos = transformedPos + m_vecPos;
    CWaterLevel::GetWaterLevel(vecWorldPos.x, vecWorldPos.y, m_vecPos.z, outVecOffset->z, true, nullptr);
    outVecOffset->z -= (transformedPos.z + vecInitialZPos.z);

    if (outVecOffset->z > m_vecBoundingMax.z) {
        outVecOffset->z = m_vecBoundingMax.z;
        *outInWaterState = tWaterLevel::COMPLETELY_UNDER_WATER;
    }
    else if (outVecOffset->z < m_vecBoundingMin.z) {
        outVecOffset->z = m_vecBoundingMin.z;
        *outInWaterState = tWaterLevel::COMPLETELY_ABOVE_WATER;
    }
    else {
        *outInWaterState = tWaterLevel::COLLIDING_WITH_WATER;
    }
}

void cBuoyancy::FindWaterLevelNorm(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState, CVector* outVecNormal)
{
    CVector transformedPos = m_EntityMatrix.TransformVector(*outVecOffset);
    auto vecWorldPos = transformedPos + m_vecPos;
    CWaterLevel::GetWaterLevel(vecWorldPos.x, vecWorldPos.y, m_vecPos.z, outVecOffset->z, true, outVecNormal);
    outVecOffset->z -= (transformedPos.z + vecInitialZPos.z);

    if (outVecOffset->z > m_vecBoundingMax.z) {
        outVecOffset->z = m_vecBoundingMax.z;
        *outInWaterState = tWaterLevel::COMPLETELY_UNDER_WATER;
    }
    else if (outVecOffset->z < m_vecBoundingMin.z) {
        outVecOffset->z = m_vecBoundingMin.z;
        *outInWaterState = tWaterLevel::COMPLETELY_ABOVE_WATER;
    }
    else {
        *outInWaterState = tWaterLevel::COLLIDING_WITH_WATER;
    }
}*/
