#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_platform.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_filesystem.h>
#include "meow.h"
#include "eventthread.h"
#include "exception.h"
#include "config.h"
#include "gl-debug.h"
#include "gl-fun.h"
#include "debugwriter.h"
#include "define.h"
#include <SDL3/SDL_stdinc.h>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <ruby/version.h>
#undef vsnprintf
#undef snprintf
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#endif
#include <boost/stacktrace.hpp>
#include <boost/version.hpp>
#include <zlib.h>
#include <physfs.h>
#include <pixman.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_cpuinfo.h>
#include "sunshine.h"
#ifdef unix_like
	#include <gtk/gtk.h>
#elif android
	#include <android/trace.h>
	#include <android/api-level.h>
#elif web
	#include <emscripten/console.h>
#elif dos
	#include <dpmi.h>
#endif
#include "crash.png.xxd"

using namespace std;

static inline const char* glGetStringInt(GLenum name){
	return (const char*) gl.GetString(name);
}

void crash(Exception::Type t, const char *fmt, ...) {
    static char crash_message[1024];
    static const char* crash_reason = nullptr;
    static const char* crash_possible_solution = nullptr;

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    unsigned int len = (unsigned int)SDL_vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (len >= sizeof(crash_message)) len = sizeof(crash_message) - 1;

    SDL_vsnprintf(crash_message, sizeof(crash_message), fmt, args);
    va_end(args);

	//TODO: rewrite to use switch :3
    if (t == Exception::ModLoaderError) {
        crash_reason = "Broken mod";
        crash_possible_solution = "Fix mode manualy or ask developer to fix it or delete mod";
    } else if (t == Exception::NoFileError) {
        crash_reason = "Broken installation";
        crash_possible_solution = "Try reinstall game";
    } else if (t == Exception::ShaderError) {
		crash_reason = "Broken Shader";
		crash_possible_solution = "Try reinstall game";
	} else if (t == Exception::RGSSError) {
		crash_reason = "Internal Error";
		crash_possible_solution = "Try reinstall game or disable some mods";
	} else if (t == Exception::RUBYError) {
		crash_reason = "Internal Error";
		crash_possible_solution = "Try reinstall game or disable some mods";
	} else if (t == Exception::IOError) {
		crash_reason = "Broken installation";
		crash_possible_solution = "Try reinstall game";
	} else if (t == Exception::TypeError) {
		crash_reason = "Internal Error";
		crash_possible_solution = "Try reinstall game or disable some mods";
	} else if (t == Exception::ArgumentError) {
		crash_reason = "Internal error";
		crash_possible_solution = "Try reinstall game or disable some mods";
	} else if (t == Exception::PHYSFSError) {
		crash_reason = "Internal error";
		crash_possible_solution = "Maybe you tryed load corrupted mod via modloader, try delete it";
	} else if (t == Exception::SDLError) {
		crash_reason = "Internal error";
		crash_possible_solution = "Internal Engine Error, maybe something wrong with your device or operating system";
	} else if (t == Exception::MKXPError) {
		crash_reason = "Internal error";
		crash_possible_solution = "Try reinstall game or disable some mods";
	}
    show_crash_sceen = true;
    rb_exit(-1);
}

