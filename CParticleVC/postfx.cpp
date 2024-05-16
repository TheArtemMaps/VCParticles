#include "plugin.h"
#include "postfx.h"
#include "Utility.h"
//RwRaster *CMBlur2::pBackBuffer;

RwRaster *CMBlur2::ms_pRadiosityRaster1;
RwRaster *CMBlur2::ms_pRadiosityRaster2;
RwIm2DVertex CMBlur2::ms_radiosityVerts[44];
RwImVertexIndex CMBlur2::ms_radiosityIndices[7*6] = {
	0, 1, 2, 2, 1, 3,
		4, 5, 2, 2, 5, 3,
	4, 5, 6, 6, 5, 7,
		8, 9, 6, 6, 9, 7,
	8, 9, 10, 10, 9, 11,
		12, 13, 10, 10, 13, 11,
	12, 13, 14, 14, 13, 15,
};


extern RwD3D9Vertex *blurVertices;
extern RwImVertexIndex *blurIndices;

RwD3D9Vertex screen_vertices[24];
RwImVertexIndex screen_indices[] = {
	0, 1, 2, 2, 1, 3,
	4, 5, 6, 6, 5, 7,
	8, 9, 10, 10, 9, 11,
};

static void *contrastPS;


void
makequad(RwCamera *cam, RwIm2DVertex *v, int width, int height, int texwidth = 0, int texheight = 0)
{
	float w, h, tw, th;
	w = width;
	h = height;
	tw = texwidth > 0 ? texwidth : w;
	th = texheight > 0 ? texheight : h;

	const float nearz = RwIm2DGetNearScreenZ();
	const float recipz = 1.0f/RwCameraGetNearClipPlane(cam);

	RwIm2DVertexSetScreenX(&v[0], 0);
	RwIm2DVertexSetScreenY(&v[0], 0);
	RwIm2DVertexSetScreenZ(&v[0], 0);
	RwIm2DVertexSetCameraZ(&v[0], nearz);
	RwIm2DVertexSetRecipCameraZ(&v[0], recipz);
	RwIm2DVertexSetU(&v[0], 0.5f / tw, recipz);
	RwIm2DVertexSetV(&v[0], 0.5f / th, recipz);
	RwIm2DVertexSetIntRGBA(&v[0], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&v[1], 0);
	RwIm2DVertexSetScreenY(&v[1], h);
	RwIm2DVertexSetScreenZ(&v[1], 0);
	RwIm2DVertexSetCameraZ(&v[1], nearz);
	RwIm2DVertexSetRecipCameraZ(&v[1], recipz);
	RwIm2DVertexSetU(&v[1], 0.5f / tw, recipz);
	RwIm2DVertexSetV(&v[1], (h + 0.5f) / th, recipz);
	RwIm2DVertexSetIntRGBA(&v[1], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&v[2], w);
	RwIm2DVertexSetScreenY(&v[2], 0);
	RwIm2DVertexSetScreenZ(&v[2], 0);
	RwIm2DVertexSetCameraZ(&v[2], nearz);
	RwIm2DVertexSetRecipCameraZ(&v[2], recipz);
	RwIm2DVertexSetU(&v[2], (w + 0.5f) / tw, recipz);
	RwIm2DVertexSetV(&v[2], 0.5f / th, recipz);
	RwIm2DVertexSetIntRGBA(&v[2], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&v[3], w);
	RwIm2DVertexSetScreenY(&v[3], h);
	RwIm2DVertexSetScreenZ(&v[3], 0);
	RwIm2DVertexSetCameraZ(&v[3], nearz);
	RwIm2DVertexSetRecipCameraZ(&v[3], recipz);
	RwIm2DVertexSetU(&v[3], (w + 0.5f) / tw, recipz);
	RwIm2DVertexSetV(&v[3], (h + 0.5f) / th, recipz);
	RwIm2DVertexSetIntRGBA(&v[3], 255, 255, 255, 255);
}
RwRaster* pBackBuffer;
void
CMBlur2::Initialise(void)
{
	//if(pBackBuffer){
	//	RwRasterDestroy(pBackBuffer);
		//pBackBuffer = NULL;
	//}
	//if(pBackBuffer == NULL)
		pBackBuffer = RwRasterCreate(RwRasterGetWidth(pFrontBuffer),
			RwRasterGetHeight(pFrontBuffer),
			RwRasterGetDepth(pFrontBuffer),
			rwRASTERTYPECAMERATEXTURE);
	//if(RwD3D9Supported())
		//if(contrastPS == nil){
		//	#include "contrastPS.h"
		//	RwD3D9CreatePixelShader((RwUInt32*)g_ps20_main, &contrastPS);
		//	assert(contrastPS);
	//	}
}

