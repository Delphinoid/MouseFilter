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

void spSubstringHelper(char *strTarget, char *strCopy, unsigned int pos, unsigned int length){

	if(pos + length <= strlen(strCopy)){
		unsigned int d;
		for(d = 0; d < length; d++){
			if(strCopy[pos + d] == '\n'){
				d = length;
			}else{
				strTarget[d] = strCopy[pos + d];
			}
		}
		strTarget[length] = '\0';
	}

}

void spLoad(settingsProfile *profile, const char *cfgPath){

	/* Set defaults */
	profile->windowsVersion = 2;
	profile->mouseSensitivity = 1.0f;
	profile->enhancePointerPrecision = 1;
	profile->acceleration = 1;
	profile->subPixelation = 1;
	profile->previousSegmentIndex = 0;
	profile->FINDSEGMENT = -1;
	profile->previousMouseRawX = 0;
	profile->previousMouseRawY = 0;
	profile->previousMouseXRemainder = 0.0f;
	profile->previousMouseYRemainder = 0.0f;
	profile->screenResolution = 96;
	profile->screenRefreshRate = 60;
	profile->pixelGain = 0.0f;

	profile->thresholdsX[0] = 0.0f;
	profile->thresholdsX[1] = 0.43f;
	profile->thresholdsX[2] = 1.25f;
	profile->thresholdsX[3] = 3.86f;
	profile->thresholdsX[4] = 40.0f;

	profile->thresholdsY[0] = 0.0f;
	profile->thresholdsY[1] = 1.37f;
	profile->thresholdsY[2] = 5.30f;
	profile->thresholdsY[3] = 24.30f;
	profile->thresholdsY[4] = 568.0f;


	/* Load settings profile */
	FILE *serverConfig = fopen(cfgPath, "r");
	char line[1000];
	char compare[27];
	char lineData[1000];

	if(serverConfig != NULL){
		while(!feof(serverConfig)){

			fgets(line, sizeof(line), serverConfig);

			// Remove any comments from the line
			char *commentPos = strstr(line, "//");
			if(commentPos != NULL){
				commentPos = '\0';
			}

			// enhancePointerPrecision
			if(strlen(line) > 26){
				spSubstringHelper(compare, line, 0, 26);
				if(strcmp(compare, "enhancePointerPrecision = ") == 0){
					spSubstringHelper(lineData, line, 26, strlen(line) - 27);
					if(strcmp(lineData, "0") == 0){
						profile->enhancePointerPrecision = 0;
					}else if(strcmp(lineData, "1") == 0){
						profile->enhancePointerPrecision = 1;
					}
				}
			}
			// thresholds
			if(strlen(line) > 22){
				spSubstringHelper(compare, line, 0, 14);
				if(strcmp(compare, "thresholdsX = ") == 0){
					spSubstringHelper(lineData, line, 14, strlen(line) - 15);
					char *token = strtok(lineData, " ");
					float tempThresholds[5];
					int tokenNum = 0;
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
							profile->thresholdsX[tokenNum] = tempThresholds[tokenNum];
						}
					}

				}else if(strcmp(compare, "thresholdsY = ") == 0){
					spSubstringHelper(lineData, line, 14, strlen(line) - 15);
					char *token = strtok(lineData, " ");
					float tempThresholds[5];
					int tokenNum = 0;
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
							profile->thresholdsY[tokenNum] = tempThresholds[tokenNum];
						}
					}

				}
			}
			// screenRefreshRate
			if(strlen(line) > 20){
				spSubstringHelper(compare, line, 0, 20);
				if(strcmp(compare, "screenRefreshRate = ") == 0){
					spSubstringHelper(lineData, line, 20, strlen(line) - 21);
					int tempInt = strtol(lineData, NULL, 10);
					if(tempInt > 0){
						profile->screenRefreshRate = (unsigned int)tempInt;
					}
				}
			}
			// mouseSensitivity and screenResolution
			if(strlen(line) > 19){
				spSubstringHelper(compare, line, 0, 19);
				if(strcmp(compare, "mouseSensitivity = ") == 0){
					spSubstringHelper(lineData, line, 19, strlen(line) - 20);
					float tempFloat = strtof(lineData, NULL);
					if(tempFloat > 0.0f){
						profile->mouseSensitivity = tempFloat;
					}

				}else if(strcmp(compare, "screenResolution = ") == 0){
					spSubstringHelper(lineData, line, 19, strlen(line) - 20);
					float tempFloat = strtof(lineData, NULL);
					if(tempFloat > 0.0f){
						profile->screenResolution = tempFloat;
					}

				}
			}
			// windowsVersion
			if(strlen(line) > 17){
				spSubstringHelper(compare, line, 0, 17);
				if(strcmp(compare, "windowsVersion = ") == 0){
					spSubstringHelper(lineData, line, 17, strlen(line) - 18);
					if(strcmp(lineData, "7") == 0){
						profile->windowsVersion = 2;
					}else if(strcmp(lineData, "XP") == 0){
						profile->windowsVersion = 0;
					}else if(strcmp(lineData, "Vista") == 0){
						profile->windowsVersion = 1;
					}
				}
			}
			// subPixelation
			if(strlen(line) > 16){
				spSubstringHelper(compare, line, 0, 16);
				if(strcmp(compare, "subPixelation = ") == 0){
					spSubstringHelper(lineData, line, 16, strlen(line) - 17);
					if(strcmp(lineData, "0") == 0){
						profile->subPixelation = 0;
					}else if(strcmp(lineData, "1") == 0){
						profile->subPixelation = 1;
					}
				}
			}
			// acceleration
			if(strlen(line) > 15){
				spSubstringHelper(compare, line, 0, 15);
				if(strcmp(compare, "acceleration = ") == 0){
					spSubstringHelper(lineData, line, 15, strlen(line) - 15);
					if(strcmp(lineData, "0") == 0){
						profile->acceleration = 0;
					}else if(strcmp(lineData, "1") == 0){
						profile->acceleration = 1;
					}else if(strcmp(lineData, "2") == 0){
						profile->acceleration = 2;
					}else if(strcmp(lineData, "3") == 0){
						profile->acceleration = 3;
					}
				}
			}

		}
	}

	fclose(serverConfig);

}

