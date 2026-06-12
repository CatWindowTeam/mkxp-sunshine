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

namespace fs = std::filesystem;

// Get directory for storing cached builds
//TODO: support for other OS and platforms
std::string getCacheDir(){
#ifdef _WIN32
	return SDL_getenv("Temp");
#elif defined(__linux__)
	return std::string(SDL_getenv("HOME")) + "/.cache";
#elif __APPLE__
	return "~/Library/Caches";
#else
	return "idk";
#endif
}

//helper
bool ensure_parent_dir(const std::filesystem::path& p){
	if (p.has_parent_path()){
		std::error_code ec;
		std::filesystem::create_directories(p.parent_path(), ec);
		return !ec;
	}    
	return true;
}

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
        crash(Exception::ModLoaderError, "Failed to load mod, filesystem error.");
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
        snprintf(hex, sizeof hex, "%02x", hash[i]);
        out += hex;
    }
    return out;
}

std::string ModLoader(Config conf){
		std::string path = conf.Modloader.ModsDirPath;
		if (!fs::exists(path) || !std::filesystem::is_directory(path)) {
			Debug() << "[MODLOADER] Mods directory not found, skip.";
			return "";
		}
		
		if (fs::is_empty(path)){
			Debug() << "[MODLOADER] Mods directory empty, skip.";
			return "";
		}

		//buildID - unique ID of a certain combination of mods
		std::string buildID = "";
		std::string buildID_tmp = ""; 
		std::vector<std::string> mod_list = {};
		try{
			//1.check if any zip(mod) file, 2. calculate sha256 hash of zip(mod) files
			for (const auto &entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied)) {
			    std::error_code ec;
			    auto p = entry.path();
			    if (!std::filesystem::is_regular_file(p, ec) || ec) continue;
			
			    auto ext = p.extension().string();
			    if (ext != ".zip") continue;
			
			    std::string full = p.string();
			    Debug() << "[MODLOADER] " << full;
			    mod_list.push_back(full);
			    mods_count++;
			
			    auto h = sha256_file(full);
			    buildID_tmp.append(h);
			}
				
			buildID = sha512(buildID_tmp);
			Debug() << "[MODLOADER] BuildID: " << buildID;
			std::string path3 = getCacheDir() + "/sunshine-" + buildID;
			int err = 0;
			modloader_is_enabled = true;
			if(!std::filesystem::exists(path3)){
				std::error_code ec;
				fs::create_directory(path3, ec);
				if (ec) {
				    crash(Exception::ModLoaderError, "Failed to create destination: %s", ec.message().c_str());
				}

				fs::copy(conf.gameFolder, path3, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
				if (ec) {
				    crash(Exception::ModLoaderError, "Copy error: %s", ec.message().c_str());
				}

				//Extracting zipsodpsofspo idk
				for (size_t i = 0; i < mod_list.size(); ++i){
					zip_t* za = zip_open(mod_list[i].c_str(), ZIP_RDONLY, &err);
					if (!za){
						crash(Exception::ModLoaderError, "Failed to open zip");
					}
					 
					zip_int64_t n = zip_get_num_entries(za, 0);
					for (zip_uint64_t i2 = 0; i2 < static_cast<zip_uint64_t>(n); ++i2) {
						zip_stat_t st;
					    if (zip_stat_index(za, i2, 0, &st) != 0) {
							crash(Exception::ModLoaderError, "zip_stat_index failed");
					    }
					    
					    std::string name = st.name;
					    std::filesystem::path target = path3 / std::filesystem::path(name);
					 
					    // If entry name ends with '/', treat as directory
					    if (!name.empty() && name.back() == '/') {
					    	std::error_code ec;
					        std::filesystem::create_directories(target, ec);
					        if (ec) crash(Exception::MEOW, "Failed to create dir");
					        continue;
					    }
					 
					    if (!ensure_parent_dir(target)){
					    	crash(Exception::ModLoaderError, "Failed to create parent dirs");
						}
					 
					    zip_file_t* zf = zip_fopen_index(za, i2, 0);
					    if (!zf) {
					    	crash(Exception::ModLoaderError, "zip_fopen_index failed");
					 	}
					    std::ofstream out(target, std::ios::binary);
					    if (!out) {
					    	crash(Exception::ModLoaderError, "Failed to open output file %s", target.c_str());
					        zip_fclose(zf);
					    }
					 
					    const zip_uint64_t bufsize = 4096;
					    std::vector<char> buf(bufsize);
					    zip_int64_t bytes_read;
					    zip_uint64_t remaining = st.size;
					    while (remaining > 0) {
					    	zip_uint64_t to_read = std::min<zip_uint64_t>(bufsize, remaining);
					        bytes_read = zip_fread(zf, buf.data(), to_read);
					        if (bytes_read < 0) {
					        	crash(Exception::ModLoaderError, "zip_fread error for %s", name.c_str());
					            break;
					        }
					        out.write(buf.data(), bytes_read);
					        remaining -= bytes_read;
					    }
					    zip_fclose(zf);
					} 
					zip_close(za);
				}
				return path3;
			}else{
				return path3;
			}
		}catch(const std::exception& e){
			crash(Exception::ModLoaderError, "Something is wrong, Exception: %s ", e.what());
		}
}
