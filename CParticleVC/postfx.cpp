#define WITHD3D
#include "plugin.h"
#define EXTENDED_COLOURFILTER

#define LIBRW

#ifndef LIBRW
#error "Need librw for EXTENDED_COLOURFILTER"
#endif

#include "CCamera.h"
#include "MBlurSA.h"
#include "postfx.h"
#include "CParticleVC.h"

RwRaster* CPostFX::pFrontBuffer;
RwRaster* CPostFX::pBackBuffer;
bool CPostFX::bJustInitialised;
int CPostFX::EffectSwitch = POSTFX_NORMAL;
bool CPostFX::BlurOn = false;
bool CPostFX::MotionBlurOn = false;

static RwIm2DVertex Vertex[4];
static RwIm2DVertex Vertex2[4];
static RwIm2DVertex Vertex3[4];
static RwImVertexIndex Index[6] = { 0, 1, 2, 0, 2, 3 };

#ifdef RW_D3D9
void* colourfilterVC_PS;
void* contrast_PS;
#endif
#ifdef RW_OPENGL
int32 u_blurcolor;
int32 u_contrastAdd;
int32 u_contrastMult;
rw::gl3::Shader* colourFilterVC;
rw::gl3::Shader* contrast;
#endif

void
CPostFX::InitOnce(void)
{
#ifdef RW_OPENGL
	u_blurcolor = rw::gl3::registerUniform("u_blurcolor");
	u_contrastAdd = rw::gl3::registerUniform("u_contrastAdd");
	u_contrastMult = rw::gl3::registerUniform("u_contrastMult");
#endif
}