void crash_screen(SDL_Window* win){
	static char msg[1024] = "";
	static char msg1[1024] = "";
	static char msg2[1024] = "";
	static char msg3[1024] = "";
	static char msg4[1024] = "";
	//creating render
	SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
	if (ren == nullptr) {
		WarnMsg("Failed to create crash sceen, please check if your device is too strong to run game: ", SDL_GetError());
	}
	SDL_Surface* crash_img = IMG_Load_IO(SDL_IOFromConstMem(assets_crash_png, assets_crash_png_len), true);
	if (crash_img == nullptr) {
		WarnMsg("Failed to create crash sceen, please check if your device is too strong to run game: ", SDL_GetError());
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, crash_img);
	SDL_DestroySurface(crash_img);	
	if (tex == nullptr) {
		WarnMsg("Failed to create crash sceen, please check if your device is too strong to run game: ", SDL_GetError());
	}
	//stringsssssssssssss
	SDL_snprintf(msg, sizeof(msg), "Message: %s", crash_message);
	SDL_snprintf(msg1, sizeof(msg1), "Possible reason: %s", crash_reason);
	SDL_snprintf(msg2, sizeof(msg2), "Possible solution: %s", crash_possible_solution);
	SDL_snprintf(msg3, sizeof(msg3), "Crashdump privacy: %s", is_privacy_crashdump_enabled);	
	//creating file and timestamp
	ofstream o;
	time_t timestamp;
	time(&timestamp);
	string timeeeeee(ctime(&timestamp));
	if (!timeeeeee.empty() && timeeeeee.back() == '\n') {
	    timeeeeee.pop_back();
	}
	string file = "crash " + timeeeeee + ".txt";
	SDL_snprintf(msg4, sizeof(msg4), "Path: %s%s", SDL_GetCurrentDirectory(), file.c_str());
	o.open(file);
	//collecting info and writing to crashdump
	if (o.is_open()){
		#ifdef DEVBUILD
			o << "EXPEREMENTAL BUILD" << endl;
		#endif
		o << "VERSION: " << VERSION_STRING << endl;
		o << "REASON: " << msg << endl;
		o << "[BOOST stacktrace()]" << endl << endl;
		o << boost::stacktrace::stacktrace() << endl;

		o << "[LOG BUFFER]" << endl << endl;
		for (const auto& s : logs) {
		    o << s << endl;
		}
		
		o << endl;
		o << "[AUDIO]" << endl;
		o << "Driver used: " << SDL_GetCurrentAudioDriver() << endl;

		o << endl;
		o << "[VIDEO]" << endl;
		o << "Driver used: " << SDL_GetCurrentVideoDriver() << endl;

		o << endl;
		o << "[VERSIONS OF LIBS]" << endl;
		const static int sdlcompiled = SDL_VERSION;
		const static int sdllinked = SDL_GetVersion();
		o << "SDL(compiled) version: " << SDL_VERSIONNUM_MAJOR(sdlcompiled) << "." << SDL_VERSIONNUM_MINOR(sdlcompiled) << "." << SDL_VERSIONNUM_MICRO(sdlcompiled) << endl;
		o << "SDL(linked) version: " << SDL_VERSIONNUM_MAJOR(sdllinked) << "." << SDL_VERSIONNUM_MINOR(sdllinked) << "." << SDL_VERSIONNUM_MICRO(sdllinked) << endl;
		o << "SDL_image(compiled) version: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << endl;
		o << "SDL_TTF(compiled) version: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_MICRO_VERSION << endl;
		o << "Ruby version: " << RUBY_API_VERSION_CODE << endl;
		o << "ZLib version: " << ZLIB_VERSION << endl;
		o << "Boost versino: " << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100 << endl;
		o << "Pixman version: " << PIXMAN_VERSION_STRING << endl;

		o << endl;
		o << "[Platform specific]" << endl;
		try{
			o << "Detected OS: " << SDL_GetPlatform() << endl;
		}catch(const std::exception& e){
			o << "Detected OS: " << e.what() << endl;
		}
		o << "System page size: " << SDL_GetSystemPageSize() << endl;
		#ifdef unix_like
			if(SDL_getenv("XDG_CURRENT_DESKTOP") != nullptr){
				o << "Desktop enviroment(XDG_CURRENT_DESKTOP): " << SDL_getenv("XDG_CURRENT_DESKTOP") << endl;
			}
		#elif android
			o << "Android API version: " << android_get_device_api_level() << endl;
			o << "External storage State: " << SDL_GetAndroidExternalStorageState() << endl;
			o << "Internal storage path: " << SDL_GetAndroidInternalStoragePath() << endl;
			o << "External Storage path: " << SDL_GetAndroidExternalStoragePath() << endl;
			o << "Cache path: " << SDL_GetAndroidCachePath() << endl;
			if(!is_privacy_crashdump_enabled){
				o << "Is ChromeBook? " << SDL_IsChromebook() << endl;
				o << "Is Phone? " << SDL_IsPhone() << endl;
				o << "Is Tablet? " << SDL_IsTablet() << endl;
				o << "Is Samsung DeX? " << SDL_IsDeXMode() << endl;
				o << "Is TV? " << SDL_IsTV() << endl;
				o << "Is Ubuntu Touch? " << SDL_IsUbuntuTouch() << endl;
			}
		#elif web
			o << "Emscripten start address of the stack: " << emscripten_stack_get_base() << endl;
			o << "Emscripten end address of the stack: " << emscripten_stack_get_end() << endl;
			o << "Emscripten current stack pointer: " << emscripten_stack_get_current() << endl;
			o << "Emscripten number of free bytes left on stack: " << emscripten_stack_get_free() << endl;
		#elif psp
			o << "PSPdev MIPS Stack Trace: " << pspDebugGetStackTrace() << endl;
		#endif
		
		o << endl;
		o << "[Hardware]" << endl;
		o << "number of logical CPU cores: " << SDL_GetNumLogicalCPUCores() << endl;
		o << "System RAM size: " << SDL_GetSystemRAM() << " MiB" << endl;
		o << "L1 cache size: " << SDL_GetCPUCacheLineSize() << endl;
		
		o << endl;
		o << "[OpenGL]" << endl;
		try{
			o << "GL Vendor: " << glGetStringInt(GL_VENDOR) << endl;
			o << "GL Renderer: " << glGetStringInt(GL_RENDERER) << endl;
			o << "GL Version: " << glGetStringInt(GL_VERSION) << endl;
			o << "GLSL Version: " << glGetStringInt(GL_SHADING_LANGUAGE_VERSION) << endl;
			o << "Shading language version: " << glGetStringInt(GL_SHADING_LANGUAGE_VERSION) << endl;
			o << "GL Extensions: " << glGetStringInt(GL_EXTENSIONS) << endl;
		}catch(const exception& e){
			o << "Crashed before OpenGL initialization: " << e.what() << endl;
		}
		o.close();
	}else{
		WarnMsg("[CRASHLOG] Failed to write crashdump file");
	}		
	
	SDL_Event e;
	bool quit = false;

	int texW = 100, texH = 100;
	int winW = 0, winH = 0;
	SDL_GetWindowSize(win, &winW, &winH);
	SDL_FRect dst{
	    (float)((winW - 100) / 2),
	    (float)((winH - 100) / 2),
	    (float)100,
	    (float)100
	};
	
	while (!quit) {
	    while (SDL_PollEvent(&e)) {
	        if (e.type == SDL_EVENT_QUIT) quit = true;
	    }
	    
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	    SDL_RenderClear(ren);
	    SDL_RenderTexture(ren, tex, NULL, &dst);
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	    SDL_RenderDebugText(ren, 10, 10, "World machine crashed, crashdump created in game directory");
	    SDL_RenderDebugText(ren, 10, 20, msg);
	    SDL_RenderDebugText(ren, 10, 30, msg1);
	    SDL_RenderDebugText(ren, 10, 40, msg2);
	    SDL_RenderDebugText(ren, 10, 50, msg3);
	    SDL_RenderDebugText(ren, 10, 60, msg4);
	    SDL_RenderDebugText(ren, 10, 70, "If you are sure that the problem is not in your modifications,");
	    SDL_RenderDebugText(ren, 10, 80, "your hands or in your device - please report the bug to the developers");
	    SDL_RenderPresent(ren);
	}
}

void ErrorMsg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    unsigned int len = (unsigned int)SDL_vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    char *buf = (char*)SDL_malloc((size_t)len + 1);
    if (!buf) { va_end(args); return; }

    SDL_vsnprintf(buf, (size_t)len + 1, fmt, args);
    va_end(args);

	SDL_snprintf(crash_message, sizeof(crash_message), "%s", buf);
	show_crash_sceen = true;
}

void WarnMsg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    unsigned int len = (unsigned int)SDL_vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    char *buf = (char*)SDL_malloc((size_t)len + 1);
    if (!buf) { va_end(args); return; }

    SDL_vsnprintf(buf, (size_t)len + 1, fmt, args);
    va_end(args);
	Debug() << "[WARNMSG]" << buf;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", buf, NULL);
}

