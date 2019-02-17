/*
 * LICENSE
 * Libpointing can be redistributed and/or modified under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 * Ad-hoc licences can be provided upon request.
 * We appreciate credit when you use it (cite the following paper), but don't require it:
 * G. Casiez and N. Roussel. No more bricolage! Methods and tools to characterize, replicate and compare pointing transfer functions. In Proceedings of UIST'11, the 24th ACM Symposium on User Interface Software and Technology, pages 603-614, October 2011. ACM.
*/

#include "settingsProfile.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define sign(x) ((x) > 0 ? 1 : -1)
#ifndef min
	#define min(x, y) ((x) <= (y) ? (x) : (y))
#endif
#ifndef max
	#define max(x, y) ((x) >= (y) ? (x) : (y))
#endif
#ifndef abs
	#define abs(x) ((x) >= 0 ? (x) : -(x))
#endif

static inline unsigned int fileParseNextLine(FILE *const restrict file, char *const restrict buffer, size_t bufferSize, char **const line, size_t *const restrict lineLength){

	if(fgets(buffer, bufferSize, file)){

		char *commentPos;
		int doneFront = 0, doneEnd = 0;
		size_t newOffset = 0;
		size_t i;

		*line = buffer;
		*lineLength = strlen(*line);

		// Remove new line and carriage return.
		if((*line)[*lineLength-1] == '\n'){
			(*line)[--(*lineLength)] = '\0';
		}
		if((*line)[*lineLength-1] == '\r'){
			(*line)[--(*lineLength)] = '\0';
		}

		// Remove any comments from the line.
		commentPos = strstr(*line, "//");
		if(commentPos != NULL){
			*lineLength -= *lineLength-(size_t)(commentPos-*line);
			*commentPos = '\0';
		}

		// Remove any indentations from the line, as well as any trailing spaces and tabs.
		for(i = 0; i < *lineLength && (!doneFront || !doneEnd); ++i){
			if(!doneFront && (*line)[i] != '\t' && (*line)[i] != ' '){
				newOffset = i;
				doneFront = 1;
			}
			if(!doneEnd && i > 0 && (*line)[*lineLength-i] != '\t' && (*line)[*lineLength-i] != ' '){
				*lineLength -= i-1;
				(*line)[*lineLength] = '\0';
				doneEnd = 1;
			}
		}

		*line += newOffset;
		*lineLength -= newOffset;

		return 1;

	}

	return 0;

}

inline void spInit(settingsProfile *const restrict profile){

	/* Set defaults. */
	profile->sensitivity = 1.f;
	profile->dpiMultiplier = 1.f;
	profile->yawMultiplier = 1.f;
	profile->pitchMultiplier = 1.f;
	profile->acceleration = 0;
	profile->previousSegmentIndex = 0;
	profile->FINDSEGMENT = -1;
	profile->pixelGain = 0.f;
	profile->previousMouseDeltaX = 0;
	profile->previousMouseDeltaY = 0;
	profile->previousMouseXRemainder = 0.f;
	profile->previousMouseYRemainder = 0.f;
	profile->winScreenResolution = 96;
	profile->winScreenRefreshRate = 60;
	profile->winSubPixelation = 1;

	profile->winThresholdsX[0] = 0.f;
	profile->winThresholdsX[1] = 0.43f;
	profile->winThresholdsX[2] = 1.25f;
	profile->winThresholdsX[3] = 3.86f;
	profile->winThresholdsX[4] = 40.f;

	profile->winThresholdsY[0] = 0.f;
	profile->winThresholdsY[1] = 1.37f;
	profile->winThresholdsY[2] = 5.30f;
	profile->winThresholdsY[3] = 24.30f;
	profile->winThresholdsY[4] = 568.f;

	profile->quakeAccel = 0.f;
	profile->quakePower = 2.f;
	profile->quakeOffset = 0.f;
	profile->quakeCap = 0.f;

	#ifdef SETTINGS_PROFILE_DEBUG
	profile->verbose = 0;
	#endif

}

