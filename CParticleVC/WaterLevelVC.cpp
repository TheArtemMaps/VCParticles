#include "plugin.h"
#include "CFileMgr.h"
#include "CFileLoader.h"
#include "CTxdStore.h"
#include "CTimer.h"
#include "CWeather.h"
#include "CCamera.h"
#include "CVehicle.h"
#include "CPlayerPed.h"
#include "CBoat.h"
#include "CWorld.h"
#include "CGeneral.h"
#include "CTimecycle.h"
#include "CCullZones.h"
#include "CClock.h"
#include "CStreaming.h"
#include "CColStore.h"
#include "CPad.h"
#include "RenderBuffer.h"
#include "CReplay.h"
#include "WaterLevelVC.h"
#include "Utility.h"
#include <game_sa/CGame.h>
#include "CParticleVC.h"
using namespace plugin;
//#define PS2
ThiscallEvent <AddressList<0x53E175, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> RenderEffectsEvent22;
CdeclEvent <AddressList<0x53DF40, H_CALL>, PRIORITY_BEFORE, ArgPickNone, void()> RenderSceneEvent;
CdeclEvent    <AddressList<0x717150, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> movingThingsEvent22;
//#define MASTER
#define RpGeometryGetMorphTarget(_geometry, _index)        \
    (&((_geometry)->morphTarget[(_index)]))
#define RpGeometryGetMaterial(_geometry, _num)   \
    (((_geometry)->matList.materials)[(_num)])
#define RpGeometryGetVertexTexCoords(_geometry, _uvIndex)    \
    ((_geometry)->texCoords[(_uvIndex) - 1])
#define RpGeometryGetPreLightColors(_geometry)             \
    ((_geometry)->preLitLum)
#define RpMorphTargetSetBoundingSphere(_mt, _sphere)       \
    (RwSphereAssign(&((_mt)->boundingSphere), (_sphere)), (_mt))
#define RpAtomicGetFrame(_atomic)                                  \
    ((RwFrame *) rwObjectGetParent(_atomic))
#define RwCameraGetCurrentCamera()                         \
    ((RwCamera *)RWSRCGLOBAL(curCamera))
#define rwMatrixSetFlags(m, flagsbit)     ((m)->flags = (flagsbit))
#define rwMatrixGetFlags(m)               ((m)->flags)
#define RwMatrixSetIdentity(m)                                     \
MACRO_START                                                             \
{                                                                       \
    (m)->right.x = (m)->up.y    = (m)->at.z    = (RwReal)((1.0));       \
    (m)->right.y = (m)->right.z = (m)->up.x    = (RwReal)((0.0));       \
    (m)->up.z    = (m)->at.x    = (m)->at.y    = (RwReal)((0.0));       \
    (m)->pos.x   = (m)->pos.y   = (m)->pos.z   = (RwReal)((0.0));       \
    rwMatrixSetFlags((m),                                               \
                     rwMatrixGetFlags(m) |                              \
                     (rwMATRIXINTERNALIDENTITY |                        \
                      rwMATRIXTYPEORTHONORMAL));                        \
}                                                                       \
MACRO_STOP
#define RwIm3DVertexSet_RGBA(vert, rgba) RwIm3DVertexSetRGBA(vert, rgba.red, rgba.green, rgba.blue, rgba.alpha) // (RwRGBAAssign(&(_dst)->color, &_src))

float TEXTURE_ADDU;
float TEXTURE_ADDV;

float _TEXTURE_MASK_ADDU;
float _TEXTURE_MASK_ADDV;

float _TEXTURE_WAKE_ADDU;
float _TEXTURE_WAKE_ADDV;

int32_t CWaterLevel::ms_nNoOfWaterLevels;
float CWaterLevel::ms_aWaterZs[48];
CRect CWaterLevel::ms_aWaterRects[48];
int8_t CWaterLevel::aWaterBlockList[MAX_LARGE_SECTORS][MAX_LARGE_SECTORS];
int8_t CWaterLevel::aWaterFineBlockList[MAX_SMALL_SECTORS][MAX_SMALL_SECTORS];
bool CWaterLevel::WavesCalculatedThisFrame;


bool CWaterLevel::RequireWavySector;
bool CWaterLevel::MaskCalculatedThisFrame;
CVector CWaterLevel::PreCalculatedMaskPosn;
bool CWaterLevel::m_bRenderSeaBed;
int32_t CWaterLevel::m_nRenderWaterLayers;

RpAtomic* CWaterLevel::ms_pWavyAtomic;
RpAtomic* CWaterLevel::ms_pMaskAtomic;
//"Custom" Don't Render Water Toggle
bool gbDontRenderWater;


RwTexture* gpWaterTex;
RwTexture* gpWaterEnvTex;
RwTexture* gpWaterEnvBaseTex;
RwTexture* gpWaterWakeTex;

RwRaster* gpWaterRaster;
RwRaster* gpWaterEnvRaster;
RwRaster* gpWaterEnvBaseRaster;
RwRaster* gpWaterWakeRaster;

bool _bSeaLife;
float _fWaterZOffset = WATER_Z_OFFSET;

#ifdef PC_WATER
float fEnvScale = 0.25f;
#else
float fEnvScale = 0.5f;
#endif
float fWave2InvLength = 0.03f;
float fWave2NormScale = 0.5f;
float fWave2Ampl = 0.1f;
uint8_t nWaterAlpha = 192;
uint8_t nWakeAlpha = 192;
float fUnder1 = 4.0;
float fUnder2 = 2.5;
float fUnder3 = 1.5;
int nMaskAlpha = 230;
float fAdd1 = 180.0f;
float fAdd2 = 80.0;
float fRedMult = 0.6f;
float fGreenMult = 1.0f;
float fBlueMult = 1.4f;
float fAlphaMult = 500.0f;
float fAlphaBase = 30.0f;
float fRandomMoveDiv = 8.0f;
float fRandomDamp = 0.99f;
float fNormMult = 2.0f;
float fNormMultB = 1.0f;
float fBumpScale = 1.5;
float fBumpTexRepeat = 2.0;
float fNormalDirectionScalar1 = 2.0f;
float fNormalDirectionScalar2 = 1.0f;
bool bTestDoNormals = true;
float fSeaBedZ = 25.0f;
float aAlphaFade[5] = { 0.4f, 1.0f, 0.2f, 1.0f, 0.4f };	//CWaterLevel::RenderWakeSegment
float fFlatWaterBlendRange = 0.05f;
float fStartBlendDistanceAdd = 64.0f;
float fMinWaterAlphaMult = -30.0f;


void
CWaterLevel::Initialise(const char* pWaterDat)
{
	ms_nNoOfWaterLevels = 0;

#ifdef MASTER
	int32_t hFile = -1;

	do
	{
		hFile = CFileMgr::OpenFile("DATA\\waterpro.dat", "rb");
	} while (hFile < 0);
#else
	int32_t hFile = CFileMgr::OpenFile(PLUGIN_PATH((char*)"DATA\\waterpro.dat"), "rb");
#endif

	if (hFile > 0)
	{
		CFileMgr::Read(hFile, (char*)&ms_nNoOfWaterLevels, sizeof(ms_nNoOfWaterLevels));
		CFileMgr::Read(hFile, (char*)ms_aWaterZs, sizeof(ms_aWaterZs));
		CFileMgr::Read(hFile, (char*)ms_aWaterRects, sizeof(ms_aWaterRects));
		CFileMgr::Read(hFile, (char*)aWaterBlockList, sizeof(aWaterBlockList));
		CFileMgr::Read(hFile, (char*)aWaterFineBlockList, sizeof(aWaterFineBlockList));
		CFileMgr::CloseFile(hFile);
	}
#ifndef MASTER
	else
	{
		debug("Init waterlevels\n");

		// collision is streamed in VC
		//CColStore::load();

		CFileMgr::SetDir("");
		hFile = CFileMgr::OpenFile(pWaterDat, "r");

		char* line;

		while ((line = CFileLoader::LoadLine(hFile)))
		{
			if (*line && *line != ';' && !strstr(line, "* ;end of file"))
			{
				float z, l, b, r, t;
				sscanf(line, "%f %f %f %f %f", &z, &l, &b, &r, &t);
				AddWaterLevel(l, b, r, t, z);
			}
		}

		CFileMgr::CloseFile(hFile);

		for (int32_t x = 0; x < MAX_SMALL_SECTORS; x++)
		{
			for (int32_t y = 0; y < MAX_SMALL_SECTORS; y++)
			{
				aWaterFineBlockList[x][y] = NO_WATER;
			}
		}

		// rasterize water rects read from file
		for (int32_t i = 0; i < ms_nNoOfWaterLevels; i++)
		{
			int32_t l = WATER_HUGE_X(ms_aWaterRects[i].left + WATER_X_OFFSET);
			int32_t r = WATER_HUGE_X(ms_aWaterRects[i].right + WATER_X_OFFSET) + 1.0f;
			int32_t t = WATER_HUGE_Y(ms_aWaterRects[i].top);
			int32_t b = WATER_HUGE_Y(ms_aWaterRects[i].bottom) + 1.0f;

			l = Clamp(l, 0, MAX_SMALL_SECTORS - 1);
			r = Clamp(r, 0, MAX_SMALL_SECTORS - 1);
			t = Clamp(t, 0, MAX_SMALL_SECTORS - 1);
			b = Clamp(b, 0, MAX_SMALL_SECTORS - 1);

			for (int32_t x = l; x <= r; x++)
			{
				for (int32_t y = t; y <= b; y++)
				{
					aWaterFineBlockList[x][y] = i;
				}
			}
		}

		// remove tiles that are obscured by land
		for (int32_t x = 0; x < MAX_SMALL_SECTORS; x++)
		{
			float worldX = WATER_START_X + x * SMALL_SECTOR_SIZE - WATER_X_OFFSET;

			for (int32_t y = 0; y < MAX_SMALL_SECTORS; y++)
			{
				if (CWaterLevel::aWaterFineBlockList[x][y] >= 0)
				{
					float worldY = WATER_START_Y + y * SMALL_SECTOR_SIZE;

					int32_t i;
					for (i = 0; i <= 8; i++)
					{
						for (int32_t j = 0; j <= 8; j++)
						{
							CVector worldPos = CVector(worldX + i * (SMALL_SECTOR_SIZE / 8), worldY + j * (SMALL_SECTOR_SIZE / 8), ms_aWaterZs[aWaterFineBlockList[x][y]]);

							if ((worldPos.x > WORLD_MIN_X && worldPos.x < WORLD_MAX_X) && (worldPos.y > WORLD_MIN_Y && worldPos.y < WORLD_MAX_Y) &&
								(!WaterLevelAccordingToRectangles(worldPos.x, worldPos.y) || TestVisibilityForFineWaterBlocks(worldPos)))
								continue;

							// at least one point in the tile wasn't blocked, so don't remove water
							i = 1000;
							break;
						}
					}

					if (i < 1000)
						aWaterFineBlockList[x][y] = NO_WATER;
				}
			}
		}

		RemoveIsolatedWater();

		// calculate coarse tiles from fine tiles
		for (int32_t x = 0; x < MAX_LARGE_SECTORS; x++)
		{
			for (int32_t y = 0; y < MAX_LARGE_SECTORS; y++)
			{
				if (aWaterFineBlockList[x * 2][y * 2] >= 0)
				{
					aWaterBlockList[x][y] = aWaterFineBlockList[x * 2][y * 2];
				}
				else if (aWaterFineBlockList[x * 2 + 1][y * 2] >= 0)
				{
					aWaterBlockList[x][y] = aWaterFineBlockList[x * 2 + 1][y * 2];
				}
				else if (aWaterFineBlockList[x * 2][y * 2 + 1] >= 0)
				{
					aWaterBlockList[x][y] = aWaterFineBlockList[x * 2][y * 2 + 1];
				}
				else if (aWaterFineBlockList[x * 2 + 1][y * 2 + 1] >= 0)
				{
					aWaterBlockList[x][y] = aWaterFineBlockList[x * 2 + 1][y * 2 + 1];
				}
				else
				{
					aWaterBlockList[x][y] = NO_WATER;
				}
			}
		}

		hFile = CFileMgr::OpenFileForWriting(PLUGIN_PATH((char*)"data\\waterpro.dat"));

		if (hFile > 0)
		{
			CFileMgr::Write(hFile, (char*)&ms_nNoOfWaterLevels, sizeof(ms_nNoOfWaterLevels));
			CFileMgr::Write(hFile, (char*)ms_aWaterZs, sizeof(ms_aWaterZs));
			CFileMgr::Write(hFile, (char*)ms_aWaterRects, sizeof(ms_aWaterRects));
			CFileMgr::Write(hFile, (char*)aWaterBlockList, sizeof(aWaterBlockList));
			CFileMgr::Write(hFile, (char*)aWaterFineBlockList, sizeof(aWaterFineBlockList));

			CFileMgr::CloseFile(hFile);
		}

		// collision is streamed in VC
		//CColStore::remove();
	}
#endif

	CTxdStore::PushCurrentTxd();

	int32_t slot = CTxdStore::FindTxdSlot("particle");
	CTxdStore::SetCurrentTxd(slot);

	if (gpWaterTex == NULL)
		gpWaterTex = RwTextureRead("waterclear256", NULL);
	gpWaterRaster = RwTextureGetRaster(gpWaterTex);

	if (gpWaterEnvTex == NULL)
		gpWaterEnvTex = RwTextureRead("waterreflection2", NULL);
	gpWaterEnvRaster = RwTextureGetRaster(gpWaterEnvTex);

#ifdef PC_WATER
	if (gpWaterEnvBaseTex == NULL)
		gpWaterEnvBaseTex = RwTextureRead("sandywater", NULL);
	gpWaterEnvBaseRaster = RwTextureGetRaster(gpWaterEnvBaseTex);
#endif

	if (gpWaterWakeTex == NULL)
		gpWaterWakeTex = RwTextureRead("waterwake", NULL);
	gpWaterWakeRaster = RwTextureGetRaster(gpWaterWakeTex);

	CTxdStore::PopCurrentTxd();

	CreateWavyAtomic();

	debug("Done Initing waterlevels\n");
}

void
CWaterLevel::Shutdown()
{
	DestroyWavyAtomic();

#define _DELETE_TEXTURE(t) if ( t )	\
		{ \
			RwTextureDestroy(t); \
			t = NULL; \
		}

	_DELETE_TEXTURE(gpWaterTex);
	_DELETE_TEXTURE(gpWaterEnvTex);
	_DELETE_TEXTURE(gpWaterWakeTex);
	_DELETE_TEXTURE(gpWaterEnvBaseTex);

#undef _DELETE_TEXTURE
}

