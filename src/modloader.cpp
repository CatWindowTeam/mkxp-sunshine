#include "modloader.h"
#include "debugwriter.h"
#include "meow.h"
#include "config.h"

#include <filesystem>
#include <string>
#include <vector>
#include <system_error>
#include <physfs.h>

#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>

#include "the_modded_machine.png.xxd"

namespace fs = std::filesystem;

static bool stop_render = false;
const static std::size_t N = 46;

static void modloader_add_to_log(const std::string data){
	Debug() << "[MODLOADER] " << data;
	modloader_logs.push_back(data);
}

static int renderer_thread(void* data){
	SDL_Window* win = static_cast<SDL_Window*>(data);
	SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
	if (ren == nullptr) {return(0);}
	SDL_Surface* crash_img = IMG_Load_IO(SDL_IOFromConstMem(assets_the_modded_machine_png, assets_the_modded_machine_png_len), true);
	if (crash_img == nullptr){
		SDL_DestroyRenderer(ren);
		return(0);
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, crash_img);
	SDL_DestroySurface(crash_img);
	if (tex == nullptr) {
		SDL_DestroyRenderer(ren);
		return(0);
	}

	int index_on_screen = 10;
	while(!stop_render){
		index_on_screen = 10;
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	    SDL_RenderClear(ren);
	    SDL_RenderTexture(ren, tex, NULL, NULL);
		SDL_SetRenderDrawColor(ren, 150, 100, 255, 255);
		std::size_t size = modloader_logs.size();
		std::size_t n = std::min<std::size_t>(static_cast<std::size_t>(N), size);
		std::size_t start = size > n ? (size - n) : 0;
		for (std::size_t i = start; i < size; ++i) {
		    SDL_RenderDebugTextFormat(ren, 10, index_on_screen, "%s", modloader_logs[i].c_str());
		    index_on_screen += 10;
		}
	    SDL_RenderPresent(ren);
	    SDL_Delay(16);
	}
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
    return(0);
}

void ModLoader(Config conf, SDL_Window* win){
    std::string path = "mods";
    if (!fs::exists(path) || !fs::is_directory(path)) {
        Debug() << "[MODLOADER] Mods directory not found, skip.";
        return;
    }else if (fs::is_empty(path)) {
        Debug() << "[MODLOADER] Mods directory empty, skip.";
        return;
    }
    
	SDL_Thread* render_thread_pointer = SDL_CreateThread(renderer_thread, "ModRenderer", win);
    std::vector<std::string> mod_list = {};
    try {
        // 1.check if any zip(mod) file, 2. calculate sha256 hash of zip(mod) files 3.mount mod via PhysFS
        for (const auto &entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            std::error_code ec;
            auto p = entry.path();
            if (!fs::is_regular_file(p, ec) || ec) continue;

            auto ext = p.extension().string();
            std::string full = p.string();

            if (ext == ".zip") {
                int ok = PHYSFS_mount(full.c_str(), "/mod-storage", 0);
                mod_list.push_back(full);
                modloader_add_to_log("Added mod " + full);
                mods_count++;
            } else if (ext == ".rb") {
                preloadScripts.insert(full);
                modloader_add_to_log("Added script " + full);
                preloaded_script_count++;
            }
        }
        sleep(1);
        stop_render = true;
        SDL_WaitThread(render_thread_pointer, NULL);
        modloader_is_enabled = true;
    } catch (const std::exception& e) {
        crash(Exception::ModLoaderError, "Something is wrong, Exception: %s ", e.what());
    }
}