inline void spLoad(settingsProfile *const restrict profile, const char *const restrict cfgPath){

	/* Load settings profile. */

	FILE *config;

	config = fopen(cfgPath, "r");
	if(config != NULL){

		char lineFeed[1024];
		char *line;
		size_t lineLength;

		while(fileParseNextLine(config, lineFeed, sizeof(lineFeed), &line, &lineLength)){

			if(lineLength > 14 && strncmp(line, "Sensitivity = ", 14) == 0){
				const float tempFloat = strtod(line+14, NULL);
				if(tempFloat > 0.f){
					profile->sensitivity = tempFloat;
				}

			}else if(lineLength > 16 && strncmp(line, "DPIMultiplier = ", 16) == 0){
				const float tempFloat = strtod(line+16, NULL);
				profile->dpiMultiplier = tempFloat;

			}else if(lineLength > 16 && strncmp(line, "YawMultiplier = ", 16) == 0){
				const float tempFloat = strtod(line+16, NULL);
				profile->yawMultiplier = tempFloat;

			}else if(lineLength > 18 && strncmp(line, "PitchMultiplier = ", 18) == 0){
				const float tempFloat = strtod(line+18, NULL);
				profile->pitchMultiplier = tempFloat;

			}else if(lineLength > 15 && strncmp(line, "Acceleration = ", 15) == 0){
				if(strncmp(line+15, "XP", 2) == 0){
					profile->acceleration = 1;
				}else if(strncmp(line+15, "Vista", 5) == 0){
					profile->acceleration = 2;
				}else if(strncmp(line+15, "7", 1) == 0){
					profile->acceleration = 3;
				}else if(strncmp(line+15, "Quake", 5) == 0){
					profile->acceleration = 4;
				}

			}else if(lineLength > 26 && strncmp(line, "WindowsScreenResolution = ", 26) == 0){
				const float tempFloat = strtod(line+26, NULL);
				if(tempFloat > 0.f){
					profile->winScreenResolution = tempFloat;
				}

			}else if(lineLength > 27 && strncmp(line, "WindowsScreenRefreshRate = ", 27) == 0){
				const int tempInt = strtol(line+20, NULL, 10);
				if(tempInt > 0){
					profile->winScreenRefreshRate = (unsigned int)tempInt;
				}

			}else if(lineLength > 23 && strncmp(line, "WindowsSubPixelation = ", 23) == 0){
				if(strncmp(line+23, "FALSE", 5) == 0){
					profile->winSubPixelation = 0;
				}else if(strncmp(line+23, "0", 1) == 0){
					profile->winSubPixelation = 0;
				}else if(strncmp(line+23, "TRUE", 4) == 0){
					profile->winSubPixelation = 1;
				}else if(strncmp(line+23, "1", 1) == 0){
					profile->winSubPixelation = 1;
				}

			}else if(lineLength >= 30 && strncmp(line, "WindowsThresholdsX = ", 21) == 0){
				float tempThresholds[5];
				int tokenNum = 0;
				char *token = strtok(line+21, " ");
				while(token){
					if(tokenNum == 5){
						++tokenNum;
						break;
					}
					tempThresholds[tokenNum] = strtod(token, NULL);
					token = strtok(NULL, " ");
					++tokenNum;
				}
				if(tokenNum == 5){
					for(--tokenNum; tokenNum >= 0; --tokenNum){
						profile->winThresholdsX[tokenNum] = tempThresholds[tokenNum];
					}
				}

			}else if(lineLength >= 30 && strncmp(line, "WindowsThresholdsX = ", 21) == 0){
				float tempThresholds[5];
				int tokenNum = 0;
				char *token = strtok(line+21, " ");
				while(token){
					if(tokenNum == 5){
						++tokenNum;
						break;
					}
					tempThresholds[tokenNum] = strtod(token, NULL);
					token = strtok(NULL, " ");
					++tokenNum;
				}
				if(tokenNum == 5){
					for(--tokenNum; tokenNum >= 0; --tokenNum){
						profile->winThresholdsY[tokenNum] = tempThresholds[tokenNum];
					}
				}

			}else if(lineLength > 13 && strncmp(line, "QuakeAccel = ", 13) == 0){
				const float tempFloat = strtod(line+13, NULL);
				if(tempFloat >= 0.f){
					profile->quakeAccel = tempFloat;
				}

			}else if(lineLength > 18 && strncmp(line, "QuakeAccelPower = ", 18) == 0){
				const float tempFloat = strtod(line+18, NULL);
				if(tempFloat >= 0.f){
					profile->quakePower = tempFloat;
				}

			}else if(lineLength > 19 && strncmp(line, "QuakeAccelOffset = ", 19) == 0){
				const float tempFloat = strtod(line+19, NULL);
				if(tempFloat >= 0.f){
					profile->quakeOffset = tempFloat;
				}

			}else if(lineLength > 15 && strncmp(line, "QuakeSensCap = ", 15) == 0){
				const float tempFloat = strtod(line+15, NULL);
				if(tempFloat >= 0.f){
					profile->quakeCap = tempFloat;
				}

			#ifdef SETTINGS_PROFILE_DEBUG
			}else if(lineLength >= 7 && strncmp(line, "Verbose", 7) == 0){
				profile->verbose = 1;
			#endif

			}

		}

	}

	fclose(config);

}

