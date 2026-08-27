/*
** config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <set>

struct Config{
	bool debugMode;
	bool fullscreen;
	bool fixedAspectRatio;
	bool resolutionOverridden;
	bool Windows_AllocConsole;
	bool smoothScaling;
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
	bool allowSymlinks;
	bool pathCache;
	bool SecurityEngine;

	std::string iconPath;

	std::string wallpaperMode;
	std::string journal_address;
	int journal_port;

	struct{
		int sourceCount;
	} SE;

	struct{
		std::string ModsDirPath;
		bool skip_modloader_screen;
	} Modloader;

	bool useScriptNames;

	std::string customScript;
	std::vector<std::string> rtps;
	std::vector<std::string> fontSubs;
	std::vector<std::string> rubyLoadpaths;
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

#endif // CONFIG_H
