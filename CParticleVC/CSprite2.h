#pragma once
#include "CDraw.h"
#include "CCamera.h"
class CSprite2
{
	static float m_f2DNearScreenZ;
	static float m_f2DFarScreenZ;
	static float m_fRecipNearClipPlane;
	static int32_t m_bFlushSpriteBufferSwitchZTest;
public:
	// Version with removed camera draw distance for particles
	static bool CalcScreenCoors(const CVector& posn, CVector& out, float& w, float& h, bool checkMaxVisible, float lim)
	{
		float InverseZ;
		out = TheCamera.m_mViewMatrix * posn;

		if (out.z <= lim || out.z >= CDraw::ms_fFarClipZ && checkMaxVisible)
			return false;

		InverseZ = 1.0f / out.z;
		out.x = SCREEN_WIDTH * InverseZ * out.x;
		out.y = SCREEN_HEIGHT * InverseZ * out.y;
		w = SCREEN_WIDTH * InverseZ;
		h = SCREEN_HEIGHT * InverseZ;
		w = w / CDraw::ms_fFOV * 70.0f;
		h = h / CDraw::ms_fFOV * 70.0f;
	//	log("CalcScreenCoors lim: %f", lim);
		return true;
	}
	static void FlushSpriteBuffer(void);
	static void InitSpriteBuffer(void);
	static void InitSpriteBuffer2D(void);
	static void RenderBufferedOneXLUSprite2D(float x, float y, float w, float h, const RwRGBA& colour, int16_t intens, uint8_t alpha);
	static void Set6Vertices2D(RwIm2DVertex* verts, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,
		const CRGBA& c0, const CRGBA& c1, const CRGBA& c2, const CRGBA& c3);
	static void Set6Vertices2D(RwIm2DVertex* verts, const CRect& r, const CRGBA& c0, const CRGBA& c1, const CRGBA& c2, const CRGBA& c3);
	static void RenderBufferedOneXLUSprite2D_Rotate_Dimension(float x, float y, float w, float h, const RwRGBA& colour, int16_t intens, float rotation, uint8_t alpha);
	static void RenderBufferedOneXLUSprite_Rotate_Dimension(float x, float y, float z, float w, float h, uint8_t r, uint8_t g, uint8_t b, int16_t intens, float recipz, float rotation, uint8_t a);
	static void RenderOneXLUSprite(float x, float y, float z, float w, float h, uint8_t r, uint8_t g, uint8_t b, int16_t intens, float recipz, uint8_t a);
	static void RenderBufferedOneXLUSprite(float x, float y, float z, float w, float h, uint8_t r, uint8_t g, uint8_t b, int16_t intens, float recipz, uint8_t a);
	static void RenderOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8_t r, uint8_t g, uint8_t b, int16_t intens, float recipz, float rotation, uint8_t a);
	static void RenderBufferedOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8_t r, uint8_t g, uint8_t b, int16_t intens, float recipz, float roll, uint8_t a);
	static void RenderBufferedOneXLUSprite_Rotate_2Colours(float x, float y, float z, float w, float h, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2, float cx, float cy, float recipz, float rotation, uint8_t a);
};