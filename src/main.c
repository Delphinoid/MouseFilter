#include <Interception/interception.h>
#include "utils.h"
#include "settingsProfile.h"
#include <string.h>

int main(int argc, char *argv[]){

	/* Initializing some things for the Interception library */
	InterceptionContext context;
	InterceptionDevice device;
	InterceptionStroke stroke;
	raise_process_priority();
	context = interception_create_context();
	interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_MOVE);

	/* Load a settings profile */
	char *profilePath;
	if(argc > 0){
		profilePath = argv[0];  // First argument is usually the path to the program
		profilePath[strrchr(profilePath, '\\') - profilePath + 1] = '\0';  // Removes program name (everything after the last backslash) from the path
		if(argc > 1){
			strcpy(profilePath + strlen(profilePath), argv[1]);  // If there is a second argument, append it to the end of the profile path
		}else{
			strcpy(profilePath + strlen(profilePath), "profile.txt");  // Otherwise use the default profile path
		}
	}else{
		profilePath = "";
	}
	settingsProfile profile;
	spLoad(&profile, profilePath);
	spPrintSettings(&profile);

	/* Interception loop */
	while(interception_receive(context, device = interception_wait(context), &stroke, 1) > 0){
		if(interception_is_mouse(device)){
			// Intercept input from mouse (in mickeys)
			InterceptionMouseStroke *mstroke = (InterceptionMouseStroke*)&stroke;
			if((mstroke->flags & INTERCEPTION_MOUSE_MOVE_ABSOLUTE) == 0){  // Checks for mouse movement
				// Modifies mouse input based on the profile loaded
				spUpdate(&profile, mstroke->x, mstroke->y, &(mstroke->x), &(mstroke->y));
			}
			interception_send(context, device, &stroke, 1);
		}
	}

	interception_destroy_context(context);
	return 0;

}
