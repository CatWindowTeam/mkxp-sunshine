/*
** main.cpp
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
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_stdinc.h>
#include <physfs.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <direct.h>
#define _chdir chdir
#else
#include <unistd.h>
#endif
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <boost/chrono.hpp>

#include "sharedstate.h"
#include "eventthread.h"
#include "gl-debug.h"
#include "debugwriter.h"
#include "exception.h"
#include "gl-fun.h"
#include "i18n.h"

#include "sunshine.h"
#include "modloader.h"

#include "define.h"
#include "meow.h"

#include "binding.h"

#include "icon.png.xxd"
#ifdef STEAM
	#include "steamshim/steamshim_child.h"
#else
	#include "gamecontrollerdb.txt.xxd"
#endif

#ifndef VERSION_STRING
	#define VERSION_STRING ">w<"
#endif

static void rgssThreadError(RGSSThreadData *rtData, const std::string &msg){
	rtData->rgssErrorMsg = msg;
	rtData->ethread->requestTerminate();
	rtData->rqTermAck.set();
}

int rgssThreadFun(void *userdata){
	RGSSThreadData *threadData = static_cast<RGSSThreadData*>(userdata);
	const Config &conf = threadData->config;
	SDL_Window *win = threadData->window;
	static char msg[512];
	SDL_GLContext glCtx;

	/* Setup GL context */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifndef NDEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	glCtx = SDL_GL_CreateContext(win);

	if (!glCtx){
		rgssThreadError(threadData, std::string(msg));
		return 0;
	}

	try{
		initGLFunctions();
	}
	catch (const Exception &exc){
		rgssThreadError(threadData, exc.msg);
		SDL_GL_DestroyContext(glCtx);
		return 0;
	}

	if (!conf.enableBlitting)
		gl.BlitFramebuffer = 0;

	gl.ClearColor(0, 0, 0, 1);
	gl.Clear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(win);

#ifndef NDEBUG
	GLDebugLogger dLogger;
#endif

	try{
		SharedState::initInstance(threadData);
	}catch (const Exception &exc){
		rgssThreadError(threadData, exc.msg);
		SDL_GL_DestroyContext(glCtx);
		return 0;
	}

	/* Start script execution */
	scriptBinding->execute();

	threadData->rqTermAck.set();
	threadData->ethread->requestTerminate();

	SharedState::finiInstance();

	SDL_GL_DestroyContext(glCtx);
	return 0;
}

static void setupWindowIcon(const Config &conf, SDL_Window *win){
	SDL_IOStream *iconSrc;

	if (conf.iconPath.empty())
		iconSrc = SDL_IOFromConstMem(assets_icon_png, assets_icon_png_len);
	else
		iconSrc = SDL_IOFromFile(conf.iconPath.c_str(), "rb");

	SDL_Surface *iconImg = IMG_Load_IO(iconSrc, true);

	if (iconImg){
		SDL_SetWindowIcon(win, iconImg);
		SDL_DestroySurface(iconImg);
	}
}

int main(int argc, char *argv[]){
	//I WANT FUCKING OPTIMIZE EVERYFING IN THIS BULLSHIT
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
    startTime = boost::chrono::high_resolution_clock::now();
	loadLanguageMetadata(); //there will be a segfault on fclose if I don't move it here
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
	SDL_SetAppMetadata("Oneshot: Sunshine", VERSION_STRING, "meow.catwindowteam.sunshine");
	//X11 work on *BSD,Solaris too!
	#if unix_like
		SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
		#if SDL_VERSION_ATLEAST(3, 4, 10)
			SDL_SetHint(SDL_HINT_VIDEO_X11_ENABLE_XSYNC_EXT, "1");
		#endif
	#elif windows
		SDL_SetHint(SDL_HINT_WINDOWS_RAW_KEYBOARD, "1");
	#elif android
		SDL_SetHint(SDL_HINT_ANDROID_ALLOW_PERSISTENT_FOLDER_ACCESS, "1");
	#elif vita
		SDL_SetHint(SDL_HINT_VITA_RESOLUTION, "1080");
	#endif
	/* initialize SDL first */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD) == false){
		WarnMsg("Error initializing SDL: %s", SDL_GetError());
		return 0;
	}

#ifdef STEAM
	if (!STEAMSHIM_init()){
		WarnMsg("Could not initialize Steamworks API");
		return 0;
	}
#endif

	if (!EventThread::allocUserEvents()){
		WarnMsg("Error allocating SDL user events");
		return 0;
	}

#ifndef WORKDIR_CURRENT
	/* set working directory */
	const char *dataDir = SDL_GetBasePath();
	if (dataDir) {
		int result = chdir(dataDir);
		(void)result;
	}