/* PS2 code. frame*blurcolour is added to frame, equivalent to multiplication by 255+blurcolour, i.e. it can only brighten.
 * LCS PSP multiplies final frame with 255-blurcolour, i.e. it can only darken. */
void
CMBlur2::OverlayRender_leeds(RwCamera *cam, RwRaster *frontbuf, RwRGBA *col, uint8_t type)
{
	int i;
	int bufw, bufh;
	int screenw, screenh;
	int blurintensity;
	int motionblurintensity;

	DefinedState();
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);

	bufw = RwRasterGetWidth(frontbuf);
	bufh = RwRasterGetHeight(frontbuf);
	screenw = RwCameraGetRaster(cam)->width;
	screenh = RwCameraGetRaster(cam)->height;

	blurintensity = 0.8f;
	motionblurintensity = 32;

	// Blur
	makequad(cam, screen_vertices, screenw, screenh, bufw, bufh);
	for(i = 0; i < 4; i++){
		screen_vertices[i].x += 1.0f;
		RwIm2DVertexSetIntRGBA(&screen_vertices[i], 255, 255, 255, blurintensity);
	}
	makequad(cam, screen_vertices+4, screenw, screenh, bufw, bufh);
	for(i = 4; i < 8; i++){
		screen_vertices[i].x += 1.0f;
		screen_vertices[i].y += 1.0f;
		RwIm2DVertexSetIntRGBA(&screen_vertices[i], 255, 255, 255, blurintensity);
	}
	makequad(cam, screen_vertices+8, screenw, screenh, bufw, bufh);
	for(i = 8; i < 12; i++){
		screen_vertices[i].y += 1.0f;
		RwIm2DVertexSetIntRGBA(&screen_vertices[i], 255, 255, 255, blurintensity);
	}

	// Colour filter
	makequad(cam, screen_vertices+12, screenw, screenh, bufw, bufh);
	for(i = 12; i < 16; i++)
		RwIm2DVertexSetIntRGBA(&screen_vertices[i], col->red, col->green, col->blue, 255);

	// Blend with last frame
	makequad(cam, screen_vertices+16, screenw, screenh, bufw, bufh);
	for(i = 16; i < 20; i++)
		RwIm2DVertexSetIntRGBA(&screen_vertices[i], 255, 255, 255, motionblurintensity);

/*
	makequad(cam, screen_vertices+20, screenw, screenh, bufw, bufh);
	for(i = 20; i < 24; i++)
		RwIm2DVertexSetIntRGBA(&screen_vertices[i], 0, 0, 0, 0);
*/

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pBackBuffer);

	// Blur
	//if(config.trailsBlur){
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screen_vertices, 12, screen_indices, 3*6);
	//}

	// Apply colour filter
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screen_vertices+12, 4, screen_indices, 6);

	// Motion blur
	if(ms_bJustInitialised)
		ms_bJustInitialised = false;
	else{
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, frontbuf);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screen_vertices+16, 4, screen_indices, 6);
	}
}

void
CMBlur2::RadiosityInit(RwCamera *cam)
{
	ms_pRadiosityRaster1 = RwRasterCreate(256, 128, RwCameraGetRaster(cam)->depth, rwRASTERTYPECAMERATEXTURE);
	ms_pRadiosityRaster2 = RwRasterCreate(256, 128, RwCameraGetRaster(cam)->depth, rwRASTERTYPECAMERATEXTURE);
//	ms_pRadiosityRaster1 = RwRasterCreate(pFrontBuffer->width, pFrontBuffer->height, pFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);
//	ms_pRadiosityRaster2 = RwRasterCreate(pFrontBuffer->width, pFrontBuffer->height, pFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);
}