void
CWaterLevel::CreateWavyAtomic()
{
	RpGeometry* wavyGeometry;
	RpGeometry* maskGeometry;
	RpMaterial* wavyMaterial;
	RpMaterial* maskMaterial;

	RpTriangle* wavytlist;
	RpTriangle* masktlist;

	RpMorphTarget* wavyMorphTarget;
	RpMorphTarget* maskMorphTarget;

	RwSphere boundingSphere;

	RwV3d* wavyVert;
	RwV3d* wavyNormal;

	RwV3d* maskVert;
	RwV3d* maskNormal;

	RwFrame* wavyFrame;
	RwFrame* maskFrame;

	{
		wavyGeometry = RpGeometryCreate(17 * 17, 512, rpGEOMETRYTRISTRIP
			| rpGEOMETRYTEXTURED
			| rpGEOMETRYPRELIT
			| rpGEOMETRYNORMALS
			| rpGEOMETRYMODULATEMATERIALCOLOR);
#ifdef PC_WATER
		RpGeometryAddMorphTarget(wavyGeometry);
#endif
	}

	{
		maskGeometry = RpGeometryCreate(33 * 33, 2048, rpGEOMETRYTRISTRIP
			| rpGEOMETRYTEXTURED
			| rpGEOMETRYPRELIT
			| rpGEOMETRYNORMALS
			| rpGEOMETRYMODULATEMATERIALCOLOR);
#ifdef PC_WATER
		RpGeometryAddMorphTarget(maskGeometry);
#endif
	}

	{
		wavyMaterial = RpMaterialCreate();
		RpMaterialSetTexture(wavyMaterial, gpWaterTex);
		RwRGBA watercolor = { 255, 255, 255, 192 };
		RpMaterialSetColor(wavyMaterial, &watercolor);
	}

	{
		maskMaterial = RpMaterialCreate();
#ifdef PC_WATER
		RpMaterialSetTexture(maskMaterial, gpWaterEnvBaseTex);
#else
		RpMaterialSetTexture(maskMaterial, gpWaterTex);
#endif
		RwRGBA watercolor = { 255, 255, 255, 192 };
		RpMaterialSetColor(maskMaterial, &watercolor);
	}

	{
		wavytlist = wavyGeometry->triangles;

		for (int32_t i = 0; i < 16; i++)
		{
			for (int32_t j = 0; j < 16; j++)
			{
				const RwUInt16 base = (RwUInt16)((16 + 1) * i + j);

				RpGeometryTriangleSetVertexIndices(wavyGeometry,
					wavytlist, (RwInt16)base, (RwInt16)(base + 1), (RwInt16)(base + 16 + 2));

				RpGeometryTriangleSetVertexIndices(wavyGeometry,
					(wavytlist + 1), (RwInt16)base, (RwInt16)(base + 16 + 2), (RwInt16)(base + 16 + 1));

				RpGeometryTriangleSetMaterial(wavyGeometry, wavytlist, wavyMaterial);

				RpGeometryTriangleSetMaterial(wavyGeometry, (wavytlist + 1), wavyMaterial);

				wavytlist += 2;
			}
		}
	}

	{
		masktlist = maskGeometry->triangles;

		for (int32_t i = 0; i < 32; i++)
		{
			for (int32_t j = 0; j < 32; j++)
			{
				const RwUInt16 base = (RwUInt16)((32 + 1) * i + j);

				RpGeometryTriangleSetVertexIndices(maskGeometry,
					masktlist, (RwInt16)base, (RwInt16)(base + 1), (RwInt16)(base + 32 + 2));

				RpGeometryTriangleSetVertexIndices(maskGeometry,
					(masktlist + 1), (RwInt16)base, (RwInt16)(base + 32 + 2), (RwInt16)(base + 32 + 1));

				RpGeometryTriangleSetMaterial(maskGeometry, masktlist, maskMaterial);

				RpGeometryTriangleSetMaterial(maskGeometry, (masktlist + 1), maskMaterial);

				masktlist += 2;
			}
		}
	}

	{
		wavyMorphTarget = RpGeometryGetMorphTarget(wavyGeometry, 0);
		wavyVert = wavyMorphTarget->verts;
		wavyNormal = wavyMorphTarget->normals;

		for (int32_t i = 0; i < 17; i++)
		{
			for (int32_t j = 0; j < 17; j++)
			{
				(*wavyVert).x = (float)i * 2.0f;
				(*wavyVert).y = (float)j * 2.0f;
				(*wavyVert).z = 0.0f;

				(*wavyNormal).x = 0.0f;
				(*wavyNormal).y = 0.0f;
				(*wavyNormal).z = 1.0f;

				wavyVert++;
				wavyNormal++;
			}
		}

		RpMorphTargetCalcBoundingSphere(wavyMorphTarget, &boundingSphere);
		RpMorphTargetSetBoundingSphere(wavyMorphTarget, &boundingSphere);
		RpGeometryUnlock(wavyGeometry);
	}

	{
		maskMorphTarget = RpGeometryGetMorphTarget(maskGeometry, 0);
		maskVert = maskMorphTarget->verts;
		maskNormal = maskMorphTarget->normals;

		for (int32_t i = 0; i < 33; i++)
		{
			for (int32_t j = 0; j < 33; j++)
			{
				(*maskVert).x = (float)i * 2.0f;
				(*maskVert).y = (float)j * 2.0f;
				(*maskVert).z = 0.0f;

				(*maskNormal).x = 0.0f;
				(*maskNormal).y = 0.0f;
				(*maskNormal).z = 1.0f;

				maskVert++;
				maskNormal++;
			}
		}

		RpMorphTargetCalcBoundingSphere(maskMorphTarget, &boundingSphere);
		RpMorphTargetSetBoundingSphere(maskMorphTarget, &boundingSphere);
		RpGeometryUnlock(maskGeometry);
	}

	{
		wavyFrame = RwFrameCreate();
		ms_pWavyAtomic = RpAtomicCreate();
		RpAtomicSetGeometry(ms_pWavyAtomic, wavyGeometry, 0);
		RpAtomicSetFrame(ms_pWavyAtomic, wavyFrame);
		RpMaterialDestroy(wavyMaterial);
		RpGeometryDestroy(wavyGeometry);
	}

	{
		maskFrame = RwFrameCreate();
		ms_pMaskAtomic = RpAtomicCreate();
		RpAtomicSetGeometry(ms_pMaskAtomic, maskGeometry, 0);
		RpAtomicSetFrame(ms_pMaskAtomic, maskFrame);
		RpMaterialDestroy(maskMaterial);
		RpGeometryDestroy(maskGeometry);
	}

	static RwFrame* wakeEnvFrame;

	if (wakeEnvFrame == NULL)
	{
		wakeEnvFrame = RwFrameCreate();
		RwMatrixSetIdentity(RwFrameGetMatrix(wakeEnvFrame));
		RwFrameUpdateObjects(wakeEnvFrame);
	}

	RpMatFXMaterialSetEffects(maskMaterial, rpMATFXEFFECTENVMAP);
	RpMatFXMaterialSetupEnvMap(maskMaterial, gpWaterEnvTex, wakeEnvFrame, TRUE, fEnvScale);
	RpMatFXAtomicEnableEffects(ms_pMaskAtomic);
}

void
CWaterLevel::DestroyWavyAtomic()
{
#define _DELETE_ATOMIC(a) \
		{ \
			RwFrame *frame; \
			frame = RpAtomicGetFrame(a); \
			RpAtomicDestroy(a); \
			RwFrameDestroy(frame); \
		}

	_DELETE_ATOMIC(ms_pWavyAtomic);
	_DELETE_ATOMIC(ms_pMaskAtomic);

#undef _DELETE_ATOMIC
}

#ifndef MASTER
void
CWaterLevel::AddWaterLevel(float fXLeft, float fYBottom, float fXRight, float fYTop, float fLevel)
{
	ms_aWaterRects[ms_nNoOfWaterLevels] = CRect(fXLeft, fYBottom, fXRight, fYTop);
	ms_aWaterZs[ms_nNoOfWaterLevels] = fLevel;
	ms_nNoOfWaterLevels++;
}

bool
CWaterLevel::WaterLevelAccordingToRectangles(float fX, float fY, float* pfOutLevel)
{
	if (ms_nNoOfWaterLevels <= 0) return false;

	for (int32_t i = 0; i < ms_nNoOfWaterLevels; i++)
	{
		if (fX >= ms_aWaterRects[i].left && fX <= ms_aWaterRects[i].right
			&& fY >= ms_aWaterRects[i].top && fY <= ms_aWaterRects[i].bottom)
		{
			if (pfOutLevel) *pfOutLevel = ms_aWaterZs[i];

			return true;
		}
	}

	return false;
}

bool
CWaterLevel::TestVisibilityForFineWaterBlocks(const CVector& worldPos)
{
	static CVector2D tab[] =
	{
		{ 50.0f, 50.0f },
		{ -50.0f, 50.0f },
		{ -50.0f, -50.0f },
		{ 50.0f, -50.0f },
		{ 50.0f, 0.0f },
		{ -50.0f, 0.0f },
		{ 0.0f, -50.0f },
		{ 0.0f, 50.0f },
	};

	CEntity* entity;
	CColPoint col;
	CVector lineStart, lineEnd;

	lineStart = worldPos;

	if (!CWorld::ProcessVerticalLine(lineStart, lineStart.z + 100.0f, col, entity, true, false, false, false, true, false, NULL))
	{
		lineStart.x += 0.4f;
		lineStart.y += 0.4f;

		if (!CWorld::ProcessVerticalLine(lineStart, lineStart.z + 100.0f, col, entity, true, false, false, false, true, false, NULL))
		{
			return false;
		}
	}

	for (int32_t i = 0; i < ARRAY_SIZE(tab); i++)
	{
		lineStart = worldPos;
		lineEnd = worldPos;

		lineEnd.x += tab[i].x;
		lineEnd.y += tab[i].y;
		lineEnd.z += 100.0f;

		if ((lineEnd.x > WORLD_MIN_X && lineEnd.x < WORLD_MAX_X) && (lineEnd.y > WORLD_MIN_Y && lineEnd.y < WORLD_MAX_Y))
		{
			if (!CWorld::ProcessLineOfSight(lineStart, lineEnd, col, entity, true, false, false, false, true, false, false, false))
			{
				lineStart.x += 0.4f;
				lineStart.y += 0.4f;
				lineEnd.x += 0.4f;
				lineEnd.y += 0.4f;

				if (!CWorld::ProcessLineOfSight(lineStart, lineEnd, col, entity, true, false, false, false, true, false, false, false))
				{
					return false;
				}
			}
		}
	}

	return true;
}

void
CWaterLevel::RemoveIsolatedWater()
{
	bool(*isConnected)[MAX_SMALL_SECTORS] = new bool[MAX_SMALL_SECTORS][MAX_SMALL_SECTORS];

	for (int32_t x = 0; x < MAX_SMALL_SECTORS; x++)
	{
		for (int32_t y = 0; y < MAX_SMALL_SECTORS; y++)
		{
			isConnected[x][y] = false;
		}
	}

	isConnected[0][0] = true;
	bool keepGoing;

	do
	{
		keepGoing = false;

		for (int32_t x = 0; x < MAX_SMALL_SECTORS; x++)
		{
			for (int32_t y = 0; y < MAX_SMALL_SECTORS; y++)
			{
				if (aWaterFineBlockList[x][y] < 0 || isConnected[x][y])
					continue;

				if (x > 0 && isConnected[x - 1][y])
				{
					isConnected[x][y] = true;
					keepGoing = true;
				}

				if (y > 0 && isConnected[x][y - 1])
				{
					isConnected[x][y] = true;
					keepGoing = true;
				}

				if (x + 1 < MAX_SMALL_SECTORS && isConnected[x + 1][y])
				{
					isConnected[x][y] = true;
					keepGoing = true;
				}

				if (y + 1 < MAX_SMALL_SECTORS && isConnected[x][y + 1])
				{
					isConnected[x][y] = true;
					keepGoing = true;
				}
			}
		}
	} while (keepGoing);

	int32_t numRemoved = 0;

	for (int32_t x = 0; x < MAX_SMALL_SECTORS; x++)
	{
		for (int32_t y = 0; y < MAX_SMALL_SECTORS; y++)
		{
			if (aWaterFineBlockList[x][y] >= 0 && !isConnected[x][y] && ms_aWaterZs[aWaterFineBlockList[x][y]] == 6.0f)
			{
				numRemoved++;
				aWaterFineBlockList[x][y] = NO_WATER;
			}
		}
	}

	debug("Removed %d isolated patches of water\n", numRemoved);

	delete[] isConnected;
}
#endif

bool
CWaterLevel::GetWaterLevel(float fX, float fY, float fZ, float* pfOutLevel, bool bDontCheckZ)
{
	int32_t x = WATER_TO_SMALL_SECTOR_X(fX + WATER_X_OFFSET);
	int32_t y = WATER_TO_SMALL_SECTOR_Y(fY);

#ifdef FIX_BUGS
	if (x < 0 || x >= MAX_SMALL_SECTORS) return false;
	if (y < 0 || y >= MAX_SMALL_SECTORS) return false;
#endif

	int8_t nBlock = aWaterFineBlockList[x][y];

	if (nBlock == NO_WATER)
		return false;

	assert(pfOutLevel != NULL);
	*pfOutLevel = ms_aWaterZs[nBlock];

	float fAngle = (CTimer::m_snTimeInMilliseconds & 4095) * (TWOPI / 4096.0f);

	float fWave = sinf
	(
		(WATER_UNSIGN_Y(fY) - y * SMALL_SECTOR_SIZE
			+ WATER_UNSIGN_X(fX + WATER_X_OFFSET) - x * SMALL_SECTOR_SIZE)

		* (TWOPI / SMALL_SECTOR_SIZE) + fAngle
	);

	float fWindFactor = CWeather::WindClipped * 0.4f + 0.2f;

	*pfOutLevel += fWave * fWindFactor;

	if (bDontCheckZ == false && (*pfOutLevel - fZ) > 3.0f)
	{
		*pfOutLevel = 0.0f;
		return false;
	}

	return true;
}

bool
CWaterLevel::GetWaterLevelNoWaves(float fX, float fY, float fZ, float* pfOutLevel)
{
	int32_t x = WATER_TO_SMALL_SECTOR_X(fX + WATER_X_OFFSET);
	int32_t y = WATER_TO_SMALL_SECTOR_Y(fY);

#ifdef FIX_BUGS
	if (x < 0 || x >= MAX_SMALL_SECTORS) return false;
	if (y < 0 || y >= MAX_SMALL_SECTORS) return false;
#endif

	int8_t nBlock = aWaterFineBlockList[x][y];

	if (nBlock == NO_WATER)
		return false;

	assert(pfOutLevel != NULL);
	*pfOutLevel = ms_aWaterZs[nBlock];

	return true;
}

float
CWaterLevel::GetWaterWavesOnly(short x, short y)
{
	float fAngle = (CTimer::m_snTimeInMilliseconds & 4095) * (TWOPI / 4096.0f);

	float fWindFactor = CWeather::WindClipped * 0.7f + 0.3f;

	float fWave = sinf(float(float(4 * y + 4 * x) * (TWOPI / SMALL_SECTOR_SIZE)) + fAngle);

	return fWave * fWindFactor;
}

CVector
CWaterLevel::GetWaterNormal(float fX, float fY)
{
	//TODO: BUG ? no x offset

	int32_t x = WATER_TO_SMALL_SECTOR_X(fX);
	int32_t y = WATER_TO_SMALL_SECTOR_Y(fY);

	float fAngle = (CTimer::m_snTimeInMilliseconds & 4095) * (TWOPI / 4096.0f);
	float fWindFactor = CWeather::WindClipped * 0.4f + 0.2f;

	float _fWave = (WATER_UNSIGN_Y(fY) - y * SMALL_SECTOR_SIZE + WATER_UNSIGN_X(fX) - x * SMALL_SECTOR_SIZE)
		* (TWOPI / SMALL_SECTOR_SIZE) + fAngle;

	CVector vA(1.0f, 0.0f, fWindFactor * (TWOPI / SMALL_SECTOR_SIZE) * cosf(_fWave));
	CVector vB(0.0f, 1.0f, fWindFactor * (TWOPI / SMALL_SECTOR_SIZE) * cosf(_fWave));

	CVector norm = CrossProduct(vA, vB);

	norm.Normalise();

	return norm;
}


inline float
_GetWaterDrawDist()
{
	if (TheCamera.GetPosition().z < 15.0f) return 1200.0f;
	if (TheCamera.GetPosition().z > 60.0f) return 2000.0f;
	return (TheCamera.GetPosition().z + -15.0f) * 800.0f / 45.0f + 1200.0f;
}

inline float
_GetWavyDrawDist()
{
	if (FindPlayerVehicle(-1, false) && FindPlayerVehicle(-1, false)->m_nVehicleSubClass == VEHICLE_BOAT)
		return 120.0f;
	else
		return 70.0f;
}

inline void
_GetCamBounds(bool* bUseCamStartY, bool* bUseCamEndY, bool* bUseCamStartX, bool* bUseCamEndX)
{
	if (TheCamera.GetForward().z > -0.8f)
	{
		if (fabsf(TheCamera.GetForward().x) > fabsf(TheCamera.GetForward().y))
		{
			if (TheCamera.GetForward().x > 0.0f)
				*bUseCamStartX = true;
			else
				*bUseCamEndX = true;
		}
		else
		{
			if (TheCamera.GetForward().y > 0.0f)
				*bUseCamStartY = true;
			else
				*bUseCamEndY = true;
		}
	}
}


