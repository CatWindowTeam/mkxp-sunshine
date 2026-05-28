#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_platform.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_sound/SDL_sound.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_video.h>

#include "meow.h"
#include "eventthread.h"
#include "exception.h"
#include "config.h"
#include "gl-debug.h"
#include "gl-fun.h"
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <ruby.h>
#include <ruby/version.h>
#include <zlib.h>
#include <AL/al.h>
#include <boost/version.hpp>
#include <physfs.h>
#include <pixman.h>

#ifdef __LINUX__
	#include <gtk/gtk.h>
	#include "xdg-user-dir-lookup.h"
#endif


SDL_MessageBoxButtonData buttons[] = {
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No" }
};

static inline const char* glGetStringInt(GLenum name){
	return (const char*) gl.GetString(name);
}


void crash(const char* reason, Exception::Type t, bool do_exp){
	char msg[1024];
	snprintf(msg, sizeof msg, "Error occured! Error message: %s\n\n Want to create a crash log? You can share the crash log with the developers and help resolve the issue.", reason);
	SDL_MessageBoxData messageboxdata = {
	    .flags = SDL_MESSAGEBOX_ERROR,
	    .window = NULL,
	    .title = "Crashlog",
	    .message = msg,
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
		if (out.is_open()){
				out << "[SUNSHINE CRASHDUMP]" << std::endl;
				try{
					out << "GL Vendor: " << glGetStringInt(GL_VENDOR) << std::endl;
					out << "GL Renderer: " << glGetStringInt(GL_RENDERER) << std::endl;
					out << "GL Version: " << glGetStringInt(GL_VERSION) << std::endl;
					out << "GLSL Version: " << glGetStringInt(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					out << "Shading language version: " << glGetStringInt(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					out << "GL Extensions: " << glGetStringInt(GL_EXTENSIONS) << std::endl;
				}catch(const std::exception& e){
					out << "Crashed before OpenGL initialization: " << e.what() << std::endl;
				}
				const int sdlcompiled = SDL_VERSION;
				const int sdllinked = SDL_GetVersion();
				out << "SDL(compiled) version: " << SDL_VERSIONNUM_MAJOR(sdlcompiled) << "." << SDL_VERSIONNUM_MINOR(sdlcompiled) << "." << SDL_VERSIONNUM_MICRO(sdlcompiled) << std::endl;
				out << "SDL(linked) version: " << SDL_VERSIONNUM_MAJOR(sdllinked) << "." << SDL_VERSIONNUM_MINOR(sdllinked) << "." << SDL_VERSIONNUM_MICRO(sdllinked) << std::endl;
				out << "SDL_image(compiled) version: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << std::endl;
				out << "SDL_sound(compiled) version: " << SDL_SOUND_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << std::endl;
				out << "SDL_TTF(compiled) version: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_MICRO_VERSION << std::endl;
				out << "Ruby version: " << RUBY_API_VERSION_CODE << std::endl;
				try{
					out << "Detected OS: " << SDL_GetPlatform() << std::endl;
				}catch(const std::exception& e){
					out << "Detected OS: " << e.what() << std::endl;
				}
				if(getenv("XDG_CURRENT_DESKTOP") == NULL){
					out << "Desktop enviroment(XDG_CURRENT_DESKTOP): Unknown"<< std::endl;
				}else{
					out << "Desktop enviroment(XDG_CURRENT_DESKTOP): " << getenv("XDG_CURRENT_DESKTOP") << std::endl;					
				}

				out << "ZLib version: " << ZLIB_VERSION << std::endl;
				out << "OpenAL version: " << AL_VERSION << std::endl;	
				out << "Boost versino: " << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100 << std::endl;
				out << "Pixman version: " << PIXMAN_VERSION_STRING << std::endl;
				
				out.close();
		}else{
			printf("[CRASHLOG] Failed to write crashdump file\n");
		}
	}

	if(do_exp == true){
		if(!t == Exception::MEOW)
			throw Exception(t, msg);
	}	
}
