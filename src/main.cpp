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

#include <alc.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_sound/SDL_sound.h>
#include <physfs.h>

#ifdef _MSC_VER
#include <direct.h>
#define _chdir chdir
#else
#include <unistd.h>
#endif
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
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
#include "security.h"
#include "modloader.h"
#include "sunshine.h"

#include "meow.h"

#include "binding.h"

#include "icon.png.xxd"

#ifdef STEAM
	#include "steamshim/steamshim_child.h"
#else
	#include "gamecontrollerdb.txt.xxd"
#endif

static void rgssThreadError(RGSSThreadData *rtData, const std::string &msg){
	rtData->rgssErrorMsg = msg;
	rtData->ethread->requestTerminate();
	rtData->rqTermAck.set();
}

static inline const char* glGetStringInt(GLenum name){
	return (const char*) gl.GetString(name);
}

int rgssThreadFun(void *userdata){
	RGSSThreadData *threadData = static_cast<RGSSThreadData*>(userdata);
	const Config &conf = threadData->config;
	SDL_Window *win = threadData->window;
	char msg[512];
	SDL_GLContext glCtx;

	/* Setup GL context */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifndef NDEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	glCtx = SDL_GL_CreateContext(win);

	if (!glCtx){
		crash(Exception::MEOW, "Error creating context: %s", SDL_GetError());
		rgssThreadError(threadData, std::string(msg));
		return 0;
	}

	try{
		initGLFunctions();
	}
	catch (const Exception &exc){
		crash(Exception::MEOW, exc.msg.c_str());
		rgssThreadError(threadData, exc.msg);
		SDL_GL_DestroyContext(glCtx);
		return 0;
	}

	if (!conf.enableBlitting)
		gl.BlitFramebuffer = 0;

	gl.ClearColor(0, 0, 0, 1);
	gl.Clear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(win);

	Debug() << "[main] GL Vendor    :" << glGetStringInt(GL_VENDOR);
    Debug() << "[main] GL Renderer  :" << glGetStringInt(GL_RENDERER);
    Debug() << "[main] GL Version   :" << glGetStringInt(GL_VERSION);
    Debug() << "[main] GLSL Version :" << glGetStringInt(GL_SHADING_LANGUAGE_VERSION);

	bool vsync = conf.vsync || conf.syncToRefreshrate;
	SDL_GL_SetSwapInterval(vsync ? 1 : 0);
	
#ifndef NDEBUG
	GLDebugLogger dLogger;
#endif

	/* Setup AL context */
	ALCcontext *alcCtx = alcCreateContext(threadData->alcDev, 0);

	if (!alcCtx){
		crash(Exception::MEOW, "Error creating OpenAL context");
		rgssThreadError(threadData, "Error creating OpenAL context");
		SDL_GL_DestroyContext(glCtx);
		return 0;
	}

	alcMakeContextCurrent(alcCtx);

	try{
		SharedState::initInstance(threadData);
	}catch (const Exception &exc){
		crash(Exception::MEOW, exc.msg.c_str());
		rgssThreadError(threadData, exc.msg);
		alcDestroyContext(alcCtx);
		SDL_GL_DestroyContext(glCtx);

		return 0;
	}

	/* Start script execution */
	scriptBinding->execute();

	threadData->rqTermAck.set();
	threadData->ethread->requestTerminate();

	SharedState::finiInstance();

	alcDestroyContext(alcCtx);
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

// mainly doing this so journal app knows where to load images from
static void setGamePathInRegistry() {
#if defined WIN32
	// this logic is currently windows specific
	const char* dataDir = SDL_GetBasePath();
	if (dataDir){
		HKEY key;
		long keyOpenError = RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\OneShot\\"), &key);

		if (keyOpenError != ERROR_SUCCESS) {
			// try creating the key first
			long keyCreateError = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\OneShot\\"), 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL);

			if (keyCreateError != ERROR_SUCCESS){
				WarnMsg("Unable to create key in registry");
			}
			else {
				keyOpenError = ERROR_SUCCESS;
			}
		}

		if (keyOpenError != ERROR_SUCCESS){
			WarnMsg("Unable to open registry.");
		}
		else {
			DWORD dataSize = (strlen(dataDir) + 1) * sizeof(char);
			if (RegSetValueEx(key, TEXT("GameDirectory"), 0, REG_SZ, (LPBYTE)dataDir, dataSize) != ERROR_SUCCESS){
				WarnMsg("Unable to set GameDirectory registry value");
			}
			RegCloseKey(key);
		}
	}
#endif
	//TODO handle this for Linux/Mac
}
int main(int argc, char *argv[]){
    SecurityManagerInit();
    startTime = boost::chrono::high_resolution_clock::now();
	loadLanguageMetadata(); //there will be a segfault on fclose if I don't move it here

	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
	SDL_SetHint(SDL_HINT_APP_ID, "OneshotSunshine");
	SDL_SetHint(SDL_HINT_APP_NAME, "Oneshot: Sunshine");
	SDL_SetHint(SDL_HINT_AUDIO_DEVICE_STREAM_NAME, "Oneshot: sunshine");
	SDL_SetHint(SDL_HINT_AUDIO_DEVICE_STREAM_ROLE, "Game");
	//X11 work on *BSD,Solaris too!
	#if defined(__linux__) || defined(BSD) || defined(__sun)
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
		#if SDL_VERSION_ATLEAST(3, 4, 10)
			SDL_SetHint(SDL_HINT_VIDEO_X11_ENABLE_XSYNC_EXT, "1");
		#endif
	#endif
	
	/* initialize SDL first */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD) == false){
		crash(Exception::MEOW, "Error initializing SDL: %s", SDL_GetError());
		return 0;
	}

