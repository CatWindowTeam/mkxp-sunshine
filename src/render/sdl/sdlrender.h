#pragma once
#include "render/irender.h"
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

template<typename T>
struct SdlProperty{
	T current;
	std::vector<T> stack;

	void push() { stack.push_back(current); }
	void pop() { if (!stack.empty()) { current = stack.back(); stack.pop_back(); } }
	void set(const T &value) { current = value; }
	void pushSet(const T &value) { push(); set(value); }
};

enum class SdlBlend{
	KeepDestAlpha,
	Normal,
	Addition,
	Substraction,
	Multiply,
	ToneAdd,
	ToneSubtract
};

struct SdlUniforms{
	float spriteMat[16];
	float matrix[16];
	Vec2i texSize;
	Vec2i translation;
	Vec4 color;
	Vec4 modulate;
	float opacity;
	float aniIndex;
	float prog;
	float hueAdjust;
	float gray;
	uint32_t current;
	uint32_t frozen;

	SdlUniforms();
};

class SDLRender : public IRender{
public:
	SDLRender(const Config &conf, SDL_Renderer *renderer);
	~SDLRender();

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

	GeometryHandle createGeometry(VertexLayout layout);
	void destroyGeometry(GeometryHandle geom);
	void allocGeometry(GeometryHandle geom, size_t bytes, GeometryUsage usage);
	void uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage usage);
	void uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data);
	void ensureQuadIndices(size_t quadCount);
	void drawQuads(GeometryHandle geom, size_t firstQuad, size_t quadCount);

	void useEffect(ShaderType effect);
	void useBlurPass(int pass);
	void applyViewportProj();
	void applyPerspectiveProj();
	void setTexSize(const Vec2i &size);
	void setTranslation(const Vec2i &value);
	void setTime(float value);
	void setEffectTexture(EffectTexture slot, TexHandle tex);
	void setSpriteMat(const float value[16]);
	void setMatrix(const float value[16]);
	void setTone(const Vec4 &value);
	void setColor(const Vec4 &value);
	void setFlash(const Vec4 &value);
	void setModulate(const Vec4 &value);
	void setOpacity(float value);
	void setBushDepth(float value);
	void setBushOpacity(float value);
	void setGray(float value);
	void setHueAdjust(float value);
	void setAniIndex(int value);
	void setOffset(const Vec2i &value);
	void setSubRect(const FloatRect &value);
	void setProg(float value);
	void setVague(float value);
	void setWallMapResolution(int x, int y);
	void setCameraPosition(int x, int y);
	void setTileMapOffset(int x, int y);
	void setLightSources(const std::vector<LightSource> &sources);
	void setAmbient(float value);

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
	SdlUniforms &uniforms();
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
	std::vector<std::unique_ptr<SdlTexture>> textures;
	std::vector<uint32_t> freeTextures;
	std::vector<std::unique_ptr<SdlGeometry>> geometries;
	std::vector<uint32_t> freeGeometries;

	SdlTexture *target;
	uint32_t boundTexture;
	int maxTexSize;

	SdlProperty<IntRect> viewport;
	SdlProperty<bool> blend;
	SdlProperty<BlendType> blendModeProp;
	SdlProperty<bool> scissorTest;
	SdlProperty<IntRect> scissorBoxProp;
	SdlProperty<Vec4> clearColor;
	SdlBlend activeBlend;

	std::vector<SdlUniforms> uniformSets;
	int currentSlot;
	int blurPass;

	std::vector<LightSource> lights;
	Vec2i cameraPos;
	float ambient;
	SdlTexture lightMap;
};
