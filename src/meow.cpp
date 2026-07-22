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
#include "debugwriter.h"
#include <SDL3/SDL_stdinc.h>
#include <time.h>
#include <fstream>
#include <ruby/version.h>
#include <zlib.h>
#include <AL/al.h>
#include <boost/version.hpp>
#include <physfs.h>
#include <pixman.h>
#include <SDL3/SDL_system.h>
#include <boost/stacktrace.hpp>
#include <SDL3/SDL_cpuinfo.h>
#include "sunshine-binding.h"
#ifdef __LINUX__
	#include <gtk/gtk.h>
	#include "xdg-user-dir-lookup.h"
#elif __ANDROID__
	#include <android/trace.h>
	#include <android/api-level.h>
#elif __EMSCRIPTEN__
	#include <emscripten/console.h>
#endif

SDL_MessageBoxButtonData buttons[] = {
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No" }
};

static inline const char* glGetStringInt(GLenum name){
	return (const char*) gl.GetString(name);
}

void crash(Exception::Type t, const char *fmt, ...){
	char msg[1024];
	va_list args;
	va_start(args, fmt);
	va_list args_copy;
	va_copy(args_copy, args);
	short len = SDL_vsnprintf(NULL, 0, fmt, args_copy);
	va_end(args_copy);
	char *buf = (char*)SDL_malloc((size_t)len + 1);
	SDL_vsnprintf(buf, (size_t)len + 1, fmt, args);
	va_end(args);
	SDL_snprintf(msg, sizeof msg, "Error occured! Error message: %s\n\n Want to create a crash SDL_log? You can share the crash SDL_log with the developers and help resolve the issue.", buf);
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
		printf("[CRASHDUMP] %s\n", buf);
	}

	if(buttonid == 1){
		std::ofstream o;
		time_t mtime = time(NULL);
		struct tm *now = localtime(&mtime);
		std::string time = std::to_string(now->tm_hour) + "." + std::to_string(now->tm_min) + "." + std::to_string(now->tm_sec);
		o.open("crash_" + time + ".txt");
		if (o.is_open()){
				o << "REASON: " << buf << std::endl;
				o << "[BOOST stacktrace()]" << std::endl;
				o << boost::stacktrace::stacktrace() << std::endl;
				o << "[OpenGL]" << std::endl;
				try{
					o << "GL Vendor: " << glGetStringInt(GL_VENDOR) << std::endl;
					o << "GL Renderer: " << glGetStringInt(GL_RENDERER) << std::endl;
					o << "GL Version: " << glGetStringInt(GL_VERSION) << std::endl;
					o << "GLSL Version: " << glGetStringInt(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					o << "Shading language version: " << glGetStringInt(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					o << "GL Extensions: " << glGetStringInt(GL_EXTENSIONS) << std::endl;
				}catch(const std::exception& e){
					o << "Crashed before OpenGL initialization: " << e.what() << std::endl;
				}
				o << "[Versions of libs]" << std::endl;
				const int sdlcompiled = SDL_VERSION;
				const int sdllinked = SDL_GetVersion();
				o << "SDL(compiled) version: " << SDL_VERSIONNUM_MAJOR(sdlcompiled) << "." << SDL_VERSIONNUM_MINOR(sdlcompiled) << "." << SDL_VERSIONNUM_MICRO(sdlcompiled) << std::endl;
				o << "SDL(linked) version: " << SDL_VERSIONNUM_MAJOR(sdllinked) << "." << SDL_VERSIONNUM_MINOR(sdllinked) << "." << SDL_VERSIONNUM_MICRO(sdllinked) << std::endl;
				o << "SDL_image(compiled) version: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << std::endl;
				o << "SDL_sound(compiled) version: " << SDL_SOUND_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << std::endl;
				o << "SDL_TTF(compiled) version: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_MICRO_VERSION << std::endl;
				o << "Ruby version: " << RUBY_API_VERSION_CODE << std::endl;
				o << "ZLib version: " << ZLIB_VERSION << std::endl;
				o << "OpenAL version: " << AL_VERSION << std::endl;
				o << "Boost versino: " << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100 << std::endl;
				o << "Pixman version: " << PIXMAN_VERSION_STRING << std::endl;
				o << "[Platform specific]" << std::endl;
				try{
					o << "Detected OS: " << SDL_GetPlatform() << std::endl;
				}catch(const std::exception& e){
					o << "Detected OS: " << e.what() << std::endl;
				}
				if(!SDL_getenv("XDG_CURRENT_DESKTOP") == NULL){
					o << "Desktop enviroment(XDG_CURRENT_DESKTOP): " << SDL_getenv("XDG_CURRENT_DESKTOP") << std::endl;
				}
				#ifdef __ANDROID__
					o << "Android API version: " << android_get_device_api_level() << std::endl;
					o << "External storage State: " << SDL_GetAndroidExternalStorageState() << std::endl;
					o << "Internal storage path: " << SDL_GetAndroidInternalStoragePath() << std::endl;
					o << "External Storage path: " << SDL_GetAndroidExternalStoragePath() << ats::endl;
					o << "Cache path: " << SDL_GetAndroidCachePath() << std::endl;
					if(!is_privacy_crashdump_enabled){
						o << "Is ChromeBook? " << SDL_IsChromebook() << std::endl;
						o << "Is Phone? " << SDL_IsPhone() << std::endl;
						o << "Is Tablet? " << SDL_IsTablet() << std::endl;
						o << "Is Samsung DeX? " << SDL_IsDeXMode() << std::endl;
						o << "Is TV? " << SDL_IsTV() << std::endl;
						o << "Is Ubuntu Touch? " << SDL_IsUbuntuTouch() << std::endl;
					}
				#elif __EMSCRIPTEN__
					o << "Emscripten start address of the stack: " << emscripten_stack_get_base() << std::endl;
					o << "Emscripten end address of the stack: " << emscripten_stack_get_end() << std::endl;
					o << "Emscripten current stack pointer: " << emscripten_stack_get_current() << std::endl;
					o << "Emscripten number of SDL_free bytes left on stack: " << emscripten_stack_get_free() << std::endl;
				#elif __PSP__
					o << "PSPdev MIPS Stack Trace: " << int pspDebugGetStackTrace() << std::endl;
				#endif
				o << "[Hardware]" << std::endl;
				o << "number of logical CPU cores: " << SDL_GetNumLogicalCPUCores() << std::endl;
				o << "System RAM size: " << SDL_GetSystemRAM() << " MiB" << std::endl;
				o.close();
		}else{
			Debug() << "[CRASHLOG] Failed to write crashdump file";
		}
	}

	if(!t == Exception::MEOW)
		throw Exception(t, msg);
}


void ErrorMsg(const char* message){
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, NULL);
}

void WarnMsg(const char* message){
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", message, NULL); 
}