void
CPostFX::Open(RwCamera* cam)
{
	if (pFrontBuffer)
		Close();

	uint32 width = Pow(2.0f, int32(log2(RwRasterGetWidth(RwCameraGetRaster(cam)))) + 1);
	uint32 height = Pow(2.0f, int32(log2(RwRasterGetHeight(RwCameraGetRaster(cam)))) + 1);
	uint32 depth = RwRasterGetDepth(RwCameraGetRaster(cam));
	pFrontBuffer = RwRasterCreate(width, height, depth, rwRASTERTYPECAMERATEXTURE);
	pBackBuffer = RwRasterCreate(width, height, depth, rwRASTERTYPECAMERATEXTURE);
	bJustInitialised = true;

	float zero, xmax, ymax;

	if (RwRasterGetDepth(RwCameraGetRaster(cam)) == 16) {
		zero = HALFPX;
		xmax = width + HALFPX;
		ymax = height + HALFPX;
	}
	else {
		zero = -HALFPX;
		xmax = width - HALFPX;
		ymax = height - HALFPX;
	}

	RwIm2DVertexSetScreenX(&Vertex[0], zero);
	RwIm2DVertexSetScreenY(&Vertex[0], zero);
	RwIm2DVertexSetScreenZ(&Vertex[0], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[0], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex[0], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex[0], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex[0], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex[0], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex[1], zero);
	RwIm2DVertexSetScreenY(&Vertex[1], ymax);
	RwIm2DVertexSetScreenZ(&Vertex[1], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[1], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex[1], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex[1], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex[1], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex[1], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex[2], xmax);
	RwIm2DVertexSetScreenY(&Vertex[2], ymax);
	RwIm2DVertexSetScreenZ(&Vertex[2], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[2], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex[2], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex[2], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex[2], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex[2], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex[3], xmax);
	RwIm2DVertexSetScreenY(&Vertex[3], zero);
	RwIm2DVertexSetScreenZ(&Vertex[3], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex[3], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex[3], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex[3], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex[3], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex[3], 255, 255, 255, 255);


	RwIm2DVertexSetScreenX(&Vertex2[0], zero + 2.0f);
	RwIm2DVertexSetScreenY(&Vertex2[0], zero + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex2[0], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex2[0], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex2[0], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex2[0], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex2[0], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex2[0], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex2[1], 2.0f);
	RwIm2DVertexSetScreenY(&Vertex2[1], ymax + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex2[1], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex2[1], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex2[1], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex2[1], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex2[1], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex2[1], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex2[2], xmax + 2.0f);
	RwIm2DVertexSetScreenY(&Vertex2[2], ymax + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex2[2], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex2[2], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex2[2], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex2[2], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex2[2], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex2[2], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex2[3], xmax + 2.0f);
	RwIm2DVertexSetScreenY(&Vertex2[3], zero + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex2[3], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex2[3], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex2[3], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex2[3], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex2[3], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex2[3], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex3[0], zero + 2.0f);
	RwIm2DVertexSetScreenY(&Vertex3[0], zero + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex3[0], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex3[0], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex3[0], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex3[0], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex3[0], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex3[0], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex3[1], 2.0f);
	RwIm2DVertexSetScreenY(&Vertex3[1], ymax + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex3[1], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex3[1], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex3[1], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex3[1], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex3[1], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex3[1], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex3[2], xmax + 2.0f);
	RwIm2DVertexSetScreenY(&Vertex3[2], ymax + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex3[2], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex3[2], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex3[2], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex3[2], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex3[2], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex3[2], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&Vertex3[3], xmax + 2.0f);
	RwIm2DVertexSetScreenY(&Vertex3[3], zero + 2.0f);
	RwIm2DVertexSetScreenZ(&Vertex3[3], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetCameraZ(&Vertex3[3], RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetRecipCameraZ(&Vertex3[3], 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetU(&Vertex3[3], 1.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetV(&Vertex3[3], 0.0f, 1.0f / RwCameraGetNearClipPlane(cam));
	RwIm2DVertexSetIntRGBA(&Vertex3[3], 255, 255, 255, 255);


/*#ifdef RW_D3D9
#include "shaders/obj/colourfilterVC_PS.inc"
	colourfilterVC_PS = RwD3D9CreatePixelShader(colourfilterVC_PS_cso);
#include "shaders/obj/contrastPS.inc"
	contrast_PS = rw::d3d::createPixelShader(contrastPS_cso);
#endif*/
#ifdef RW_OPENGL
	using namespace rw::gl3;

	{
#include "shaders/obj/im2d_vert.inc"
#include "shaders/obj/colourfilterVC_frag.inc"
		const char* vs[] = { shaderDecl, header_vert_src, im2d_vert_src, nil };
		const char* fs[] = { shaderDecl, header_frag_src, colourfilterVC_frag_src, nil };
		colourFilterVC = Shader::create(vs, fs);
		assert(colourFilterVC);
	}

	{
#include "shaders/obj/im2d_vert.inc"
#include "shaders/obj/contrast_frag.inc"
		const char* vs[] = { shaderDecl, header_vert_src, im2d_vert_src, nil };
		const char* fs[] = { shaderDecl, header_frag_src, contrast_frag_src, nil };
		contrast = Shader::create(vs, fs);
		assert(contrast);
	}

#endif
}

void
CPostFX::Close(void)
{
	if (pFrontBuffer) {
		RwRasterDestroy(pFrontBuffer);
		pFrontBuffer = nil;
	}
	if (pBackBuffer) {
		RwRasterDestroy(pBackBuffer);
		pBackBuffer = nil;
	}
#ifdef RW_D3D9
	if (colourfilterVC_PS) {
	//	rw::d3d::destroyPixelShader(colourfilterVC_PS);
		colourfilterVC_PS = nil;
	}
	if (contrast_PS) {
		//RwD3DShader(contrast_PS);
		contrast_PS = nil;
	}
#endif
#ifdef RW_OPENGL
	if (colourFilterVC) {
		colourFilterVC->destroy();
		colourFilterVC = nil;
	}
	if (contrast) {
		contrast->destroy();
		contrast = nil;
	}
#endif
}

void
CPostFX::RenderOverlayBlur(RwCamera* cam, int32 r, int32 g, int32 b, int32 a)
{
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);

	RwIm2DVertexSetIntRGBA(&Vertex[0], r * 2, g * 2, b * 2, 30);
	RwIm2DVertexSetIntRGBA(&Vertex[1], r * 2, g * 2, b * 2, 30);
	RwIm2DVertexSetIntRGBA(&Vertex[2], r * 2, g * 2, b * 2, 30);
	RwIm2DVertexSetIntRGBA(&Vertex[3], r * 2, g * 2, b * 2, 30);
	RwIm2DVertexSetIntRGBA(&Vertex2[0], r * 2, g * 2, b * 2, 30);
	RwIm2DVertexSetIntRGBA(&Vertex2[1], r * 2, g * 2, b * 2, 30);
	RwIm2DVertexSetIntRGBA(&Vertex2[2], r * 2, g * 2, b * 2, 30);
	RwIm2DVertexSetIntRGBA(&Vertex2[3], r * 2, g * 2, b * 2, 30);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, BlurOn ? Vertex2 : Vertex, 4, Index, 6);


	RwIm2DVertexSetIntRGBA(&Vertex2[0], r, g, b, a);
	RwIm2DVertexSetIntRGBA(&Vertex[0], r, g, b, a);
	RwIm2DVertexSetIntRGBA(&Vertex2[1], r, g, b, a);
	RwIm2DVertexSetIntRGBA(&Vertex[1], r, g, b, a);
	RwIm2DVertexSetIntRGBA(&Vertex2[2], r, g, b, a);
	RwIm2DVertexSetIntRGBA(&Vertex[2], r, g, b, a);
	RwIm2DVertexSetIntRGBA(&Vertex2[3], r, g, b, a);
	RwIm2DVertexSetIntRGBA(&Vertex[3], r, g, b, a);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);

	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, Vertex, 4, Index, 6);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, BlurOn ? Vertex2 : Vertex, 4, Index, 6);
}

