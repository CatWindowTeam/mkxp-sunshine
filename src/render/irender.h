#pragma once
#include "etc.h"
#include "render/effects.h"
#include <stdint.h>
#include <string>
#include <vector>
#include "config.h"

struct SDL_Surface;
struct SDL_Window;

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

struct GeometryHandle{
	uint32_t id;

	explicit GeometryHandle(uint32_t id = 0) : id(id) {}

	bool valid() const { return id != 0; }
	bool operator==(const GeometryHandle &o) const { return id == o.id; }
	bool operator!=(const GeometryHandle &o) const { return id != o.id; }
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

enum class VertexLayout{
	Simple,
	Color,
	Full
};

enum class GeometryUsage{
	Static,
	Dynamic
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
	virtual const char *apiName() const = 0;

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

	virtual GeometryHandle createGeometry(VertexLayout layout) = 0;
	virtual void destroyGeometry(GeometryHandle geom) = 0;
	virtual void allocGeometry(GeometryHandle geom, size_t bytes, GeometryUsage usage) = 0;
	virtual void uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage usage) = 0;
	virtual void uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data) = 0;
	virtual void ensureQuadIndices(size_t quadCount) = 0;
	virtual void drawQuads(GeometryHandle geom, size_t firstQuad, size_t quadCount) = 0;

	virtual void useEffect(ShaderType effect) = 0;
	virtual void useBlurPass(int pass) = 0;
	virtual void applyViewportProj() = 0;
	virtual void applyPerspectiveProj() = 0;
	virtual void setTexSize(const Vec2i &size) = 0;
	virtual void setTranslation(const Vec2i &value) = 0;
	virtual void setTime(float value) = 0;
	virtual void setEffectTexture(EffectTexture slot, TexHandle tex) = 0;
	virtual void setSpriteMat(const float value[16]) = 0;
	virtual void setMatrix(const float value[16]) = 0;
	virtual void setTone(const Vec4 &value) = 0;
	virtual void setColor(const Vec4 &value) = 0;
	virtual void setFlash(const Vec4 &value) = 0;
	virtual void setModulate(const Vec4 &value) = 0;
	virtual void setOpacity(float value) = 0;
	virtual void setBushDepth(float value) = 0;
	virtual void setBushOpacity(float value) = 0;
	virtual void setGray(float value) = 0;
	virtual void setHueAdjust(float value) = 0;
	virtual void setAniIndex(int value) = 0;
	virtual void setOffset(const Vec2i &value) = 0;
	virtual void setSubRect(const FloatRect &value) = 0;
	virtual void setProg(float value) = 0;
	virtual void setVague(float value) = 0;
	virtual void setWallMapResolution(int x, int y) = 0;
	virtual void setCameraPosition(int x, int y) = 0;
	virtual void setTileMapOffset(int x, int y) = 0;
	virtual void setLightSources(const std::vector<LightSource> &sources) = 0;
	virtual void setAmbient(float value) = 0;
	virtual void setScale(float value) = 0;

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

class IRenderContext{
public:
	virtual ~IRenderContext() {}
};

uint64_t renderWindowFlags();
void setupRenderWindowAttributes();
std::vector<std::string> renderBackendOrder();
bool probeRenderBackend(SDL_Window *window);
IRenderContext *createRenderContext(SDL_Window *window);

IRender *createRender();
IRender &activeRender();
void setActiveRender(IRender *render);
