#include "plugin.h"
#include "CFileMgr.h"
#include "ParticleMgr.h"
#include "Utility.h"
#include "ParticleType.h"
#include <csignal>
using namespace std;
uint8_t work_buff[55000];
cParticleSystemMgr mod_ParticleSystemManager;
const char* ParticleFilename = PLUGIN_PATH((char*)"DATA\\PARTICLE.CFG");

cParticleSystemMgr::cParticleSystemMgr()
{
	memset(this, 0, sizeof(*this));
}

void cParticleSystemMgr::Initialise()
{
	LoadParticleData();

	for (int32_t i = 0; i < MAX_PARTICLES; i++)
		m_aParticles[i].m_pParticles = NULL;
}

void cParticleSystemMgr::LoadParticleData()
{
	//CFileMgr::SetDir(PLUGIN_PATH((char*)"DATA"));
	CFileMgr::LoadFile(PLUGIN_PATH((char*)"DATA\\PARTICLE.CFG"), work_buff, ARRAY_SIZE(work_buff), "r");
	//CFileMgr::SetDir("");
	char full_path[MAX_PATH];
	GetFullPathNameA(PLUGIN_PATH((char*)"DATA\\PARTICLE.CFG"), MAX_PATH, full_path, NULL);

	if (GetFileAttributesA(full_path) == INVALID_FILE_ATTRIBUTES) {
		ErrorWindow("particle.cfg is missing from %s, reason is: %s, please install it! Exiting game...", PLUGIN_PATH((char*)"data", strerror(errno)));
		//MessageBoxA(HWND_DESKTOP, "particle.cfg is missing from data folder. Exiting game...", "CParticleVC.SA.asi", MB_ICONERROR);
		//exit(0);
	}
	tParticleSystemData* entry = NULL;
	int32_t type = PARTICLE_FIRST;

	char* lineStart = (char*)work_buff;
	char* lineEnd = lineStart + 1;

	char line[500];
	char delims[4];

	while (true)
	{
		ASSERT(lineStart != NULL);
		ASSERT(lineEnd != NULL);

		while (*lineEnd != '\n')
			++lineEnd;

		int32_t lineLength = lineEnd - lineStart;

		ASSERT(lineLength < 500);

		strncpy(line, lineStart, lineLength);

		line[lineLength] = '\0';

		if (!strcmp(line, ";the end"))
			break;

		if (*line != ';')
		{
			int32_t param = CFG_PARAM_FIRST;

			strcpy(delims, " \t");

			char* value = strtok(line, delims);

			ASSERT(value != NULL);

			do
			{
				switch (param)
				{
				case CFG_PARAM_PARTICLE_TYPE_NAME:
					ASSERT(type < MAX_PARTICLES); // To make sure that there's no extra particles that don't even exist
					entry = &m_aParticles[type];
					ASSERT(entry != NULL);
					entry->m_Type = (tParticleType)type++;
					strcpy(entry->m_aName, value);
					break;

				case CFG_PARAM_RENDER_COLOURING_R:
					entry->m_RenderColouring.red = atoi(value);
					break;

				case CFG_PARAM_RENDER_COLOURING_G:
					entry->m_RenderColouring.green = atoi(value);
					break;

				case CFG_PARAM_RENDER_COLOURING_B:
					entry->m_RenderColouring.blue = atoi(value);
					break;

				case CFG_PARAM_INITIAL_COLOR_VARIATION:
					entry->m_InitialColorVariation = min(atoi(value), 100);
					break;

				case CFG_PARAM_FADE_DESTINATION_COLOR_R:
					entry->m_FadeDestinationColor.red = atoi(value);
					break;

				case CFG_PARAM_FADE_DESTINATION_COLOR_G:
					entry->m_FadeDestinationColor.green = atoi(value);
					break;

				case CFG_PARAM_FADE_DESTINATION_COLOR_B:
					entry->m_FadeDestinationColor.blue = atoi(value);
					break;

				case CFG_PARAM_COLOR_FADE_TIME:
					entry->m_ColorFadeTime = atoi(value);
					break;

				case CFG_PARAM_DEFAULT_INITIAL_RADIUS:
					entry->m_fDefaultInitialRadius = atof(value);
					break;

				case CFG_PARAM_EXPANSION_RATE:
					entry->m_fExpansionRate = atof(value);
					break;

				case CFG_PARAM_INITIAL_INTENSITY:
					entry->m_nFadeToBlackInitialIntensity = atoi(value);
					break;

				case CFG_PARAM_FADE_TIME:
					entry->m_nFadeToBlackTime = atoi(value);
					break;

				case CFG_PARAM_FADE_AMOUNT:
					entry->m_nFadeToBlackAmount = atoi(value);
					break;

				case CFG_PARAM_INITIAL_ALPHA_INTENSITY:
					entry->m_nFadeAlphaInitialIntensity = atoi(value);
					break;

				case CFG_PARAM_FADE_ALPHA_TIME:
					entry->m_nFadeAlphaTime = atoi(value);
					break;

				case CFG_PARAM_FADE_ALPHA_AMOUNT:
					entry->m_nFadeAlphaAmount = atoi(value);
					break;

				case CFG_PARAM_INITIAL_ANGLE:
					entry->m_nZRotationInitialAngle = atoi(value);
					break;

				case CFG_PARAM_CHANGE_TIME:
					entry->m_nZRotationChangeTime = atoi(value);
					break;

				case CFG_PARAM_ANGLE_CHANGE_AMOUNT:
					entry->m_nZRotationAngleChangeAmount = atoi(value);
					break;

				case CFG_PARAM_INITIAL_Z_RADIUS:
					entry->m_fInitialZRadius = atof(value);
					break;

				case CFG_PARAM_Z_RADIUS_CHANGE_TIME:
					entry->m_nZRadiusChangeTime = atoi(value);
					break;

				case CFG_PARAM_Z_RADIUS_CHANGE_AMOUNT:
					entry->m_fZRadiusChangeAmount = atof(value);
					break;

				case CFG_PARAM_ANIMATION_SPEED:
					entry->m_nAnimationSpeed = atoi(value);
					break;

				case CFG_PARAM_START_ANIMATION_FRAME:
					entry->m_nStartAnimationFrame = atoi(value);
					break;

				case CFG_PARAM_FINAL_ANIMATION_FRAME:
					entry->m_nFinalAnimationFrame = atoi(value);
					break;

				case CFG_PARAM_ROTATION_SPEED:
					entry->m_nRotationSpeed = atoi(value);
					break;

				case CFG_PARAM_GRAVITATIONAL_ACCELERATION:
					entry->m_fGravitationalAcceleration = atof(value);
					break;

				case CFG_PARAM_FRICTION_DECCELERATION:
					entry->m_nFrictionDecceleration = atoi(value);
					break;

				case CFG_PARAM_LIFE_SPAN:
					entry->m_nLifeSpan = atoi(value);
					break;

				case CFG_PARAM_POSITION_RANDOM_ERROR:
					entry->m_fPositionRandomError = atof(value);
					break;

				case CFG_PARAM_VELOCITY_RANDOM_ERROR:
					entry->m_fVelocityRandomError = atof(value);
					break;

				case CFG_PARAM_EXPANSION_RATE_ERROR:
					entry->m_fExpansionRateError = atof(value);
					break;

				case CFG_PARAM_ROTATION_RATE_ERROR:
					entry->m_nRotationRateError = atoi(value);
					break;

				case CFG_PARAM_LIFE_SPAN_ERROR_SHAPE:
					entry->m_nLifeSpanErrorShape = atoi(value);
					break;

				case CFG_PARAM_TRAIL_LENGTH_MULTIPLIER:
					entry->m_fTrailLengthMultiplier = atof(value);
					break;

				case CFG_PARAM_STRETCH_VALUE_X:
					entry->m_vecTextureStretch.x = atof(value);
					break;

				case CFG_PARAM_STRETCH_VALUE_Y:
					entry->m_vecTextureStretch.y = atof(value);
					break;

				case CFG_PARAM_WIND_FACTOR:
					entry->m_fWindFactor = atof(value);
					break;

				case CFG_PARAM_PARTICLE_CREATE_RANGE:
					entry->m_fCreateRange = SQR(atof(value));
					break;

				case CFG_PARAM_FLAGS:
					entry->Flags = atoi(value);
					break;
				/*case CFG_PARAM_TEXTURE:
				entry->m_ppRaster = (RwRaster**)value;
				break;*/
				}

				value = strtok(NULL, delims);

				param++;

				if (param > CFG_PARAM_LAST)
					param = CFG_PARAM_FIRST;

			} while (value != NULL);
		}

		lineEnd++;
		lineStart = lineEnd;
		lineEnd++;
	}
}