#ifdef SETTINGS_PROFILE_TERMINAL_OUTPUT
inline void spPrintSettings(const settingsProfile *const restrict profile){
	puts("Profile loaded");
	puts("-------------------------------------------------------------------------------");
	printf("Sensitivity: %f\n", profile->sensitivity);
	if(profile->dpiMultiplier != 1.f){
		printf("DPI Multiplier: %f\n", profile->dpiMultiplier);
	}
	if(profile->yawMultiplier != 1.f){
		printf("Yaw Multiplier: %f\n", profile->yawMultiplier);
	}
	if(profile->pitchMultiplier != 1.f){
		printf("Pitch Multiplier: %f\n", profile->pitchMultiplier);
	}
	printf("Acceleration method: %s\n", profile->acceleration == 1 ? "Windows XP Enhanced Pointer Precision" : (profile->acceleration == 2 ? "Windows Vista Enhanced Pointer Precision" : (profile->acceleration == 3 ? "Windows 7 Enhanced Pointer Precision" : (profile->acceleration == 4 ? "Quake Acceleration" : "None"))));
	if(profile->acceleration == 1 || profile->acceleration == 2 || profile->acceleration == 3){
		printf("Screen resolution: %f PPI\n", profile->winScreenResolution);
	}
	if(profile->acceleration == 3){
		printf("Screen refresh rate: %u Hz\n", profile->winScreenRefreshRate);
	}
	if(profile->acceleration == 1 || profile->acceleration == 2){
		printf("Sub-pixelation: %s\n", profile->winSubPixelation == 0 ? "Off" : "On");
	}
	if(profile->acceleration == 1 || profile->acceleration == 2 || profile->acceleration == 3){
		printf("Mouse speed thresholds: {%f, %f, %f, %f, %f}\n", profile->winThresholdsX[0], profile->winThresholdsX[1], profile->winThresholdsX[2], profile->winThresholdsX[3], profile->winThresholdsX[4]);
		printf("Pointer speed thresholds: {%f, %f, %f, %f, %f}\n", profile->winThresholdsY[0], profile->winThresholdsY[1], profile->winThresholdsY[2], profile->winThresholdsY[3], profile->winThresholdsY[4]);
	}
	if(profile->acceleration == 4){
		printf("Quake Acceleration: %f\n", profile->quakeAccel);
		printf("Quake Acceleration Power: %f\n", profile->quakePower);
		printf("Quake Acceleration Offset: %f\n", profile->quakeOffset);
		printf("Quake Sensitivity Cap: %f\n", profile->quakeCap);
	}
	puts("-------------------------------------------------------------------------------\n");
}
#endif