void
CPostFX::RenderOverlaySniper(RwCamera* cam, int32 r, int32 g, int32 b, int32 a)
{
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);

	RwIm2DVertexSetIntRGBA(&Vertex[0], r, g, b, 80);
	RwIm2DVertexSetIntRGBA(&Vertex[1], r, g, b, 80);
	RwIm2DVertexSetIntRGBA(&Vertex[2], r, g, b, 80);
	RwIm2DVertexSetIntRGBA(&Vertex[3], r, g, b, 80);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, Vertex, 4, Index, 6);
}

float CPostFX::Intensity = 1.0f;

void
CPostFX::RenderOverlayShader(RwCamera* cam, int32 r, int32 g, int32 b, int32 a)
{
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pBackBuffer);

	if (EffectSwitch == POSTFX_MOBILE) {
		float mult[3], add[3];
		mult[0] = (r - 64) / 256.0f + 1.4f;
		mult[1] = (g - 64) / 256.0f + 1.4f;
		mult[2] = (b - 64) / 256.0f + 1.4f;
		add[0] = r / 1536.f - 0.05f;
		add[1] = g / 1536.f - 0.05f;
		add[2] = b / 1536.f - 0.05f;
#ifdef RW_D3D9
		_rwD3D9SetPixelShaderConstant(10, mult, 1);
		_rwD3D9SetPixelShaderConstant(11, add, 1);

		//RwIm2Dinstant = contrast_PS;
#endif
#ifdef RW_OPENGL
		rw::gl3::im2dOverrideShader = contrast;
		contrast->use();
		glUniform3fv(contrast->uniformLocations[u_contrastMult], 1, mult);
		glUniform3fv(contrast->uniformLocations[u_contrastAdd], 1, add);
#endif
	}
	else {
		float f = Intensity;
		float blurcolors[4];
		blurcolors[0] = r * f / 255.0f;
		blurcolors[1] = g * f / 255.0f;
		blurcolors[2] = b * f / 255.0f;
		blurcolors[3] = 30 / 255.0f;
#ifdef RW_D3D9
		_rwD3D9SetPixelShaderConstant(10, blurcolors, 1);
	//	rw::d3d::im2dOverridePS = colourfilterVC_PS;
#endif
#ifdef RW_OPENGL
		rw::gl3::im2dOverrideShader = colourFilterVC;
		colourFilterVC->use();
		glUniform4fv(colourFilterVC->uniformLocations[u_blurcolor], 1, blurcolors);
#endif
	}
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, Vertex, 4, Index, 6);
#ifdef RW_D3D9
//	rw::d3d::im2dOverridePS = nil;
#endif
#ifdef RW_OPENGL
	rw::gl3::im2dOverrideShader = nil;