inline bool
_IsColideWithBlock(int32_t x, int32_t y, int32_t& block)
{
	block = CWaterLevel::aWaterFineBlockList[x + 0][y + 0];
	if (block >= 0)
		return true;

	block = CWaterLevel::aWaterFineBlockList[x + 0][y + 1];
	if (block >= 0)
	{
		block = CWaterLevel::aWaterFineBlockList[x + 0][y + 2];
		if (block >= 0)
			return true;
	}

	block = CWaterLevel::aWaterFineBlockList[x + 1][y + 0];
	if (block >= 0)
		return true;

	block = CWaterLevel::aWaterFineBlockList[x + 1][y + 1];
	if (block >= 0)
	{
		block = CWaterLevel::aWaterFineBlockList[x + 1][y + 2];
		if (block >= 0)
			return true;
	}

	block = CWaterLevel::aWaterFineBlockList[x + 2][y + 0];
	if (block >= 0)
		return true;

	block = CWaterLevel::aWaterFineBlockList[x + 2][y + 1];
	if (block >= 0)
	{
		block = CWaterLevel::aWaterFineBlockList[x + 2][y + 2];
		if (block >= 0)
			return true;
	}

	return false;
}

inline float
SectorRadius(float fSize)
{
	return Sqrt(Pow(fSize, 2) + Pow(fSize, 2));
}

void
CWaterLevel::RenderWater()
{
	//"Custom" Don't Render Water Toggle
#ifndef MASTER
	if (gbDontRenderWater)
		return;
#endif
	bool bUseCamEndX = false;
	bool bUseCamStartY = false;

	bool bUseCamStartX = false;
	bool bUseCamEndY = false;

	if (!CGame::CanSeeWaterFromCurrArea())
		return;

	_GetCamBounds(&bUseCamStartY, &bUseCamEndY, &bUseCamStartX, &bUseCamEndX);

	float fHugeSectorMaxRenderDist = _GetWaterDrawDist();
	float fHugeSectorMaxRenderDistSqr = SQR(fHugeSectorMaxRenderDist);

	float windAddUV = CWeather::WindClipped * 0.0005f + 0.0006f;

	float fAngle = (CTimer::m_snTimeInMilliseconds & 4095) * (TWOPI / 4096.0f);

	if (!CTimer::m_UserPause)
	{
		TEXTURE_ADDU += windAddUV;
		TEXTURE_ADDV += windAddUV;

		_TEXTURE_MASK_ADDU += sinf(fAngle) * 0.0005f + 1.1f * windAddUV;
		_TEXTURE_MASK_ADDV -= cosf(fAngle * 1.3f) * 0.0005f + 1.2f * windAddUV;

		_TEXTURE_WAKE_ADDU -= sinf(fAngle) * 0.0003f + windAddUV;
		_TEXTURE_WAKE_ADDV += cosf(fAngle * 0.7f) * 0.0003f + windAddUV;
	}

	if (_TEXTURE_MASK_ADDU >= 1.0f)
		_TEXTURE_MASK_ADDU = 0.0f;
	if (_TEXTURE_MASK_ADDV >= 1.0f)
		_TEXTURE_MASK_ADDV = 0.0f;

	if (_TEXTURE_WAKE_ADDU >= 1.0f)
		_TEXTURE_WAKE_ADDU = 0.0f;
	if (_TEXTURE_WAKE_ADDV >= 1.0f)
		_TEXTURE_WAKE_ADDV = 0.0f;

	if (TEXTURE_ADDU >= 1.0f)
		TEXTURE_ADDU = 0.0f;
	if (TEXTURE_ADDV >= 1.0f)
		TEXTURE_ADDV = 0.0f;

#ifdef PC_WATER
	_fWaterZOffset = CWeather::WindClipped * 0.5f + 0.25f;
#endif

	RwRGBA color = { 0, 0, 0, 255 };

	color.red = CTimeCycle::m_CurrentColours.m_fWaterRed;
	color.green = CTimeCycle::m_CurrentColours.m_fWaterGreen;
	color.blue = CTimeCycle::m_CurrentColours.m_fWaterBlue;

#ifndef PC_WATER
	RwRGBA colorUnderwater = { 0, 0, 0, 255 };
	colorUnderwater.red = (uint32_t)(0.8f * (float)colorUnderwater.red);
	colorUnderwater.green = (uint32_t)(0.8f * (float)colorUnderwater.green);
	colorUnderwater.blue = (uint32_t)(0.8f * (float)colorUnderwater.blue);
#endif

	TempBufferVerticesStored = 0;
	TempBufferIndicesStored = 0;

#ifndef PC_WATER
	WavesCalculatedThisFrame = false;
#endif

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)gpWaterRaster);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);

	CVector2D camPos(TheCamera.GetPosition().x, TheCamera.GetPosition().y);

	int32_t nStartX = WATER_TO_HUGE_SECTOR_X(camPos.x - fHugeSectorMaxRenderDist + WATER_X_OFFSET);
	int32_t nEndX = WATER_TO_HUGE_SECTOR_X(camPos.x + fHugeSectorMaxRenderDist + WATER_X_OFFSET) + 1;
	int32_t nStartY = WATER_TO_HUGE_SECTOR_Y(camPos.y - fHugeSectorMaxRenderDist);
	int32_t nEndY = WATER_TO_HUGE_SECTOR_Y(camPos.y + fHugeSectorMaxRenderDist) + 1;

	if (bUseCamStartX)
		nStartX = WATER_TO_HUGE_SECTOR_X(camPos.x + WATER_X_OFFSET);
	if (bUseCamEndX)
		nEndX = WATER_TO_HUGE_SECTOR_X(camPos.x + WATER_X_OFFSET);
	if (bUseCamStartY)
		nStartY = WATER_TO_HUGE_SECTOR_Y(camPos.y);
	if (bUseCamEndY)
		nEndY = WATER_TO_HUGE_SECTOR_Y(camPos.y);

	nStartX = Clamp(nStartX, 0, MAX_HUGE_SECTORS - 1);
	nEndX = Clamp(nEndX, 0, MAX_HUGE_SECTORS - 1);
	nStartY = Clamp(nStartY, 0, MAX_HUGE_SECTORS - 1);
	nEndY = Clamp(nEndY, 0, MAX_HUGE_SECTORS - 1);

	for (int32_t x = nStartX; x <= nEndX; x++)
	{
		for (int32_t y = nStartY; y <= nEndY; y++)
		{
			if (aWaterBlockList[2 * x + 0][2 * y + 0] >= 0
				|| aWaterBlockList[2 * x + 1][2 * y + 0] >= 0
				|| aWaterBlockList[2 * x + 0][2 * y + 1] >= 0
				|| aWaterBlockList[2 * x + 1][2 * y + 1] >= 0)
			{
				float fX = WATER_FROM_HUGE_SECTOR_X(x) - WATER_X_OFFSET;
				float fY = WATER_FROM_HUGE_SECTOR_Y(y);

				CVector2D vecHugeSectorCentre(fX + HUGE_SECTOR_SIZE / 2, fY + HUGE_SECTOR_SIZE / 2);

				float fHugeSectorDistToCamSqr = (camPos - vecHugeSectorCentre).MagnitudeSqr();

				if (fHugeSectorMaxRenderDistSqr > fHugeSectorDistToCamSqr)
				{
					if (TheCamera.IsSphereVisible(CVector(vecHugeSectorCentre.x, vecHugeSectorCentre.y, 0.0f), SectorRadius(HUGE_SECTOR_SIZE)))
					{
#ifndef PC_WATER
						WavesCalculatedThisFrame = true;
#endif


						float fZ;

						if (aWaterBlockList[2 * x + 0][2 * y + 0] >= 0)
							fZ = ms_aWaterZs[aWaterBlockList[2 * x + 0][2 * y + 0]];

						if (aWaterBlockList[2 * x + 1][2 * y + 0] >= 0)
							fZ = ms_aWaterZs[aWaterBlockList[2 * x + 1][2 * y + 0]];

						if (aWaterBlockList[2 * x + 0][2 * y + 1] >= 0)
							fZ = ms_aWaterZs[aWaterBlockList[2 * x + 0][2 * y + 1]];

						if (aWaterBlockList[2 * x + 1][2 * y + 1] >= 0)
							fZ = ms_aWaterZs[aWaterBlockList[2 * x + 1][2 * y + 1]];

						if (fHugeSectorDistToCamSqr >= SQR(500.0f))
						{
							RenderOneFlatHugeWaterPoly(fX, fY, fZ, color);
						}
						else
						{
#ifndef PC_WATER
							if (m_bRenderSeaBed)
								RenderOneSlopedUnderWaterPoly(fX, fY, fZ, colorUnderwater);
#endif
							// see RenderTransparentWater()
							;
						}
					}
				}
			}
		}
	}

	/*
	 -----------      ----------------------       ----------------------
	|    [N]    |    |       [ EndY ]       |     |       [  top ]       |
	|           |    |                      |     |                      |
	|[W] [0] [E]|    |[StartX]  []  [ EndX ]|     |[ left ]  []  [ right]|
	|           |    |                      |     |                      |
	|    [S]    |    |       [StartY]       |     |       [bottom]       |
	 -----------      ----------------------       ----------------------


	[S] [StartY]  [bottom]
	[N] [EndY]    [top]
	[W] [StartX]  [left]
	[E] [EndX]    [right]

	[S]    -> [N] && [W]  -> [E]
	bottom -> top && left -> right
	*/

	for (int32_t x = 0; x < 26; x++)
	{
		for (int32_t y = 0; y < 5; y++)
		{
			float fX = WATER_SIGN_X(float(x) * EXTRAHUGE_SECTOR_SIZE) - 1280.0f - WATER_X_OFFSET;
			float fY = WATER_SIGN_Y(float(y) * EXTRAHUGE_SECTOR_SIZE) - 1280.0f;

			if (!bUseCamStartY)
			{
				CVector2D vecExtraHugeSectorCentre(fX + EXTRAHUGE_SECTOR_SIZE / 2, fY + EXTRAHUGE_SECTOR_SIZE / 2);

				float fCamDistToSector = (vecExtraHugeSectorCentre - camPos).Magnitude();

				if (fCamDistToSector < fHugeSectorMaxRenderDistSqr)
				{
					if (TheCamera.IsSphereVisible(CVector(vecExtraHugeSectorCentre.x, vecExtraHugeSectorCentre.y, 0.0f), SectorRadius(EXTRAHUGE_SECTOR_SIZE)))
					{
						RenderOneFlatExtraHugeWaterPoly(
							vecExtraHugeSectorCentre.x - EXTRAHUGE_SECTOR_SIZE / 2,
							vecExtraHugeSectorCentre.y - EXTRAHUGE_SECTOR_SIZE / 2,
							0.0f,
							color);
					}
				}
			}

			if (!bUseCamEndY)
			{
				CVector2D vecExtraHugeSectorCentre(fX + EXTRAHUGE_SECTOR_SIZE / 2, -(fY + EXTRAHUGE_SECTOR_SIZE / 2));

				float fCamDistToSector = (vecExtraHugeSectorCentre - camPos).Magnitude();

				if (fCamDistToSector < fHugeSectorMaxRenderDistSqr)
				{
					if (TheCamera.IsSphereVisible(CVector(vecExtraHugeSectorCentre.x, vecExtraHugeSectorCentre.y, 0.0f), SectorRadius(EXTRAHUGE_SECTOR_SIZE)))
					{
						RenderOneFlatExtraHugeWaterPoly(
							vecExtraHugeSectorCentre.x - EXTRAHUGE_SECTOR_SIZE / 2,
							vecExtraHugeSectorCentre.y - EXTRAHUGE_SECTOR_SIZE / 2,
							0.0f,
							color);
					}
				}
			}
		}
	}

	for (int32_t y = 5; y < 21; y++)
	{
		for (int32_t x = 0; x < 5; x++)
		{
			float fX = WATER_SIGN_X(float(x) * EXTRAHUGE_SECTOR_SIZE) - 1280.0f - WATER_X_OFFSET;
			float fX2 = WATER_SIGN_X(float(x) * EXTRAHUGE_SECTOR_SIZE) - 1280.0f + WATER_X_OFFSET;
			float fY = WATER_SIGN_Y(float(y) * EXTRAHUGE_SECTOR_SIZE) - 1280.0f;

			if (!bUseCamStartX)
			{
				CVector2D vecExtraHugeSectorCentre(fX + EXTRAHUGE_SECTOR_SIZE / 2, fY + EXTRAHUGE_SECTOR_SIZE / 2);

				float fCamDistToSector = (vecExtraHugeSectorCentre - camPos).Magnitude();

				if (fCamDistToSector < fHugeSectorMaxRenderDistSqr)
				{
					if (TheCamera.IsSphereVisible(CVector(vecExtraHugeSectorCentre.x, vecExtraHugeSectorCentre.y, 0.0f), SectorRadius(EXTRAHUGE_SECTOR_SIZE)))
					{
						RenderOneFlatExtraHugeWaterPoly(
							vecExtraHugeSectorCentre.x - EXTRAHUGE_SECTOR_SIZE / 2,
							vecExtraHugeSectorCentre.y - EXTRAHUGE_SECTOR_SIZE / 2,
							0.0f,
							color);
					}
				}
			}

			if (!bUseCamEndX)
			{
				CVector2D vecExtraHugeSectorCentre(-(fX2 + EXTRAHUGE_SECTOR_SIZE / 2), fY + EXTRAHUGE_SECTOR_SIZE / 2);

				float fCamDistToSector = (vecExtraHugeSectorCentre - camPos).Magnitude();

				if (fCamDistToSector < fHugeSectorMaxRenderDistSqr)
				{
					if (TheCamera.IsSphereVisible(CVector(vecExtraHugeSectorCentre.x, vecExtraHugeSectorCentre.x, 0.0f), SectorRadius(EXTRAHUGE_SECTOR_SIZE)))
					{
						RenderOneFlatExtraHugeWaterPoly(
							vecExtraHugeSectorCentre.x - EXTRAHUGE_SECTOR_SIZE / 2,
							vecExtraHugeSectorCentre.y - EXTRAHUGE_SECTOR_SIZE / 2,
							0.0f,
							color);
					}
				}
			}
		}
	}

	RenderAndEmptyRenderBuffer();

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	//if (WavesCalculatedThisFrame)
	//{
	//	RenderSeaBirds();
	//	RenderShipsOnHorizon();
		//CParticle::HandleShipsAtHorizonStuff();
		//HandleBeachToysStuff();
	//}

	//if (_bSeaLife)
		//HandleSeaLifeForms();

	DefinedState();
}