#ifdef STEAM
	if (!STEAMSHIM_init()){
		crash(Exception::MEOW, "Could not initialize Steamworks API");
		return 0;
	}
#endif

	if (!EventThread::allocUserEvents()){
		crash(Exception::MEOW, "Error allocating SDL user events");
		return 0;
	}

#ifndef WORKDIR_CURRENT
	/* set working directory */
	const char *dataDir = SDL_GetBasePath();
	if (dataDir) {
		int result = chdir(dataDir);
		(void)result;
		SDL_free((void*)dataDir); // SDL_GetBasePath returns a heap pointer; SDL_free expects void*
	}
#endif

	setGamePathInRegistry();

	/* Initialize physfs here so that config can call PHYSFS_getPrefDir */
	PHYSFS_init(argv[0]);

	/* now we load the config */
	Config conf;
	conf.read(argc, argv);
	#if defined WIN32
		if(conf.Windows_AllocConsole == true){
    			AllocConsole();
    			freopen("CONOUT$", "w", stdout);
    			freopen("CONOUT$", "w", stderr);
		}
	#endif

	
	if (!conf.gameFolder.empty()){
		if (chdir(conf.gameFolder.c_str()) != 0){
			crash(Exception::MEOW, "Unable to switch into gameFolder %s", conf.gameFolder);
			return 0;
		}
	}

	std::string new_path = ModLoader(conf);
	
	if(new_path != ""){
		if (chdir(new_path.c_str()) != 0){
			crash(Exception::MEOW, "Unable to switch into new gameFolder %s", new_path);
			return 0;
		}	
	}

	extern int screenMain(Config &conf);
	if (conf.screenMode)
		return screenMain(conf);

	if (conf.windowTitle.empty())
		conf.windowTitle = conf.game.title;

	if (TTF_Init() == false){
		crash(Exception::MEOW, "Error initializing SDL_ttf: %s", SDL_GetError());
		SDL_Quit();
	}

	if (Sound_Init() == false){
		crash(Exception::MEOW, "Error initializing SDL_sound: %s", Sound_GetError());
		TTF_Quit();
		SDL_Quit();

		return 0;
	}

	SDL_Window *win;
	Uint32 winFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS;

	// #ifdef __APPLE__
	// 	winFlags |= SDL_WINDOW_RESIZABLE;
	// #endif

	win = SDL_CreateWindow(conf.windowTitle.c_str(), conf.defScreenW, conf.defScreenH, winFlags);
	if (conf.fullscreen)
		SDL_SetWindowFullscreen(win, true);

	if (!win){
		crash(Exception::MEOW, "Error creating window: %s", SDL_GetError());
		return 0;
	}

	/* OSX and Windows have their own native ways of
	 * dealing with icons; don't interfere with them */
#ifdef __LINUX__
	setupWindowIcon(conf, win);
#else
	(void) setupWindowIcon;
#endif

	ALCdevice *alcDev = alcOpenDevice(0);

	if (!alcDev){
		SDL_DestroyWindow(win);
		crash(Exception::MEOW, "Error opening OpenAL device");
		TTF_Quit();
		SDL_Quit();

		return 0;
	}

	SDL_DisplayMode mode;
	/* Can't sync to display refresh rate if its value is unknown */
	//if (!mode.refresh_rate)
	//	conf.syncToRefreshrate = false;

	EventThread eventThread;
	RGSSThreadData rtData(&eventThread, win, alcDev, mode.refresh_rate, conf);

#ifndef STEAM
	/* Add controller bindings from embedded controller DB */
	SDL_IOStream *controllerDB = SDL_IOFromConstMem(assets_gamecontrollerdb_txt, assets_gamecontrollerdb_txt_len);
	SDL_AddGamepadMappingsFromIO(controllerDB, 1);
#endif

	int winW, winH;
	SDL_GetWindowSize(win, &winW, &winH); // SDL_GL_GetDrawableSize(win, &winW, &winH);
	rtData.windowSizeMsg.post(Vec2i(winW, winH));

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
			Debug() << "[main]RGSS thread ack'd request after" << i*10 << "ms";
			break;
		}

		/* Give RGSS thread some time to respond */
		SDL_Delay(10);
	}

	/* If RGSS thread ack'd request, wait for it to shutdown,
	 * otherwise abandon hope and just end the process as is. */
	if (rtData.rqTermAck)
		SDL_WaitThread(rgssThread, 0);
	else
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, conf.windowTitle.c_str(), "The RGSS script seems to be stuck and Sunshine will now force quit", win);

	if (!rtData.rgssErrorMsg.empty())
		crash(Exception::MEOW, rtData.rgssErrorMsg.c_str());

	/* Clean up any remainin events */
	eventThread.cleanup();

	Debug() << "[main] Shutting down.";

	unloadLocale();
	unloadLanguageMetadata();

	alcCloseDevice(alcDev);
	SDL_DestroyWindow(win);

	Sound_Quit();
	TTF_Quit();
	SDL_Quit(); // i got "Thread 1 received signal ?, Unknown signal" here on windows after closing game

#ifdef STEAM
	STEAMSHIM_deinit();
#endif

	return 0;
}
