#pragma once
#include "render/irender.h"

uint64_t glWindowFlags();
void glSetupWindowAttributes();
IRenderContext *createGLRenderContext(SDL_Window *window);
IRender *createGLRender(const Config &conf);

uint64_t sdlWindowFlags();
void sdlSetupWindowAttributes();
IRenderContext *createSDLRenderContext(SDL_Window *window);
IRender *createSDLRender(const Config &conf);