static inline float spSmoothMouseGain(const settingsProfile *const restrict profile, const float deviceSpeed, int *const restrict segment){

	/*
	 values of threshold that give the pointer speed in inches/s from
	 the speed of the device in inches/s intermediate values are
	 interpolated
	 http://www.microsoft.com/whdc/archive/pointer-bal.mspx

	 [HKEY_CURRENT_USER\Control Panel\Mouse]
	 "SmoothMouseXCurve"=hex:00,00,00,00,00,00,00,00,
	                         15,6e,00,00,00,00,00,00,
	                         00,40,01,00,00,00,00,00,
	                         29,dc,03,00,00,00,00,00,
	                         00,00,28,00,00,00,00,00

	 "SmoothMouseYCurve"=hex:00,00,00,00,00,00,00,00,
	                         b8,5e,01,00,00,00,00,00,
	                         cd,4c,05,00,00,00,00,00,
	                         cd,4c,18,00,00,00,00,00,
	                         00,00,38,02,00,00,00,00
	*/

	int i;
	float slope;
	float intercept;

	if(deviceSpeed == 0.0){
		*segment = 0;
		return deviceSpeed;
	}

	if(*segment == profile->FINDSEGMENT){

		for(i = 0; i < 3; ++i){
			if(deviceSpeed < profile->winThresholdsX[i+1]){
				break;
			}
		}
		*segment = i;

	}else{
		i = *segment;
	}

	slope = (profile->winThresholdsY[i+1] - profile->winThresholdsY[i]) / (profile->winThresholdsX[i+1] - profile->winThresholdsX[i]);
	intercept = profile->winThresholdsY[i] - slope * profile->winThresholdsX[i];

	return slope + intercept / deviceSpeed;

}

