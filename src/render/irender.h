#pragma once
#include "etc.h"
#include <stdint.h>

struct SDL_Surface;
struct SDL_Window;
struct Config;

struct TexHandle{
	uint32_t id;

	explicit TexHandle(uint32_t id = 0) : id(id) {}

	bool valid() const { return id != 0; }
	bool operator==(const TexHandle &o) const { return id == o.id; }
	bool operator!=(const TexHandle &o) const { return id != o.id; }
};

struct FboHandle{
	uint32_t id;

	explicit FboHandle(uint32_t id = 0) : id(id) {}

	bool valid() const { return id != 0; }
	bool operator==(const FboHandle &o) const { return id == o.id; }
	bool operator!=(const FboHandle &o) const { return id != o.id; }
};

struct RenderTarget{
	TexHandle tex;
	FboHandle fbo;
	int width, height;

	RenderTarget() : width(0), height(0) {}

	bool operator==(const RenderTarget &o) const { return tex == o.tex && fbo == o.fbo; }
};

enum class PixelFormat{
	RGBA,
	Luminance
};

enum class BlendOverride{
	ToneAdd,
	ToneSubtract,
	Overlay
};

class IRender{
public:
	virtual ~IRender() {}

	virtual int maxTextureSize() const = 0;
	virtual bool repeatNpotSupported() const = 0;

	virtual TexHandle createTexture(int w, int h, PixelFormat fmt = PixelFormat::RGBA) = 0;
	virtual void resizeTexture(TexHandle tex, int w, int h, PixelFormat fmt = PixelFormat::RGBA) = 0;
	virtual void destroyTexture(TexHandle tex) = 0;
	virtual void bindTexture(TexHandle tex) = 0;
	virtual void unbindTexture() = 0;
	virtual void setTextureSmooth(TexHandle tex, bool smooth) = 0;
	virtual void setTextureRepeat(TexHandle tex, bool repeat) = 0;
	virtual void uploadTexture(TexHandle tex, int w, int h, const void *pixels, PixelFormat fmt = PixelFormat::RGBA) = 0;
	virtual void uploadTextureRect(TexHandle tex, int x, int y, int w, int h, const void *pixels, PixelFormat fmt = PixelFormat::RGBA) = 0;
	virtual void uploadTextureRect(TexHandle tex, int dstX, int dstY, int w, int h, SDL_Surface *src, int srcX, int srcY) = 0;

	virtual RenderTarget createRenderTarget(int w, int h) = 0;
	virtual void resizeRenderTarget(RenderTarget &target, int w, int h) = 0;
	virtual void destroyRenderTarget(RenderTarget &target) = 0;
	virtual void bindRenderTarget(const RenderTarget &target) = 0;
	virtual void bindScreenTarget() = 0;
	virtual void clear() = 0;
	virtual void readPixels(const RenderTarget &target, int w, int h, void *out) = 0;

	virtual void setViewport(const IntRect &rect) = 0;
	virtual void pushViewport(const IntRect &rect) = 0;
	virtual void popViewport() = 0;
	virtual void refreshViewport() = 0;

	virtual void pushBlend(bool enabled) = 0;
	virtual void popBlend() = 0;

	virtual void pushBlendMode(BlendType mode) = 0;
	virtual void popBlendMode() = 0;
	virtual void setBlendOverride(BlendOverride mode) = 0;
	virtual void refreshBlendMode() = 0;

	virtual void pushScissorTest(bool enabled) = 0;
	virtual void popScissorTest() = 0;

	virtual void pushScissorBox(const IntRect &rect) = 0;
	virtual void saveScissorBox() = 0;
	virtual void intersectScissorBox(const IntRect &rect) = 0;
	virtual void popScissorBox() = 0;
	virtual void setScissorBox(const IntRect &rect) = 0;
	virtual const IntRect &scissorBox() = 0;

	virtual void pushClearColor(const Vec4 &color) = 0;
	virtual void popClearColor() = 0;

	virtual void beginBlit(const RenderTarget &target) = 0;
	virtual void beginBlitScreen(const Vec2i &size) = 0;
	virtual void blitSource(const RenderTarget &source) = 0;
	virtual void blitRect(const IntRect &src, const Vec2i &dstPos) = 0;
	virtual void blitRect(const IntRect &src, const IntRect &dst, bool smooth = false) = 0;
	virtual void endBlit() = 0;

	virtual void swapWindow(SDL_Window *window) = 0;
	virtual void suspendContext(SDL_Window *window) = 0;
	virtual void resumeContext(SDL_Window *window) = 0;
};

IRender *createRender(const Config &conf);