void
CMBlur2::RadiosityCreateImmediateData(RwCamera *cam)
{
	static float uOffsets[] = { -1.0f, 1.0f, 0.0f, 0.0f,   -1.0f, 1.0f, -1.0f, 1.0f };
	static float vOffsets[] = { 0.0f, 0.0f, -1.0f, 1.0f,   -1.0f, -1.0f, 1.0f, 1.0f };
	int i;
	RwUInt32 c;
	int w, h;

	w = 256;
	h = 128;

	// TODO: tex coords correct?
	makequad(cam, ms_radiosityVerts, 256, 128);
	makequad(cam, ms_radiosityVerts+4, RwCameraGetRaster(cam)->width, RwCameraGetRaster(cam)->height);

	// black vertices; at 8
	for(i = 0; i < 4; i++){
		ms_radiosityVerts[i+8] = ms_radiosityVerts[i];
		ms_radiosityVerts[i+8].emissiveColor = 0;
	}

	// two sets blur vertices; at 12
	c = D3DCOLOR_ARGB(0xFF, 36, 36, 36);
	for(i = 0; i < 2*4*4; i++){
		ms_radiosityVerts[i+12] = ms_radiosityVerts[i%4];
		ms_radiosityVerts[i+12].emissiveColor = c;
		switch(i%4){
		case 0:
			ms_radiosityVerts[i+12].u = (uOffsets[i/4] + 0.5f) / w;
			ms_radiosityVerts[i+12].v = (vOffsets[i/4] + 0.5f) / h;
			break;
		case 1:
			ms_radiosityVerts[i+12].u = (uOffsets[i/4] + 0.5f) / w;
			ms_radiosityVerts[i+12].v = (h + vOffsets[i/4] + 0.5f) / h;
			break;
		case 2:
			ms_radiosityVerts[i+12].u = (w + uOffsets[i/4] + 0.5f) / w;
			ms_radiosityVerts[i+12].v = (vOffsets[i/4] + 0.5f) / h;
			break;
		case 3:
			ms_radiosityVerts[i+12].u = (w + uOffsets[i/4] + 0.5f) / w;
			ms_radiosityVerts[i+12].v = (h + vOffsets[i/4] + 0.5f) / h;
			break;
		}
	}
}

void
CMBlur2::RadiosityRender(RwCamera *cam, int limit, int intensity)
{
	int i;
	RwRaster *fb;
	RwRaster *fb1, *fb2, *tmp;

	if(ms_pRadiosityRaster1 == NULL || ms_pRadiosityRaster2 == NULL)
		RadiosityInit(cam);

	fb = RwCameraGetRaster(cam);
	assert(ms_pRadiosityRaster1);
	assert(ms_pRadiosityRaster1);

	RwRect r;
	r.x = 0;
	r.y = 0;
	r.w = 256;
	r.h = 128;

//	RwRasterPushContext(ms_pRadiosityRaster2);
//	RwRasterRenderScaled(fb, &r);
////	RwRasterRenderFast(fb, 0, 0);
//	RwRasterPopContext();

	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	RadiosityCreateImmediateData(cam);


	// Have to jump through extra hoops to downsample because RwRasterRender* doesn't seem to work
	static RwIm2DVertex tmpverts[4];
	makequad(cam, tmpverts, RwCameraGetRaster(cam)->width, RwCameraGetRaster(cam)->height,
		RwRasterGetWidth(pBackBuffer), RwRasterGetHeight(pBackBuffer));
	RwIm2DVertexSetScreenX(&tmpverts[0], 0);
	RwIm2DVertexSetScreenY(&tmpverts[0], 0);
	RwIm2DVertexSetScreenX(&tmpverts[1], 0);
	RwIm2DVertexSetScreenY(&tmpverts[1], 128);
	RwIm2DVertexSetScreenX(&tmpverts[2], 256);
	RwIm2DVertexSetScreenY(&tmpverts[2], 0);
	RwIm2DVertexSetScreenX(&tmpverts[3], 256);
	RwIm2DVertexSetScreenY(&tmpverts[3], 128);

	RwCameraEndUpdate(cam);
	RwCameraSetRaster(cam, ms_pRadiosityRaster2);
	RwCameraBeginUpdate(cam);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)0);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pBackBuffer);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, tmpverts, 4, ms_radiosityIndices, 6);


	for(i = 0; i < 4; i++)
		ms_radiosityVerts[i].emissiveColor = D3DCOLOR_ARGB(limit/2, 255, 255, 255);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, ms_radiosityVerts, 4, ms_radiosityIndices, 6);
	RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	fb1 = ms_pRadiosityRaster1;
	fb2 = ms_pRadiosityRaster2;
	for(i = 0; i < 3; i++){
		RwCameraEndUpdate(cam);
		RwCameraSetRaster(cam, fb1);
		RwCameraBeginUpdate(cam);

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)0);
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, ms_radiosityVerts+8, 4, ms_radiosityIndices, 6);

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, fb2);
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSCLAMP);
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSCLAMP);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
		if((i % 2) == 0)
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, ms_radiosityVerts+12, 4*4, ms_radiosityIndices, 6*7);
		else
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, ms_radiosityVerts+28, 4*4, ms_radiosityIndices, 6*7);

		tmp = fb1;
		fb1 = fb2;
		fb2 = tmp;
	}

	RwCameraEndUpdate(cam);
	RwCameraSetRaster(cam, fb);
	RwCameraBeginUpdate(cam);

	for(i = 4; i < 8; i++)
		ms_radiosityVerts[i].emissiveColor = D3DCOLOR_ARGB(intensity*4, 255, 255, 255);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, fb2);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSCLAMP);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSCLAMP);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	for(i = 0; i < 3; i++)
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, ms_radiosityVerts+4, 4, ms_radiosityIndices, 6);

	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
}

