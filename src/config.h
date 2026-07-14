/*
** config.h
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is SDL_free software: you can redistribute it and/or modify
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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <set>

struct Config{
	bool debugMode;
	bool screenMode;
	bool printFPS;
	bool fullscreen;
	bool fixedAspectRatio;
	bool Windows_AllocConsole;
	bool smoothScaling;
	bool vsync;
	bool pancakes;
	int defScreenW;
	int defScreenH;
	std::string windowTitle;
	
	int fixedFramerate;
	bool frameSkip;
	bool syncToRefreshrate;

	bool solidFonts;

	bool subImageFix;
	bool enableBlitting;
	int maxTextureSize;

	std::string gameFolder;
	int AspectPreset;
	std::string AspectPresetRubyConst;
	bool allowSymlinks;
	bool pathCache;

	std::string iconPath;

	struct{
		int sourceCount;
	} SE;
	
	struct{
			std::string ModsDirPath;
			bool use_default_save_path;
	} Modloader;

	bool useScriptNames;

	std::string customScript;
	std::set<std::string> preloadScripts;
	std::vector<std::string> rtps;

	std::vector<std::string> fontSubs;

	std::vector<std::string> rubyLoadpaths;

	/* Editor flags */
	struct {
		bool debug;
		bool battleTest;
	} editor;

	/* Game INI contents */
	struct {
		std::string scripts;
		std::string title;
	} game;

	/* Internal */
	std::string customDataPath;
	std::string commonDataPath;

	Config();

	void read(int argc, char *argv[]);
};

// 0 - 3:2      - 720:480
// 1 - 4:3      - 640x480 (default)
// 2 - 16:9     - 960x540
// 3 - 16:10    - 960x600
// 4 - 16:9 HD  - 1280x720 X
// 5 - 16:9 FHD - 1920x1080 X

//  Value X    Y    Ruby const
#define RESOLUTIONS_LIST(X) \
	X(0,  720,  480,  _3_2) \
	X(1,  640,  480,  _4_3) \
	X(2,  960,  540,  _16_9) \
	X(3,  960,  600,  _16_10) \
	X(4,  1280, 720,  _16_9_hd) \
	X(5,  1920, 1080, _16_9_fhd)

#endif // CONFIG_H