void
CWaterLevel::RenderTransparentWater(void)
{
	bool bUseCamEndX = false;
	bool bUseCamStartY = false;

	bool bUseCamStartX = false;
	bool bUseCamEndY = false;

	_bSeaLife = false;

	if (!CGame::CanSeeWaterFromCurrArea())
		return;

	//PUSH_RENDERGROUP("CWaterLevel::RenderTransparentWater");

	float fWaterDrawDist = _GetWavyDrawDist();
	float fWaterDrawDistLarge = fWaterDrawDist + 90.0f;
	float fWavySectorMaxRenderDistSqr = SQR(fWaterDrawDist);

	_GetCamBounds(&bUseCamStartY, &bUseCamEndY, &bUseCamStartX, &bUseCamEndX);

	float fHugeSectorMaxRenderDist = _GetWaterDrawDist();
	float fHugeSectorMaxRenderDistSqr = SQR(fHugeSectorMaxRenderDist);

	RenderBoatWakes();

	RwRGBA color;

	color.red = CTimeCycle::m_CurrentColours.m_fWaterRed;
	color.green = CTimeCycle::m_CurrentColours.m_fWaterGreen;
	color.blue = CTimeCycle::m_CurrentColours.m_fWaterBlue;
	color.alpha = 255;

	RwRGBA colorTrans;

	colorTrans.red = CTimeCycle::m_CurrentColours.m_fWaterRed;
	colorTrans.green = CTimeCycle::m_CurrentColours.m_fWaterGreen;
	colorTrans.blue = CTimeCycle::m_CurrentColours.m_fWaterBlue;
	colorTrans.alpha = CTimeCycle::m_CurrentColours.m_fWaterAlpha;

	TempBufferVerticesStored = 0;
	TempBufferIndicesStored = 0;

#ifndef PC_WATER
	WavesCalculatedThisFrame = false;
#endif

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)gpWaterRaster);
#ifndef PC_WATER
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
#endif

	CVector2D camPos(TheCamera.GetPosition().x, TheCamera.GetPosition().y);

	int32_t nStartX = WATER_TO_HUGE_SECTOR_X(camPos.x - fHugeSectorMaxRenderDist + WATER_X_OFFSET);
	int32_t nEndX = WATER_TO_HUGE_SECTOR_X(camPos.x + fHugeSectorMaxRenderDist + WATER_X_OFFSET) + 1;
	int32_t nStartY = WATER_TO_HUGE_SECTOR_Y(camPos.y - fHugeSectorMaxRenderDist);
	int32_t nEndY = WATER_TO_HUGE_SECTOR_Y(camPos.y + fHugeSectorMaxRenderDist) + 1;

	if (bUseCamStartX)
		nStartX = WATER_TO_HUGE_SECTOR_X(camPos.x + WATER_X_OFFSET);
	if (bUseCamEndX)
		nEndX = WATER_TO_HUGE_SECTOR_X(camPos.x + WATER_X_OFFSET);
	if (bUseCamStartY)
		nStartY = WATER_TO_HUGE_SECTOR_Y(camPos.y);
	if (bUseCamEndY)
		nEndY = WATER_TO_HUGE_SECTOR_Y(camPos.y);

	nStartX = Clamp(nStartX, 0, MAX_HUGE_SECTORS - 1);
	nEndX = Clamp(nEndX, 0, MAX_HUGE_SECTORS - 1);
	nStartY = Clamp(nStartY, 0, MAX_HUGE_SECTORS - 1);
	nEndY = Clamp(nEndY, 0, MAX_HUGE_SECTORS - 1);


	for (int32_t x = nStartX; x <= nEndX; x++)
	{
		for (int32_t y = nStartY; y <= nEndY; y++)
		{
			if (aWaterBlockList[2 * x + 0][2 * y + 0] >= 0
				|| aWaterBlockList[2 * x + 1][2 * y + 0] >= 0
				|| aWaterBlockList[2 * x + 0][2 * y + 1] >= 0
				|| aWaterBlockList[2 * x + 1][2 * y + 1] >= 0)
			{
				float fX = WATER_FROM_HUGE_SECTOR_X(x) - WATER_X_OFFSET;
				float fY = WATER_FROM_HUGE_SECTOR_Y(y);

				CVector2D vecHugeSectorCentre
				(
					fX + HUGE_SECTOR_SIZE / 2,
					fY + HUGE_SECTOR_SIZE / 2
				);

				float fHugeSectorDistToCamSqr = (camPos - vecHugeSectorCentre).MagnitudeSqr();

				if (fHugeSectorMaxRenderDistSqr > fHugeSectorDistToCamSqr)
				{
					if (TheCamera.IsSphereVisible(CVector(vecHugeSectorCentre.x, vecHugeSectorCentre.y, 0.0f), SectorRadius(HUGE_SECTOR_SIZE)))
					{
						if (fHugeSectorDistToCamSqr >= SQR(500.0f))
						{
							// see RenderWater()
							;
						}
						else
						{
							for (int32_t x2 = 2 * x; x2 <= 2 * x + 1; x2++)
							{
								for (int32_t y2 = 2 * y; y2 <= 2 * y + 1; y2++)
								{
									if (aWaterBlockList[x2][y2] >= 0)
									{
										float fLargeX = WATER_FROM_LARGE_SECTOR_X(x2) - WATER_X_OFFSET;
										float fLargeY = WATER_FROM_LARGE_SECTOR_Y(y2);

										CVector2D vecLargeSectorCentre(fLargeX + LARGE_SECTOR_SIZE / 2, fLargeY + LARGE_SECTOR_SIZE / 2);

										float fLargeSectorDistToCamSqr = (camPos - vecLargeSectorCentre).MagnitudeSqr();

										if (fLargeSectorDistToCamSqr < fHugeSectorMaxRenderDistSqr)
										{
											if (TheCamera.IsSphereVisible(CVector(vecLargeSectorCentre.x, vecLargeSectorCentre.y, 0.0f), SectorRadius(LARGE_SECTOR_SIZE)))
											{
												// Render four small(32x32) sectors, or one large(64x64).

												//														
												//	       [N]
												//	    ---------
												//	    |0x1|1x1|
												//	[W] --------- [E]
												//	    |0x0|1x0|
												//	    ---------
												//	       [S]
												//	

												float fLargeSectorDrawDistSqr = SQR((fWaterDrawDistLarge + 16.0f));

												if (fLargeSectorDistToCamSqr < fLargeSectorDrawDistSqr)
												{
													_bSeaLife = true;

													float fZ;

													// WS
													if (aWaterFineBlockList[2 * x2 + 0][2 * y2 + 0] >= 0)
													{
														float fSmallX = fLargeX;
														float fSmallY = fLargeY;

														CVector2D vecSmallSectorCentre(fSmallX + SMALL_SECTOR_SIZE / 2, fSmallY + SMALL_SECTOR_SIZE / 2);

														float fSmallSectorDistToCamSqr = (camPos - vecSmallSectorCentre).MagnitudeSqr();
														fZ = ms_aWaterZs[aWaterFineBlockList[2 * x2 + 0][2 * y2 + 0]];

														if (fSmallSectorDistToCamSqr < fWavySectorMaxRenderDistSqr)
															RenderOneWavySector(fSmallX, fSmallY, fZ, colorTrans);
														else
															RenderOneFlatSmallWaterPolyBlended(fSmallX, fSmallY, fZ, camPos.x, camPos.y, color, colorTrans, fWaterDrawDist);
													}

													// SE
													if (aWaterFineBlockList[2 * x2 + 1][2 * y2 + 0] >= 0)
													{
														float fSmallX = fLargeX + (LARGE_SECTOR_SIZE / 2);
														float fSmallY = fLargeY;

														CVector2D vecSmallSectorCentre(fSmallX + SMALL_SECTOR_SIZE / 2, fSmallY + SMALL_SECTOR_SIZE / 2);

														float fSmallSectorDistToCamSqr = (camPos - vecSmallSectorCentre).MagnitudeSqr();
														fZ = ms_aWaterZs[aWaterFineBlockList[2 * x2 + 1][2 * y2 + 0]];

														if (fSmallSectorDistToCamSqr < fWavySectorMaxRenderDistSqr)
															RenderOneWavySector(fSmallX, fSmallY, fZ, colorTrans);
														else
															RenderOneFlatSmallWaterPolyBlended(fSmallX, fSmallY, fZ, camPos.x, camPos.y, color, colorTrans, fWaterDrawDist);
													}

													// WN
													if (aWaterFineBlockList[2 * x2 + 0][2 * y2 + 1] >= 0)
													{
														float fSmallX = fLargeX;
														float fSmallY = fLargeY + (LARGE_SECTOR_SIZE / 2);

														CVector2D vecSmallSectorCentre(fSmallX + SMALL_SECTOR_SIZE / 2, fSmallY + SMALL_SECTOR_SIZE / 2);

														float fSmallSectorDistToCamSqr = (camPos - vecSmallSectorCentre).MagnitudeSqr();
														fZ = ms_aWaterZs[aWaterFineBlockList[2 * x2 + 0][2 * y2 + 1]];

														if (fSmallSectorDistToCamSqr < fWavySectorMaxRenderDistSqr)
															RenderOneWavySector(fSmallX, fSmallY, fZ, colorTrans);
														else
															RenderOneFlatSmallWaterPolyBlended(fSmallX, fSmallY, fZ, camPos.x, camPos.y, color, colorTrans, fWaterDrawDist);
													}

													//NE
													if (aWaterFineBlockList[2 * x2 + 1][2 * y2 + 1] >= 0)
													{
														float fSmallX = fLargeX + (LARGE_SECTOR_SIZE / 2);
														float fSmallY = fLargeY + (LARGE_SECTOR_SIZE / 2);

														CVector2D vecSmallSectorCentre(fSmallX + SMALL_SECTOR_SIZE / 2, fSmallY + SMALL_SECTOR_SIZE / 2);

														float fSmallSectorDistToCamSqr = (camPos - vecSmallSectorCentre).MagnitudeSqr();
														fZ = ms_aWaterZs[aWaterFineBlockList[2 * x2 + 1][2 * y2 + 1]];

														if (fSmallSectorDistToCamSqr < fWavySectorMaxRenderDistSqr)
															RenderOneWavySector(fSmallX, fSmallY, fZ, colorTrans);
														else
															RenderOneFlatSmallWaterPolyBlended(fSmallX, fSmallY, fZ, camPos.x, camPos.y, color, colorTrans, fWaterDrawDist);
													}
												}
												else
												{
													float fZ;

													fZ = ms_aWaterZs[aWaterBlockList[x2][y2]];

													RenderOneFlatLargeWaterPoly(fLargeX, fLargeY, fZ, color);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	RenderAndEmptyRenderBuffer();

#ifdef PC_WATER
	if (MaskCalculatedThisFrame
		&& (m_nRenderWaterLayers == 0 || m_nRenderWaterLayers == 2 || m_nRenderWaterLayers == 3))
	{
		RwV3d pos = { 0.0f, 0.0f, 0.0f };

		pos.x = PreCalculatedMaskPosn.x;
		pos.y = PreCalculatedMaskPosn.y;
		pos.z = PreCalculatedMaskPosn.z;

		RpMatFXMaterialSetEnvMapFrame(RpGeometryGetMaterial(RpAtomicGetGeometry(ms_pMaskAtomic), 0),
			RwCameraGetFrame(RwCameraGetCurrentCamera()));

		RwFrameTranslate(RpAtomicGetFrame(ms_pMaskAtomic), &pos, rwCOMBINEREPLACE);

		RpAtomicRender(ms_pMaskAtomic);
	}
#else
	//if (!CCullZones::water())
	//{
		int32_t signX = 0;
		int32_t signY = 0;

		float fCamX = camPos.x - SMALL_SECTOR_SIZE;
		float fCamY = camPos.y - SMALL_SECTOR_SIZE;

		if (TheCamera.GetForward().x > 0.3f)
			signX = 1;
		else if (TheCamera.GetForward().x < -0.3f)
			signX = -1;

		fCamX += 0.3f * (float)signX * float(SMALL_SECTOR_SIZE * 2.0f); // 19.2f

		if (TheCamera.GetForward().y > 0.3f)
			signY = 1;
		else if (TheCamera.GetForward().y < -0.3f)
			signY = -1;

		fCamY += 0.3f * (float)signY * float(SMALL_SECTOR_SIZE * 2.0f); // 19.2f

		int32_t nBlock;

		int32_t BlockX = WATER_TO_SMALL_SECTOR_X(fCamX + WATER_X_OFFSET) + 1;
		int32_t BlockY = WATER_TO_SMALL_SECTOR_Y(fCamY) + 1;

		if (_IsColideWithBlock(BlockX, BlockY, nBlock))
		{
			if (m_nRenderWaterLayers != 1 && m_nRenderWaterLayers != 6)
			{
				float fMaskX = floor(fCamX / 2.0f) * 2.0f;
				float fMaskY = floor(fCamY / 2.0f) * 2.0f;
				float fWaterZ = CWaterLevel::ms_aWaterZs[nBlock];
				float fSectorX = WATER_FROM_SMALL_SECTOR_X(BlockX) - WATER_X_OFFSET;
				float fSectorY = WATER_FROM_SMALL_SECTOR_Y(BlockY);

				RenderWavyMask(fMaskX, fMaskY, fWaterZ,
					fSectorX, fSectorY,
					signX, signY, colorTrans);
			}
		}
	//}

	DefinedState();
#endif

	//POP_RENDERGROUP();
}

void CWaterLevel::RenderOneFlatSmallWaterPoly(float fX, float fY, float fZ, RwRGBA const& color)
{
	if (TempBufferIndicesStored >= TEMPBUFFERINDEXSIZE - 6 || TempBufferVerticesStored >= TEMPBUFFERVERTSIZE - 4)
		RenderAndEmptyRenderBuffer();

	int32_t vidx = TempBufferVerticesStored;

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 0], fX, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 0], color);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 1], fX, fY + SMALL_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDV + 1.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 1], color);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 2], fX + SMALL_SECTOR_SIZE, fY + SMALL_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDU + 1.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDV + 1.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 2], color);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 3], fX + SMALL_SECTOR_SIZE, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDU + 1.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 3], color);


	int32_t iidx = TempBufferIndicesStored;

	TempBufferRenderIndexList[iidx + 0] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 1] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[iidx + 2] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[iidx + 3] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 4] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[iidx + 5] = TempBufferVerticesStored + 2;

	TempBufferVerticesStored += 4;
	TempBufferIndicesStored += 6;
}

void
CWaterLevel::RenderOneFlatLargeWaterPoly(float fX, float fY, float fZ, RwRGBA const& color)
{
	if (TempBufferIndicesStored >= TEMPBUFFERINDEXSIZE - 6 || TempBufferVerticesStored >= TEMPBUFFERVERTSIZE - 4)
		RenderAndEmptyRenderBuffer();

	int32_t vidx = TempBufferVerticesStored;

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 0], fX, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 0], color);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 1], fX, fY + LARGE_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDV + 2.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 1], color);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 2], fX + LARGE_SECTOR_SIZE, fY + LARGE_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDU + 2.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDV + 2.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 2], color);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 3], fX + LARGE_SECTOR_SIZE, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDU + 2.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 3], color);


	int32_t iidx = TempBufferIndicesStored;

	TempBufferRenderIndexList[iidx + 0] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 1] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[iidx + 2] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[iidx + 3] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 4] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[iidx + 5] = TempBufferVerticesStored + 2;

	TempBufferVerticesStored += 4;
	TempBufferIndicesStored += 6;
}

void
CWaterLevel::RenderOneFlatHugeWaterPoly(float fX, float fY, float fZ, RwRGBA const& color)
{
	if (TempBufferIndicesStored >= TEMPBUFFERINDEXSIZE - 6 || TempBufferVerticesStored >= TEMPBUFFERVERTSIZE - 4)
		RenderAndEmptyRenderBuffer();

	int32_t vidx = TempBufferVerticesStored;

	RwRGBA c;

	c.red = color.red;
	c.green = color.green;
	c.blue = color.blue;
	c.alpha = 255;

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 0], fX, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 0], c);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 1], fX, fY + HUGE_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDV + 4.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 1], c);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 2], fX + HUGE_SECTOR_SIZE, fY + HUGE_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDU + 4.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDV + 4.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 2], c);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 3], fX + HUGE_SECTOR_SIZE, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDU + 4.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 3], c);


	int32_t iidx = TempBufferIndicesStored;

	TempBufferRenderIndexList[iidx + 0] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 1] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[iidx + 2] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[iidx + 3] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 4] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[iidx + 5] = TempBufferVerticesStored + 2;

	TempBufferVerticesStored += 4;
	TempBufferIndicesStored += 6;
}

void
CWaterLevel::RenderOneFlatExtraHugeWaterPoly(float fX, float fY, float fZ, RwRGBA const& color)
{
	if (TempBufferIndicesStored >= TEMPBUFFERINDEXSIZE - 6 || TempBufferVerticesStored >= TEMPBUFFERVERTSIZE - 4)
		RenderAndEmptyRenderBuffer();

	int32_t vidx = TempBufferVerticesStored;

	RwRGBA c;

	c.red = color.red;
	c.green = color.green;
	c.blue = color.blue;
	c.alpha = 255;

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 0], fX, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 0], c);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 1], fX, fY + EXTRAHUGE_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDV + 8.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 1], c);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 2], fX + EXTRAHUGE_SECTOR_SIZE, fY + EXTRAHUGE_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDU + 8.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDV + 8.0f);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 2], c);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 3], fX + EXTRAHUGE_SECTOR_SIZE, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDU + 8.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDV);
	RwIm3DVertexSet_RGBA(&TempBufferRenderVertices[vidx + 3], c);


	int32_t iidx = TempBufferIndicesStored;

	TempBufferRenderIndexList[iidx + 0] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 1] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[iidx + 2] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[iidx + 3] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 4] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[iidx + 5] = TempBufferVerticesStored + 2;

	TempBufferVerticesStored += 4;
	TempBufferIndicesStored += 6;
}

