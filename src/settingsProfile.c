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

#define Sign(X) ((X>0)?(1):(-1))
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef abs
#define abs(_a)	((_a >= 0) ? _a : -_a)
#endif

void spLoad(settingsProfile *profile, const char *cfgPath){

	/* Set defaults */
	profile->sensitivity = 1.f;
	profile->yawMultiplier = 0.f;
	profile->pitchMultiplier = 0.f;
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
	profile->quakeAccel = 1.f;
	profile->quakePower = 1.f;
	profile->quakeOffset = 0.f;
	profile->quakeCap = 0.f;
	profile->verbose = 0;


	/* Load settings profile */
	FILE *serverConfig = fopen(cfgPath, "r");
	char lineFeed[1024];
	char *line;
	char compare[1024];
	unsigned int lineLength;


	if(serverConfig != NULL){
		while(!feof(serverConfig)){

			fgets(lineFeed, sizeof(lineFeed), serverConfig);
			line = lineFeed;
			lineLength = strlen(line);

			// Remove new line and carriage return
			if(line[lineLength-1] == '\n'){
				line[--lineLength] = '\0';
			}
			if(line[lineLength-1] == '\r'){
				line[--lineLength] = '\0';
			}
			// Remove any comments from the line
			char *commentPos = strstr(line, "//");
			if(commentPos != NULL){
				lineLength -= commentPos-line;
				*commentPos = '\0';
			}
			// Remove any indentations from the line, as well as any trailing spaces and tabs
			unsigned char doneFront = 0, doneEnd = 0;
			unsigned int newOffset = 0;
			unsigned int d;
			for(d = 0; (d < lineLength && !doneFront && !doneEnd); d++){
				if(!doneFront && line[d] != '\t' && line[d] != ' '){
					newOffset = d;
					doneFront = 1;
				}
				if(!doneEnd && d > 1 && d < lineLength && line[lineLength-d] != '\t' && line[lineLength-d] != ' '){
					lineLength -= d-1;
					line[lineLength] = '\0';
					doneEnd = 1;
				}
			}
			line += newOffset;
			lineLength -= newOffset;

			if(lineLength > 14 && strncpy(compare, line, 14) && (compare[14] = '\0') == 0 && strcmp(compare, "Sensitivity = ") == 0){
				float tempFloat = strtof(line+14, NULL);
				if(tempFloat > 0.f){
					profile->sensitivity = tempFloat;
				}

			}else if(lineLength > 16 && strncpy(compare, line, 16) && (compare[16] = '\0') == 0 && strcmp(compare, "YawMultiplier = ") == 0){
				float tempFloat = strtof(line+16, NULL);
				profile->yawMultiplier = tempFloat;

			}else if(lineLength > 18 && strncpy(compare, line, 18) && (compare[18] = '\0') == 0 && strcmp(compare, "PitchMultiplier = ") == 0){
				float tempFloat = strtof(line+18, NULL);
				profile->pitchMultiplier = tempFloat;

			}else if(lineLength > 15 && strncpy(compare, line, 15) && (compare[15] = '\0') == 0 && strcmp(compare, "Acceleration = ") == 0){
				if(strcmp(line+15, "XP") == 0){
					profile->acceleration = 1;
				}else if(strcmp(line+15, "Vista") == 0){
					profile->acceleration = 2;
				}else if(strcmp(line+15, "7") == 0){
					profile->acceleration = 3;
				}else if(strcmp(line+15, "Quake") == 0){
					profile->acceleration = 4;
				}

			}else if(lineLength > 26 && strncpy(compare, line, 26) && (compare[26] = '\0') == 0 && strcmp(compare, "WindowsScreenResolution = ") == 0){
				float tempFloat = strtof(line+26, NULL);
				if(tempFloat > 0.f){
					profile->winScreenResolution = tempFloat;
				}

			}else if(lineLength > 27 && strncpy(compare, line, 27) && (compare[27] = '\0') == 0 && strcmp(compare, "WindowsScreenRefreshRate = ") == 0){
				int tempInt = strtol(line+20, NULL, 10);
				if(tempInt > 0){
					profile->winScreenRefreshRate = (unsigned int)tempInt;
				}

			}else if(lineLength > 23 && strncpy(compare, line, 23) && (compare[23] = '\0') == 0 && strcmp(compare, "WindowsSubPixelation = ") == 0){
				if(strcmp(line+23, "FALSE") == 0){
					profile->winSubPixelation = 0;
				}else if(strcmp(line+23, "0") == 0){
					profile->winSubPixelation = 0;
				}else if(strcmp(line+23, "TRUE") == 0){
					profile->winSubPixelation = 1;
				}else if(strcmp(line+23, "1") == 0){
					profile->winSubPixelation = 1;
				}

			}else if(lineLength >= 30 && strncpy(compare, line, 21) && (compare[21] = '\0') == 0 && strcmp(compare, "WindowsThresholdsX = ") == 0){
				float tempThresholds[5];
				int tokenNum = 0;
				char *token = strtok(line+21, " ");
				while(token){
					if(tokenNum == 5){
						tokenNum++;
						break;
					}
					tempThresholds[tokenNum] = strtof(token, NULL);
					token = strtok(NULL, " ");
					tokenNum++;
				}
				if(tokenNum == 5){
					for(tokenNum--; tokenNum >= 0; tokenNum--){
						profile->winThresholdsX[tokenNum] = tempThresholds[tokenNum];
					}
				}

			}else if(lineLength >= 30 && strncpy(compare, line, 21) && (compare[21] = '\0') == 0 && strcmp(compare, "WindowsThresholdsX = ") == 0){
				float tempThresholds[5];
				int tokenNum = 0;
				char *token = strtok(line+21, " ");
				while(token){
					if(tokenNum == 5){
						tokenNum++;
						break;
					}
					tempThresholds[tokenNum] = strtof(token, NULL);
					token = strtok(NULL, " ");
					tokenNum++;
				}
				if(tokenNum == 5){
					for(tokenNum--; tokenNum >= 0; tokenNum--){
						profile->winThresholdsY[tokenNum] = tempThresholds[tokenNum];
					}
				}

			}else if(lineLength > 13 && strncpy(compare, line, 13) && (compare[13] = '\0') == 0 && strcmp(compare, "QuakeAccel = ") == 0){
				float tempFloat = strtof(line+13, NULL);
				if(tempFloat >= 0.f){
					profile->quakeAccel = tempFloat;
				}

			}else if(lineLength > 18 && strncpy(compare, line, 18) && (compare[18] = '\0') == 0 && strcmp(compare, "QuakeAccelPower = ") == 0){
				float tempFloat = strtof(line+18, NULL);
				if(tempFloat >= 0.f){
					profile->quakePower = tempFloat;
				}

			}else if(lineLength > 19 && strncpy(compare, line, 19) && (compare[19] = '\0') == 0 && strcmp(compare, "QuakeAccelOffset = ") == 0){
				float tempFloat = strtof(line+19, NULL);
				if(tempFloat >= 0.f){
					profile->quakeOffset = tempFloat;
				}

			}else if(lineLength > 15 && strncpy(compare, line, 15) && (compare[15] = '\0') == 0 && strcmp(compare, "QuakeSensCap = ") == 0){
				float tempFloat = strtof(line+15, NULL);
				if(tempFloat >= 0.f){
					profile->quakeCap = tempFloat;
				}

			}else if(lineLength >= 7 && strncpy(compare, line, 7) && (compare[7] = '\0') == 0 && strcmp(compare, "Verbose") == 0){
				profile->verbose = 1;
			}

		}
	}

	fclose(serverConfig);

}

