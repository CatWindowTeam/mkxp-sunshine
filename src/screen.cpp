#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#define FPS 60
#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 240

#include "config.h"
#include "debugwriter.h"
#include "pipe.h"
#include "meow.h"

static void showInitError(const std::string &msg){
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Sunshine Error", msg.c_str(), 0);
}

static bool readMessage(Pipe &ipc, char *buf, size_t size){
	size_t index = 0;
	while (index < size - 1) {
		if (ipc.read(buf + index)) {
			++index;
		} else { break; }
	}
	buf[index] = 0;

	return index > 0;
}

int screenMain(Config &conf){
	char msg[512];
	const SDL_Color colorKey = {0x00, 0xFF, 0x00, 0xFF};
	const SDL_Color black = {0x00, 0x00, 0x00, 0xFF};

	Pipe ipc("oneshot-pipe", Pipe::Read);

	SDL_Window *win;
	win = SDL_CreateWindow("The Journal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT);

	if (!win){
		snprintf(msg, sizeof msg, "Error creating window: %s", SDL_GetError());
		crash(msg);
		return 0;
	}

	SDL_Surface *shape = SDL_CreateSurface(DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_PixelFormat::SDL_PIXELFORMAT_RGBA32);
	SDL_Palette *palette = SDL_CreateSurfacePalette(shape);
	SDL_SetPaletteColors(palette, &black, 0, 1);

	char messageBuf[256];

	unsigned int ticks = SDL_GetTicks();
	for (;;) {
		// Handle events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_EVENT_QUIT:
				return 0;
			}
		}

		// Change shape
		if (readMessage(ipc, messageBuf, sizeof(messageBuf))) {
			if (strcmp(messageBuf, "END") == 0)
				break;
			std::string imgname = conf.gameFolder + "/Journal/" + messageBuf + ".png";
			SDL_DestroySurface(shape);
			if ((shape = IMG_Load(imgname.c_str())) == NULL)
				break;
			SDL_SetWindowSize(win, shape->w, shape->h);
			// SDL_SetWindowShape(win, shape, &shapeMode);
			SDL_SetWindowShape(win, shape);
		}

		// Redraw
		SDL_BlitSurface(shape, NULL, SDL_GetWindowSurface(win), NULL);
		SDL_UpdateWindowSurface(win);

		// Regulate framerate
	    unsigned int ticksDelta = SDL_GetTicks() - ticks;
	    if (ticksDelta < 1000 / FPS)
	        SDL_Delay(1000 / FPS - ticksDelta);
	    ticks = SDL_GetTicks();
	}

	SDL_DestroySurface(shape);
	return 0;
}