#endif
	/* Initialize physfs here so that config can call PHYSFS_getPrefDir */
	PHYSFS_init(argv[0]);

	/* now we load the config */
	Config conf;
	conf.read(argc, argv);
	#if windows
		if(conf.Windows_AllocConsole == true){
    			AllocConsole();
    			freopen("CONOUT$", "w", stdout);
    			freopen("CONOUT$", "w", stderr);
		}
	#endif

	if (!conf.gameFolder.empty()){
		if (chdir(conf.gameFolder.c_str()) != 0){
			WarnMsg("Unable to switch into gameFolder %s", conf.gameFolder.c_str());
			return 0;
		}
	}

	std::string path;
	
	if (!conf.gameFolder.empty()) {
	    if (conf.gameFolder == ".") {
	        path = std::filesystem::current_path().string();
	    } else {
	        path = std::filesystem::absolute(conf.gameFolder).string();
	    }
	} else {
	    path = std::filesystem::current_path().string();
	}
	
	std::ofstream out(std::filesystem::temp_directory_path() / "sunshine");
	if (!out) {
	    WarnMsg("Failed to write game directory path to temp file, problems with journal app expected!");
	} else {
	    out << path;
	}
	
	extern int screenMain(Config &conf);
	if (conf.screenMode)
		return screenMain(conf);

	if (conf.windowTitle.empty())
		conf.windowTitle = conf.game.title;

	if (TTF_Init() == false){
		WarnMsg("Error initializing SDL_ttf: %s", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	if (MIX_Init() == false){
		WarnMsg("Error initializing SDL_mixer: %s", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		return 0;
	}

	SDL_Window *win;
	Uint32 winFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS;

	win = SDL_CreateWindow(conf.windowTitle.c_str(), conf.defScreenW, conf.defScreenH, winFlags);
	if (conf.fullscreen)
		SDL_SetWindowFullscreen(win, true);

	if (!win){
		WarnMsg("Error creating window: %s", SDL_GetError());
		MIX_Quit();
		TTF_Quit();
		SDL_Quit();
		return 0;
	}

	/* OSX and Windows have their own native ways of
	 * dealing with icons; don't interfere with them */
#ifdef unix_like
	setupWindowIcon(conf, win);
#else
	(void) setupWindowIcon;
#endif

	SDL_AudioSpec spec{};
	spec.format = SDL_AUDIO_F32;
	spec.channels = 2;
	spec.freq = 44100;

	MIX_Mixer* mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);

	if (!mixer){
		SDL_DestroyWindow(win);
		WarnMsg("Error creating Mixer Device, check your system audio configuration");
		MIX_Quit();
		TTF_Quit();
		SDL_Quit();	
		return 0;
	}

	SDL_DisplayMode mode;
	EventThread eventThread;
	RGSSThreadData rtData(&eventThread, win, mixer, mode.refresh_rate, conf);

#ifndef STEAM
	/* Add controller bindings from embedded controller DB */
	SDL_IOStream *controllerDB = SDL_IOFromConstMem(assets_gamecontrollerdb_txt, assets_gamecontrollerdb_txt_len);
	SDL_AddGamepadMappingsFromIO(controllerDB, 1);
#endif

	int winW, winH;
	SDL_GetWindowSize(win, &winW, &winH); // SDL_GL_GetDrawableSize(win, &winW, &winH);
	rtData.windowSizeMsg.post(Vec2i(winW, winH));

	ModLoader(conf, win);
	
	/* Load and post key bindings */
	rtData.bindingUpdateMsg.post(loadBindings(conf));
	/* Start RGSS thread */
	SDL_Thread *rgssThread = SDL_CreateThread(rgssThreadFun, "rgss", &rtData);

	/* Start event processing */
	eventThread.process(rtData);

	/* Request RGSS thread to stop */
	rtData.rqTerm.set();

	/* Wait for RGSS thread response */
	for (int i = 0; i < 1000; ++i){
		/* We can stop waiting when the request was ack'd */
		if (rtData.rqTermAck){
			Debug() << "[main] RGSS thread ack'd request after" << i*10 << "ms";
			break;
		}

		/* Give RGSS thread some time to respond */
		SDL_Delay(10);
	}

	/* If RGSS thread ack'd request, wait for it to shutdown,
	 * otherwise abandon hope and just end the process as is. */
	if (rtData.rqTermAck){
		SDL_WaitThread(rgssThread, 0);
	}else{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, conf.windowTitle.c_str(), "The RGSS script seems to be stuck and Sunshine will now force quit", win);
	}

	if (!rtData.rgssErrorMsg.empty())
		ErrorMsg(rtData.rgssErrorMsg.c_str());
	
	/* Clean up any remainin events */
	eventThread.cleanup();

	unloadLocale();
	unloadLanguageMetadata();

	if(show_crash_sceen){
		crash_screen(win);
	}
	
	MIX_DestroyMixer(mixer);
	SDL_DestroyWindow(win);

	MIX_Quit();
	TTF_Quit();
	SDL_Quit();

#ifdef STEAM
	STEAMSHIM_deinit();
#endif

	return 0;
}