void
CWaterLevel::RenderOneWavySector(float fX, float fY, float fZ, RwRGBA const& color, bool bDontRender)
{
	CVector vecSectorPos(fX + (SMALL_SECTOR_SIZE / 2), fY + (SMALL_SECTOR_SIZE / 2), fZ + 2.0f);

//	if (COcclusion::IsAABoxOccluded(vecSectorPos, SMALL_SECTOR_SIZE, SMALL_SECTOR_SIZE, 4.0f))
	//	return;

#ifdef PC_WATER
	RequireWavySector = true;
#else
	if (!WavesCalculatedThisFrame)
	{
		WavesCalculatedThisFrame = true;

		float fAngle = (CTimer::m_snTimeInMilliseconds & 4095) * (TWOPI / 4096.0f);

		RpGeometry* wavyGeometry = ms_pWavyAtomic->geometry;
		RwTexCoords* wavyTexCoords = RpGeometryGetVertexTexCoords(wavyGeometry, rwTEXTURECOORDINATEINDEX0);
		RpMorphTarget* wavyMorph = RpGeometryGetMorphTarget(wavyGeometry, 0);
		RwRGBA* wavyPreLight = RpGeometryGetPreLightColors(wavyGeometry);
		RwV3d* wavyMorphVerts = wavyMorph->verts;
		RwV3d* wavyMorphNormals = wavyMorph->normals;

		RpGeometryLock(wavyGeometry, rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS | rpGEOMETRYLOCKPRELIGHT | rpGEOMETRYLOCKTEXCOORDS);

		RwMatrix* camMat = RwFrameGetLTM(RwCameraGetFrame(RwCameraGetCurrentCamera())); //or curWorld

		float randomDampInv2 = (1.0f - fRandomDamp) * 2.0f;

		float move = 1.0f / 16.0f;
		float randomMove = 1.0f / (16.0f * fRandomMoveDiv);

		float vertMul = 0.5f;

		float wind = CWeather::WindClipped * 0.4f + 0.2f;
		float waveWind = CWeather::WindClipped * fWave2Ampl + 0.05f;

		float waveA = (TWOPI / 16.0f)
			* ((fNormalDirectionScalar1 * fabsf(camMat->at.x + camMat->at.y) + fNormMult) * (CWeather::WindClipped * 0.4f + 0.2f));

		float waveB = TWOPI / (16.0f * fWave2NormScale)
			* ((fNormalDirectionScalar2 * fabsf(camMat->at.y - camMat->at.x) + fNormMultB) * (CWeather::WindClipped * 0.2f + 0.1f));

		CVector vA(1.0f, 0.0f, 0.0f);
		CVector vB(0.0f, 1.0f, 0.0f);

		for (int32_t i = 0; i < 17; i++)
		{
			for (int32_t j = 0; j < 17; j++)
			{
				wavyTexCoords->u = float(i) * move + TEXTURE_ADDV;
				wavyTexCoords->v = float(j) * move + TEXTURE_ADDU;

				RwRGBAAssign(wavyPreLight, &color);

				if (i > 0 && i < 16 && j > 0 && j < 16)
				{
					wavyMorphVerts->x += CGeneral::GetRandomNumberInRange(-1.0f, 1.0f) * randomMove;
					wavyMorphVerts->x *= fRandomDamp;
					wavyMorphVerts->x += float(i) * randomDampInv2;

					wavyMorphVerts->y += CGeneral::GetRandomNumberInRange(-1.0f, 1.0f) * randomMove;
					wavyMorphVerts->y *= fRandomDamp;
					wavyMorphVerts->y += float(j) * randomDampInv2;
				}

				float morphVertXHalf = (i == 16) ? 0.0f : vertMul * wavyMorphVerts->x;
				float morphVertYHalf = (j == 16) ? 0.0f : vertMul * wavyMorphVerts->y;

				float waveMulA = (morphVertYHalf + morphVertXHalf) * (TWOPI / 16.0f) + fAngle;
				float waveMulB = (morphVertYHalf - morphVertXHalf) * (TWOPI / (16.0f * fWave2InvLength)) + fAngle;

				wavyMorphVerts->z = wind * sinf(waveMulA) + waveWind * sinf(waveMulB);

				vA.z = (waveA * cosf(waveMulA)) - (waveB * cosf(waveMulB));
				vB.z = (waveA * cosf(waveMulA)) + (waveB * cosf(waveMulB));

				CVector norm = CrossProduct(vA, vB);
				norm.Normalise();

				wavyMorphNormals->x = norm.x;
				wavyMorphNormals->y = norm.y;
				wavyMorphNormals->z = norm.z;

				++wavyPreLight;
				++wavyTexCoords;

				++wavyMorphVerts;
				++wavyMorphNormals;
			}
		}

		RpGeometryUnlock(wavyGeometry);
	}

	float fCentreX = fX + (SMALL_SECTOR_SIZE / 2);
	float fCentreY = fY + (SMALL_SECTOR_SIZE / 2);
#endif

#ifdef PC_WATER
	if (WavesCalculatedThisFrame)
#endif
	{
		if (bDontRender == false
			&& m_nRenderWaterLayers != 2
			&& m_nRenderWaterLayers != 4
			&& m_nRenderWaterLayers != 6)
		{
			RwV3d pos = { 0.0f, 0.0f, 0.0f };

			pos.x = fX;
			pos.y = fY;
			pos.z = fZ;

			RwFrameTranslate(RpAtomicGetFrame(ms_pWavyAtomic), &pos, rwCOMBINEREPLACE);

			ms_pWavyAtomic->renderCallBack;
		}
	}
}

int16_t
_RoundValue(int32_t v)
{
	int16_t result = v;

	while (result < 0)  result += 16;
	while (result > 16) result -= 16;

	return result;
}

void
CWaterLevel::RenderWavyMask(float fX, float fY, float fZ,
	float fSectorX, float fSectorY,
#ifdef PC_WATER
	float fCamPosX, float fCamPosY,
	float fCamDirX, float fCamDirY, RwRGBA const& color)
#else
	int32_t nCamDirX, int32_t nCamDirY, RwRGBA const& color)
