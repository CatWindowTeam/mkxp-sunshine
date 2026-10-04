#pragma once
#include "render/statefulrender.h"
#include <SDL3/SDL_render.h>
#include <map>
#include <memory>
#include <vector>

struct SdlTexture{
	SDL_Texture *texture;
	int width;
	int height;
	bool smooth;
	bool repeat;

	SdlTexture() : texture(0), width(0), height(0), smooth(false), repeat(false) {}
};

struct SdlGeometry{
	VertexLayout layout;
	std::vector<uint8_t> data;
};

class SDLRender : public StatefulRender{
public:
	SDLRender(const Config &conf, SDL_Renderer *renderer);
	~SDLRender();

	int maxTextureSize() const;
	bool repeatNpotSupported() const;

	TexHandle createTexture(int w, int h, PixelFormat fmt);
	void resizeTexture(TexHandle tex, int w, int h, PixelFormat fmt);
	void destroyTexture(TexHandle tex);
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







	GeometryHandle createGeometry(VertexLayout layout);
	void destroyGeometry(GeometryHandle geom);
	void allocGeometry(GeometryHandle geom, size_t bytes, GeometryUsage usage);
	void uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage usage);
	void uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data);
	void ensureQuadIndices(size_t quadCount);
	void drawQuads(GeometryHandle geom, size_t firstQuad, size_t quadCount);


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
	struct Vertices{
		std::vector<SDL_Vertex> list;
		std::vector<int> indices;
	};

	SdlTexture *texture(uint32_t id);
	void createSdlTexture(SdlTexture &tex, int w, int h);
	void applyState();
	SDL_BlendMode blendMode(bool forceBlend);
	SDL_BlendMode composeOrFallback(SDL_BlendFactor srcColor, SDL_BlendFactor dstColor, SDL_BlendOperation colorOp,
	                                SDL_BlendFactor srcAlpha, SDL_BlendFactor dstAlpha, SDL_BlendOperation alphaOp,
	                                SDL_BlendMode fallback);
	void submit(SdlTexture *tex, const Vertices &v, SDL_BlendMode mode, bool smooth);
	void readTexture(SdlTexture &tex, std::vector<uint8_t> &out);
	void drawFilteredCopy(SdlTexture &src, const Vertices &v, int filter);
	void drawBlur(SdlTexture &src, const Vertices &v);
	void drawTransition(const Vertices &v);
	void drawLightMap(const Vertices &v, const SdlTexture &bound);
	void drawTextureQuad(SdlTexture *tex, const SDL_Vertex corners[4], SDL_BlendMode mode, bool smooth);
	int screenHeight();

	SDL_Renderer *renderer;
	SDL_Texture *probe;
	std::map<SDL_BlendMode, bool> customSupport;
	HandlePool<SdlTexture> textures;
	HandlePool<SdlGeometry> geometries;

	SdlTexture *target;
	int maxTexSize;



	SdlTexture lightMap;
};
