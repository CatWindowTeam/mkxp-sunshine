#include <SDL3/SDL_video.h>
enum Backend {
	DEFAULT,
	SOFTWARE,
	OPENGL,
	SDL_GPU
};

enum TextureFilter {
	NEAREST,
	LINEAR
};

enum TextureFormat {
    RGBA8,
    RGBA16F
};

enum TextureAddressMode {
    Clamp,
    Repeat
};

class RenderBase {
public:
	//backend initializatoon
	virtual int RenderInit() { return 0; }
	//create window
    virtual SDL_Window* RenderInitWindow() { return NULL; }
    //de init
    virtual void RenderDeInit() {}
    //Debuger
    virtual void RenderDebugger() {}
};

void RenderSelectBackend(Backend b){}