#endif
{
#ifndef PC_WATER
	bool bRender = true;
	if (m_nRenderWaterLayers != 0 && m_nRenderWaterLayers != 2 && m_nRenderWaterLayers != 3)
		bRender = false;
#endif
	CVector vecSectorPos(fX + (LARGE_SECTOR_SIZE / 2), fY + (LARGE_SECTOR_SIZE / 2), fZ + 2.0f);

	//if (COcclusion::IsAABoxOccluded(vecSectorPos, LARGE_SECTOR_SIZE, LARGE_SECTOR_SIZE, 4.0f))
	//	return;

#ifndef PC_WATER
	float fUOffset = fX - (MAX_LARGE_SECTORS * (int32_t)floorf(fX / MAX_LARGE_SECTORS));
	float fVOffset = fY - (MAX_LARGE_SECTORS * (int32_t)floorf(fY / MAX_LARGE_SECTORS));

	int32_t nSecsX = (int32_t)((fX - fSectorX) / 2.0f);
	int32_t nSecsY = (int32_t)((fY - fSectorY) / 2.0f);
#endif

	RpGeometry* wavyGeometry = ms_pWavyAtomic->geometry;
	RpMorphTarget* wavyMorph = RpGeometryGetMorphTarget(wavyGeometry, 0);
	RwV3d* wavyMorphVerts = wavyMorph->verts;
	RwV3d* wavyMorphNormals = wavyMorph->normals;

	RpGeometry* maskGeometry = ms_pMaskAtomic->geometry;
	RwTexCoords* maskTexCoords = RpGeometryGetVertexTexCoords(maskGeometry, rwTEXTURECOORDINATEINDEX0);
	RwRGBA* maskPreLight = RpGeometryGetPreLightColors(maskGeometry);
	RpMorphTarget* maskMorph = RpGeometryGetMorphTarget(maskGeometry, 0);
	RwV3d* maskMorphVerts = maskMorph->verts;
	RwV3d* maskMorphNormals = maskMorph->normals;

	RpGeometryLock(maskGeometry, rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS | rpGEOMETRYLOCKPRELIGHT | rpGEOMETRYLOCKTEXCOORDS);

#ifndef PC_WATER
	RpMaterial* maskMat = RpGeometryGetMaterial(maskGeometry, 0);
	RpMatFXMaterialSetEnvMapFrame(maskMat, RwCameraGetFrame(RwCameraGetCurrentCamera()));
	RpMatFXMaterialSetEnvMapCoefficient(maskMat, fEnvScale);
	RpMatFXMaterialSetEnvMapFrameBufferAlpha(maskMat, TRUE);
#endif

#ifndef PC_WATER
	float fMinSparkZ = (CWeather::WindClipped * fWave2Ampl + 0.05f +
		CWeather::WindClipped * 0.4f + 0.2) * (1.0f - 0.04f * CWeather::SunGlare);

	int32_t randval = GetRandomNumber();

	float fUVStep = 0.125f;
	float f27 = 2.0f;

	float fMinU = (fUOffset / 16.0f) + _TEXTURE_MASK_ADDU;
	float fMinV = (fVOffset / 16.0f) + _TEXTURE_MASK_ADDV;

	float fAlphaMul = ((float)color.alpha * 0.4f) / 16.0f;

	float fXOffset = 16.0f;
	if (nCamDirX > 0)
		fXOffset = 6.4f;
	else if (nCamDirX < 0)
		fXOffset = 25.6f;

	float fYOffset = 16.0f;
	if (nCamDirY > 0)
		fYOffset = 6.4f;
	else if (nCamDirY < 0)
		fYOffset = 25.6f;

	int16_t nX = _RoundValue(nSecsX - 1);
	int16_t nY = _RoundValue(nSecsY - 1);
#else
	float fMinSparkZ = (fWave2Ampl * CWeather::WindClipped + 0.05f +
		0.4f * CWeather::WindClipped + 0.2) * (1.0f - 0.02f * CWeather::SunGlare);

	int32_t randval = CGeneral::GetRandomNumber() & 255;

	int16 nX = _RoundValue((int32_t)((fX - fSectorX) * 0.5f) - 1);
	int16 nY = _RoundValue((int32_t)((fY - fSectorY) * 0.5f) - 1);
#endif
	int16_t idxX = nX;

	for (int32_t i = 0; i < 17; i++)
	{
		int16_t idxY = nY;

		if (++idxX > 16)
			idxX -= 16;

		for (int32_t j = 0; j < 17; j++)
		{
			if (++idxY > 16)
				idxY -= 16;

			const int32_t a = (0 * 16);
			const int32_t b = (1 * 16);
			const int32_t c = (33 * 16);
			const int32_t d = (34 * 16);

			int32_t base = (i * 33 + j);

#ifndef  PC_WATER
			maskTexCoords[base + a].u = fMinU + ((float)i * fUVStep);
			maskTexCoords[base + a].v = fMinV + ((float)j * fUVStep);

			maskTexCoords[base + b].u = maskTexCoords[base + a].u;
			maskTexCoords[base + b].v = maskTexCoords[base + a].v + (16.0f * fUVStep);

			maskTexCoords[base + c].u = maskTexCoords[base + a].u + (16.0f * fUVStep);
			maskTexCoords[base + c].v = maskTexCoords[base + a].v;

			maskTexCoords[base + d].u = maskTexCoords[base + a].u + (16.0f * fUVStep);
			maskTexCoords[base + d].v = maskTexCoords[base + a].v + (16.0f * fUVStep);
#else
			maskTexCoords[base + a].v = float(j) / SMALL_SECTOR_SIZE + ((fCamPosY - fY) / 64);
			maskTexCoords[base + c].v = maskTexCoords[base + a].v;
			maskTexCoords[base + d].v = maskTexCoords[base + a].v + 0.5f;
			maskTexCoords[base + b].v = maskTexCoords[base + d].v;

			maskTexCoords[base + a].u = float(i) / SMALL_SECTOR_SIZE + ((fCamPosX - fX) / 64);
			maskTexCoords[base + b].u = maskTexCoords[base + a].u;
			maskTexCoords[base + d].u = maskTexCoords[base + a].u + 0.5f;
			maskTexCoords[base + c].u = maskTexCoords[base + d].u;
#endif

			maskMorphVerts[base + a].x = (wavyMorphVerts[idxY + (17 * idxX)].x - (float)idxX * 2.0f) + (float(i) * 2.0f);
			maskMorphVerts[base + b].x = maskMorphVerts[base + a].x;
			maskMorphVerts[base + c].x = maskMorphVerts[base + a].x + SMALL_SECTOR_SIZE;
			maskMorphVerts[base + d].x = maskMorphVerts[base + c].x;

			maskMorphVerts[base + a].y = (wavyMorphVerts[idxY + (17 * idxX)].y - (float)idxY * 2.0f) + (float(j) * 2.0f);
			maskMorphVerts[base + c].y = maskMorphVerts[base + a].y;
			maskMorphVerts[base + b].y = maskMorphVerts[base + a].y + SMALL_SECTOR_SIZE;
			maskMorphVerts[base + d].y = maskMorphVerts[base + b].y;

			maskMorphVerts[base + a].z = wavyMorphVerts[idxY + (17 * idxX)].z;
			maskMorphVerts[base + d].z = maskMorphVerts[base + a].z;
			maskMorphVerts[base + c].z = maskMorphVerts[base + d].z;
			maskMorphVerts[base + b].z = maskMorphVerts[base + c].z;

#ifndef PC_WATER
			if (maskMorphVerts[base].z >= fMinSparkZ)
#else
			if (maskMorphVerts[base].z > fMinSparkZ)
#endif
			{
				switch ((i + j + randval) & 3)
				{
				case 0:
				{
					CVector vecPos
					(
						fX + maskMorphVerts[base + a].x,
						fY + maskMorphVerts[base + a].y,
						fZ + maskMorphVerts[base + a].z + 0.12f
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
					CVector vecPos
					(
						fX + maskMorphVerts[base + c].x,
						fY + maskMorphVerts[base + c].y,
						fZ + maskMorphVerts[base + c].z + 0.12f
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
					CVector vecPos
					(
						fX + maskMorphVerts[base + b].x,
						fY + maskMorphVerts[base + b].y,
						fZ + maskMorphVerts[base + b].z + 0.12f
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
					CVector vecPos
					(
						fX + maskMorphVerts[base + d].x,
						fY + maskMorphVerts[base + d].y,
						fZ + maskMorphVerts[base + d].z + 0.12f
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

			maskMorphNormals[base + a].x = wavyMorphNormals[idxY + (17 * idxX)].x;
			maskMorphNormals[base + a].y = wavyMorphNormals[idxY + (17 * idxX)].y;
			maskMorphNormals[base + a].z = wavyMorphNormals[idxY + (17 * idxX)].z;

			maskMorphNormals[base + d].x = maskMorphNormals[base + a].x;
			maskMorphNormals[base + d].y = maskMorphNormals[base + a].y;
			maskMorphNormals[base + d].z = maskMorphNormals[base + a].z;

			maskMorphNormals[base + c].x = maskMorphNormals[base + d].x;
			maskMorphNormals[base + c].y = maskMorphNormals[base + d].y;
			maskMorphNormals[base + c].z = maskMorphNormals[base + d].z;

			maskMorphNormals[base + b].x = maskMorphNormals[base + c].x;
			maskMorphNormals[base + b].y = maskMorphNormals[base + c].y;
			maskMorphNormals[base + b].z = maskMorphNormals[base + c].z;

			maskPreLight[base + a].red = color.red;
			maskPreLight[base + a].green = color.green;
			maskPreLight[base + a].blue = color.blue;
			maskPreLight[base + a].alpha = color.alpha;

			maskPreLight[base + d].red = maskPreLight[base + a].red;
			maskPreLight[base + d].green = maskPreLight[base + a].green;
			maskPreLight[base + d].blue = maskPreLight[base + a].blue;
			maskPreLight[base + d].alpha = maskPreLight[base + a].alpha;

			maskPreLight[base + c].red = maskPreLight[base + d].red;
			maskPreLight[base + c].green = maskPreLight[base + d].green;
			maskPreLight[base + c].blue = maskPreLight[base + d].blue;
			maskPreLight[base + c].alpha = maskPreLight[base + d].alpha;

			maskPreLight[base + b].red = maskPreLight[base + c].red;
			maskPreLight[base + b].green = maskPreLight[base + c].green;
			maskPreLight[base + b].blue = maskPreLight[base + c].blue;
			maskPreLight[base + b].alpha = maskPreLight[base + c].alpha;

#ifndef PC_WATER
			maskPreLight[base + a].alpha = max(0, (int32_t)((float)color.alpha - (fAlphaMul * (fabsf((float)i - fXOffset) + fabsf((float)j - fYOffset)))));
			maskPreLight[base + b].alpha = max(0, (int32_t)((float)color.alpha - (fAlphaMul * (fabsf((float)i - fXOffset) + fabsf(16.0f + (float)j - fYOffset)))));
			maskPreLight[base + c].alpha = max(0, (int32_t)((float)color.alpha - (fAlphaMul * (fabsf(16.0f + (float)i - fXOffset) + fabsf((float)j - fYOffset)))));
			maskPreLight[base + d].alpha = max(0, (int32_t)((float)color.alpha - (fAlphaMul * (fabsf(16.0f + (float)i - fXOffset) + fabsf(16.0f + (float)j - fYOffset)))));
#endif
		}
	}

	RpGeometryUnlock(maskGeometry);

#ifndef PC_WATER
	{
		RwV3d pos = { 0.0f, 0.0f, 0.0f };

		pos.x = fX;
		pos.y = fY;
		pos.z = fZ + 0.05f;

		RwFrameTranslate(RpAtomicGetFrame(ms_pMaskAtomic), &pos, rwCOMBINEREPLACE);

		if (bRender)
		{
#ifdef PS2
			RpSkyTexCacheFlush();
#endif
			ms_pMaskAtomic->renderCallBack;
		}
	}
#endif
}

#ifdef PC_WATER
void
CWaterLevel::PreCalcWaterGeometry(void)
{
	if (!RequireWavySector)
	{
		WavesCalculatedThisFrame = false;
		MaskCalculatedThisFrame = false;
		return;
	}

	RequireWavySector = false;
	WavesCalculatedThisFrame = true;

	RwRGBA color;

	color.red = CTimeCycle::GetWaterRed();
	color.green = CTimeCycle::GetWaterGreen();
	color.blue = CTimeCycle::GetWaterBlue();
	color.alpha = CTimeCycle::GetWaterAlpha();

	PreCalcWavySector(color);

	if (CCullZones::WaterFudge())
	{
		MaskCalculatedThisFrame = false;
		return;
	}

	CVector CamFwdDir = TheCamera.GetForward();
	CamFwdDir.z = 0.0f;
	CamFwdDir.Normalise();

	float fCamX = TheCamera.GetPosition().x - SMALL_SECTOR_SIZE;
	float fCamY = TheCamera.GetPosition().y - SMALL_SECTOR_SIZE;

	//1.4144272f; 1.4144f;
	float signX = CamFwdDir.x * 1.4144272f;
	float signY = CamFwdDir.y * 1.4144272f;

	signX = Clamp(signX, -1.0f, 1.0f);
	fCamX += 0.4f * signX * float(SMALL_SECTOR_SIZE * 2.0f);

	signY = Clamp(signY, -1.0f, 1.0f);
	fCamY += 0.4f * signY * float(SMALL_SECTOR_SIZE * 2.0f);

	int32_t nBlock;

	int32_t BlockX = WATER_TO_SMALL_SECTOR_X(fCamX + WATER_X_OFFSET) + 1;
	int32_t BlockY = WATER_TO_SMALL_SECTOR_Y(fCamY) + 1;

	ASSERT(BlockX >= 0 && BlockX < MAX_SMALL_SECTORS);
	ASSERT(BlockY >= 0 && BlockY < MAX_SMALL_SECTORS);

	if (_IsColideWithBlock(BlockX, BlockY, nBlock))
	{
		float fMaskX = Floor(fCamX / 2.0f) * 2.0f;
		float fMaskY = Floor(fCamY / 2.0f) * 2.0f;

		float fSectorX = WATER_FROM_SMALL_SECTOR_X(BlockX) - WATER_X_OFFSET;
		float fSectorY = WATER_FROM_SMALL_SECTOR_Y(BlockY);

		if (PreCalcWavyMask(fMaskX, fMaskY, ms_aWaterZs[nBlock],
			fSectorX, fSectorY, fCamX, fCamY, CamFwdDir.x, CamFwdDir.y, color))
		{
			PreCalculatedMaskPosn.x = fMaskX;
			PreCalculatedMaskPosn.y = fMaskY;
			PreCalculatedMaskPosn.z = ms_aWaterZs[nBlock] + 0.05f;

			MaskCalculatedThisFrame = true;
		}
		else
			MaskCalculatedThisFrame = false;
	}
	else
		MaskCalculatedThisFrame = false;
}

bool
CWaterLevel::PreCalcWavySector(RwRGBA const& color)
{
	float fAngle = (CTimer::m_snTimeInMilliseconds & 4095) * (TWOPI / 4096.0f);

	RpGeometry* wavyGeometry = RpAtomicGetGeometry(ms_pWavyAtomic);

	RwTexCoords* wavyTexCoords = RpGeometryGetVertexTexCoords(wavyGeometry, rwTEXTURECOORDINATEINDEX0);
	RpMorphTarget* wavyMorph = RpGeometryGetMorphTarget(wavyGeometry, 0);

	RwRGBA* wavyPreLight = RpGeometryGetPreLightColors(wavyGeometry);
	RwV3d* wavyMorphVerts = RpMorphTargetGetVertices(wavyMorph);
	RwV3d* wavyMorphNormals = RpMorphTargetGetVertexNormals(wavyMorph);

	if (!m_bRenderSeaBed)
		RpGeometryLock(wavyGeometry, rpGEOMETRYLOCKVERTICES
			| rpGEOMETRYLOCKNORMALS
			| rpGEOMETRYLOCKPRELIGHT
			| rpGEOMETRYLOCKTEXCOORDS);

	CVector camPosUp = TheCamera.GetForward();

	float randomDampInv2 = (1.0f - fRandomDamp) * 2.0f;

	float randomMove = 1.0f / (16.0f * fRandomMoveDiv);

	float wind = CWeather::WindClipped * 0.4f + 0.2f;
	float waveWind = CWeather::WindClipped * fWave2Ampl + 0.05f;

	float waveA = (TWOPI / 16.0f)
		* ((CWeather::WindClipped * 0.4f + 0.2f) * (fNormalDirectionScalar1 * Abs(camPosUp.x + camPosUp.y) + fNormMult));

	float waveB = TWOPI / (16.0f * fWave2NormScale)
		* ((CWeather::WindClipped * 0.2f + 0.1f) * (fNormalDirectionScalar2 * Abs(camPosUp.y - camPosUp.x) + fNormMultB));


	CVector vA(1.0f, 0.0f, 0.0f);
	CVector vB(0.0f, 1.0f, 0.0f);

	for (int32_t i = 0; i < 17; i++)
	{
		for (int32_t j = 0; j < 17; j++)
		{
			wavyTexCoords->u = (float(i) / 16.0f) + TEXTURE_ADDV;
			wavyTexCoords->v = (float(j) / 16.0f) + TEXTURE_ADDU;

			RwRGBAAssign(wavyPreLight, &color);

			if (i > 0 && i < 16 && j > 0 && j < 16)
			{
				wavyMorphVerts->x += CGeneral::GetRandomNumberInRange(-1.0f, 1.0f) * randomMove;
				wavyMorphVerts->x *= fRandomDamp;
				wavyMorphVerts->x += float(i) * randomDampInv2;

				wavyMorphVerts->y += CGeneral::GetRandomNumberInRange(-1.0f, 1.0f) * randomMove;
				wavyMorphVerts->y *= fRandomDamp;
				wavyMorphVerts->y += float(j) * randomDampInv2;
			}

			float morphVertXHalf = (i == 16) ? 0.0f : 0.5f * wavyMorphVerts->x;
			float morphVertYHalf = (j == 16) ? 0.0f : 0.5f * wavyMorphVerts->y;

			float waveMulA = (morphVertYHalf + morphVertXHalf) * (TWOPI / 16.0f) + fAngle;
			float waveMulB = (morphVertYHalf - morphVertXHalf) * (TWOPI / (16.0f * fWave2InvLength)) + fAngle;

			wavyMorphVerts->z = wind * Sin(waveMulA) + waveWind * Sin(waveMulB);

			vA.z = (waveA * Cos(waveMulA)) - (waveB * Cos(waveMulB));
			vB.z = (waveA * Cos(waveMulA)) + (waveB * Cos(waveMulB));

			CVector norm = CrossProduct(vA, vB);
			norm.Normalise();

			wavyMorphNormals->x = norm.x;
			wavyMorphNormals->y = norm.y;
			wavyMorphNormals->z = norm.z;

			++wavyPreLight;
			++wavyTexCoords;

			++wavyMorphVerts;
			++wavyMorphNormals;
		}
	}

	RpGeometryUnlock(wavyGeometry);

	return true;
}

bool
CWaterLevel::PreCalcWavyMask(float fX, float fY, float fZ,
	float fSectorX, float fSectorY,
	float fCamPosX, float fCamPosY,
	float fCamDirX, float fCamDirY,
	RwRGBA const& color)
{
	CVector vecSectorPos(fX + (MAX_LARGE_SECTORS / 2), fY + (MAX_LARGE_SECTORS / 2), fZ + 2.0f);

	if (COcclusion::IsAABoxOccluded(vecSectorPos, MAX_LARGE_SECTORS, MAX_LARGE_SECTORS, 4.0f))
		return false;

	Floor(fX / MAX_LARGE_SECTORS);
	Floor(fY / MAX_LARGE_SECTORS);

	RpGeometry* wavyGeometry = RpAtomicGetGeometry(ms_pWavyAtomic);
	RpMorphTarget* wavyMorph = RpGeometryGetMorphTarget(wavyGeometry, 0);
	RwV3d* wavyMorphVerts = RpMorphTargetGetVertices(wavyMorph);
	RwV3d* wavyMorphNormals = RpMorphTargetGetVertexNormals(wavyMorph);

	RpGeometry* maskGeometry = RpAtomicGetGeometry(ms_pMaskAtomic);
	RwTexCoords* maskTexCoords = RpGeometryGetVertexTexCoords(maskGeometry, rwTEXTURECOORDINATEINDEX0);
	RwRGBA* maskPreLight = RpGeometryGetPreLightColors(maskGeometry);
	RpMorphTarget* maskMorph = RpGeometryGetMorphTarget(maskGeometry, 0);
	RwV3d* maskMorphVerts = RpMorphTargetGetVertices(maskMorph);
	RwV3d* maskMorphNormals = RpMorphTargetGetVertexNormals(maskMorph);

	if (!m_bRenderSeaBed)
		RpGeometryLock(maskGeometry, rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS | rpGEOMETRYLOCKPRELIGHT | rpGEOMETRYLOCKTEXCOORDS);


	float fMinSparkZ = (fWave2Ampl * CWeather::WindClipped + 0.05f +
		0.4f * CWeather::WindClipped + 0.2) * (1.0f - 0.02f * CWeather::SunGlare);

	int32_t randval = CGeneral::GetRandomNumber() & 255;

	int16 nX = _RoundValue((int32_t)((fX - fSectorX) * 0.5f) - 1);
	int16 nY = _RoundValue((int32_t)((fY - fSectorY) * 0.5f) - 1);

	int16 idxX = nX;

	for (int32_t i = 0; i < 17; i++)
	{
		int16 idxY = nY;

		if (++idxX > 16)
			idxX -= 16;

		for (int32_t j = 0; j < 17; j++)
		{
			if (++idxY > 16)
				idxY -= 16;

			const int32_t a = (0 * 16);
			const int32_t b = (1 * 16);
			const int32_t c = (33 * 16);
			const int32_t d = (34 * 16);

			int32_t base = (i * 33 + j);

			maskTexCoords[base + a].v = float(j) / 32 + ((fCamPosY - fY) / 64);
			maskTexCoords[base + c].v = maskTexCoords[base + a].v;
			maskTexCoords[base + d].v = maskTexCoords[base + a].v + 0.5f;
			maskTexCoords[base + b].v = maskTexCoords[base + d].v;

			maskTexCoords[base + a].u = float(i) / 32 + ((fCamPosX - fX) / 64);
			maskTexCoords[base + b].u = maskTexCoords[base + a].u;
			maskTexCoords[base + d].u = maskTexCoords[base + a].u + 0.5f;
			maskTexCoords[base + c].u = maskTexCoords[base + d].u;

			maskMorphVerts[base + a].x = (wavyMorphVerts[idxY + (17 * idxX)].x - (float)idxX * 2.0f) + (float(i) * 2.0f);
			maskMorphVerts[base + b].x = maskMorphVerts[base + a].x;
			maskMorphVerts[base + c].x = maskMorphVerts[base + a].x + SMALL_SECTOR_SIZE;
			maskMorphVerts[base + d].x = maskMorphVerts[base + c].x;

			maskMorphVerts[base + a].y = (wavyMorphVerts[idxY + (17 * idxX)].y - (float)idxY * 2.0f) + (float(j) * 2.0f);
			maskMorphVerts[base + c].y = maskMorphVerts[base + a].y;
			maskMorphVerts[base + b].y = maskMorphVerts[base + a].y + SMALL_SECTOR_SIZE;
			maskMorphVerts[base + d].y = maskMorphVerts[base + b].y;

			maskMorphVerts[base + a].z = wavyMorphVerts[idxY + (17 * idxX)].z;
			maskMorphVerts[base + d].z = maskMorphVerts[base + a].z;
			maskMorphVerts[base + c].z = maskMorphVerts[base + d].z;
			maskMorphVerts[base + b].z = maskMorphVerts[base + c].z;

			if (maskMorphVerts[base].z > fMinSparkZ)
			{
				switch ((i + j + randval) & 3)
				{
				case 0:
				{
					CVector vecPos
					(
						fX + maskMorphVerts[base + a].x,
						fY + maskMorphVerts[base + a].y,
						fZ + maskMorphVerts[base + a].z + 0.12f
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
					CVector vecPos
					(
						fX + maskMorphVerts[base + c].x,
						fY + maskMorphVerts[base + c].y,
						fZ + maskMorphVerts[base + c].z + 0.12f
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
					CVector vecPos
					(
						fX + maskMorphVerts[base + b].x,
						fY + maskMorphVerts[base + b].y,
						fZ + maskMorphVerts[base + b].z + 0.12f
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
					CVector vecPos
					(
						fX + maskMorphVerts[base + d].x,
						fY + maskMorphVerts[base + d].y,
						fZ + maskMorphVerts[base + d].z + 0.12f
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

			maskMorphNormals[base + a].x = wavyMorphNormals[idxY + (17 * idxX)].x;
			maskMorphNormals[base + a].y = wavyMorphNormals[idxY + (17 * idxX)].y;
			maskMorphNormals[base + a].z = wavyMorphNormals[idxY + (17 * idxX)].z;

			maskMorphNormals[base + d].x = maskMorphNormals[base + a].x;
			maskMorphNormals[base + d].y = maskMorphNormals[base + a].y;
			maskMorphNormals[base + d].z = maskMorphNormals[base + a].z;

			maskMorphNormals[base + c].x = maskMorphNormals[base + d].x;
			maskMorphNormals[base + c].y = maskMorphNormals[base + d].y;
			maskMorphNormals[base + c].z = maskMorphNormals[base + d].z;

			maskMorphNormals[base + b].x = maskMorphNormals[base + c].x;
			maskMorphNormals[base + b].y = maskMorphNormals[base + c].y;
			maskMorphNormals[base + b].z = maskMorphNormals[base + c].z;

			maskPreLight[base + a].red = color.red;
			maskPreLight[base + a].green = color.green;
			maskPreLight[base + a].blue = color.blue;
			maskPreLight[base + a].alpha = color.alpha;

			maskPreLight[base + d].red = maskPreLight[base + a].red;
			maskPreLight[base + d].green = maskPreLight[base + a].green;
			maskPreLight[base + d].blue = maskPreLight[base + a].blue;
			maskPreLight[base + d].alpha = maskPreLight[base + a].alpha;

			maskPreLight[base + c].red = maskPreLight[base + d].red;
			maskPreLight[base + c].green = maskPreLight[base + d].green;
			maskPreLight[base + c].blue = maskPreLight[base + d].blue;
			maskPreLight[base + c].alpha = maskPreLight[base + d].alpha;

			maskPreLight[base + b].red = maskPreLight[base + c].red;
			maskPreLight[base + b].green = maskPreLight[base + c].green;
			maskPreLight[base + b].blue = maskPreLight[base + c].blue;
			maskPreLight[base + b].alpha = maskPreLight[base + c].alpha;
		}
	}

	RpGeometryUnlock(maskGeometry);
	return true;
}
#endif

void
CWaterLevel::RenderBoatWakes(void)
{
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)gpWaterWakeRaster);
#ifndef PC_WATER
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
#endif

#ifdef _XBOX
	// TODO save and restore rwRENDERSTATESRCBLEND rwRENDERSTATEDESTBLEND
#endif

	CBoat::FillBoatList();

	float fWakeZ = 5.97f;
	float fWakeLifeTimeMult = 0.01f / CBoat::WAKE_LIFETIME;

	for (int32_t idx = 0; idx < ARRAY_SIZE(CBoat::apFrameWakeGeneratingBoats); idx++)
	{
		CBoat* pBoat = CBoat::apFrameWakeGeneratingBoats[idx];

		if (pBoat == NULL)
			break;

		CVector2D vecDistA(pBoat->GetForward().x, pBoat->GetForward().y);


		float fSize = pBoat->GetColModel()->m_boundBox.m_vecMax.z
			* 0.65f;

		if (pBoat->m_nModelIndex == MODEL_SKIMMER)
			fSize *= 0.4f;

		float fAplhaA = 255.0f;
		float fSizeA = fSize;
		float fAplhaB;
		float fSizeB;

		for (int32_t wake = 1; wake < (int32_t)pBoat->m_avecWakePoints; wake++)
		{
			bool bRender = true;

			float fTimeleft = CBoat::WAKE_LIFETIME - pBoat->m_afWakePointLifeTime[wake];

			float fWakeSizeB = ((float)wake * 0.19f) + fSize - fWakeLifeTimeMult * max(fTimeleft, 0.0f);

			fSizeB = fWakeSizeB / CBoat::MIN_WAKE_INTERVAL;
			if (fSizeB < 0.0f)
				fSizeB = 1.0f;

			if (wake == (int32_t)pBoat->m_avecWakePoints - 1)
			{
				// set alpha to 0 if it's last point
				fAplhaB = 0.0f;
			}
			else
			{
				// clip (-100, 500), less lifetime - less val
				float val = 500.0f - (CBoat::WAKE_LIFETIME - pBoat->m_afWakePointLifeTime[wake])
					* 600.0f / CBoat::WAKE_LIFETIME;

				fAplhaB = Clamp(val, 0.0f, 255.0f);
			}

			CVector2D vecDistB = pBoat->m_avecWakePoints[wake - 1] - pBoat->m_avecWakePoints[wake];

			float fScal = vecDistB.MagnitudeSqr();

			// normalize if distance between points is greater than 3

			if (fScal > SQR(3.0f))
			{
				float fNorm = 1.0f / sqrt(fScal);

				vecDistB.x *= fNorm;
				vecDistB.y *= fNorm;

				// disable render if distance between points too big

				if (sqrt(fScal) > 13.0f)
					bRender = false;
			}

			CVector2D vecAA
			(
				pBoat->m_avecWakePoints[wake - 1].x - (fSizeA * vecDistA.y),
				pBoat->m_avecWakePoints[wake - 1].y + (fSizeA * vecDistA.x)
			);
			CVector2D vecAB
			(
				pBoat->m_avecWakePoints[wake - 1].x + (fSizeA * vecDistA.y),
				pBoat->m_avecWakePoints[wake - 1].y - (fSizeA * vecDistA.x)
			);
			CVector2D vecBA
			(
				pBoat->m_avecWakePoints[wake].x + (fSizeB * vecDistB.y),
				pBoat->m_avecWakePoints[wake].y - (fSizeB * vecDistB.x)
			);
			CVector2D vecBB
			(
				pBoat->m_avecWakePoints[wake].x - (fSizeB * vecDistB.y),
				pBoat->m_avecWakePoints[wake].y + (fSizeB * vecDistB.x)
			);

			if (bRender)
				RenderWakeSegment(vecAA, vecAB, vecBA, vecBB, fSizeA, fSizeB, fAplhaA, fAplhaB, fWakeZ);

			vecDistA = vecDistB;
			fSizeA = fSizeB;

			fAplhaB = fAplhaA;
		}
	}

	RenderAndEmptyRenderBuffer();
}

inline float
_GetWindedWave(float fX, float fY)
{
	float fAngle = (CTimer::m_snTimeInMilliseconds & 4095) * (TWOPI / 4096.0f);
	float x = WATER_HUGE_X(fX + WATER_X_OFFSET);
	float y = WATER_HUGE_Y(fY);

	float fWindFactor(CWeather::WindClipped * 0.4f + 0.2f);
	float fWave = sinf(((x - floorf(x)) + (y - floorf(y))) * TWOPI + fAngle);

	return fWindFactor * fWave;
}

void
CWaterLevel::RenderWakeSegment(CVector2D& vecA, CVector2D& vecB, CVector2D& vecC, CVector2D& vecD,
	float& fSizeA, float& fSizeB,
	float& fAlphaA, float& fAlphaB,
	float& fWakeZ)
{
	for (int32_t i = 0; i < 4; i++)
	{
		if (TempBufferIndicesStored >= TEMPBUFFERINDEXSIZE - 6 || TempBufferVerticesStored >= TEMPBUFFERVERTSIZE - 4)
			RenderAndEmptyRenderBuffer();

		float fCurStep = (float)i / 4;
		float fNxtStep = (float)(i + 1) / 4;

		float fLeftCurStep = 1.0f - fCurStep;
		float fLeftNxtStep = 1.0f - fNxtStep;

		uint8_t AlphaA = (uint32_t)(fAlphaA * aAlphaFade[i]);
		uint8_t AlphaB = (uint32_t)(fAlphaA * aAlphaFade[i + 1]);
		uint8_t AlphaC = (uint32_t)(fAlphaB * aAlphaFade[i + 1]);
		uint8_t AlphaD = (uint32_t)(fAlphaB * aAlphaFade[i]);

		CVector2D PosA = vecB * fCurStep + vecA * fLeftCurStep;
		CVector2D PosB = vecB * fNxtStep + vecA * fLeftNxtStep;
		CVector2D PosC = vecC * fNxtStep + vecD * fLeftNxtStep;
		CVector2D PosD = vecC * fCurStep + vecD * fLeftCurStep;

		float fUA = (PosA.x / 4) + _TEXTURE_WAKE_ADDU;
		float fVA = (PosA.y / 4) + _TEXTURE_WAKE_ADDV;

		float fUB = (PosB.x / 4) + _TEXTURE_WAKE_ADDU;
		float fVB = (PosB.y / 4) + _TEXTURE_WAKE_ADDV;

		float fUC = (PosC.x / 4) + _TEXTURE_WAKE_ADDU;
		float fVC = (PosC.y / 4) + _TEXTURE_WAKE_ADDV;

		float fUD = (PosD.x / 4) + _TEXTURE_WAKE_ADDU;
		float fVD = (PosD.y / 4) + _TEXTURE_WAKE_ADDV;

#define MIN4(a, b, c, d) (min((a), min((b), min((c), (d)))))
		float fMinU = floorf(MIN4(fUA, fUB, fUC, fUD));
		float fMinV = floorf(MIN4(fVA, fVB, fVC, fVD));
#undef MIN4

		float fZA = _GetWindedWave(PosA.x, PosA.y) + fWakeZ;
		float fZB = _GetWindedWave(PosB.x, PosB.y) + fWakeZ;
		float fZC = _GetWindedWave(PosC.x, PosC.y) + fWakeZ;
		float fZD = _GetWindedWave(PosD.x, PosD.y) + fWakeZ;

		int32_t vidx = TempBufferVerticesStored;

		RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 0], PosA.x, PosA.y, fZA);
		RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 0], fUA - fMinU);
		RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 0], fVA - fMinV);
		RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 0], 255, 255, 255, AlphaA);

		RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 1], PosB.x, PosB.y, fZB);
		RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 1], fUB - fMinU);
		RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 1], fVB - fMinV);
		RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 1], 255, 255, 255, AlphaB);

		RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 2], PosC.x, PosC.y, fZC);
		RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 2], fUC - fMinU);
		RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 2], fVC - fMinV);
		RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 2], 255, 255, 255, AlphaC);

		RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 3], PosD.x, PosD.y, fZD);
		RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 3], fUD - fMinU);
		RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 3], fVD - fMinV);
		RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 3], 255, 255, 255, AlphaD);

		int32_t iidx = TempBufferIndicesStored;

		TempBufferRenderIndexList[iidx + 0] = TempBufferVerticesStored + 0;
		TempBufferRenderIndexList[iidx + 1] = TempBufferVerticesStored + 2;
		TempBufferRenderIndexList[iidx + 2] = TempBufferVerticesStored + 1;
		TempBufferRenderIndexList[iidx + 3] = TempBufferVerticesStored + 0;
		TempBufferRenderIndexList[iidx + 4] = TempBufferVerticesStored + 3;
		TempBufferRenderIndexList[iidx + 5] = TempBufferVerticesStored + 2;

		TempBufferVerticesStored += 4;

		TempBufferIndicesStored += 6;
	}
}