#endif
}
static uint8_t DrunkBlurRed = 128;
static uint8_t DrunkBlurGreen = 128;
static uint8_t DrunkBlurBlue = 128;
static int32_t DrunkBlurIncrement = 1;
void
CPostFX::RenderMotionBlur(RwCamera* cam, uint32 blur)
{
	if (blur == 0)
		return;

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	if (CMBlur::Drunkness > 0.0f) {
		if (!Pulsating) {
			RwIm2DVertexSetIntRGBA(&Vertex[0], 255, 255, 255, blur);
			RwIm2DVertexSetIntRGBA(&Vertex[1], 255, 255, 255, blur);
			RwIm2DVertexSetIntRGBA(&Vertex[2], 255, 255, 255, blur);
			RwIm2DVertexSetIntRGBA(&Vertex[3], 255, 255, 255, blur);
		}
		else {
			RwIm2DVertexSetIntRGBA(&Vertex[0], DrunkBlurRed, DrunkBlurGreen, DrunkBlurBlue, blur);
			RwIm2DVertexSetIntRGBA(&Vertex[1], DrunkBlurRed, DrunkBlurGreen, DrunkBlurBlue, blur);
			RwIm2DVertexSetIntRGBA(&Vertex[2], DrunkBlurRed, DrunkBlurGreen, DrunkBlurBlue, blur);
			RwIm2DVertexSetIntRGBA(&Vertex[3], DrunkBlurRed, DrunkBlurGreen, DrunkBlurBlue, blur);
			if (DrunkBlurIncrement) {
				if (DrunkBlurRed < 255) DrunkBlurRed++;
				if (DrunkBlurGreen < 255) DrunkBlurGreen++;
				if (DrunkBlurBlue < 255) DrunkBlurBlue++;
				if (DrunkBlurRed == 255)
					DrunkBlurIncrement = 0;
			}
			else {
				if (DrunkBlurRed > 128) DrunkBlurRed--;
				if (DrunkBlurGreen > 128) DrunkBlurGreen--;
				if (DrunkBlurBlue > 128) DrunkBlurBlue--;
				if (DrunkBlurRed == 128)
					DrunkBlurIncrement = 1;
			}
			//	log("r %d g %d b %d", DrunkBlurRed, DrunkBlurGreen, DrunkBlurBlue);

			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, Vertex, 4, Index, 6);
		}
	}

	RwIm2DVertexSetIntRGBA(&Vertex[0], 255, 255, 255, blur);
	RwIm2DVertexSetIntRGBA(&Vertex[1], 255, 255, 255, blur);
	RwIm2DVertexSetIntRGBA(&Vertex[2], 255, 255, 255, blur);
	RwIm2DVertexSetIntRGBA(&Vertex[3], 255, 255, 255, blur);

	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, Vertex, 4, Index, 6);
}

bool
CPostFX::NeedBackBuffer(void)
{
	// Current frame -- needed for non-blur effect
	switch (EffectSwitch) {
	case POSTFX_OFF:
	case POSTFX_SIMPLE:
		// no actual rendering here
		return false;
	case POSTFX_NORMAL:
		if (MotionBlurOn)
			return false;
		else
			return true;
	case POSTFX_MOBILE:
		return true;
	}
	return false;
}

