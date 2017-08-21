/*
 * LICENSE
 * Libpointing can be redistributed and/or modified under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 * Ad-hoc licences can be provided upon request.
 * We appreciate credit when you use it (cite the following paper), but don't require it:
 * G. Casiez and N. Roussel. No more bricolage! Methods and tools to characterize, replicate and compare pointing transfer functions. In Proceedings of UIST'11, the 24th ACM Symposium on User Interface Software and Technology, pages 603-614, October 2011. ACM.
*/

#ifndef SETTINGSPROFILE_H
#define SETTINGSPROFILE_H

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

typedef struct {

	float sensitivity;  // Sensitivity multiplier
	float dpiMultiplier;
	float yawMultiplier;
	float pitchMultiplier;
	unsigned int acceleration;  // Type of acceleration: 0 = none, 1 = XP, 2 = Vista, 3 = 7, 4 = Quake
	int previousSegmentIndex;  // used for interpolation
	int FINDSEGMENT;  // used for interpolation
	float pixelGain;
	int previousMouseDeltaX;
	int previousMouseDeltaY;
	float previousMouseXRemainder;
	float previousMouseYRemainder;
	float winScreenResolution;          // Screen resolution, measured in PPI. Used with enhanced pointer precision
	unsigned int winScreenRefreshRate;  // Screen refresh rate, measured in hertz. Used with Windows 7 enhanced pointer precision
	unsigned char winSubPixelation;     // Whether or not sub-pixelation is forced. Used with XP and Vista enhanced pointer precision
	float winThresholdsX[5];            // Thresholds for mouse speed. Used with enhanced pointer precision
	float winThresholdsY[5];            // Thresholds for resulting pointer speed. Used with enhanced pointer precision
	float quakeAccel;   // cl_mouseaccel
	float quakePower;   // cl_mouseaccelpower
	float quakeOffset;  // cl_mouseacceloffset
	float quakeCap;     // cl_mousesenscap
	unsigned char verbose;  // When set to 1, enables a verbose terminal output

} settingsProfile;

void spLoad(settingsProfile *profile, const char *cfgPath);
void spPrintSettings(const settingsProfile *profile);
extern inline void spUpdate(settingsProfile *profile, int mouseRawX, int mouseRawY, int *mouseXin, int *mouseYin);

#endif
