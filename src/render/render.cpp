#include "render/backends.h"
#include "config.h"

static IRender *activeInstance = 0;

IRender &activeRender(){
	return *activeInstance;
}

void setActiveRender(IRender *render){
	activeInstance = render;
}

std::vector<std::string> renderBackendOrder(){
	if (conf.renderer == "gpu" || conf.renderer == "gl" || conf.renderer == "sdl")
		return std::vector<std::string>(1, conf.renderer);

	std::vector<std::string> order;
	order.push_back("gpu");
	order.push_back("gl");
	order.push_back("sdl");
	return order;
}

uint64_t renderWindowFlags(){
	if (conf.renderer == "gpu")
		return gpuWindowFlags();

	if (conf.renderer == "sdl")
		return sdlWindowFlags();

	return glWindowFlags();
}

void setupRenderWindowAttributes(){
	if (conf.renderer == "gpu")
		gpuSetupWindowAttributes();
	else if (conf.renderer == "sdl")
		sdlSetupWindowAttributes();
	else
		glSetupWindowAttributes();
}

bool probeRenderBackend(SDL_Window *window){
	if (conf.renderer == "gpu")
		return gpuProbe(window);

	if (conf.renderer == "sdl")
		return sdlProbe(window);

	return glProbe(window);
}

IRenderContext *createRenderContext(SDL_Window *window){
	if (conf.renderer == "gpu")
		return createGPURenderContext(window);

	if (conf.renderer == "sdl")
		return createSDLRenderContext(window);

	return createGLRenderContext(window);
}

IRender *createRender(){
	if (conf.renderer == "gpu")
		return createGPURender(conf);

	if (conf.renderer == "sdl")
		return createSDLRender(conf);

	return createGLRender(conf);
}
