// Error handling and reporting
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_platform.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_stdinc.h>
#include "meow.h"
#include "util.h"
#include "exception.h"
#include "config.h"
#include "debugwriter.h"
#include "define.h"
#include "binding.h"
#include <physfs.h>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#ifdef mkxp_android
	#include <android/api-level.h>
#endif

#include "crash.png.xxd"

void crash(Exception::Type type, const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(crash_message, sizeof(crash_message), fmt, args);
    va_end(args);
    show_crash_screen = true;
    scriptBinding->terminate();
}

void set_crash_info(const char *fmt, ...){
	char message[1024];

    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(message, sizeof(message), fmt, args);
    
    size_t len = clamp(SDL_strlen(message), (size_t)0, sizeof(crash_message) - 1);
    SDL_memset(crash_message, 0, sizeof(crash_message));
    if(len != 0){
    	SDL_memcpy(crash_message, message, len);
    }else{
    	SDL_memcpy(crash_message, "(empty error message)", 22);
    }
    
    va_end(args);
    show_crash_screen = true;
}

// Here we prepare information that we display on crash screen and write in crashdump later
static std::vector<std::string> prepare_crash_info(){
	std::vector<std::string> c;
	c.emplace_back("If you're sure that the problem is not with");
	c.emplace_back("your device, nor with your modifications, nor your ham-fisted setup, please");
	c.emplace_back("report the bug to the developers.");
	c.emplace_back(std::string{"VERSION: "} + VERSION_STRING);
	c.emplace_back(std::string{"MSG: "} + crash_message);
	c.emplace_back("");
	c.emplace_back("[LOGS]");
	c.insert(c.end(), logs.begin(), logs.end());
	c.emplace_back("");
	c.emplace_back(std::string{"Audio driver: "} + SDL_GetCurrentAudioDriver());
	c.emplace_back(std::string{"Video Driver: "} + SDL_GetCurrentVideoDriver());
	c.emplace_back(std::string{"Detected Platform: "} + SDL_GetPlatform());
	c.emplace_back(std::string{"Last PhysFS error: "} + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	#ifdef mkxp_android
		c.emplace_back(std::string{"Android API: "} + std::to_string(android_get_device_api_level()));
	#endif
	return c;
}

void crash_screen(SDL_Window* win){
	SDL_ShowCursor();
	// Skip Crash screen if failed initialize
	static bool skip_crash_screen = false;
	//creating render, yes i deleted checks
	SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
	SDL_Surface* crash_img = IMG_Load_IO(SDL_IOFromConstMem(assets_crash_png, assets_crash_png_len), true);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, crash_img);
	SDL_DestroySurface(crash_img);

	static int w = 0;
	static unsigned int pager_start = 0;
	unsigned static int count = 20;
	SDL_GetCurrentRenderOutputSize(ren, NULL, &w);
	static int items_count = (((w / 10) * 10) / 10);
	static unsigned int pager_end = items_count;
	const static char* shit = "[S - save crash info]";
	//cd -- crashdump
	std::vector<std::string> cd = prepare_crash_info();
	if(!skip_crash_screen){
		SDL_Event e;
		bool quit = false;
		int texW = 100, texH = 100;
		//if something, use default
		int winW = 640, winH = 480;
		SDL_GetWindowSize(win, &winW, &winH);
		SDL_FRect dst{
	    	(float)((winW - 100) / 2),
	    	(float)((winH - 100) / 2),
			100.0f,
	    	100.0f
	    };

		while(!quit) {
	    	while(SDL_PollEvent(&e)) {
	        	if(e.type == SDL_EVENT_QUIT) quit = true;
				if(e.type == SDL_EVENT_KEY_UP){
					if(e.key.scancode == SDL_SCANCODE_DOWN) {
						if(pager_end < cd.size()){
							pager_start++;
							pager_end++;
						}
    				}else if(e.key.scancode == SDL_SCANCODE_UP) {
						if(pager_start != 0){
							pager_start--;
							pager_end--;
						}
					}else if(e.key.scancode == SDL_SCANCODE_S){
						std::ofstream file("crash.txt");
	    				for(const auto& l : cd){
							file << l << '\n';
						}
						shit = "[SAVED]";
					}else if(e.key.scancode == SDL_SCANCODE_Q){
						quit = true;
					}
				}
		    }
			count = 40;
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	    	SDL_RenderClear(ren);
			SDL_SetRenderScale(ren, 1.0f, 1.0f);
	    	SDL_RenderTexture(ren, tex, NULL, &dst);
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
			SDL_SetRenderScale(ren, 2.0f, 2.0f);
	    	SDL_RenderDebugText(ren, 5, 5, "World machine crashed :(");
			SDL_SetRenderScale(ren, 1.5f, 1.5f);
			SDL_RenderDebugText(ren, 5, 20, shit);
			SDL_SetRenderScale(ren, 1.0f, 1.0f);
			char index_str[32];
			for(int i = pager_start; i <= pager_end; ++i){
				if(i < cd.size()){
					count = count + 10;
					SDL_snprintf(index_str, sizeof(index_str), "%03d", i);
					SDL_RenderDebugTextFormat(ren, 10, count, "%s  %s", index_str, cd[i].c_str());
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
    Debug() << "[E] " << crash_message;
    show_crash_screen = true;
}

void ErrorMsg(Exception::Type t, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(crash_message, sizeof(crash_message), fmt, args);
    va_end(args);
    Debug() << "[E] " << crash_message;
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
    Debug() << "[W] " << buf;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", buf, NULL);
    SDL_free(buf);
}