void spPrintSettings(settingsProfile *profile){
	printf("Profile loaded\n");
	printf("-------------------------------------------------------------------------------\n");
	printf("Sensitivity: %f\n", profile->sensitivity);
	printf("Yaw Multiplier: %f\n", profile->yawMultiplier);
	printf("Pitch Multiplier: %f\n", profile->pitchMultiplier);
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
	printf("-------------------------------------------------------------------------------\n\n");
}

float spSmoothMouseGain(settingsProfile *profile, float deviceSpeed, int *segment){

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

	if(deviceSpeed == 0.f){
		*segment = 0;
		return deviceSpeed;
	}

	int i;
	if(*segment == profile->FINDSEGMENT){

		for(i = 0; i < 3; i++){
			if(deviceSpeed < profile->winThresholdsX[i+1]){
				break;
			}
		}
		*segment = i;

	}else{
		i = *segment;
	}

	float slope = (profile->winThresholdsY[i+1] - profile->winThresholdsY[i]) / (profile->winThresholdsX[i+1] - profile->winThresholdsX[i]);
	float intercept = profile->winThresholdsY[i] - slope * profile->winThresholdsX[i];
	return slope + intercept / deviceSpeed;

}

inline void spUpdate(settingsProfile *profile, int mouseDeltaX, int mouseDeltaY, int *mouseX, int *mouseY){

	float accelerationMultiplier = profile->sensitivity;

	// Windows enhanced pointer precision
	if(profile->acceleration >= 1 && profile->acceleration <= 3){

		float screenResolutionFactor;
		if(profile->acceleration == 3){
			// Fixed acceleration method
			screenResolutionFactor = (float)profile->winScreenResolution / 150.f;
		}else{
			// Handle last movement's remainders as XP does
			if(profile->acceleration == 1){
				if(Sign(mouseDeltaX) != Sign(profile->previousMouseDeltaX) || mouseDeltaX == 0){
					profile->previousMouseXRemainder = 0.f;
				}
				profile->previousMouseDeltaX = mouseDeltaX;
				if(Sign(mouseDeltaY) != Sign(profile->previousMouseDeltaY) || mouseDeltaY == 0){
					profile->previousMouseYRemainder = 0.f;
				}
				profile->previousMouseDeltaY = mouseDeltaY;

			// Handle movement's remainders as Vista does
			}else if(profile->acceleration == 2){
				if(mouseDeltaX != 0){
					if(Sign(mouseDeltaX) != Sign(profile->previousMouseDeltaX)){
						profile->previousMouseXRemainder = 0.f;
					}
					profile->previousMouseDeltaX = mouseDeltaX;
				}
				if(mouseDeltaY != 0){
					if(Sign(mouseDeltaY) != Sign(profile->previousMouseDeltaY)){
						profile->previousMouseYRemainder = 0.f;
					}
					profile->previousMouseDeltaY = mouseDeltaY;
				}
			}
			// Original acceleration method
			screenResolutionFactor = (float)profile->winScreenRefreshRate / (float)profile->winScreenResolution;
		}

		// Calculate accelerated mouse deltas
		float mouseMag = max(abs(mouseDeltaX), abs(mouseDeltaY)) + min(abs(mouseDeltaX), abs(mouseDeltaY)) / 2.f;
		int currentSegmentIndex = profile->FINDSEGMENT;
		profile->pixelGain = screenResolutionFactor
		                     * profile->sensitivity
		                     * spSmoothMouseGain(profile, mouseMag / 3.5f, &currentSegmentIndex)
		                     / 3.5f;

		if(currentSegmentIndex > profile->previousSegmentIndex){
			// Average with calculation using previous curve segment
			float pixelGainUsingPreviousSegment = screenResolutionFactor
			                                      * profile->sensitivity
			                                      * spSmoothMouseGain(profile, mouseMag / 3.5f, &(profile->previousSegmentIndex))
			                                      / 3.5f;
			profile->pixelGain = (profile->pixelGain + pixelGainUsingPreviousSegment) / 2.f;
		}
		profile->previousSegmentIndex = currentSegmentIndex;
		accelerationMultiplier = profile->pixelGain;

	// Quake acceleration
	}else if(profile->acceleration == 4){

		float velocity = sqrtf(mouseDeltaX*mouseDeltaX+mouseDeltaY*mouseDeltaY);
		accelerationMultiplier = profile->sensitivity + powf(profile->quakeAccel*(velocity-profile->quakeOffset), profile->quakePower-1.f);

	}

	// Apply yaw and pitch multipliers
	if(profile->yawMultiplier != 0.f){
		mouseDeltaX *= profile->yawMultiplier;
	}
	if(profile->pitchMultiplier != 0.f){
		mouseDeltaY *= profile->pitchMultiplier;
	}

	// Calculate full mouse deltas, including remainders
	float mouseXplusRemainder = mouseDeltaX * accelerationMultiplier + profile->previousMouseXRemainder;
	float mouseYplusRemainder = mouseDeltaY * accelerationMultiplier + profile->previousMouseYRemainder;

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

	// Verbose terminal output
	if(profile->verbose){
		printf("Raw: %i, %i  -  Accelerated: %i, %i  -  Remainder: %f, %f\n", mouseDeltaX, mouseDeltaY, *mouseX, *mouseY, profile->previousMouseXRemainder, profile->previousMouseYRemainder);
	}

}