void spPrintSettings(settingsProfile *profile){
	printf("Profile loaded\n");
	printf("-------------------------------------------------------------------------------\n");
	printf("Windows version: Windows %s\n", profile->windowsVersion == 0 ? "XP" : (profile->windowsVersion == 1 ? "Vista" : "7"));
	printf("Mouse sensitivity: %f\n", profile->mouseSensitivity);
	printf("Acceleration method: %s\n", profile->acceleration == 0 ? "None" : (profile->acceleration == 1 ? "Enhanced pointer precision" : (profile->acceleration == 2 ? "Non-interpolated" : "Linear")));
	printf("Sub-pixelation: %s\n", profile->subPixelation == 0 ? "Off" : "On");
	printf("Screen resolution: %f PPI\n", profile->screenResolution);
	printf("Screen refresh rate: %u Hz\n", profile->screenRefreshRate);
	printf("Mouse speed thresholds: {%f, %f, %f, %f, %f}\n", profile->thresholdsX[0], profile->thresholdsX[1], profile->thresholdsX[2], profile->thresholdsX[3], profile->thresholdsX[4]);
	printf("Pointer speed thresholds: {%f, %f, %f, %f, %f}\n", profile->thresholdsY[0], profile->thresholdsY[1], profile->thresholdsY[2], profile->thresholdsY[3], profile->thresholdsY[4]);
	printf("-------------------------------------------------------------------------------\n");
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

	if(deviceSpeed == 0.0){
		*segment = 0;
		return deviceSpeed;
	}

	int i;
	if(*segment == profile->FINDSEGMENT){

		for(i = 0; i < 3; i++){
			if(deviceSpeed < profile->thresholdsX[i+1]){
				break;
			}
		}
		*segment = i;

	}else{
		i = *segment;
	}

	float slope = (profile->thresholdsY[i+1] - profile->thresholdsY[i]) / (profile->thresholdsX[i+1] - profile->thresholdsX[i]);
	float intercept = profile->thresholdsY[i] - slope * profile->thresholdsX[i];
	return slope + intercept / deviceSpeed;

}

