#include "render/backends.h"
#include "config.h"

static IRender *activeInstance = 0;

IRender &activeRender(){
	return *activeInstance;
}

void setActiveRender(IRender *render){
	activeInstance = render;
}

static bool useSDL(){
	return conf.renderer == "sdl";
}

uint64_t renderWindowFlags(){
	return useSDL() ? sdlWindowFlags() : glWindowFlags();
}

void setupRenderWindowAttributes(){
	if (useSDL())
		sdlSetupWindowAttributes();
	else
		glSetupWindowAttributes();
}

IRenderContext *createRenderContext(SDL_Window *window){
	return useSDL() ? createSDLRenderContext(window) : createGLRenderContext(window);
}

IRender *createRender(const Config &conf){
	return conf.renderer == "sdl" ? createSDLRender(conf) : createGLRender(conf);
}
