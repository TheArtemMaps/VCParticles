#include "plugin.h"
#include "CFont.h"
#include "CDebug.h"
#include "Utility.h"
#include <csignal>

// Default assert causes the game to crash, dunno why, credits to re3 project
#ifndef MASTER
void re3_assert(const char* expr, const char* filename, unsigned int lineno, const char* func)
{
#ifdef _WIN32
	int nCode;

	strcpy_s(re3_buff, re3_buffsize, "Assertion failed!");
	strcat_s(re3_buff, re3_buffsize, "\n");

	strcat_s(re3_buff, re3_buffsize, "File: ");
	strcat_s(re3_buff, re3_buffsize, filename);
	strcat_s(re3_buff, re3_buffsize, "\n");

	strcat_s(re3_buff, re3_buffsize, "Line: ");
	_itoa_s(lineno, re3_buff + strlen(re3_buff), re3_buffsize - strlen(re3_buff), 10);
	strcat_s(re3_buff, re3_buffsize, "\n");

	strcat_s(re3_buff, re3_buffsize, "Function: ");
	strcat_s(re3_buff, re3_buffsize, func);
	strcat_s(re3_buff, re3_buffsize, "\n");

	strcat_s(re3_buff, re3_buffsize, "Expression: ");
	strcat_s(re3_buff, re3_buffsize, expr);
	strcat_s(re3_buff, re3_buffsize, "\n");

	strcat_s(re3_buff, re3_buffsize, "\n");
	strcat_s(re3_buff, re3_buffsize, "(Press Retry to debug the application)");


	nCode = ::MessageBoxA(NULL, re3_buff, "GTA SA Assertion Failed!",
		MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);

	if (nCode == IDABORT)
	{
		raise(SIGABRT);
		_exit(3);
	}

	if (nCode == IDRETRY)
	{
		__debugbreak();
		return;
	}

	if (nCode == IDIGNORE)
		return;

	abort();
#else
	// TODO
	printf("\nGTA SA ASSERTION FAILED\n\tFile: %s\n\tLine: %d\n\tFunction: %s\n\tExpression: %s\n", filename, lineno, func, expr);
	assert(false);
#endif
}
#endif

#define DEBUG_X_POS (425)
#define DEBUG_Y_POS (-55)
#define DEBUG_LINE_HEIGHT (15)
int16_t CDebug::ms_nCurrentTextLine;
char CDebug::ms_aTextBuffer[MAX_LINES][MAX_STR_LEN];

void
CDebug::DebugInitTextBuffer()
{
	ms_nCurrentTextLine = 0;
}

void
CDebug::DebugAddText(const char* str)
{
	int32_t i = 0;
	if (*str != '\0') {
		while (i < MAX_STR_LEN - 1) {
			ms_aTextBuffer[ms_nCurrentTextLine][i++] = *(str++);
			if (*str == '\0')
				break;
		}
	}

	ms_aTextBuffer[ms_nCurrentTextLine++][i] = '\0';
	if (ms_nCurrentTextLine >= MAX_LINES)
		ms_nCurrentTextLine = 0;
}
void
AsciiToUnicode(const char* src, char* dst)
{
	while ((*dst++ = (unsigned char)*src++) != '\0');
}
void
CDebug::DebugDisplayTextBuffer()
{
#ifndef MASTER
		if (gbDebugStuffInRelease)
		{
			int32_t i = 0;
			int32_t y = DEBUG_Y_POS;
#ifdef FIX_BUGS
			CFont::SetOrientation(ALIGN_CENTER);
			CFont::SetBackground(false, false);
			CFont::SetScale(0.65f, 1.0f); // 0.7f, 1.4f
			CFont::SetWrapx(9999.0f);
			CFont::SetFontStyle(FONT_PRICEDOWN);
			CFont::SetProportional(true);
			CFont::SetJustify(true);
				/*CFont::SetColor(CRGBA(255, 255, 255, 255));*/
#else
			// this is not even readable
			CFont::SetPropOff();
			CFont::SetBackgroundOff();
			CFont::SetScale(1.0f, 1.0f);
			CFont::SetCentreOff();
			CFont::SetRightJustifyOn();
			CFont::SetRightJustifyWrap(0.0f);
			CFont::SetBackGroundOnlyTextOff();
			CFont::SetFontStyle(FONT_STANDARD);
			CFont::SetPropOff();
#endif
			do {
				char* line;
				while (true) {
					line = ms_aTextBuffer[(ms_nCurrentTextLine + i++) % MAX_LINES];
					if (*line != '\0')
						break;
					y += DEBUG_LINE_HEIGHT;
					if (i == MAX_LINES) {
						CFont::DrawFonts();
						return;
					}
				}
				AsciiToUnicode(line, re3_buff);
				CFont::SetColor(CRGBA(0, 0, 0, 255));
				CFont::PrintString(DEBUG_X_POS, y - 1, re3_buff);
				CFont::SetColor(CRGBA(255, 128, 128, 255));
				CFont::PrintString(DEBUG_X_POS + 1, y, re3_buff);
				y += DEBUG_LINE_HEIGHT;
			} while (i != MAX_LINES);
			CFont::DrawFonts();
		}
#endif
	}


