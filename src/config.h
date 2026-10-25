#pragma once
#include <string>
#include <vector>
#include "define.h"

struct Config {
    bool debugMode = false;
    bool fullscreen = false;
    bool fixedAspectRatio = true;
    bool smoothScaling = false;

    #ifdef vita
		resolutionOverridden = true;
		defScreenW = 960;
		defScreenH = 544;
	#else
		bool resolutionOverridden = false;
    	int defScreenW = 640;
    	int defScreenH = 480;
    #endif
    const char* windowTitle = "Oneshot: Sunshine";

    bool syncToRefreshrate = false;
    std::string renderer = "auto";
    bool renderStats = false;
    bool subImageFix = false;
    bool enableBlitting = true;
    bool allowSymlinks = false;
    bool pathCache = true;
    std::string journal_address = "127.0.0.1";
    int journal_port = 23821;

    struct {
        std::string ModsDirPath = "mods";
        bool skip_modloader_screen = false;
    } Modloader;

    bool useScriptNames = false;

    std::vector<std::string> fontSubs;

    struct {
        std::string scripts = "Data/xScripts.rxdata";
    } game;
    
	#ifdef vita
    	std::string customDataPath;
    	std::string commonDataPath = "ux0:/data/Sunshine";
    	std::string gameFolder = "ux0:/data/Sunshine";
    #else
    	std::string customDataPath;
    	std::string commonDataPath;
    	std::string gameFolder = ".";	
    #endif
    void read(int argc, char* argv[]);
};

extern Config conf;
