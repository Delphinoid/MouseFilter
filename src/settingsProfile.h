/*
 * LICENSE
 * Libpointing can be redistributed and/or modified under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 * Ad-hoc licences can be provided upon request.
 * We appreciate credit when you use it (cite the following paper), but don't require it:
 * G. Casiez and N. Roussel. No more bricolage! Methods and tools to characterize, replicate and compare pointing transfer functions. In Proceedings of UIST'11, the 24th ACM Symposium on User Interface Software and Technology, pages 603-614, October 2011. ACM.
*/

#ifndef SETTINGSPROFILE_H
#define SETTINGSPROFILE_H

typedef struct {

	unsigned int windowsVersion;  // 0 = XP, 1 = Vista, 2 = 7
	float mouseSensitivity;  // Sensitivity multiplier
	unsigned int enhancePointerPrecision;  // Whether or not threshold-based mouse acceleration is enabled
	unsigned int accelMethod;    // Whether or not we're using the patched mouse acceleration method
	unsigned int subPixelation;  // Whether or not sub-pixelation is enabled
	int previousSegmentIndex;  // used for interpolation
	int FINDSEGMENT;  // used for interpolation
	int previousMouseRawX;
	int previousMouseRawY;
	float previousMouseXRemainder;
	float previousMouseYRemainder;
	float screenResolution;   // PPI
	unsigned int screenRefreshRate;  // Hz
	float pixelGain;
	float thresholdsX[5];  // Thresholds for mouse speed
	float thresholdsY[5];  // Thresholds for resulting pointer speed

} settingsProfile;

void spLoad(settingsProfile *profile, const char *cfgPath);
void spPrintSettings(settingsProfile *profile);
extern inline void spUpdate(settingsProfile *profile, int mouseRawX, int mouseRawY, int *mouseX, int *mouseY);

#endif