bool
CPostFX::NeedFrontBuffer(int32 type)
{
	auto weapType = FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_eWeaponType;
	// Last frame -- needed for motion blur
	if (CMBlur::Drunkness > 0.0f)
		return true;
	if (type == MOTION_BLUR_SNIPER && SniperRifleGreenBlur[weapType])
		return true;

	switch (EffectSwitch) {
	case POSTFX_OFF:
	case POSTFX_SIMPLE:
		// no actual rendering here
		return false;
	case POSTFX_NORMAL:
		if (MotionBlurOn)
			return true;
		else
			return false;
	case POSTFX_MOBILE:
		return false;
	}
	return false;
}

void
CPostFX::GetBackBuffer(RwCamera* cam)
{
	RwRasterPushContext(pBackBuffer);
	RwRasterRenderFast(RwCameraGetRaster(cam), 0, 0);
	RwRasterPopContext();
}

void
CPostFX::Render(RwCamera* cam, uint32 red, uint32 green, uint32 blue, uint32 blur, int32 type, uint32 bluralpha)
{
	auto weapType = FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].m_eWeaponType;
	if (pFrontBuffer == nil)
		Open(cam);
	assert(pFrontBuffer, ERROR_NULL_POINTER);
	assert(pBackBuffer, ERROR_NULL_POINTER);

	if (type == MOTION_BLUR_LIGHT_SCENE) {
		SmoothColor(red, green, blue, blur);
		red = AvgRed;
		green = AvgGreen;
		blue = AvgBlue;
		blur = AvgAlpha;
	}

	if (NeedBackBuffer())
		GetBackBuffer(cam);

	DefinedState();

	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);

	if (type == MOTION_BLUR_SNIPER && SniperRifleGreenBlur[weapType]) {
		if (!bJustInitialised)
			RenderOverlaySniper(cam, red, green, blue, blur);
	}
	else switch (EffectSwitch) {
	case POSTFX_OFF:
	case POSTFX_SIMPLE:
		// no actual rendering here
		break;
	case POSTFX_NORMAL:
		if (MotionBlurOn) {
			if (!bJustInitialised)
				RenderOverlayBlur(cam, red, green, blue, blur);
		}
		else {
			RenderOverlayShader(cam, red, green, blue, blur);
		}
		break;
	case POSTFX_MOBILE:
		RenderOverlayShader(cam, red, green, blue, blur);
		break;
	}

	if (!bJustInitialised)
		RenderMotionBlur(cam, 175.0f * CMBlur::Drunkness);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nil);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	if (NeedFrontBuffer(type)) {
		RwRasterPushContext(pFrontBuffer);
		RwRasterRenderFast(RwCameraGetRaster(cam), 0, 0);
		RwRasterPopContext();
		bJustInitialised = false;
	}
	else
		bJustInitialised = true;

	
}

int CPostFX::PrevRed[NUMAVERAGE], CPostFX::AvgRed;
int CPostFX::PrevGreen[NUMAVERAGE], CPostFX::AvgGreen;
int CPostFX::PrevBlue[NUMAVERAGE], CPostFX::AvgBlue;
int CPostFX::PrevAlpha[NUMAVERAGE], CPostFX::AvgAlpha;
int CPostFX::Next;
int CPostFX::NumValues;

// This is rather annoying...the blur color can flicker slightly
// which becomes very visible when amplified by the shader
void
CPostFX::SmoothColor(uint32 red, uint32 green, uint32 blue, uint32 alpha)
{
	PrevRed[Next] = red;
	PrevGreen[Next] = green;
	PrevBlue[Next] = blue;
	PrevAlpha[Next] = alpha;
	Next = (Next + 1) % NUMAVERAGE;
	NumValues = Min(NumValues + 1, (int)NUMAVERAGE);

	AvgRed = 0;
	AvgGreen = 0;
	AvgBlue = 0;
	AvgAlpha = 0;
	for (int i = 0; i < NumValues; i++) {
		AvgRed += PrevRed[i];
		AvgGreen += PrevGreen[i];
		AvgBlue += PrevBlue[i];
		AvgAlpha += PrevAlpha[i];
	}
	AvgRed /= NumValues;
	AvgGreen /= NumValues;
	AvgBlue /= NumValues;
	AvgAlpha /= NumValues;
}