void
CMBlur2::MotionBlurRender_leeds(RwCamera *cam, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t type/*, uint8 mbluralpha*/)
{
	RwRGBA col;
	if(BlurOn){
		col.red = red;
		col.green = green;
		col.blue = blue;
		col.alpha = alpha;

		if(pBackBuffer == NULL ||
		   RwRasterGetWidth(pBackBuffer) != RwRasterGetWidth(pFrontBuffer) ||
		   RwRasterGetHeight(pBackBuffer) != RwRasterGetHeight(pFrontBuffer))
			Initialise();

		RwRasterPushContext(pBackBuffer);
		RwRasterRenderFast(cam->frameBuffer, 0, 0);
		RwRasterPopContext();

		//if(config.radiosity < 0) config.radiosity = 0;
		//if(config.radiosity)
			RadiosityRender(cam, 3, 3);

		RwRasterPushContext(pBackBuffer);
		RwRasterRenderFast(cam->frameBuffer, 0, 0);
		RwRasterPopContext();

		OverlayRender_leeds(cam, pFrontBuffer, &col, type);

		RwRasterPushContext(pFrontBuffer);
		RwRasterRenderFast(cam->frameBuffer, 0, 0);
		RwRasterPopContext();
	}
}

void
CMBlur2::MotionBlurRender_mobile(RwCamera *cam, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t type)
{
	int bufw, bufh;
	int screenw, screenh;

	//if(!BlurOn || !RwD3D9Supported())
	//	return;

	if(pBackBuffer == NULL ||
	   RwRasterGetWidth(pBackBuffer) != RwRasterGetWidth(pFrontBuffer) ||
	   RwRasterGetHeight(pBackBuffer) != RwRasterGetHeight(pFrontBuffer))
		Initialise();

	RwRasterPushContext(pBackBuffer);
	RwRasterRenderFast(cam->frameBuffer, 0, 0);
	RwRasterPopContext();



	bufw = RwRasterGetWidth(pBackBuffer);
	bufh = RwRasterGetHeight(pBackBuffer);
	screenw = RwCameraGetRaster(cam)->width;
	screenh = RwCameraGetRaster(cam)->height;

	makequad(cam, screen_vertices, screenw, screenh, bufw, bufh);

	float mult[3], add[3];
		mult[0] = (red-64)/256.0f + 1.4f;
		mult[1] = (green-64)/256.0f + 1.4f;
		mult[2] = (blue-64)/256.0f + 1.4f;
		add[0] = red/1536.f - 0.05f;
		add[1] = green/1536.f - 0.05f;
		add[2] = blue/1536.f - 0.05f;
	
	//RwD3D9SetPixelShaderConstant(3, &mult, 1);
	//RwD3D9SetPixelShaderConstant(4, &add, 1);

	//RwD3D9SetIm2DPixelShader(contrastPS);

	DefinedState();
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pBackBuffer);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSCLAMP);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSCLAMP);

	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, screen_vertices, 4, screen_indices, 6);

	//RwD3D9SetIm2DPixelShader(nil);
}
