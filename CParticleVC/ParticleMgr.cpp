#include "plugin.h"
#include "CFileMgr.h"
#include "ParticleMgr.h"
#include "Utility.h"
#include "ParticleType.h"
#include <csignal>
#include "IniReader.h"
#include <CTxdStore.h>
using namespace std;
uint8_t work_buff[55000];
cParticleSystemMgr mod_ParticleSystemManager;
/*RwTexture* gpParticleTexture[63];
int32_t ParticlesTotalTexturesCount;
int32_t ParticlesTotalTexturesCount2;
bool ParticleInitialized[1024] = { false };
RwTexture* ParticleTexture[1024];
RwRaster* Raster[1024];*/
const char* ParticleFilename = PLUGIN_PATH((char*)"DATA\\PARTICLE.CFG");

cParticleSystemMgr::cParticleSystemMgr()
{
	memset(this, 0, sizeof(*this));
}

void cParticleSystemMgr::Initialise()
{
	LoadParticleData();
	/*CIniReader ini(PLUGIN_PATH((char*)"CParticleVC.ini"));
	for (int32_t i = 0; i < 1024; i++) {
		CTxdStore::PushCurrentTxd();
		int32_t slot2 = CTxdStore::AddTxdSlot("particleVC");
		CTxdStore::LoadTxd(slot2, PLUGIN_PATH((char*)"MODELS\\PARTICLEVC.TXD"));
		CTxdStore::AddRef(slot2);
		int32_t slot = CTxdStore::FindTxdSlot("particleVC");
		CTxdStore::SetCurrentTxd(slot);
		std::string name = "PARTICLE";
		std::string section = std::format("{}{}", name, i);
		std::string texture[1024] = { ini.ReadString(section, "texture", "") };
		ParticleTexture[i] = RwTextureRead(texture[i].c_str(), NULL);
		Raster[i] = RwTextureGetRaster(ParticleTexture[i]);
		log("Section: %s", section);
		RwUInt8 red = ini.ReadInteger(section, "render_colouring_r", -1);
		//if (section.empty()) break;
		//assert(ParticleTexture != NULL);
		if (red != -1) {
			ParticleInitialized[i] = true;
			m_aParticlesINI[i].m_ppRaster = &gpCloudTex[0]->raster;
			*m_aParticlesINI[i].m_aNamestd = ini.ReadString(section, "name", "");
			m_aParticlesINI[i].m_RenderColouring.red = red;
			m_aParticlesINI[i].m_RenderColouring.green = ini.ReadInteger(section, "render_colouring_g", 0);
			m_aParticlesINI[i].m_RenderColouring.blue = ini.ReadInteger(section, "render_colouring_b", 0);
			m_aParticlesINI[i].m_InitialColorVariation = ini.ReadInteger(section, "initial_color_variation", 0);
			m_aParticlesINI[i].m_FadeDestinationColor.red = ini.ReadInteger(section, "fade_destination_color_r", 0);
			m_aParticlesINI[i].m_FadeDestinationColor.green = ini.ReadInteger(section, "fade_destination_color_g", 0);
			m_aParticlesINI[i].m_FadeDestinationColor.blue = ini.ReadInteger(section, "fade_destination_color_b", 0);
			m_aParticlesINI[i].m_ColorFadeTime = ini.ReadInteger(section, "color_fade_time", 0);
			m_aParticlesINI[i].m_fDefaultInitialRadius = ini.ReadFloat(section, "default_initial_radius", 0.0f);
			m_aParticlesINI[i].m_fExpansionRate = ini.ReadFloat(section, "expansion_rate", 0.0f);
			m_aParticlesINI[i].m_nFadeToBlackInitialIntensity = ini.ReadInteger(section, "initial_intensity", 0);
			m_aParticlesINI[i].m_nFadeToBlackTime = ini.ReadInteger(section, "fade_time", 0);
			m_aParticlesINI[i].m_nFadeToBlackAmount = ini.ReadInteger(section, "fade_amount", 0);
			m_aParticlesINI[i].m_nFadeAlphaInitialIntensity = ini.ReadInteger(section, "initial_alpha_intensity", 0);
			m_aParticlesINI[i].m_nFadeAlphaTime = ini.ReadInteger(section, "fade_alpha_time", 0);
			m_aParticlesINI[i].m_nFadeAlphaAmount = ini.ReadInteger(section, "fade_alpha_amount", 0);
			m_aParticlesINI[i].m_nZRotationInitialAngle = ini.ReadInteger(section, "initial_angle", 0);
			m_aParticlesINI[i].m_nZRotationChangeTime = ini.ReadInteger(section, "change_time", 0);
			m_aParticlesINI[i].m_nZRotationAngleChangeAmount = ini.ReadInteger(section, "angle_change_amount", 0);
			m_aParticlesINI[i].m_fInitialZRadius = ini.ReadFloat(section, "initial_z_radius", 0.0f);
			m_aParticlesINI[i].m_nZRadiusChangeTime = ini.ReadInteger(section, "z_radius_change_time", 0);
			m_aParticlesINI[i].m_fZRadiusChangeAmount = ini.ReadFloat(section, "z_radius_change_amount", 0.0f);
			m_aParticlesINI[i].m_nAnimationSpeed = ini.ReadInteger(section, "animation_speed", 0);
			m_aParticlesINI[i].m_nStartAnimationFrame = ini.ReadInteger(section, "start_animation_frame", 0);
			m_aParticlesINI[i].m_nFinalAnimationFrame = ini.ReadInteger(section, "final_animation_frame", 0);
			m_aParticlesINI[i].m_nRotationSpeed = ini.ReadInteger(section, "rotation_speed", 0);
			m_aParticlesINI[i].m_fGravitationalAcceleration = ini.ReadFloat(section, "gravitational_acceleration", 0.0f);
			m_aParticlesINI[i].m_nFrictionDecceleration = ini.ReadInteger(section, "friction_decceleration", 0);
			m_aParticlesINI[i].m_nLifeSpan = ini.ReadInteger(section, "life_span", 0);
			m_aParticlesINI[i].m_fPositionRandomError = ini.ReadFloat(section, "position_random_error", 0.0f);
			m_aParticlesINI[i].m_fVelocityRandomError = ini.ReadFloat(section, "velocity_random_error", 0.0f);
			m_aParticlesINI[i].m_fExpansionRateError = ini.ReadFloat(section, "expansion_rate_error", 0.0f);
			m_aParticlesINI[i].m_nRotationRateError = ini.ReadInteger(section, "rotation_rate_error", 0);
			m_aParticlesINI[i].m_nLifeSpanErrorShape = ini.ReadInteger(section, "life_span_error_shape", 0);
			m_aParticlesINI[i].m_fTrailLengthMultiplier = ini.ReadFloat(section, "trail_length_multiplier", 0.0f);
			m_aParticlesINI[i].m_vecTextureStretch.x = ini.ReadFloat(section, "stretch_value_x", 0.0f);
			m_aParticlesINI[i].m_vecTextureStretch.y = ini.ReadFloat(section, "stretch_value_y", 0.0f);
			m_aParticlesINI[i].m_fWindFactor = ini.ReadFloat(section, "wind_factor", 0.0f);
			m_aParticlesINI[i].m_fCreateRange = ini.ReadFloat(section, "particle_create_range", 0.0f);
			m_aParticlesINI[i].Flags = ini.ReadInteger(section, "flags", 0);
			log("Loading Particle From INI: %s\n"
				"\tInteger Type: %d\n"
				"\tColour (R,G,B): (%d,%d,%d)\n"
				"\tInitial Color Variation: %d\n"
				"\tFade Destination Colour (R,G,B): (%d,%d,%d)\n"
				"\tColor Fade Time: %d\n"
				"\tDefault Initial Radius: %f\n"
				"\tExpansion Rate: %f\n"
				"\tInitial Intensity: %d\n"
				"\tFade Time: %d\n"
				"\tFade Amount: %d\n"
				"\tInitial Alpha Intensity: %d\n"
				"\tFade Alpha Time: %d\n"
				"\tFade Alpha Amount: %d\n"
				"\tInitial Angle: %d\n"
				"\tChange Time: %d\n"
				"\tAngle Change Amount: %d\n"
				"\tInitial Z Radius: %f\n"
				"\tZ Radius Change Time: %d\n"
				"\tZ Radius Change Amount: %f\n"
				"\tAnimation Speed: %d\n"
				"\tStart Animation Frame: %d\n"
				"\tFinal Animation Frame: %d\n"
				"\tRotation Speed: %d\n"
				"\tGravitational Acceleration: %f\n"
				"\tFriction Deceleration: %d\n"
				"\tLife Span: %d\n"
				"\tPosition Random Error: %f\n"
				"\tVelocity Random Error: %f\n"
				"\tExpansion Rate Error: %f\n"
				"\tRotation Rate Error: %d\n"
				"\tLife Span Error Shape: %d\n"
				"\tTrail Length Multiplier: %f\n"
				"\tStretch Value X: %f\n"
				"\tStretch Value Y: %f\n"
				"\tWind Factor: %f\n"
				"\tParticle Create Range: %f\n"
				"\tFlags: %d\n",
				m_aParticlesINI[i].m_aNamestd,
				m_aParticlesINI[i].m_Type,
				m_aParticlesINI[i].m_RenderColouring.red, m_aParticlesINI[i].m_RenderColouring.green, m_aParticlesINI[i].m_RenderColouring.blue,
				m_aParticlesINI[i].m_InitialColorVariation,
				m_aParticlesINI[i].m_FadeDestinationColor.red, m_aParticlesINI[i].m_FadeDestinationColor.green, m_aParticlesINI[i].m_FadeDestinationColor.blue,
				m_aParticlesINI[i].m_ColorFadeTime,
				m_aParticlesINI[i].m_fDefaultInitialRadius,
				m_aParticlesINI[i].m_fExpansionRate,
				m_aParticlesINI[i].m_nFadeToBlackInitialIntensity,
				m_aParticlesINI[i].m_nFadeToBlackTime,
				m_aParticlesINI[i].m_nFadeToBlackAmount,
				m_aParticlesINI[i].m_nFadeAlphaInitialIntensity,
				m_aParticlesINI[i].m_nFadeAlphaTime,
				m_aParticlesINI[i].m_nFadeAlphaAmount,
				m_aParticlesINI[i].m_nZRotationInitialAngle,
				m_aParticlesINI[i].m_nZRotationChangeTime,
				m_aParticlesINI[i].m_nZRotationAngleChangeAmount,
				m_aParticlesINI[i].m_fInitialZRadius,
				m_aParticlesINI[i].m_nZRadiusChangeTime,
				m_aParticlesINI[i].m_fZRadiusChangeAmount,
				m_aParticlesINI[i].m_nAnimationSpeed,
				m_aParticlesINI[i].m_nStartAnimationFrame,
				m_aParticlesINI[i].m_nFinalAnimationFrame,
				m_aParticlesINI[i].m_nRotationSpeed,
				m_aParticlesINI[i].m_fGravitationalAcceleration,
				m_aParticlesINI[i].m_nFrictionDecceleration,
				m_aParticlesINI[i].m_nLifeSpan,
				m_aParticlesINI[i].m_fPositionRandomError,
				m_aParticlesINI[i].m_fVelocityRandomError,
				m_aParticlesINI[i].m_fExpansionRateError,
				m_aParticlesINI[i].m_nRotationRateError,
				m_aParticlesINI[i].m_nLifeSpanErrorShape,
				m_aParticlesINI[i].m_fTrailLengthMultiplier,
				m_aParticlesINI[i].m_vecTextureStretch.x,
				m_aParticlesINI[i].m_vecTextureStretch.y,
				m_aParticlesINI[i].m_fWindFactor,
				m_aParticlesINI[i].m_fCreateRange,
				m_aParticlesINI[i].Flags);
			//CTxdStore::PopCurrentTxd();
		}
	}*/
	for (int32_t i = 0; i < MAX_PARTICLES; i++) {
		m_aParticles[i].m_pParticles = NULL;
	//	m_aParticlesINI[i].m_pParticles = NULL;
		log("Loading Particle: %s\n"
			"\tInteger Type: %d\n"
			"\tColour (R,G,B): (%d,%d,%d)\n"
			"\tInitial Color Variation: %d\n"
			"\tFade Destination Colour (R,G,B): (%d,%d,%d)\n"
			"\tColor Fade Time: %d\n"
			"\tDefault Initial Radius: %f\n"
			"\tExpansion Rate: %f\n"
			"\tInitial Intensity: %d\n"
			"\tFade Time: %d\n"
			"\tFade Amount: %d\n"
			"\tInitial Alpha Intensity: %d\n"
			"\tFade Alpha Time: %d\n"
			"\tFade Alpha Amount: %d\n"
			"\tInitial Angle: %d\n"
			"\tChange Time: %d\n"
			"\tAngle Change Amount: %d\n"
			"\tInitial Z Radius: %f\n"
			"\tZ Radius Change Time: %d\n"
			"\tZ Radius Change Amount: %f\n"
			"\tAnimation Speed: %d\n"
			"\tStart Animation Frame: %d\n"
			"\tFinal Animation Frame: %d\n"
			"\tRotation Speed: %d\n"
			"\tGravitational Acceleration: %f\n"
			"\tFriction Deceleration: %d\n"
			"\tLife Span: %d\n"
			"\tPosition Random Error: %f\n"
			"\tVelocity Random Error: %f\n"
			"\tExpansion Rate Error: %f\n"
			"\tRotation Rate Error: %d\n"
			"\tLife Span Error Shape: %d\n"
			"\tTrail Length Multiplier: %f\n"
			"\tStretch Value X: %f\n"
			"\tStretch Value Y: %f\n"
			"\tWind Factor: %f\n"
			"\tParticle Create Range: %f\n"
			"\tFlags: %d\n",
			m_aParticles[i].m_aName,
			m_aParticles[i].m_Type,
			m_aParticles[i].m_RenderColouring.red, m_aParticles[i].m_RenderColouring.green, m_aParticles[i].m_RenderColouring.blue,
			m_aParticles[i].m_InitialColorVariation,
			m_aParticles[i].m_FadeDestinationColor.red, m_aParticles[i].m_FadeDestinationColor.green, m_aParticles[i].m_FadeDestinationColor.blue,
			m_aParticles[i].m_ColorFadeTime,
			m_aParticles[i].m_fDefaultInitialRadius,
			m_aParticles[i].m_fExpansionRate,
			m_aParticles[i].m_nFadeToBlackInitialIntensity,
			m_aParticles[i].m_nFadeToBlackTime,
			m_aParticles[i].m_nFadeToBlackAmount,
			m_aParticles[i].m_nFadeAlphaInitialIntensity,
			m_aParticles[i].m_nFadeAlphaTime,
			m_aParticles[i].m_nFadeAlphaAmount,
			m_aParticles[i].m_nZRotationInitialAngle,
			m_aParticles[i].m_nZRotationChangeTime,
			m_aParticles[i].m_nZRotationAngleChangeAmount,
			m_aParticles[i].m_fInitialZRadius,
			m_aParticles[i].m_nZRadiusChangeTime,
			m_aParticles[i].m_fZRadiusChangeAmount,
			m_aParticles[i].m_nAnimationSpeed,
			m_aParticles[i].m_nStartAnimationFrame,
			m_aParticles[i].m_nFinalAnimationFrame,
			m_aParticles[i].m_nRotationSpeed,
			m_aParticles[i].m_fGravitationalAcceleration,
			m_aParticles[i].m_nFrictionDecceleration,
			m_aParticles[i].m_nLifeSpan,
			m_aParticles[i].m_fPositionRandomError,
			m_aParticles[i].m_fVelocityRandomError,
			m_aParticles[i].m_fExpansionRateError,
			m_aParticles[i].m_nRotationRateError,
			m_aParticles[i].m_nLifeSpanErrorShape,
			m_aParticles[i].m_fTrailLengthMultiplier,
			m_aParticles[i].m_vecTextureStretch.x,
			m_aParticles[i].m_vecTextureStretch.y,
			m_aParticles[i].m_fWindFactor,
			m_aParticles[i].m_fCreateRange,
			m_aParticles[i].Flags);
	}
}

void cParticleSystemMgr::LoadParticleData()
{
	//CFileMgr::SetDir(PLUGIN_PATH((char*)"DATA"));
	CFileMgr::LoadFile(PLUGIN_PATH((char*)"DATA\\PARTICLE.CFG"), work_buff, ARRAY_SIZE(work_buff), "r");
	//CFileMgr::SetDir("");
	char full_path[MAX_PATH];
	GetFullPathNameA(PLUGIN_PATH((char*)"DATA\\PARTICLE.CFG"), MAX_PATH, full_path, NULL);

	if (GetFileAttributesA(full_path) == INVALID_FILE_ATTRIBUTES) {
		ErrorWindow("particle.cfg is missing from %s, please install it! Exiting game...", PLUGIN_PATH((char*)"data"));
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