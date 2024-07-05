#pragma once
enum FxType
{
	FXTYPE_WATER1,
	FXTYPE_WATER2,
	FXTYPE_BLOOD1,
	FXTYPE_BLOOD2,
	FXTYPE_HEATHAZE,
	FXTYPE_SPLASH1,
	FXTYPE_SPLASH2,
	FXTYPE_SPLASH3
};

enum MotionBlurType
{
	MOTION_BLUR_NONE = 0,
	MOTION_BLUR_SNIPER,
	MOTION_BLUR_LIGHT_SCENE,
	MOTION_BLUR_SECURITY_CAM,
	MOTION_BLUR_CUT_SCENE,
	MOTION_BLUR_INTRO,
	MOTION_BLUR_INTRO2,
	MOTION_BLUR_SNIPER_ZOOM,
	MOTION_BLUR_INTRO3,
	MOTION_BLUR_INTRO4,
};
class CMBlur
{
public:
	static RwRaster* pFrontBuffer;
	static bool ms_bJustInitialised;
	static bool ms_bScaledBlur;
	static bool BlurOn;
	static float Drunkness;

	static int32_t pBufVertCount;

public:
	static RwBool MotionBlurOpen(RwCamera* cam);
	static RwBool MotionBlurClose(void);
	static void CreateImmediateModeData(RwCamera* cam, RwRect* rect);
	static void CreateImmediateModeData(RwCamera* cam, RwRect* rect, RwIm2DVertex* verts, RwRGBA color, float u1Off, float v1Off, float u2Off, float v2Off, float z, int fullTexture);
	static void MotionBlurRender(RwCamera* cam, uint32_t red, uint32_t green, uint32_t blue, uint32_t blur, int32_t type, uint32_t bluralpha);
	static void OverlayRender(RwCamera* cam, RwRaster* raster, RwRGBA color, int32_t type, int32_t bluralpha);
	static void SetDrunkBlur(float drunkness);
	static void ClearDrunkBlur();
	static void RenderMotionBlur();
	static bool PosInside(RwRect* rect, float x1, float y1, float x2, float y2);
	static bool AddRenderFx(RwCamera* cam, RwRect* rect, float z, FxType type);
	static void OverlayRenderFx(RwCamera* cam, RwRaster* frontBuf);
};
void
SetMotionBlur(int r, int g, int b, int a, int type);
void
SetMotionBlurAlpha(int a);
extern bool clearWaterDrop;
extern int32_t numWaterDropOnScreen;
extern RwRaster* gpCarSplashRaster[];
extern RwRaster* gpHeatHazeRaster;
extern RwRaster* gpDotRaster;
extern RwRaster* gpRainDripRaster[];
extern RwRaster* gpRainDripDarkRaster[];
extern FxType BlurType;
extern uint32_t m_BlurRed, m_BlurGreen, m_BlurBlue, m_motionBlur, m_BlurType, m_imotionBlurAddAlpha;