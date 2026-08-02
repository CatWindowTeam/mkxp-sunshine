//https://terminalroot.com/how-to-generate-sha256-hash-with-cpp-and-openssl/
//https://stackoverflow.com/questions/15347123/how-to-construct-a-stdstring-from-a-stdvectorstring
//https://stackoverflow.com/questions/54260184/how-to-sha256-hash-a-text-file-in-chunks-with-openssl-sha-h
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
#include <openssl/sha.h>
#include <SDL3/SDL_stdinc.h>
#include <zip.h>
#include <locale>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <system_error>
#include <SDL3/SDL_system.h>
#include <physfs.h>
namespace fs = std::filesystem;

std::string sha512(const std::string str){
  unsigned char hash[SHA512_DIGEST_LENGTH];

  SHA512_CTX sha512;
  SHA512_Init(&sha512);
  SHA512_Update(&sha512, str.c_str(), str.size());
  SHA512_Final(hash, &sha512);

  std::stringstream ss;

  for(int i = 0; i < SHA512_DIGEST_LENGTH; i++){
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( hash[i] );
  }
  return ss.str();
}

std::string sha256_file(const std::string &fn) {
    FILE *file = fopen(fn.c_str(), "rb");
    if (!file) {
        crash(Exception::IOError, true, "Failed to load mod, filesystem error.");
    }

    unsigned char buf[1024];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    size_t len;
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    while ((len = fread(buf, 1, sizeof buf, file)) != 0){
        SHA256_Update(&ctx, buf, len);
    }
    fclose(file);
    SHA256_Final(hash, &ctx);

    std::string out;
    out.reserve(SHA256_DIGEST_LENGTH * 2);
    char hex[3] = {0};
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        SDL_snprintf(hex, sizeof hex, "%02x", hash[i]);
        out += hex;
    }
    return out;
}

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

		//buildID - unique ID of a certain combination of mods
		std::string buildID = "";
		std::string buildID_tmp = ""; 
		std::vector<std::string> mod_list = {};
		try{
			//1.check if any zip(mod) file, 2. calculate sha256 hash of zip(mod) files 3.mount mod via PhysFS
 			for (const auto &entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied)) {
			    std::error_code ec;
			    auto p = entry.path();
			    if (!fs::is_regular_file(p, ec) || ec) continue;
			    auto ext = p.extension().string();
			    if (ext != ".zip") continue;
			    std::string full = p.string();
			    int ok = PHYSFS_mount(full.c_str(), "/mod-storage", 0);
			    if (!ok) {
			      crash(Exception::ModLoaderError, false, "PhysFS_mount failed: %s", PHYSFS_getLastError());
			    }
			    Debug() << "[MODLOADER] " << full;
			    mod_list.push_back(full);
			    mods_count++;
			
			    auto h = sha256_file(full);
			    buildID_tmp.append(h);
			}
				
			buildID = sha512(buildID_tmp);
			Debug() << "[MODLOADER] BuildID: " << buildID;
			modloader_is_enabled = true;			
		}catch(const std::exception& e){
			crash(Exception::ModLoaderError, true, "Something is wrong, Exception: %s ", e.what());
		}
}