void
CWaterLevel::RenderOneSlopedUnderWaterPoly(float fX, float fY, float fZ, RwRGBA const& color)
{
	CVector2D camPos(TheCamera.GetPosition().x, TheCamera.GetPosition().y);

	float fDistA = (CVector2D(fX, fY) - camPos).Magnitude() + -140.0f;
	float fDistB = (CVector2D(fX, fY + HUGE_SECTOR_SIZE) - camPos).Magnitude() + -140.0f;
	float fDistC = (CVector2D(fX + HUGE_SECTOR_SIZE, fY + HUGE_SECTOR_SIZE) - camPos).Magnitude() + -140.0f;
	float fDistD = (CVector2D(fX + HUGE_SECTOR_SIZE, fY) - camPos).Magnitude() + -140.0f;

#ifndef PC_WATER
#define CALCSEABED(v, d) \
		{ \
			if ( d < 0.0f ) \
				v = 0.1f + fSeaBedZ; \
			else if ( d > 240.0f ) \
				v = 0.1f; \
			else \
				v = 0.1f + ((fSeaBedZ * (240.0f - d)) / 240.0f); \
		}
#else
#define CALCSEABED(v, d)	\
		{	\
		v = 0.1f;	\
		if ( d < 0.0f )	\
			v += fSeaBedZ;	\
		else if ( d <= 240.0f )	\
			v += (fSeaBedZ / 240.0f) * (240.0f - d);	\
		}
#endif
	float fSeaBedA, fSeaBedB, fSeaBedC, fSeaBedD;

	CALCSEABED(fSeaBedA, fDistA);
	CALCSEABED(fSeaBedB, fDistB);
	CALCSEABED(fSeaBedC, fDistC);
	CALCSEABED(fSeaBedD, fDistD);

#undef CALCSEABED

	if (TempBufferIndicesStored >= TEMPBUFFERINDEXSIZE - 6 || TempBufferVerticesStored >= TEMPBUFFERVERTSIZE - 4)
		RenderAndEmptyRenderBuffer();

	int32_t vidx = TempBufferVerticesStored;

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 0], fX, fY, fZ - _fWaterZOffset - fSeaBedA);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 0], 0.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 0], 0.0f);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 0], color.red, color.green, color.blue, 255);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 1], fX, fY + HUGE_SECTOR_SIZE, fZ - _fWaterZOffset - fSeaBedB);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 1], 0.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 1], 4.0f);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 1], color.red, color.green, color.blue, 255);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 2], fX + HUGE_SECTOR_SIZE, fY + HUGE_SECTOR_SIZE, fZ - _fWaterZOffset - fSeaBedC);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 2], 4.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 2], 4.0f);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 2], color.red, color.green, color.blue, 255);

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 3], fX + HUGE_SECTOR_SIZE, fY, fZ - _fWaterZOffset - fSeaBedD);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 3], 4.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 3], 0.0f);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 3], color.red, color.green, color.blue, 255);

	int32_t iidx = TempBufferIndicesStored;

	TempBufferRenderIndexList[iidx + 0] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 1] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[iidx + 2] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[iidx + 3] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 4] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[iidx + 5] = TempBufferVerticesStored + 2;

	TempBufferVerticesStored += 4;

	TempBufferIndicesStored += 6;
}

void
CWaterLevel::RenderOneFlatSmallWaterPolyBlended(float fX, float fY, float fZ, float fCamX, float fCamY,
	RwRGBA const& color, RwRGBA const& colorTrans,
	float fDrawDist)
{
	if (TempBufferIndicesStored >= TEMPBUFFERINDEXSIZE - 6 || TempBufferVerticesStored >= TEMPBUFFERVERTSIZE - 4)
		RenderAndEmptyRenderBuffer();

	int32_t vidx = TempBufferVerticesStored;

	float fBlendDrawDist = fDrawDist + fStartBlendDistanceAdd;

	float fDistStartX = SQR(fX - fCamX);
	float fDistStartY = SQR(fY - fCamY);
	float fDistEndX = SQR((fX + SMALL_SECTOR_SIZE) - fCamX);
	float fDistEndY = SQR((fY + SMALL_SECTOR_SIZE) - fCamY);


	float fAlphaBlendMulA
		= min(fFlatWaterBlendRange * max(sqrt(fDistStartX + fDistStartY) - fBlendDrawDist, fMinWaterAlphaMult), 1.0f);
	float fAlphaBlendMulB
		= min(fFlatWaterBlendRange * max(sqrt(fDistStartX + fDistEndY) - fBlendDrawDist, fMinWaterAlphaMult), 1.0f);
	float fAlphaBlendMulC
		= min(fFlatWaterBlendRange * max(sqrt(fDistEndX + fDistEndY) - fBlendDrawDist, fMinWaterAlphaMult), 1.0f);
	float fAlphaBlendMulD
		= min(fFlatWaterBlendRange * max(sqrt(fDistEndX + fDistStartY) - fBlendDrawDist, fMinWaterAlphaMult), 1.0f);


	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 0], fX, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 0], TEXTURE_ADDV);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 0], color.red, color.green, color.blue,
		(colorTrans.alpha + (color.alpha - colorTrans.alpha) * (uint8_t)(int32_t)fAlphaBlendMulA));

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 1], fX, fY + SMALL_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDU);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 1], TEXTURE_ADDV + 1.0f);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 1], color.red, color.green, color.blue,
		(colorTrans.alpha + (color.alpha - colorTrans.alpha) * (uint8_t)(int32_t)fAlphaBlendMulB));

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 2], fX + SMALL_SECTOR_SIZE, fY + SMALL_SECTOR_SIZE, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDU + 1.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 2], TEXTURE_ADDV + 1.0f);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 2], color.red, color.green, color.blue,
		(colorTrans.alpha + (color.alpha - colorTrans.alpha) * (uint8_t)(int32_t)fAlphaBlendMulC));

	RwIm3DVertexSetPos(&TempBufferRenderVertices[vidx + 3], fX + SMALL_SECTOR_SIZE, fY, fZ - _fWaterZOffset);
	RwIm3DVertexSetU(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDU + 1.0f);
	RwIm3DVertexSetV(&TempBufferRenderVertices[vidx + 3], TEXTURE_ADDV);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[vidx + 3], color.red, color.green, color.blue,
		(colorTrans.alpha + (color.alpha - colorTrans.alpha) * (uint8_t)(int32_t)fAlphaBlendMulD));


	int32_t iidx = TempBufferIndicesStored;

	TempBufferRenderIndexList[iidx + 0] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 1] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[iidx + 2] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[iidx + 3] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[iidx + 4] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[iidx + 5] = TempBufferVerticesStored + 2;

	TempBufferVerticesStored += 4;

	TempBufferIndicesStored += 6;
}

