#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_platform.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_sound/SDL_sound.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_video.h>
#include "meow.h"
#include "eventthread.h"
#include "exception.h"
#include "config.h"
#include "gl-debug.h"
#include "gl-fun.h"
#include "debugwriter.h"
#include "define.h"
#include <SDL3/SDL_stdinc.h>
#include <time.h>
#include <fstream>
#include <ruby/version.h>
#include <ruby/internal/intern/vm.h>
#include <ruby/internal/error.h>
#include <ruby/debug.h>
#include <ruby.h>
#undef vsnprintf
#undef snprintf
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#endif
#include <boost/stacktrace.hpp>
#include <boost/version.hpp>
#include <zlib.h>
#include <AL/al.h>
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

    if (t == Exception::ModLoaderError) {
        crash_reason = "Broken mod";
        crash_possible_solution = "Fix mode manualy or ask developer to fix it or delete mod";
    } else if (t == Exception::NoFileError) {
        crash_reason = "Broken installation";
        crash_possible_solution = "Try reinstall game";
    }

    show_crash_sceen = true;
    rb_exit(-1);
}

void crash_screen(SDL_Window* win){
	static char msg[1024] = "";
	static char msg1[1024] = "";
	static char msg2[1024] = "";
	static char msg3[1024] = "";
	SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
	if (ren == nullptr) {
		ErrorMsg("Failed to create crash sceen, please check if your device is too strong to run game: ", SDL_GetError());
	}
	SDL_Surface* crash_img = IMG_Load_IO(SDL_IOFromConstMem(assets_crash_png, assets_crash_png_len), true);
	if (crash_img == nullptr) {
		ErrorMsg("Failed to create crash sceen, please check if your device is too strong to run game: ", SDL_GetError());
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, crash_img);
	SDL_DestroySurface(crash_img);	
	if (tex == nullptr) {
		ErrorMsg("Failed to create crash sceen, please check if your device is too strong to run game: ", SDL_GetError());
	}
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_snprintf(msg, sizeof(msg), "Message: %s", crash_message);
	SDL_snprintf(msg1, sizeof(msg1), "Possible reason: %s", crash_reason);
	SDL_snprintf(msg2, sizeof(msg2), "Possible solution: %s", crash_possible_solution);
	SDL_snprintf(msg3, sizeof(msg3), "Crashdump privacy: %s", is_privacy_crashdump_enabled);
	
	std::ofstream o;
	time_t mtime = time(NULL);
	struct tm *now = localtime(&mtime);
	std::string time = std::to_string(now->tm_hour) + "." + std::to_string(now->tm_min) + "." + std::to_string(now->tm_sec);
	o.open("crash_" + time + ".txt");
	if (o.is_open()){
		o << "REASON: " << msg << std::endl;
		o << "[BOOST stacktrace()]" << std::endl;
		o << boost::stacktrace::stacktrace() << std::endl;
		const static int sdlcompiled = SDL_VERSION;
		const static int sdllinked = SDL_GetVersion();
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
		#ifdef unix_like
			if(SDL_getenv("XDG_CURRENT_DESKTOP") != nullptr){
				o << "Desktop enviroment(XDG_CURRENT_DESKTOP): " << SDL_getenv("XDG_CURRENT_DESKTOP") << std::endl;
			}
		#elif android
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
		#elif web
			o << "Emscripten start address of the stack: " << emscripten_stack_get_base() << std::endl;
			o << "Emscripten end address of the stack: " << emscripten_stack_get_end() << std::endl;
			o << "Emscripten current stack pointer: " << emscripten_stack_get_current() << std::endl;
			o << "Emscripten number of free bytes left on stack: " << emscripten_stack_get_free() << std::endl;
		#elif psp
			o << "PSPdev MIPS Stack Trace: " << pspDebugGetStackTrace() << std::endl;
		#elif dos
			o << "DPMI virtual interrupt state: " << __dpmi_get_virtual_interrupt_state() << std::endl;
			o << "DPMI selector increment value: " << __dpmi_get_selector_increment_value() << std::endl;
			o << "DPMI coprocessor status: " << __dpmi_get_coprocessor_status() << std::endl;	
			o << "DPMI is 80387 processor?: " << _detect_80387() << std::endl;
		#endif
		o << "[Hardware]" << std::endl;
		o << "number of logical CPU cores: " << SDL_GetNumLogicalCPUCores() << std::endl;
		o << "System RAM size: " << SDL_GetSystemRAM() << " MiB" << std::endl;
		o << "[Ruby]" << std::endl;
		o << "Is GC was busy? " << rb_during_gc() << std::endl;
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
		o.close();
	}else{
		WarnMsg("[CRASHLOG] Failed to write crashdump file");
	}		
	


	SDL_Event e;
	bool quit = false;

	while (!quit) {
	    while (SDL_PollEvent(&e)) {
	        if (e.type == SDL_EVENT_QUIT) quit = true;
	    }
	
	    SDL_RenderClear(ren);
	    SDL_RenderTexture(ren, tex, NULL, NULL);
	
	    SDL_RenderDebugText(ren, 10, 10, "World machine crashed, crashdump created in game directory");
	    SDL_RenderDebugText(ren, 10, 20, msg);
	    SDL_RenderDebugText(ren, 10, 30, msg1);
	    SDL_RenderDebugText(ren, 10, 40, msg2);
	    SDL_RenderDebugText(ren, 10, 50, msg3);
	
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

    if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", buf, NULL)) {
        // TODO: error handling
    }
}
