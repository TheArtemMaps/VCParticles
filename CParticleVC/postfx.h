#pragma once
#include "MBlurSA.h"
class CMBlur2 : public CMBlur {
public:

	//static bool &ms_bJustInitialised;
	//static bool &BlurOn;
	//static RwRaster *&pFrontBuffer;	// this holds the last rendered frame
	//static RwRaster *pBackBuffer;	// this is a copy of the current frame

	// VCS Radiosity;
	static RwRaster *ms_pRadiosityRaster1;
	static RwRaster *ms_pRadiosityRaster2;
	static RwIm2DVertex ms_radiosityVerts[44];
	static RwImVertexIndex ms_radiosityIndices[7*6];

	static void MotionBlurRender_leeds(RwCamera *cam, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t type);
	static void OverlayRender_leeds(RwCamera *cam, RwRaster *frontbuf, RwRGBA *col, uint8_t type);
	static void MotionBlurRender_mobile(RwCamera *cam, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t type);

	static void RadiosityInit(RwCamera *cam);
	static void RadiosityCreateImmediateData(RwCamera *cam);
	static void RadiosityRender(RwCamera *cam, int limit, int intensity);

	static void Initialise(void);
};

