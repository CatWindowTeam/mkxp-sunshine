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
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_cpuinfo.h>
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
#include <ios>
#include <ruby/version.h>
#include <ruby/internal/interpreter.h>
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
#include "sunshine.h"
#ifdef unix_like
	#include <gtk/gtk.h>
#elif mkxp_android
	#include <android/trace.h>
	#include <android/api-level.h>
#elif web
	#include <emscripten/console.h>
#endif
#include "crash.png.xxd"
using namespace std;

//help functions
static inline const char* glGetStringInt(GLenum name){
	return (const char*) gl.GetString(name);
}

#define STR2(x) #x
#define STR(x) STR2(x)

#if defined(__clang__)
  #define COMPILER_NAME "Clang"
  #define COMPILER_VER  STR(__clang_major__) "." STR(__clang_minor__) "." STR(__clang_patchlevel__)
#elif defined(__GNUC__)
  #define COMPILER_NAME "GCC"
  #define COMPILER_VER  STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
  #define COMPILER_NAME "MSVC"
  #define COMPILER_VER  STR(_MSC_VER)
#elif defined(__INTEL_COMPILER)
  #define COMPILER_NAME "Intel"
  #define COMPILER_VER  "n/a"
#else
  #define COMPILER_NAME "Unknown compiler"
  #define COMPILER_VER  "n/a"
#endif

// https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
const static char* get_processor(){
	#if defined(__x86_64__) || defined(_M_X64)
        return "x86_64";
        #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
        return "x86_32";
        #elif defined(__ARM_ARCH_2__)
        return "ARM2";
        #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
        return "ARM3";
        #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
        return "ARM4T";
        #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
        return "ARM5"
        #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
        return "ARM6T2";
        #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
        return "ARM6";
        #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        return "ARM7";
        #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        return "ARM7A";
        #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        return "ARM7R";
        #elif defined(__ARM_ARCH_7M__)
        return "ARM7M";
        #elif defined(__ARM_ARCH_7S__)
        return "ARM7S";
        #elif defined(__aarch64__) || defined(_M_ARM64)
        return "ARM64";
        #elif defined(mips) || defined(__mips__) || defined(__mips)
        return "MIPS";
        #elif defined(__sh__)
        return "SUPERH";
        #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
        return "POWERPC";
        #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        return "POWERPC64";
        #elif defined(__sparc__) || defined(__sparc)
        return "SPARC";
        #elif defined(__m68k__)
        return "M68K";
        #else
        return "UNKNOWN";
        #endif
}

static void get_reason_and_solution(Exception::Type t) {
    switch (t) {
        case Exception::ModLoaderError:
            crash_reason = "Broken mod";
            crash_possible_solution = "Fix mode manualy or ask developer to fix it or delete mod";
            break;
        case Exception::NoFileError:
            crash_reason = "Broken installation";
            crash_possible_solution = "Try reinstall game";
            break;
        case Exception::ShaderError:
            crash_reason = "Broken Shader";
            crash_possible_solution = "Try reinstall game";
            break;
        case Exception::RGSSError:
            crash_reason = "Internal Error";
            crash_possible_solution = "Try reinstall game or disable some mods";
            break;
        case Exception::RUBYError:
            crash_reason = "Internal Error";
            crash_possible_solution = "Try reinstall game or disable some mods";
            break;
        case Exception::IOError:
            crash_reason = "Broken installation";
            crash_possible_solution = "Try reinstall game";
            break;
        case Exception::TypeError:
            crash_reason = "Internal Error";
            crash_possible_solution = "Try reinstall game or disable some mods";
            break;
        case Exception::ArgumentError:
            crash_reason = "Internal error";
            crash_possible_solution = "Try reinstall game or disable some mods";
            break;
        case Exception::PHYSFSError:
            crash_reason = "Internal error";
            crash_possible_solution = "Maybe you tryed load corrupted mod via modloader, try delete it";
            break;
        case Exception::SDLError:
            crash_reason = "Internal error";
            crash_possible_solution = "Internal Engine Error, maybe something wrong with your device or operating system";
            break;
        case Exception::MKXPError:
            crash_reason = "Internal error";
            crash_possible_solution = "Try reinstall game or disable some mods";
            break;
        default:
            break;
    }
}

