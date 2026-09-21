//TODO: autogen of descs
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>
#include "config.h"
#include "meow.h"
#include "util.h"
#include "define.h"

static std::string prefPath(const char *org, const char *app){
	const char *path = SDL_GetPrefPath(org, app);
	if (!path)
		return std::string();
	return path;
}

Config conf;
void Config::read(int argc, char* argv[]) {
	#if defined(mkxp_android) && !defined(TERMUX)
		commonDataPath = prefPath(SDL_GetAndroidInternalStoragePath(), "/SunshineSaves");
		gameFolder = "";
		gameFolder.append(SDL_GetAndroidInternalStoragePath()).append("/Sunshine");
	#else
		commonDataPath = prefPath("CatWindowTeam", "/Sunshine");
	#endif
    CLI::App a{"Engine of Sunshine"};
    argv = a.ensure_utf8(argv);
    a.add_option("-d,--debug", debugMode, "Enable reset on F12 press and other debug stuff");
    a.add_option("-f,--fullscreen", fullscreen, "Start game in fullscreen mode but Game Scripts"
    											"can just enforce window mode so this exists only for debuging");
    a.add_option("--fixedAspectRatio", fixedAspectRatio, "Preserve game screen aspect ratio, as opposed to stretch-to-fill");
    a.add_option("--Windows.Alloc_console", Windows_AllocConsole, "Create new console window with debug output, windows only");
	a.add_option("--width", defScreenW, "Window width");
	a.add_option("--height", defScreenH, "Window height");
	a.add_option("--windowTitle", windowTitle, "Window Title");
	a.add_option("--fixedFramerate", fixedFramerate, "Enforce a static frame rate");
	a.add_option("--frameSkip", frameSkip, "Skip (don't draw) frames when behind. Can be overriden by game scripts");
	a.add_option("--syncToRefreshrate", syncToRefreshrate, "Use a fixed framerate that is approx. equal to the "
																"native screen refresh rate. This is different from"
																" 'fixedFramerate' because the actual frame rate is"
																"reported back to the game, ensuring correct timers."
																"If the screen refresh rate cannot be determined, this option is force-disabled");
	a.add_option("--solidFonts", solidFonts, "Don't use alpha blending when rendering text");
	a.add_option("--subImageFix", subImageFix, "Work around buggy graphics drivers which don't properly synchronize texture access," 
													  "most apparent when text doesn't show up or the map tileset doesn't render at all");
	a.add_option("--enableBlitting", enableBlitting, "framebuffer blitting if the driver is capable of it. Some drivers carry buggy"
														   "implementations of this functionality, so disabling it can be used as a workaround");
	a.add_option("--maxTextureSize", maxTextureSize, "Limit the maximum size (width, height) of most textures mkxp will create"
															"(exceptions are rendering backbuffers and similar). If set to 0, the hardware maximum is used."
															"This is useful for recording traces that can be played back on machines with lower specs.");
	a.add_option("--gameFolder", gameFolder, "Game Folder path");
	a.add_option("--allowSymlinks", allowSymlinks, "Allow symlinks for game assets to be followed");
	a.add_option("--pathCache", pathCache, "Index all accesible assets via their lower case path (emulates windows case insensitivity)");
	a.add_option("--JournalAddress", journal_address, "journal_address");
	a.add_option("--JournalPort", journal_port, "journal_port");
	a.add_option("--Modloader.ModsDirPath", Modloader.ModsDirPath, "Debug Mode");
	a.add_option("--Modloader.skip_modloader_screen", Modloader.skip_modloader_screen, "Skip Modloader screen");
	a.add_option("--game.scripts", game.scripts, "Scripts File");
	a.add_option<std::vector<std::string>>("--fS,--fontSubs", fontSubs, "Font substitutions allow drop-in replacements of fonts"
																		" to be used without changing the RGSS scripts,"
																		" eg. providing 'Open Sans' when the game thinkgs it's"
																		" using 'Arial'. Font family to be substituted and"
																		" replacement family are separated by one sole '>'."
																		"Be careful not to include any spaces."
																		"This is not connected to the built-in font, which is "
																		"always used when a non-existing font family is requested by RGSS.");
    a.set_config("--config", "sunshine.conf", "Config file", false);
    try{
        a.parse(argc, argv);
    }catch (const CLI::ParseError &e) {
    	if(e.get_exit_code() == 0){
    		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Help", a.help().c_str(), NULL);
    	}else{
    		WarnMsg("Failed to parse config or command line arguments! Error: %s", e.what());
    		std::exit(e.get_exit_code());	
    	}
    }
    resolutionOverridden = defScreenW != 640 || defScreenH != 480;
    #ifdef STEAM
    	/* Override fullscreen config if Big Picture */
    	if (const char *env = SDL_getenv("SteamTenfoot")){
    		if (!SDL_strcmp(env, "1"))
    			fullscreen = true;
    	}
    #endif
}
