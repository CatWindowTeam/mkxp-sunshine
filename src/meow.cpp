#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_platform.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_cpuinfo.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_mouse.h>
#include "meow.h"
#include "exception.h"
#include "config.h"
#include "gl-fun.h"
#include "debugwriter.h"
#include "define.h"
#include "sharedstate.h"
#include <SDL3/SDL_stdinc.h>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ruby/internal/interpreter.h>
#undef vsnprintf
#undef snprintf
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#endif
#include <boost/stacktrace.hpp>
#include <physfs.h>
#include "sunshine.h"
#ifdef android
	#include <android/api-level.h>
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
            crash_possible_solution = "Try reinstall game or mods if you load some mods.";
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

void crash(Exception::Type type, const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(crash_message, sizeof(crash_message), fmt, args);
    va_end(args);

    get_reason_and_solution(type);
    show_crash_screen = true;

    if (is_ruby_initialized) {
        ruby_stop(-1);
    }
}


// Here we prepare information that we display on crash screen and weite in crashdump later
static std::vector<std::string> prepare_crash_info(){
	boost::stacktrace::stacktrace trace;
	std::vector<std::string> c = {};
	c.emplace_back("If you are sure that the problem is not with");
	c.emplace_back("your device, not with your modifications, or in your hands, please");
	c.emplace_back("report the bug to the developers");
	c.emplace_back(std::string{"VERSION: "} + VERSION_STRING);
	c.emplace_back(std::string{"Possible reason: "} + crash_reason);
	c.emplace_back(std::string{"Possible solution: "} + crash_possible_solution);
	c.emplace_back(std::string{"MSG: "} + crash_message);
	c.emplace_back(std::string{"COMPILER: "} + COMPILER_NAME + std::string{" "} + COMPILER_VER);
	c.emplace_back("");
	c.emplace_back("[LOGS]");
	c.insert(c.end(), logs.begin(), logs.end());
	c.emplace_back("[LOGS END]");
	c.emplace_back("");
	c.emplace_back(std::string{"Audio driver: "} + SDL_GetCurrentAudioDriver());
	c.emplace_back(std::string{"Video Driver: "} + SDL_GetCurrentVideoDriver());
	c.emplace_back(std::string{"Detected Platform: "} + SDL_GetPlatform());
	c.emplace_back(std::string{"Last PhysFS error: "} + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	c.emplace_back(std::string{"Last SDL Error on current thread: "} + SDL_GetError());
	#ifdef android
		c.emplace_back(std::string{"Android API: "} + std::to_string(android_get_device_api_level()));
	#endif
	c.emplace_back("");
	//maybe we should use C++ stacktrace?
	c.emplace_back("[STACK TRACE]");
	for (const auto& frame : trace) {
    		c.push_back(boost::stacktrace::to_string(frame));
	}
	c.emplace_back("");
	return c;
}


void crash_screen(SDL_Window* win){
	SDL_ShowCursor();
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


	static int w = 0;
	static unsigned int pager_start = 0;
	unsigned static int count = 20;
	SDL_GetCurrentRenderOutputSize(ren, NULL, &w);
	static int items_count = (((w / 10) * 10) / 10);
	static unsigned int pager_end = items_count;
	//cd -- crashdump
	std::vector<std::string> cd = prepare_crash_info();
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
				if (e.type == SDL_EVENT_KEY_UP){
					if (e.key.scancode == SDL_SCANCODE_DOWN) {
						if(pager_end < cd.size()){
							 pager_start++;
							 pager_end++;
						}
    					} else if (e.key.scancode == SDL_SCANCODE_UP) {
						if(pager_start != 0){
                                                         pager_start--;
							 pager_end--;
                                                }
				        }
				}
	    		}

			count = 20;
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	    		SDL_RenderClear(ren);
			SDL_SetRenderScale(ren, 1.0f, 1.0f);
	    		SDL_RenderTexture(ren, tex, NULL, &dst);
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
			SDL_SetRenderScale(ren, 2.0f, 2.0f);
	    		SDL_RenderDebugTextFormat(ren, 5, 5, "World machine crashed! :(");
			SDL_SetRenderScale(ren, 1.0f, 1.0f);
			for (int i = pager_start; i <= pager_end; ++i){
				if(i < (cd.size() - 1)){
					count = count + 10;
					SDL_RenderDebugTextFormat(ren, 10, count, "%s| %s", std::format("{:03}", i).c_str(), cd[i].c_str());
				}
			}
	    		SDL_RenderPresent(ren);
	    		SDL_Delay(96);
		}
		SDL_DestroyTexture(tex);
		SDL_DestroyRenderer(ren);
	}
}

void ErrorMsg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(crash_message, sizeof(crash_message), fmt, args);
    va_end(args);
    show_crash_screen = true;
}

void ErrorMsg(Exception::Type t, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(crash_message, sizeof(crash_message), fmt, args);
    va_end(args);
    get_reason_and_solution(t);
    show_crash_screen = true;
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
