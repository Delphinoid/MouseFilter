#include <Interception/interception.h>
#include "settingsProfile.h"
#include <string.h>
#include <windows.h>

int main(int argc, char **argv){

	settingsProfile profile;
	InterceptionContext context;
	InterceptionDevice device;
	InterceptionStroke stroke;

	// Initializing some things for the Interception library.
	context = interception_create_context();
	interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_MOVE);

	// Load a settings profile.
	spInit(&profile);
	if(argc > 0){
		char profilePath[1024];
		const size_t length = strrchr(argv[0], '\\') + 1 - argv[0];
		strncpy(&profilePath[0], argv[0], length);
		if(argc > 1){
			strcpy(profilePath + length, argv[1]);
		}else{
			strncpy(profilePath + length, "profile.txt\0", 12);
		}
		spLoad(&profile, profilePath);
	}
	#ifdef SETTINGS_PROFILE_TERMINAL_OUTPUT
	spPrintSettings(&profile);
	#endif

	// Make the process high-priority.
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	// Interception loop.
	while(interception_receive(context, device = interception_wait(context), &stroke, 1) > 0){
		if(interception_is_mouse(device)){
			// Intercept input from mouse (in mickeys).
			InterceptionMouseStroke *mstroke = (InterceptionMouseStroke*)&stroke;
			// Check for mouse movement.
			if((mstroke->flags & INTERCEPTION_MOUSE_MOVE_ABSOLUTE) == 0){
				// Modify mouse input based on the profile loaded.
				spUpdate(&profile, mstroke->x, mstroke->y, &(mstroke->x), &(mstroke->y));
			}
			interception_send(context, device, &stroke, 1);
		}
	}

	interception_destroy_context(context);
	return 0;

}
