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
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_system.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
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
#include <exception>
#include <boost/chrono.hpp>
#include "sharedstate.h"
#include "eventthread.h"
#include "debugwriter.h"
#include "exception.h"
#include "gl-fun.h"
#include "i18n.h"
#include "sunshine.h"
#include "modloader.h"
#include "define.h"
#include "meow.h"
#include "binding.h"
#include "CLI11.hpp"
#include "icon.png.xxd"

#ifndef NDEBUG
	#include "gl-debug.h"
#endif

#ifdef _WIN32
	#include <windows.h>
#endif

#ifdef STEAM
	#include "steamshim/steamshim_child.h"
#else
	#include "gamecontrollerdb.txt.xxd"
#endif

#ifdef DEBUG
	#ifdef ps2
		SDL_PS2_SKIP_IOP_RESET();
	#endif
#endif

#ifndef VERSION_STRING
	#define VERSION_STRING "Unknown"
#endif

static void rgssThreadError(RGSSThreadData *rtData, const std::string &msg){
	rtData->rgssErrorMsg = msg;
	rtData->ethread->requestTerminate();
	rtData->rqTermAck.set();
}

int rgssThreadFun(void *userdata){
	RGSSThreadData *threadData = static_cast<RGSSThreadData*>(userdata);
	SDL_Window *win = threadData->window;
	SDL_GLContext glCtx;

	/* Setup GL context */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//https://wiki.libsdl.org/SDL3/README-android
	#ifdef android
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	#endif

	#ifndef NDEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif

	glCtx = SDL_GL_CreateContext(win);
	if (!glCtx){
		rgssThreadError(threadData, "Failed to create OpenGL context");
		return 0;
	}

	try{
		initGLFunctions();
	}catch(const Exception &exc){
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
	iconSrc = SDL_IOFromConstMem(assets_icon_png, assets_icon_png_len);
	SDL_Surface *iconImg = IMG_Load_IO(iconSrc, true);
	if (iconImg){
		SDL_SetWindowIcon(win, iconImg);
		SDL_DestroySurface(iconImg);
	}
}

int main(int argc, char *argv[]){
	conf.read(argc, argv);
	SDL_SetHint("SDL_HINT_INVALID_PARAM_CHECKS", "1");
    startTime = boost::chrono::high_resolution_clock::now();
    loadLanguageMetadata();
	SDL_SetHint("SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS", "0");
	SDL_SetAppMetadata("Oneshot: Sunshine", VERSION_STRING, "meow.catwindowteam.sunshine");
	#if unix_like
		SDL_SetHint("SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR", "0");
		SDL_SetHint("SDL_HINT_VIDEO_X11_ENABLE_XSYNC_EXT", "1");
	#elif windows
		SDL_SetHint("SDL_HINT_WINDOWS_RAW_KEYBOARD", "1");
		SDL_SetHint("SDL_HINT_WINDOWS_RAW_KEYBOARD_EXCLUDE_HOTKEYS", "1");
	#elif vita
		SDL_SetHint(SDL_HINT_VITA_PVR_OPENGL, "0");
		SDL_SetHint(SDL_HINT_VITA_RESOLUTION, "1080");
	#elif ps2
		SDL_SetHint("SDL_HINT_PS2_GS_MODE", "NTSC");
	#elif android
		SDL_SetHint(SDL_HINT_ANDROID_ALLOW_PERSISTENT_FOLDER_ACCESS, "1");
		SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
		SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
	#elif vita
		SDL_SetHint(SDL_HINT_VITA_RESOLUTION, "1080");
	#endif

	/* initialize SDL first */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD) == false){
		WarnMsg("Error initializing SDL: %s", SDL_GetError());
		return 1;
	}

	#ifdef STEAM
		if (!STEAMSHIM_init()){
			WarnMsg("Could not initialize Steamworks API");
			return 1;
		}
	#endif

	if (!EventThread::allocUserEvents()){
		WarnMsg("Error allocating SDL user events");
		return 1;
	}

	/* Initialize physfs here so that config can call PHYSFS_getPrefDir */
	#ifdef android
		PHYSFS_AndroidInit androidInit;
		androidInit.jnienv = SDL_GetAndroidJNIEnv();
		androidInit.context = SDL_GetAndroidActivity();
		PHYSFS_init((const char *)&androidInit);
	#else
		PHYSFS_init(argc > 0 ? argv[0] : "oneshot");
	#endif

	#if windows
		if(conf.Windows_AllocConsole == true){
    			AllocConsole();
    			freopen("CONOUT$", "w", stdout);
    			freopen("CONOUT$", "w", stderr);
		}
	#endif

	if (!conf.gameFolder.empty()){
		if(chdir(conf.gameFolder.c_str()) != 0){
			WarnMsg("Unable to switch into gameFolder %s", conf.gameFolder.c_str());
			return 0;
		}
	}

	std::string path;
	if (!conf.gameFolder.empty()) {
	    if(conf.gameFolder == ".") {
	        path = std::filesystem::current_path().string();
	    }else{
	        path = std::filesystem::absolute(conf.gameFolder).string();
	    }
	}else{
	    path = std::filesystem::current_path().string();
	}

	#ifdef vita
		std::string shit = "ux0:/data/Sunshine";
	#else
		std::string shit = SDL_GetUserFolder(SDL_FOLDER_HOME);
	#endif
	std::ofstream out(std::filesystem::path(shit) / "sunshine");
	if (!out) {
	    WarnMsg("Failed to write game directory path to temp file, problems with journal app expected!");
	} else {
	    out << path;
	}

	if (TTF_Init() == false){
		WarnMsg("Error initializing SDL_ttf: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	if (MIX_Init() == false){
		WarnMsg("Error initializing SDL_mixer: %s", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	SDL_Window *win;
	Uint32 winFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_HIGH_PIXEL_DENSITY;

	win = SDL_CreateWindow(conf.windowTitle.c_str(), conf.defScreenW, conf.defScreenH, winFlags);
	if (!win){
		WarnMsg("%s", SDL_GetError());
		MIX_Quit();
		TTF_Quit();
		SDL_Quit();
		return 1;
	}
	if (conf.fullscreen){ SDL_SetWindowFullscreen(win, true); }
	
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
		return 1;
	}

	EventThread eventThread;
	float refreshRate = 60.0f;
	SDL_DisplayID displayID = SDL_GetDisplayForWindow(win);
	const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayID);
	if(mode != nullptr && mode->refresh_rate > 0.0f) {
	    refreshRate = mode->refresh_rate;
	}

	RGSSThreadData rtData(&eventThread, win, mixer, refreshRate);	
	#ifndef STEAM
		/* Add controller bindings from embedded controller DB */
		SDL_IOStream *controllerDB = SDL_IOFromConstMem(assets_gamecontrollerdb_txt, assets_gamecontrollerdb_txt_len);
		SDL_AddGamepadMappingsFromIO(controllerDB, 1);
	#endif

	int winW, winH;
	SDL_GetWindowSize(win, &winW, &winH);
	rtData.windowSizeMsg.post(Vec2i(winW, winH));

	#ifdef vita
		if (!PHYSFS_mount("app0:sunshine.zip", nullptr, 0)) {
	    	WarnMsg("Failed to mount assets archive: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	#endif
	/* start modloader */
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
			Debug() << "[main] RGSS thread ack'd request after " << i*10 << "ms";
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
	if(show_crash_screen){
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
