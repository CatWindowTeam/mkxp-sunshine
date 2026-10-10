#pragma once
#include "render/irender.h"

#ifdef RENDER_GL_MKXP
	uint64_t glWindowFlags();
	void glSetupWindowAttributes();
	bool glProbe(SDL_Window *window);
	IRenderContext *createGLRenderContext(SDL_Window *window);
	IRender *createGLRender(const Config &conf);
#endif

#ifdef RENDER_SIMPLE
	uint64_t sdlWindowFlags();
	void sdlSetupWindowAttributes();
	bool sdlProbe(SDL_Window *window);
	IRenderContext *createSDLRenderContext(SDL_Window *window);
	IRender *createSDLRender(const Config &conf);
#endif

#ifdef RENDER_GPU
	uint64_t gpuWindowFlags();
	void gpuSetupWindowAttributes();
	bool gpuProbe(SDL_Window *window);
	IRenderContext *createGPURenderContext(SDL_Window *window);
	IRender *createGPURender(const Config &conf);
#endif
