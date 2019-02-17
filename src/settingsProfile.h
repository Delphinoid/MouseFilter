/*
 * LICENSE
 * Libpointing can be redistributed and/or modified under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 * Ad-hoc licences can be provided upon request.
 * We appreciate credit when you use it (cite the following paper), but don't require it:
 * G. Casiez and N. Roussel. No more bricolage! Methods and tools to characterize, replicate and compare pointing transfer functions. In Proceedings of UIST'11, the 24th ACM Symposium on User Interface Software and Technology, pages 603-614, October 2011. ACM.
*/

#ifndef SETTINGSPROFILE_H
#define SETTINGSPROFILE_H

#define SETTINGS_PROFILE_TERMINAL_OUTPUT
#define SETTINGS_PROFILE_DEBUG

typedef struct {

	float sensitivity;  // Sensitivity multiplier.
	float dpiMultiplier;
	float yawMultiplier;
	float pitchMultiplier;
	unsigned int acceleration;  // Type of acceleration: 0 = none, 1 = XP, 2 = Vista, 3 = 7, 4 = Quake.
	int previousSegmentIndex;  // Used for interpolation.
	int FINDSEGMENT;           // Used for interpolation.
	float pixelGain;
	int previousMouseDeltaX;
	int previousMouseDeltaY;
	float previousMouseXRemainder;
	float previousMouseYRemainder;
	float winScreenResolution;          // Screen resolution, measured in PPI. Used with enhanced pointer precision.
	unsigned int winScreenRefreshRate;  // Screen refresh rate, measured in hertz. Used with Windows 7 enhanced pointer precision.
	signed char winSubPixelation;       // Whether or not sub-pixelation is forced. Used with XP and Vista enhanced pointer precision.
	float winThresholdsX[5];            // Thresholds for mouse speed. Used with enhanced pointer precision.
	float winThresholdsY[5];            // Thresholds for resulting pointer speed. Used with enhanced pointer precision.
	float quakeAccel;   // cl_mouseaccel.
	float quakePower;   // cl_mouseaccelpower.
	float quakeOffset;  // cl_mouseacceloffset.
	float quakeCap;     // cl_mousesenscap.
	#ifdef SETTINGS_PROFILE_DEBUG
	signed char verbose;  // When set to 1, enables a verbose terminal output.
	#endif

} settingsProfile;

void spInit(settingsProfile *const restrict profile);
void spLoad(settingsProfile *const restrict profile, const char *const restrict cfgPath);
#ifdef SETTINGS_PROFILE_TERMINAL_OUTPUT
void spPrintSettings(const settingsProfile *const restrict profile);
#endif
void spUpdate(settingsProfile *const restrict profile, int mouseDeltaX, int mouseDeltaY, int *const restrict mouseX, int *const restrict mouseY);

#endif
