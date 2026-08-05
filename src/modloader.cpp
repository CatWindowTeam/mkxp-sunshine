//https://stackoverflow.com/questions/15347123/how-to-construct-a-stdstring-from-a-stdvectorstring
//https://stackoverflow.com/questions/1673445/how-to-convert-unsigned-char-to-stdstring-in-c
//https://www.geeksforgeeks.org/cpp/cpp-program-to-read-and-print-all-files-from-a-zip-file/
#include "modloader.h"
#include "debugwriter.h"
#include "meow.h"
#include "config.h"
#include <cctype>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <SDL3/SDL_stdinc.h>
#include <locale>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <system_error>
#include <SDL3/SDL_system.h>
#include <physfs.h>
namespace fs = std::filesystem;

void ModLoader(){
		std::string path = "mods";
		if (!fs::exists(path) || !fs::is_directory(path)) {
			Debug() << "[MODLOADER] Mods directory not found, skip.";
			return;
		}
		
		if (fs::is_empty(path)){
			Debug() << "[MODLOADER] Mods directory empty, skip.";
			return;
		}
		std::vector<std::string> mod_list = {};
		try{
			//1.check if any zip(mod) file, 2. calculate sha256 hash of zip(mod) files 3.mount mod via PhysFS
 			for (const auto &entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
			    std::error_code ec;
			    auto p = entry.path();
			    if (!fs::is_regular_file(p, ec) || ec) continue;
			    auto ext = p.extension().string();
			    std::string full = p.string();
			    if (ext == ".zip"){
					int ok = PHYSFS_mount(full.c_str(), "/mod-storage", 0);
			    	if (!ok) {
			    		crash(Exception::ModLoaderError, false, "PhysFS_mount failed: %s", PHYSFS_getLastError());
			    	}
			    	Debug() << "[MODLOADER] " << full;
			    	mod_list.push_back(full);
			    	mods_count++;		
			    }else if(ext == ".rb"){
			    	preloadScripts.insert(full);
			    	Debug() << "[MODLOADER] Loading Preload Script: " << full;
			    	preloaded_script_count++;
			    }
			}
			modloader_is_enabled = true;			
		}catch(const std::exception& e){
			crash(Exception::ModLoaderError, true, "Something is wrong, Exception: %s ", e.what());
		}
}