float
CWaterLevel::CalcDistanceToWater(float fX, float fY)
{
	const float fSectorMaxRenderDist = 250.0f;

	int32_t nStartX = WATER_TO_SMALL_SECTOR_X(fX - fSectorMaxRenderDist + WATER_X_OFFSET) - 1;
	int32_t nEndX = WATER_TO_SMALL_SECTOR_X(fX + fSectorMaxRenderDist + WATER_X_OFFSET) + 1;
	int32_t nStartY = WATER_TO_SMALL_SECTOR_Y(fY - fSectorMaxRenderDist) - 1;
	int32_t nEndY = WATER_TO_SMALL_SECTOR_Y(fY + fSectorMaxRenderDist) + 1;

	nStartX = Clamp(nStartX, 0, MAX_SMALL_SECTORS - 1);
	nEndX = Clamp(nEndX, 0, MAX_SMALL_SECTORS - 1);
	nStartY = Clamp(nStartY, 0, MAX_SMALL_SECTORS - 1);
	nEndY = Clamp(nEndY, 0, MAX_SMALL_SECTORS - 1);

	float fDistSqr = 1.0e10f;

	for (int32_t x = nStartX; x <= nEndX; x++)
	{
		for (int32_t y = nStartY; y <= nEndY; y++)
		{
			if (aWaterFineBlockList[x][y] >= 0)
			{
				float fSectorX = WATER_FROM_SMALL_SECTOR_X(x) - WATER_X_OFFSET;
				float fSectorY = WATER_FROM_SMALL_SECTOR_Y(y);

				CVector2D vecDist
				(
					fSectorX + SMALL_SECTOR_SIZE - fX,
					fSectorY + SMALL_SECTOR_SIZE - fY
				);

				fDistSqr = min(vecDist.MagnitudeSqr(), fDistSqr);
			}
		}
	}

	return Clamp(Sqrt(fDistSqr) - 23.0f, 0.0f, fSectorMaxRenderDist);
}

void
CWaterLevel::RenderAndEmptyRenderBuffer()
{
	if (TempBufferVerticesStored)
	{
		//LittleTest();

		if (RwIm3DTransform(TempBufferRenderVertices, TempBufferVerticesStored, NULL, rwIM3D_VERTEXUV))
		{
			RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, TempBufferRenderIndexList, TempBufferIndicesStored);
			RwIm3DEnd();
		}
	}

	TempBufferIndicesStored = 0;
	TempBufferVerticesStored = 0;
}

bool
CWaterLevel::GetGroundLevel(CVector const& vecPosn, float* pfOutLevel, ColData* pData, float fDistance)
{
	CColPoint point;
	CEntity* entity;

	if (!CWorld::ProcessVerticalLine(vecPosn + CVector(0.0f, 0.0f, fDistance),
		-fDistance, point, entity, true, false, false, false, true, false, NULL))
		return false;

	*pfOutLevel = point.m_vecPoint.z;

	if (pData != NULL)
	{
		pData->SurfaceType = point.m_nSurfaceTypeB;
		pData->PieceType = point.m_nPieceTypeB;
	}

	return true;
}

bool
CWaterLevel::IsLocationOutOfWorldBounds_WS(CVector const& vecPosn, int nOffset)
{
	int32_t x = int32_t((vecPosn.x / 50.0f) + 48.0f);
	int32_t y = int32_t((vecPosn.y / 50.0f) + 40.0f);

	return x < nOffset || x >= 80 - nOffset || y < nOffset || y >= 80 - nOffset;
}

bool
CWaterLevel::GetGroundLevel_WS(CVector const& vecPosn, float* pfOutLevel, ColData* pData, float fDistance)
{
	if (IsLocationOutOfWorldBounds_WS(vecPosn, 0))
		return false;
	else
		return GetGroundLevel(vecPosn, pfOutLevel, pData, fDistance);
}

bool
CWaterLevel::GetWaterDepth(CVector const& vecPosn, float* pfDepth, float* pfLevelNoWaves, float* pfGroundLevel)
{
	float fLevelNoWaves;
	float fGroundLevel;

	if (!GetWaterLevelNoWaves(vecPosn.x, vecPosn.y, vecPosn.z, &fLevelNoWaves))
		return false;

	if (!GetGroundLevel(vecPosn, &fGroundLevel, NULL, 30.0f))
		fGroundLevel = -100.0;

	if (pfDepth != NULL)
		*pfDepth = fLevelNoWaves - fGroundLevel;

	if (pfLevelNoWaves != NULL)
		*pfLevelNoWaves = fLevelNoWaves;

	if (pfGroundLevel != NULL)
		*pfGroundLevel = fGroundLevel;

	return true;
}

/*void
CWaterLevel::RenderSeaBirds()
{
	CVector cur_pos = TheCamera.GetPosition();

	if (!CCullZones::CamNoRain()
		&& !CCullZones::PlayerNoRain()
		&& (CWeather::NewWeatherType == WEATHER_SUNNY || CWeather::NewWeatherType == WEATHER_EXTRA_SUNNY)
		&& CClock::ms_nGameClockHours > 6 && CClock::ms_nGameClockHours < 20)
	{
		static CVector prev_pos(0.0f, 0.0f, 0.0f);
		static CVector prev_front(0.0f, 0.0f, 0.0f);
		static int32_t timecounter;

		if (Abs(prev_pos.x - cur_pos.x) + Abs(prev_pos.y - cur_pos.y) + Abs(prev_pos.z - cur_pos.z) > 1.5f)
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

				uint16 nSinCosIdx = CGeneral::GetRandomNumber() % (CParticle::SIN_COS_TABLE_SIZE - 1);

				float fCos = CParticle::Cos(nSinCosIdx);
				float fSin = CParticle::Sin(nSinCosIdx);

				vecPos.x += (fCos - fSin) * fAngle;
				vecPos.y += (fSin + fCos) * fAngle;
				vecPos.z += CGeneral::GetRandomNumberInRange(10.0f, 30.0f);

				CVector vecDir(CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
					CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
					0.0f);

				CParticle::AddParticle(PARTICLE_BIRD_FRONT, vecPos, vecDir, NULL, 0.0f, 0, 0, 0, 0);
			}
		}
	}
}

void
CWaterLevel::RenderShipsOnHorizon()
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

			uint16 nSinCosIdx = CGeneral::GetRandomNumber() % (CParticle::SIN_COS_TABLE_SIZE - 1);

			float fCos = CParticle::Cos(nSinCosIdx);
			float fSin = CParticle::Sin(nSinCosIdx);

			vecPos.x += (fCos - fSin) * fAngle;
			vecPos.y += (fSin + fCos) * fAngle;

			float fLevelNoWaves;

			if (GetWaterLevelNoWaves(vecPos.x, vecPos.y, vecPos.z, &fLevelNoWaves))
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
						NULL, 0.0f, 0, 0, CGeneral::GetRandomNumber() & 7, 0);
				}
			}
		}
	}
}

void
CWaterLevel::HandleSeaLifeForms()
{
	if (CReplay::IsPlayingBack())
		return;

	CVector cur_pos = FindPlayerPed()->GetPosition();

	static CVector prev_pos(0.0f, 0.0f, 0.0f);
	static int32_t timecounter;

	if (Abs(prev_pos.x - cur_pos.x) + Abs(prev_pos.y - cur_pos.y) + Abs(prev_pos.z - cur_pos.z) > 1.5f)
	{
		prev_pos = cur_pos;
		timecounter = CTimer::m_snTimeInMilliseconds;
	}
	else if ((CTimer::m_snTimeInMilliseconds - timecounter) > 5000)
	{
		if (CWaterCreatures::IsSpaceForMoreWaterCreatures())
		{
			for (int32_t i = 0; i < 3; i++)
			{
				CVector vecPos = cur_pos;

				float fAngle = CGeneral::GetRandomNumberInRange(15.0f, 30.0f);

				uint16 nSinCosIdx = CGeneral::GetRandomNumber() % (CParticle::SIN_COS_TABLE_SIZE - 1);

				float fCos = CParticle::Cos(nSinCosIdx);
				float fSin = CParticle::Sin(nSinCosIdx);

				vecPos.x += (fCos - fSin) * fAngle;
				vecPos.y += (fSin + fCos) * fAngle;

				CWaterCreatures::CreateOne(vecPos, -1);
			}
		}
	}

	CWaterCreatures::UpdateAll();
}

void
CWaterLevel::HandleBeachToysStuff(void)
{
#ifdef FIX_BUGS
	CVector cur_pos = FindPlayerCoors(-1);
#else
	CVector cur_pos = FindPlayerPed()->GetPosition();
#endif

	static bool bBeachBallInit = true;
	static CVector FirstBeachBallPos = cur_pos;
	static bool bLoungeInit = true;
	static CVector FirstLoungePos = cur_pos;
	static CVector prev_pos(0.0f, 0.0f, 0.0f);
	static int32_t timecounter;

	if (fabsf(prev_pos.x - cur_pos.x) + fabsf(prev_pos.y - cur_pos.y) + fabsf(prev_pos.z - cur_pos.z) > 1.5f)
	{
		prev_pos = cur_pos;
		timecounter = CTimer::m_snTimeInMilliseconds;
	}
	else if ((CTimer::m_snTimeInMilliseconds - timecounter) > 5000)
	{
		static int32_t toygenTime = CTimer::m_snTimeInMilliseconds;

		if ((CTimer::m_snTimeInMilliseconds - toygenTime) > 20000)
		{
			toygenTime = CTimer::m_snTimeInMilliseconds;

			if (bBeachBallInit || (cur_pos - FirstBeachBallPos).MagnitudeSqr() > 6400.0f)
			{
				for (int32_t i = 0; i < 3; i++)
				{
					CVector vecPos = cur_pos;

					float fAngle = CGeneral::GetRandomNumberInRange(20.0f, 35.0f);

					uint16_t nSinCosIdx = GetRandomNumber() % (CParticle::SIN_COS_TABLE_SIZE - 1);

					float fCos = CParticle::Cos(nSinCosIdx);
					float fSin = CParticle::Sin(nSinCosIdx);

					vecPos.x += (fCos - fSin) * fAngle;
					vecPos.y += (fSin + fCos) * fAngle;

					if (TheCamera.IsSphereVisible(vecPos, 1.0f))
					{
						float fWaterLevel;

						if (!GetWaterLevel(vecPos.x, vecPos.y, vecPos.z, &fWaterLevel, false))
						{
							float fGroundLevel;
							ColData coldata;

							if (GetGroundLevel(vecPos, &fGroundLevel, &coldata, 30.0f))
							{
								if (coldata.SurfaceType == SURFACE_SAND)
								{
									CEntity* toy = CreateBeachToy(vecPos, BEACHTOY_BALL);

									if (toy)
									{
										FirstBeachBallPos = cur_pos;
										bBeachBallInit = false;
										i = 10;
									}
								}
							}
						}
					}
				}
			}

			if (bLoungeInit || (cur_pos - FirstLoungePos).MagnitudeSqr() > 6400.0f)
			{
				for (int32_t i = 0; i < 5; i++)
				{
					CVector vecPos = cur_pos;

					float fAngle = CGeneral::GetRandomNumberInRange(20.0f, 35.0f);

					uint16 nSinCosIdx = CGeneral::GetRandomNumber() % (CParticle::SIN_COS_TABLE_SIZE - 1);

					float fCos = CParticle::Cos(nSinCosIdx);
					float fSin = CParticle::Sin(nSinCosIdx);

					vecPos.x += (fCos - fSin) * fAngle;
					vecPos.y += (fSin + fCos) * fAngle;

					if (TheCamera.IsSphereVisible(vecPos, 2.0f))
					{
						float fWaterLevel;

						if (!GetWaterLevel(vecPos.x, vecPos.y, vecPos.z, &fWaterLevel, false))
						{
							float fGroundLevel;
							ColData coldata;

							if (GetGroundLevel(vecPos, &fGroundLevel, &coldata, 30.0f))
							{
								if (coldata.SurfaceType == SURFACE_SAND)
								{
									CEntity* toy = CreateBeachToy(vecPos, BEACHTOY_ANY_LOUNGE);
									if (toy)
									{
										toy->SetHeading(DEGTORAD(CGeneral::GetRandomNumberInRange(0.0f, 359.0f)));
										FirstLoungePos = cur_pos;
										bLoungeInit = false;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

CEntity*
CWaterLevel::CreateBeachToy(CVector const& vec, eBeachToy beachtoy)
{
	if (CObject::nNoTempObjects >= NUMTEMPOBJECTS)
		return NULL;

	int finalToy = beachtoy;
	bool isStatic = false;
	int model = MI_BEACHBALL;
	switch (beachtoy) {
	case BEACHTOY_ANY_LOUNGE:
		switch (CGeneral::GetRandomNumber() & 7) {
		case 1:
		case 7:
			finalToy = BEACHTOY_LOUNGE_WOOD_UP;
			break;
		case 3:
		case 5:
			finalToy = BEACHTOY_LOUNGE_TOWEL_UP;
			break;
		default:
			finalToy = BEACHTOY_LOUNGE_WOOD_ON;
			break;
		}
		break;
	case BEACHTOY_ANY_TOWEL:
		switch (CGeneral::GetRandomNumber() & 7) {
		case 1:
		case 7:
			finalToy = BEACHTOY_TOWEL2;
			break;
		case 2:
		case 6:
			finalToy = BEACHTOY_TOWEL3;
			break;
		case 3:
		case 5:
			finalToy = BEACHTOY_TOWEL4;
			break;
		default:
			finalToy = BEACHTOY_TOWEL1;
			break;
		}
		if (CObject::nNoTempObjects >= 35) {
			return NULL;
		}
	default:
		break;
	}
	switch (finalToy) {
	case BEACHTOY_BALL:
		isStatic = false;
		model = MI_BEACHBALL;
		break;
	case BEACHTOY_LOUNGE_WOOD_UP:
		isStatic = false;
		model = MI_LOUNGE_WOOD_UP;
		break;
	case BEACHTOY_LOUNGE_TOWEL_UP:
		isStatic = false;
		model = MI_LOUNGE_TOWEL_UP;
		break;
	case BEACHTOY_LOUNGE_WOOD_ON:
		isStatic = false;
		model = MI_LOUNGE_WOOD_DN;
		break;
	case BEACHTOY_LOTION:
		model = MI_LOTION;
		isStatic = true;
		break;
	case BEACHTOY_TOWEL1:
		model = MI_BEACHTOWEL01;
		isStatic = true;
		break;
	case BEACHTOY_TOWEL2:
		model = MI_BEACHTOWEL02;
		isStatic = true;
		break;
	case BEACHTOY_TOWEL3:
		model = MI_BEACHTOWEL03;
		isStatic = true;
		break;
	case BEACHTOY_TOWEL4:
		model = MI_BEACHTOWEL04;
		isStatic = true;
		break;
	default:
		break;
	}
	CObject* toy = new CObject(model, true);
	if (toy) {
		toy->SetPosition(vec);
		toy->GetMatrix().UpdateRW();
		toy->m_vecMoveSpeed = CVector(0.f, 0.f, 0.f);
		toy->m_vecTurnSpeed = CVector(0.f, 0.f, 0.f);
		toy->ObjectCreatedBy = TEMP_OBJECT;
		toy->bIsStatic = isStatic;
		CObject::nNoTempObjects++;
		toy->m_nEndOfLifeTime = CTimer::m_snTimeInMilliseconds + 43200000;
		CWorld::Add(toy);
		return toy;
	}
	else
		return NULL;
}*/
/*class VCWaterSA {
public:
	VCWaterSA() {
		// Добавьте здесь код инициализации плагина
		Events::initGameEvent += []() {
			CWaterLevel::Initialise(PLUGIN_PATH((char*)"DATA\\WATER.DAT"));
		//	patch::RedirectJump(0x6EF650, CWaterLevel::RenderWater);
		//	patch::RedirectJump(0x6E59E0, CWaterLevel::Shutdown);
			//patch::RedirectJump(0x6ED9A0, CWaterLevel::RenderBoatWakes);
			//patch::RedirectJump(0x6E7670, CWaterLevel::RenderTransparentWater);
		};

		RenderEffectsEvent22 += []() {
		//	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
			CWaterLevel::RenderWater();
		//	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
			CWaterLevel::RenderTransparentWater();
		};
	}
} vCWaterSA;*/