inline void spUpdate(settingsProfile *const restrict profile, int mouseDeltaX, int mouseDeltaY, int *const restrict mouseX, int *const restrict mouseY){

	float accelerationMultiplier = profile->sensitivity;

	// Apply the DPI multiplier to match the desired DPI
	mouseDeltaX *= profile->dpiMultiplier;
	mouseDeltaY *= profile->dpiMultiplier;

	// Windows enhanced pointer precision
	if(profile->acceleration >= 1 && profile->acceleration <= 3){

		float screenResolutionFactor;
		float mouseMag;
		int currentSegmentIndex;

		if(profile->acceleration == 3){
			// Fixed acceleration method
			screenResolutionFactor = (float)profile->winScreenResolution / 150.f;
		}else{
			// Handle last movement's remainders as XP does
			if(profile->acceleration == 1){
				if(sign(mouseDeltaX) != sign(profile->previousMouseDeltaX) || mouseDeltaX == 0){
					profile->previousMouseXRemainder = 0.f;
				}
				profile->previousMouseDeltaX = mouseDeltaX;
				if(sign(mouseDeltaY) != sign(profile->previousMouseDeltaY) || mouseDeltaY == 0){
					profile->previousMouseYRemainder = 0.f;
				}
				profile->previousMouseDeltaY = mouseDeltaY;

			// Handle movement's remainders as Vista does
			}else if(profile->acceleration == 2){
				if(mouseDeltaX != 0){
					if(sign(mouseDeltaX) != sign(profile->previousMouseDeltaX)){
						profile->previousMouseXRemainder = 0.f;
					}
					profile->previousMouseDeltaX = mouseDeltaX;
				}
				if(mouseDeltaY != 0){
					if(sign(mouseDeltaY) != sign(profile->previousMouseDeltaY)){
						profile->previousMouseYRemainder = 0.f;
					}
					profile->previousMouseDeltaY = mouseDeltaY;
				}
			}
			// Original acceleration method
			screenResolutionFactor = (float)profile->winScreenRefreshRate / (float)profile->winScreenResolution;
		}

		// Calculate accelerated mouse deltas
		mouseMag = max(abs(mouseDeltaX), abs(mouseDeltaY)) + min(abs(mouseDeltaX), abs(mouseDeltaY)) / 2.f;
		currentSegmentIndex = profile->FINDSEGMENT;
		profile->pixelGain = screenResolutionFactor
		                     * profile->sensitivity
		                     * spSmoothMouseGain(profile, mouseMag / 3.5f, &currentSegmentIndex)
		                     / 3.5f;

		if(currentSegmentIndex > profile->previousSegmentIndex){
			// Average with calculation using previous curve segment
			const float pixelGainUsingPreviousSegment = screenResolutionFactor
			                                            * profile->sensitivity
			                                            * spSmoothMouseGain(profile, mouseMag / 3.5f, &(profile->previousSegmentIndex))
			                                            / 3.5f;
			profile->pixelGain = (profile->pixelGain + pixelGainUsingPreviousSegment) / 2.f;
		}
		profile->previousSegmentIndex = currentSegmentIndex;
		accelerationMultiplier = profile->pixelGain;

	// Quake acceleration
	}else if(profile->acceleration == 4){

		const float velocity = sqrtf(mouseDeltaX*mouseDeltaX+mouseDeltaY*mouseDeltaY);
		accelerationMultiplier = profile->sensitivity + powf(profile->quakeAccel*(velocity-profile->quakeOffset), profile->quakePower-1.f);

	}

	// Apply yaw and pitch multipliers
	mouseDeltaX *= profile->yawMultiplier;
	mouseDeltaY *= profile->pitchMultiplier;

	{

		// Calculate full mouse deltas, including remainders
		const float mouseXplusRemainder = mouseDeltaX * accelerationMultiplier + profile->previousMouseXRemainder;
		const float mouseYplusRemainder = mouseDeltaY * accelerationMultiplier + profile->previousMouseYRemainder;

		// Update
		if(profile->acceleration == 1 || profile->acceleration == 2){
			if(!profile->winSubPixelation && fabs(mouseXplusRemainder) <= abs(mouseDeltaX)){
				// XP & Vista when disableSubPixelation (never set, AFAIK)
				*mouseX = mouseDeltaX;
				profile->previousMouseXRemainder = 0.0;
				profile->pixelGain = 1.0;
			}else{
				// XP & Vista
				*mouseX = (int)floor(mouseXplusRemainder);
				profile->previousMouseXRemainder = mouseXplusRemainder - *mouseX;
			}
			if(!profile->winSubPixelation && fabs(mouseYplusRemainder) <= abs(mouseDeltaY)){
				// XP & Vista when disableSubPixelation (never set, AFAIK)
				*mouseY = mouseDeltaY;
				profile->previousMouseYRemainder = 0.f;
				profile->pixelGain = 1.f;
			}else{
				// XP & Vista
				*mouseY = (int)floor(mouseYplusRemainder);
				profile->previousMouseYRemainder = mouseYplusRemainder - *mouseY;
			}

		}else if(profile->acceleration == 0 && accelerationMultiplier == 1.f){
			// Slider = 0, no remainder to handle
			*mouseX = mouseDeltaX;
			*mouseY = mouseDeltaY;

		}else{

			if(mouseXplusRemainder >= 0.f){
				*mouseX = (int)floor(mouseXplusRemainder);
			}else{
				*mouseX = -(int)floor(-mouseXplusRemainder);
			}
			if(mouseYplusRemainder >= 0.f){
				*mouseY = (int)floor(mouseYplusRemainder);
			}else{
				*mouseY = -(int)floor(-mouseYplusRemainder);
			}
			profile->previousMouseXRemainder = mouseXplusRemainder - *mouseX;
			profile->previousMouseYRemainder = mouseYplusRemainder - *mouseY;

		}

	}

	#ifdef SETTINGS_PROFILE_DEBUG
	// Verbose terminal output.
	if(profile->verbose){
		printf("Raw: %i, %i  -  Accelerated: %i, %i  -  Remainder: %f, %f\n", mouseDeltaX, mouseDeltaY, *mouseX, *mouseY, profile->previousMouseXRemainder, profile->previousMouseYRemainder);
	}
	#endif

}
