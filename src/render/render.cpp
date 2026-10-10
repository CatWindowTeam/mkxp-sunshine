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
	if (conf.renderer == "gpu" || conf.renderer == "gl-mkxp" || conf.renderer == "simple")
		return std::vector<std::string>(1, conf.renderer);

	std::vector<std::string> order;
	#ifdef RENDER_GPU
		order.push_back("gpu");
	#endif

	#ifdef RENDER_GL_MKXP
		order.push_back("gl-mkxp");
	#endif

	#ifdef RENDER_SIMPLE
		order.push_back("simple");
	#endif
	return order;
}

uint64_t renderWindowFlags(){
	#ifdef RENDER_GPU
		if(conf.renderer == "gpu")
			return gpuWindowFlags();
	#endif

	#ifdef RENDER_SIMPLE
		if(conf.renderer == "simple")
			return sdlWindowFlags();
	#endif

	#ifdef RENDER_GL_MKXP
		return glWindowFlags();
	#endif
}

void setupRenderWindowAttributes(){
	if(conf.renderer == "gpu"){
		#ifdef RENDER_GPU
			gpuSetupWindowAttributes();
		#endif
	}else if(conf.renderer == "simple"){
		#ifdef RENDER_SIMPLE
			sdlSetupWindowAttributes();
		#endif
	}else{
		#ifdef RENDER_GL_MKXP
			glSetupWindowAttributes();
		#endif
	}
}

bool probeRenderBackend(SDL_Window *window){
	#ifdef RENDER_GPU
		if(conf.renderer == "gpu")
			return gpuProbe(window);
	#endif

	#ifdef RENDER_SIMPLE
		if(conf.renderer == "simple")
			return sdlProbe(window);
	#endif

	#ifdef RENDER_GL_MKXP
		return glProbe(window);
	#endif
}

IRenderContext *createRenderContext(SDL_Window *window){
	#ifdef RENDER_GPU
		if(conf.renderer == "gpu")
			return createGPURenderContext(window);
	#endif

	#ifdef RENDER_SIMPLE
		if(conf.renderer == "simple")
			return createSDLRenderContext(window);
	#endif

	#ifdef RENDER_GL_MKXP
		return createGLRenderContext(window);
	#endif
}

IRender *createRender(){
	#ifdef RENDER_GPU
		if(conf.renderer == "gpu")
			return createGPURender(conf);
	#endif

	#ifdef RENDER_SIMPLE
		if(conf.renderer == "simple")
			return createSDLRender(conf);
	#endif

	#ifdef RENDER_GL_MKXP
		return createGLRender(conf);
	#endif
}