inline void spUpdate(settingsProfile *profile, int mouseRawX, int mouseRawY, int *mouseX, int *mouseY){

	if(profile->enhancePointerPrecision && profile->acceleration == 1){

		// Handle last movement's remainders as XP does
		if(profile->windowsVersion == 0){
			if(Sign(mouseRawX) != Sign(profile->previousMouseRawX) || mouseRawX == 0){
				profile->previousMouseXRemainder = 0.0f;
			}
			profile->previousMouseRawX = mouseRawX;
			if(Sign(mouseRawY) != Sign(profile->previousMouseRawY) || mouseRawY == 0){
				profile->previousMouseYRemainder = 0.0f;
			}
			profile->previousMouseRawY = mouseRawY;
		}

		// Handle movement's remainders as Vista does
		if(profile->windowsVersion == 1){
			if(mouseRawX != 0){
				if(Sign(mouseRawX) != Sign(profile->previousMouseRawX)){
					profile->previousMouseXRemainder = 0.0f;
				}
				profile->previousMouseRawX = mouseRawX;
			}
			if(mouseRawY != 0){
				if(Sign(mouseRawY) != Sign(profile->previousMouseRawY)){
					profile->previousMouseYRemainder = 0.0f;
				}
				profile->previousMouseRawY = mouseRawY;
			}
		}

		// Fixed or original acceleration method
		float screenResolutionFactor;
		if(profile->windowsVersion == 2){
			screenResolutionFactor = (float)profile->screenResolution / 150.0f;
		}else{
			screenResolutionFactor = (float)profile->screenRefreshRate / (float)profile->screenResolution;
		}

		// Calculate accelerated mouse deltas
		float mouseMag = max(abs(mouseRawX), abs(mouseRawY)) + min(abs(mouseRawX), abs(mouseRawY)) / 2.0f;
		int currentSegmentIndex = profile->FINDSEGMENT;
		profile->pixelGain = screenResolutionFactor
		                     * profile->mouseSensitivity
		                     * spSmoothMouseGain(profile, mouseMag / 3.5f, &currentSegmentIndex)
		                     / 3.5f;

		if(currentSegmentIndex > profile->previousSegmentIndex){
			// Average with calculation using previous curve segment
			float pixelGainUsingPreviousSegment = screenResolutionFactor
			                                      * profile->mouseSensitivity
			                                      * spSmoothMouseGain(profile, mouseMag / 3.5f, &(profile->previousSegmentIndex))
			                                      / 3.5f;
			profile->pixelGain = (profile->pixelGain + pixelGainUsingPreviousSegment) / 2.0f;
		}
		profile->previousSegmentIndex = currentSegmentIndex;

		// Calculate accelerated mouse deltas
		float mouseXplusRemainder = mouseRawX * profile->pixelGain + profile->previousMouseXRemainder;
		float mouseYplusRemainder = mouseRawY * profile->pixelGain + profile->previousMouseYRemainder;

		// Split mouse delta into integer part (applied now) and remainder part (saved for next time)
		// (NOTE: Only when disableSubPixelation==true does this have any significant or cumulative effect)

		// Update x
		if(profile->windowsVersion == 2){
			// Windows 7
			if(mouseXplusRemainder >= 0){
				*mouseX = (int)floor(mouseXplusRemainder);
			}else{
				*mouseX = -(int)floor(-mouseXplusRemainder);
			}
			profile->previousMouseXRemainder = mouseXplusRemainder - *mouseX;
		}else if(!profile->subPixelation && fabs(mouseXplusRemainder) <= abs(mouseRawX)){
			// XP & Vista when disableSubPixelation (never set, AFAIK)
			*mouseX = mouseRawX;
			profile->previousMouseXRemainder = 0.0;
			profile->pixelGain = 1.0;
		}else{
			// XP & Vista
			*mouseX = (int)floor(mouseXplusRemainder);
			profile->previousMouseXRemainder = mouseXplusRemainder - *mouseX;
		}

		// Update y
		if(profile->windowsVersion == 2){
			// Windows 7
			if(mouseYplusRemainder >= 0){
				*mouseY = (int)floor(mouseYplusRemainder);
			}else{
				*mouseY = -(int)floor(-mouseYplusRemainder);
			}
			profile->previousMouseYRemainder = mouseYplusRemainder - *mouseY;
		}else if(!profile->subPixelation && fabs(mouseYplusRemainder) <= abs(mouseRawY)){
			// XP & Vista when disableSubPixelation (never set, AFAIK)
			*mouseY = mouseRawY;
			profile->previousMouseYRemainder = 0.0;
			profile->pixelGain = 1.0;
		}else{
			// XP & Vista
			*mouseY = (int)floor(mouseYplusRemainder);
			profile->previousMouseYRemainder = mouseYplusRemainder - *mouseY;
		}

	}else{

		// Get mouse sensitivity multiplier
		float accelerationMultiplier = profile->mouseSensitivity;
		if(profile->acceleration == 2){
			float speed = sqrt(mouseRawX*mouseRawX + mouseRawY*mouseRawY);
			int i;
			for(i = 4; i >= 0; i--){
				if(speed >= profile->thresholdsX[i]){
					if(profile->acceleration == 2 || i >= 4){
						// Non-interpolated accel
						accelerationMultiplier *= profile->thresholdsY[i];
						i = -1;
					}else{
						// Linear accel
						accelerationMultiplier *= (speed - profile->thresholdsX[i]) * (profile->thresholdsY[i+1] - profile->thresholdsY[i]) /
						                          (profile->thresholdsX[i+1] - profile->thresholdsX[i]) +
						                          profile->thresholdsY[i+1];
						i = -1;
					}
				}
			}
		}

		if(accelerationMultiplier == 1.0f){ // Slider = 0, no remainder to handle

			*mouseX = mouseRawX;
			*mouseY = mouseRawY;

		}else{

			// Calculate accelerated mouse deltas
			float mouseXplusRemainder = mouseRawX * accelerationMultiplier + profile->previousMouseXRemainder;
			float mouseYplusRemainder = mouseRawY * accelerationMultiplier + profile->previousMouseYRemainder;

			if(mouseXplusRemainder >= 0){
				*mouseX = (int)floor(mouseXplusRemainder);
			}else{
				*mouseX = -(int)floor(-mouseXplusRemainder);
			}
			profile->previousMouseXRemainder = mouseXplusRemainder - *mouseX;

			if(mouseYplusRemainder >= 0){
				*mouseY = (int)floor(mouseYplusRemainder);
			}else{
				*mouseY = -(int)floor(-mouseYplusRemainder);
			}
			profile->previousMouseYRemainder = mouseYplusRemainder - *mouseY;

		}

	}

}