void crash(Exception::Type t, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    unsigned int len = (unsigned int)SDL_vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (len >= sizeof(crash_message)) len = sizeof(crash_message) - 1;

    SDL_vsnprintf(crash_message, sizeof(crash_message), fmt, args);
    va_end(args);
	get_reason_and_solution(t);
    show_crash_sceen = true;
	//Protect against segfaults
    if(is_ruby_initialized){
    	ruby_stop(-1);
    }
}

void crash_screen(SDL_Window* win){
	// Skip Crash screen if failed initialize
	static bool skip_crash_screen = false;
	//creating render
	SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
	if (ren == nullptr) {
		WarnMsg("Failed to create renderer, please check if your device is too strong to run game or report bug.Error Message: ", SDL_GetError());
		skip_crash_screen = true;
	}
	SDL_Surface* crash_img = IMG_Load_IO(SDL_IOFromConstMem(assets_crash_png, assets_crash_png_len), true);
	if (crash_img == nullptr) {
		WarnMsg("Failed to create surface, please check if your device is too strong to run game or report bug.Error Message: ", SDL_GetError());
		skip_crash_screen = true;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, crash_img);
	SDL_DestroySurface(crash_img);
	if (tex == nullptr) {
		WarnMsg("Failed to create texture, please check if your device is too strong to run game or report bug.Error Message: ", SDL_GetError());
		skip_crash_screen = true;
	}else{
		SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
	}
	//creating file and timestamp
	ofstream o;
	time_t timestamp;
	time(&timestamp);
	string timeeeeee(ctime(&timestamp));
	if (!timeeeeee.empty() && timeeeeee.back() == '\n') {
	    timeeeeee.pop_back();
	}
	string file = "crash " + timeeeeee + ".txt";
	std::erase(file, ':');
	o.open(file);
	//collecting info and writing to crashdump
	if (o.is_open()){
		#ifdef DEVBUILD
			o << "EXPEREMENTAL BUILD\n";
		#endif
		o << "VERSION: " << VERSION_STRING << endl;
		o << "REASON: " << crash_message << endl;
		o << "Compiler info: " << COMPILER_NAME << " " << COMPILER_VER << endl << endl;
		o << "[BOOST stacktrace()]\n\n";
		o << boost::stacktrace::stacktrace() << endl;

		o << "[LOG BUFFER]\n\n";
		for (const auto& s : logs) {
		    o << s << endl;
		}

		o << "\n[AUDIO]\n";
		o << "Driver used: " << SDL_GetCurrentAudioDriver() << endl;

		o << "\n[VIDEO]\n";
		o << "Driver used: " << SDL_GetCurrentVideoDriver() << endl;

		o << "\n[VERSIONS OF LIBS]\n";
		const static int sdlcompiled = SDL_VERSION;
		const static int sdllinked = SDL_GetVersion();
		o << "SDL(compiled) version: " << SDL_VERSIONNUM_MAJOR(sdlcompiled) << "." << SDL_VERSIONNUM_MINOR(sdlcompiled) << "." << SDL_VERSIONNUM_MICRO(sdlcompiled) << endl;
		o << "SDL_image(compiled) version: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_MICRO_VERSION << endl;
		o << "SDL_TTF(compiled) version: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_MICRO_VERSION << endl;
		o << "SDL_mixer version: " << MIX_Version() << endl;
		o << "Ruby version: " << RUBY_API_VERSION_CODE << endl;
		o << "ZLib version: " << ZLIB_VERSION << endl;
		o << "Boost versino: " << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100 << endl;
		o << "Pixman version: " << PIXMAN_VERSION_STRING << endl;

		o << "\n[Platform specific]\n";
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
		#elif mkxp_android
			o << "Android API version: " << android_get_device_api_level() << endl;
			if(!is_privacy_crashdump_enabled){
				o << "Is ChromeBook? " << SDL_IsChromebook() << endl;
				o << "Is Phone? " << SDL_IsPhone() << endl;
				o << "Is Tablet? " << SDL_IsTablet() << endl;
				o << "Is Samsung DeX? " << SDL_IsDeXMode() << endl;
				o << "Is TV? " << SDL_IsTV() << endl;
			}
		#elif web
			o << "Emscripten start address of the stack: " << emscripten_stack_get_base() << endl;
			o << "Emscripten end address of the stack: " << emscripten_stack_get_end() << endl;
			o << "Emscripten current stack pointer: " << emscripten_stack_get_current() << endl;
			o << "Emscripten number of free bytes left on stack: " << emscripten_stack_get_free() << endl;
		#elif psp
			o << "PSPdev MIPS Stack Trace: " << pspDebugGetStackTrace() << endl;
		#endif

		o << "\n[Hardware]\n";
		o << "number of logical CPU cores: " << SDL_GetNumLogicalCPUCores() << endl;
		o << "System RAM size: " << SDL_GetSystemRAM() << " MiB" << endl;
		if(!is_privacy_crashdump_enabled){
			o << "L1 cache size: " << SDL_GetCPUCacheLineSize() << endl;
		}
		o << "CPU: " << get_processor() << endl;

		o << "\n[OpenGL]\n";
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

	if(!skip_crash_screen){
		SDL_Event e;
		bool quit = false;

		int texW = 100, texH = 100;
		int winW = 0, winH = 0;
		SDL_GetWindowSize(win, &winW, &winH);
		SDL_FRect dst{
	    	(float)((winW - 100) / 2),
	    	(float)((winH - 100) / 2),
	    	(float)100,
	    	(float)100};

		while (!quit) {
	    	while (SDL_PollEvent(&e)) {
	        	if (e.type == SDL_EVENT_QUIT) quit = true;
	    	}

			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	    	SDL_RenderClear(ren);
	    	SDL_RenderTexture(ren, tex, NULL, &dst);
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	    	SDL_RenderDebugTextFormat(ren, 10, 10, "World machine crashed, crashdump created in game directory");
	    	SDL_RenderDebugTextFormat(ren, 10, 20, "%s", crash_message);
	    	SDL_RenderDebugTextFormat(ren, 10, 30, "Possible reason: %s", crash_reason);
	    	SDL_RenderDebugTextFormat(ren, 10, 40, "Possible solution: %s", crash_possible_solution);
	    	SDL_RenderDebugTextFormat(ren, 10, 50, "Path: %s%s", SDL_GetCurrentDirectory(), file.c_str());
	    	SDL_RenderDebugTextFormat(ren, 10, 60, "Crashdump privacy: %s", is_privacy_crashdump_enabled ? "enabled" : "disabled");
	    	SDL_RenderDebugTextFormat(ren, 10, 70, "If you are sure that the problem is not in your modifications,");
	    	SDL_RenderDebugTextFormat(ren, 10, 80, "your hands or in your device - please report the bug to the developers");
	    	SDL_RenderPresent(ren);
	    	SDL_Delay(32);
		}
		SDL_DestroyTexture(tex);
		SDL_DestroyRenderer(ren);
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

    Debug() << "[ERRORMSG]" << buf;
    SDL_snprintf(crash_message, sizeof(crash_message), "%s", buf);
    show_crash_sceen = true;
    SDL_free(buf);
}

void ErrorMsg(Exception::Type t, const char *fmt, ...) {
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

    Debug() << "[ERRORMSG]" << buf;
    SDL_snprintf(crash_message, sizeof(crash_message), "%s", buf);
    get_reason_and_solution(t);
    show_crash_sceen = true;
    SDL_free(buf);
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
    SDL_free(buf);
}
