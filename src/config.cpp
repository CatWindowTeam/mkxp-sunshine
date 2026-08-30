/*
** config.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <physfs.h>

#include <fstream>
#include <SDL3/SDL_stdinc.h>
#include <cstdlib>

#include "debugwriter.h"
#include "util.h"
#include "sdl-util.h"
#include <SDL3/SDL_system.h>
namespace std{
	std::ostream& operator<<(std::ostream &os, const std::vector<std::string> &vec){
		for (auto item : vec){
			os << item << " ";
		}
		return os;
	}
}

static std::string prefPath(const char *org, const char *app){
	const char *path = PHYSFS_getPrefDir(org, app);

	if (!path)
		return std::string();

	return path;
}

template<typename T>
std::set<T> setFromVec(const std::vector<T> &vec){
	return std::set<T>(vec.begin(), vec.end());
}

typedef std::vector<std::string> StringVec;
namespace po = boost::program_options;

#define CONF_FILE "oneshot.conf"

Config::Config() {}

void Config::read(int argc, char *argv[]){
#define PO_DESC_ALL \
	PO_DESC(debugMode,                       bool,        false       ) \
	PO_DESC(fullscreen,                      bool,        false       ) \
	PO_DESC(fixedAspectRatio,                bool,        true        ) \
	PO_DESC(smoothScaling,                   bool,        false       ) \
	PO_DESC(defScreenW,                      int,         0           ) \
	PO_DESC(defScreenH,                      int,         0           ) \
	PO_DESC(windowTitle,                     std::string, ""          ) \
	PO_DESC(commonDataPath,                  std::string, ""          ) \
	PO_DESC(Modloader.ModsDirPath,           std::string, "mods"      ) \
	PO_DESC(Modloader.skip_modloader_screen, bool,        false       ) \
	PO_DESC(fixedFramerate,                  int,         0           ) \
	PO_DESC(frameSkip,                       bool,        true        ) \
	PO_DESC(syncToRefreshrate,               bool,        false       ) \
	PO_DESC(solidFonts,                      bool,        false       ) \
	PO_DESC(subImageFix,                     bool,        false       ) \
	PO_DESC(enableBlitting,                  bool,        true        ) \
	PO_DESC(maxTextureSize,                  int,         0           ) \
	PO_DESC(gameFolder,                      std::string, "."         ) \
	PO_DESC(allowSymlinks,                   bool,        false       ) \
	PO_DESC(iconPath,                        std::string, ""          ) \
	PO_DESC(wallpaperMode,                   std::string, "normal"    ) \
	PO_DESC(SE.sourceCount,                  int,         6           ) \
	PO_DESC(pathCache,                       bool,        true        ) \
	PO_DESC(Windows_AllocConsole,            bool,        false       ) \
	PO_DESC(pancakes,                        bool,        false       ) \
	PO_DESC(SecurityEngine,                  bool,        true        ) \
	PO_DESC(journal_address,                 std::string, "127.0.0.1" ) \
	PO_DESC(journal_port,                    int,         23821       )

// Not gonna take your shit boost
#define GUARD_ALL( SDL_exp ) try { SDL_exp } catch(...) {}
#define PO_DESC(key, type, def) (#key, po::value< type >()->default_value(def))

	po::options_description podesc;
	podesc.add_options()
	        PO_DESC_ALL
	        ("fontSub", po::value<StringVec>()->composing()->default_value(StringVec()))
	        ("rubyLoadpath", po::value<StringVec>()->composing()->default_value(StringVec()));

	po::variables_map vm;

	/* Parse command line options */
	try{
		po::parsed_options cmdPo = po::command_line_parser(argc, argv).options(podesc).run();
		po::store(cmdPo, vm);
	}catch (po::error &error){
		Debug() << "[config] " << error.what();
	}

	/* Parse configuration file */
	SDLRWStream confFile(CONF_FILE, "r");

	if (confFile){
		try{
			po::store(po::parse_config_file(confFile.stream(), podesc, true), vm);
			po::notify(vm);
		}
		catch (po::error &error){
			Debug() << CONF_FILE":" << error.what();
		}
	}

#undef PO_DESC
#define PO_DESC(key, type, def) GUARD_ALL( key = vm[#key].as< type >(); )

	PO_DESC_ALL;

	GUARD_ALL( fontSubs = vm["fontSub"].as<StringVec>(); );

	GUARD_ALL( rubyLoadpaths = vm["rubyLoadpath"].as<StringVec>(); );

#undef PO_DESC
#undef PO_DESC_ALL

	SE.sourceCount = clamp(SE.sourceCount, 1, 64);
	#if defined(__ANDROID__) && !defined(TERMUX)
	commonDataPath = prefPath(SDL_GetAndroidInternalStoragePath(), "/SunshineSaves");
	gameFolder = "";
	gameFolder.append(SDL_GetAndroidInternalStoragePath()).append("/Sunshine");
	#else
	commonDataPath = prefPath(".", "Sunshine");
	if(pancakes){
		commonDataPath = prefPath(".", "Sunshine_Pancakes");
	}
	#endif

	if(windowTitle == "")
		game.title = "OneShot: Sunshine";
	game.scripts = "Data/xScripts.rxdata";

	resolutionOverridden = defScreenW > 0 || defScreenH > 0;
	defScreenW = defScreenW <= 0 ? 640 : defScreenW;
	defScreenH = defScreenH <= 0 ? 480 : defScreenH;

#ifdef STEAM
	/* Override fullscreen config if Big Picture */
	if (const char *env = SDL_getenv("SteamTenfoot")){
		if (!SDL_strcmp(env, "1"))
			fullscreen = true;
	}
#endif
}