// custom
CDebug::ScreenStr CDebug::ms_aScreenStrs[MAX_SCREEN_STRS];
int CDebug::ms_nScreenStrs;

void
CDebug::DisplayScreenStrings()
{
	int i;
	CFont::SetOrientation(ALIGN_CENTER);
	CFont::SetBackground(false, false);
	CFont::SetBackgroundColor(CRGBA(0, 0, 0, 180));
	CFont::SetScale(1.0f, 1.0f);
	CFont::SetFontStyle(FONT_PRICEDOWN);
	CFont::SetProportional(true);
	CFont::SetDropShadowPosition(1);
	CFont::SetJustify(true);
	CFont::SetDropColor(CRGBA(0, 0, 0, 255));

	for (i = 0; i < ms_nScreenStrs; i++) {
		/*
				AsciiToUnicode(ms_aScreenStrs[i].str, gUString);
				CFont::SetColor(CRGBA(0, 0, 0, 255));
				CFont::PrintString(ms_aScreenStrs[i].x, ms_aScreenStrs[i].y, gUString);
				CFont::SetColor(CRGBA(255, 255, 255, 255));
				CFont::PrintString(ms_aScreenStrs[i].x+1, ms_aScreenStrs[i].y+1, gUString);
		*/
		ObrsPrintfString(ms_aScreenStrs[i].str, ms_aScreenStrs[i].x, ms_aScreenStrs[i].y);
	}
	CFont::DrawFonts();

	ms_nScreenStrs = 0;
}

void
CDebug::PrintAt(const char* str, int x, int y)
{
	if (ms_nScreenStrs >= MAX_SCREEN_STRS)
		return;
	strncpy(ms_aScreenStrs[ms_nScreenStrs].str, str, 256);
	ms_aScreenStrs[ms_nScreenStrs].x = x;//*12;
	ms_aScreenStrs[ms_nScreenStrs].y = y;//*22;
	ms_nScreenStrs++;
}

void debugshit(const char* text, ...)
{
	va_list va;
	va_start(va, text);
	vsprintf_s(re3_buff, 1024, text, va);
	va_end(va);
	printf("%s\n", re3_buff);
	CDebug::DebugAddText(re3_buff);
}

void ErrorWindow(const char* msg, ...)
{
	va_list va;
	va_start(va, msg);
	vsprintf_s(re3_buff, re3_buffsize, msg, va);
	va_end(va);

	::MessageBoxA(NULL, re3_buff, "CParticleVC.SA.asi: Error! Oh well...",
		MB_OK | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);

	raise(SIGABRT);
	_exit(3);
}
