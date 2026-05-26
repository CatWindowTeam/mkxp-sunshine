#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_platform.h>
#include "meow.h"
#include "config.h"
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <ruby.h>

#ifdef __LINUX__
	#include <gtk/gtk.h>
	#include <gdk/gdk.h>
#endif


SDL_MessageBoxButtonData buttons[] = {
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No" }
};

int crash(const char* reason, int exit_code){
	const Config conf;
	SDL_MessageBoxData messageboxdata = {
	    .flags = SDL_MESSAGEBOX_ERROR,
	    .window = NULL,
	    .title = "Crashdump",
	    .message = "Error occured! Want to create a crash log?",
	    .numbuttons = SDL_arraysize(buttons),
	    .buttons = buttons,
	    .colorScheme = NULL
	};

	
	int buttonid = 0;
	if (SDL_ShowMessageBox(&messageboxdata, &buttonid) == false) {
		printf("[CRASHDUMP] %s\n", reason);
	}

	if(buttonid == 1){
		std::ofstream out;
		time_t mtime = time(NULL);
		struct tm *now = localtime(&mtime);
		std::string time = std::to_string(now->tm_hour) + "." + std::to_string(now->tm_min) + "." + std::to_string(now->tm_sec);
		out.open("crash_" + time + ".txt");
		const int sdlcompiled = SDL_VERSION;
		const int sdllinked = SDL_GetVersion();
		if (out.is_open()){
				out << "[SUNSHINE CRASHDUMP]" << std::endl;
				out << "CONFIG" << std::endl;
				out << "defScreenW: " << conf.defScreenW << std::endl;
				out << "defScreenH: " << conf.defScreenH << std::endl;
				out << "debugMode: " << conf.debugMode << std::endl;
				out << "screenMode: " << conf.screenMode << std::endl;
				out << "fullscreen: " << conf.fullscreen << std::endl;
				out << "fixedAspectRatio: " << conf.fixedAspectRatio << std::endl;
				out << "smoothScaling: " << conf.smoothScaling << std::endl;
				out << "vsync: " << conf.vsync << std::endl;
				out << "EnableSixteenByNine: " << conf.EnableSixteenByNine << std::endl;
				out << "windowTitle: " << conf.windowTitle << std::endl;
				out << "defSfixedFrameratecreenW: " << conf.fixedFramerate << std::endl;
				out << "frameSkip: " << conf.frameSkip << std::endl;
				out << "syncToRefreshrate: " << conf.syncToRefreshrate << std::endl;
				out << "solidFonts: " << conf.solidFonts << std::endl;
				out << "subImageFix: " << conf.subImageFix << std::endl;
				out << "enableBlitting: " << conf.enableBlitting << std::endl;
				out << "maxTextureSize: " << conf.maxTextureSize << std::endl;
				out << "gameFolder: " << conf.gameFolder << std::endl;
				out << "allowSymlinks: " << conf.allowSymlinks << std::endl;
				out << "pathCache: " << conf.pathCache << std::endl;
				out << "iconPath: " << conf.iconPath << std::endl;
				out << "useScriptNames: " << conf.useScriptNames << std::endl;
				out << "customScript: " << conf.customScript << std::endl;
				out << "customDataPath: " << conf.customDataPath << std::endl;
				out << "commonDataPath: " << conf.commonDataPath << std::endl;
				out << "" << std::endl;
				out << "Ruby version: " << rb_gv_get("ruby_version") << std::endl;
				out << "SDL(compiled) version: " << SDL_VERSIONNUM_MAJOR(sdlcompiled) << "." << SDL_VERSIONNUM_MINOR(sdlcompiled) << "." << SDL_VERSIONNUM_MICRO(sdlcompiled) << std::endl;
				out << "SDL(linked) version: " << SDL_VERSIONNUM_MAJOR(sdllinked) << "." << SDL_VERSIONNUM_MINOR(sdllinked) << "." << SDL_VERSIONNUM_MICRO(sdllinked) << std::endl;
				out << "Detected OS: " << SDL_GetPlatform() << std::endl;
				#ifdef __LINUX__
					out << "GTK(compiled) version: " << GTK_MAJOR_VERSION << "." << GTK_MINOR_VERSION << "." << GTK_MICRO_VERSION << std::endl;
				#endif
				out.close();
		}else{
			printf("[CRASHLOG] Failed to write crashdump file\n");
		}
	}
	
	return exit_code;
}
