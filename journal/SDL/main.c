#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include "icon.png.xxd"
#include "default.png.xxd"

char image[256] = "default"

int LoadImage(SDL_Renderer* ren){
	if(image == "default"){
		SDL_Surface* img = IMG_Load_IO(SDL_IOFromConstMem(assets_the_modded_machine_png, assets_the_modded_machine_png_len), true);
	}else{
		
	}
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* win = SDL_CreateWindow("SDL3 Image",640, 480, 0);
    if (win == NULL) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
    if (ren == NULL) {
    	SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* bmp = SDL_LoadBMP("lettuce.bmp");
    if (bmp == NULL) {
    	SDL_Log("Image loading Error: %s", SDL_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_DestroySurface(bmp);

    if (tex == NULL) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(ren);
        SDL_RenderTexture(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
