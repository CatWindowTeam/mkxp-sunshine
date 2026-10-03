#pragma once
#include "render/irender.h"
#include "glstate.h"
#include "sharedstate.h"

typedef void *GLContextHandle;

class GLRender : public IRender{
public:
	GLRender(const Config &conf);

	GLState &state() { return glStateObj; }

	int maxTextureSize() const;
	bool repeatNpotSupported() const;

	TexHandle createTexture(int w, int h, PixelFormat fmt);
	void resizeTexture(TexHandle tex, int w, int h, PixelFormat fmt);
	void destroyTexture(TexHandle tex);
	void bindTexture(TexHandle tex);
	void unbindTexture();
	void setTextureSmooth(TexHandle tex, bool smooth);
	void setTextureRepeat(TexHandle tex, bool repeat);
	void uploadTexture(TexHandle tex, int w, int h, const void *pixels, PixelFormat fmt);
	void uploadTextureRect(TexHandle tex, int x, int y, int w, int h, const void *pixels, PixelFormat fmt);
	void uploadTextureRect(TexHandle tex, int dstX, int dstY, int w, int h, SDL_Surface *src, int srcX, int srcY);

	RenderTarget createRenderTarget(int w, int h);
	void resizeRenderTarget(RenderTarget &target, int w, int h);
	void destroyRenderTarget(RenderTarget &target);
	void bindRenderTarget(const RenderTarget &target);
	void bindScreenTarget();
	void clear();
	void readPixels(const RenderTarget &target, int w, int h, void *out);

	void setViewport(const IntRect &rect);
	void pushViewport(const IntRect &rect);
	void popViewport();
	void refreshViewport();

	void pushBlend(bool enabled);
	void popBlend();

	void pushBlendMode(BlendType mode);
	void popBlendMode();
	void setBlendOverride(BlendOverride mode);
	void refreshBlendMode();

	void pushScissorTest(bool enabled);
	void popScissorTest();

	void pushScissorBox(const IntRect &rect);
	void saveScissorBox();
	void intersectScissorBox(const IntRect &rect);
	void popScissorBox();
	void setScissorBox(const IntRect &rect);
	const IntRect &scissorBox();

	void pushClearColor(const Vec4 &color);
	void popClearColor();

	void beginBlit(const RenderTarget &target);
	void beginBlitScreen(const Vec2i &size);
	void blitSource(const RenderTarget &source);
	void blitRect(const IntRect &src, const Vec2i &dstPos);
	void blitRect(const IntRect &src, const IntRect &dst, bool smooth);
	void endBlit();

	void swapWindow(SDL_Window *window);
	void suspendContext(SDL_Window *window);
	void resumeContext(SDL_Window *window);

private:
	void beginBlitTo(FboHandle fbo, const Vec2i &size);

	GLState glStateObj;
	GLContextHandle context;
};

#define glState static_cast<GLRender&>(shState->render()).state()
