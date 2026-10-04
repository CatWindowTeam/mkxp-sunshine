#pragma once
#include "render/irender.h"
#include <SDL3/SDL_gpu.h>
#include <map>
#include <memory>
#include <vector>

struct GpuVertex{
	float x, y, u, v;
	float r, g, b, a;
};

struct GpuVertexParams{
	float xform[16];
	float translation[2];
	float texSizeInv[2];
	float offset[2];
	float aniIndex;
	int tilemapMode;
	int blurMode;
	int pad[3];
};

struct GpuFragParams{
	float tone[4];
	float color[4];
	float flash[4];
	float modulate[4];
	float subRect[4];
	float texSizeInv[2];
	float opacity;
	float gray;
	float hueAdjust;
	float bushDepth;
	float bushOpacity;
	float time;
	float prog;
	float vague;
	int effect;
	float pad;
};

struct GpuLightParams{
	float sources[64][4];
	float colors[64][4];
	float texSizeInv[2];
	float cameraPos[2];
	float ambient;
	int count;
	float pad[2];
};

struct GpuTexture{
	SDL_GPUTexture *texture;
	int width;
	int height;
	bool smooth;
	bool repeat;

	GpuTexture() : texture(0), width(0), height(0), smooth(false), repeat(false) {}
};

struct GpuGeometry{
	VertexLayout layout;
	std::vector<GpuVertex> vertices;
};

struct GpuUniforms{
	float proj[16];
	float spriteMat[16];
	float matrix[16];
	Vec2i texSize;
	Vec2i translation;
	Vec2i offset;
	Vec4 tone;
	Vec4 color;
	Vec4 flash;
	Vec4 modulate;
	Vec4 subRect;
	float time;
	float opacity;
	float bushDepth;
	float bushOpacity;
	float gray;
	float hueAdjust;
	float aniIndex;
	float prog;
	float vague;
	uint32_t aux[4];

	GpuUniforms();
};

enum class GpuBlend{
	None,
	Normal,
	KeepDestAlpha,
	Addition,
	Substraction,
	Multiply,
	ToneAdd,
	ToneSubtract,
	Count
};

struct GpuCommand{
	enum Type{
		Draw,
		Clear,
		Upload
	};

	Type type;
	uint32_t target;
	int pipeline;
	GpuBlend blend;
	SDL_Rect viewport;
	SDL_Rect scissor;
	bool scissorOn;
	uint32_t firstVertex;
	uint32_t quadCount;
	GpuVertexParams vertexParams;
	GpuFragParams fragParams;
	int payload;
	uint32_t tex[4];
	uint8_t sampler[4];
	float clearColor[4];
	SDL_Rect rect;
};

template<typename T>
struct GpuProperty{
	T current;
	std::vector<T> stack;

	void push() { stack.push_back(current); }
	void pop() { if (!stack.empty()) { current = stack.back(); stack.pop_back(); } }
	void set(const T &value) { current = value; }
	void pushSet(const T &value) { push(); set(value); }
};

class GPURender : public IRender{
public:
	GPURender(const Config &conf, SDL_GPUDevice *device, SDL_Window *window);
	~GPURender();

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
	struct PipelineKey{
		int pipeline;
		int blend;
		int format;

		bool operator<(const PipelineKey &o) const;
	};

	GpuTexture *texture(uint32_t id);
	GpuUniforms &uniforms();
	void createGpuTexture(GpuTexture &tex, int w, int h);
	void recordUpload(uint32_t id, int x, int y, int w, int h, std::vector<uint8_t> &data);
	void recordClear(uint32_t id, const Vec4 &color);
	GpuBlend currentBlend() const;
	void fillCommandState(GpuCommand &cmd, const GpuUniforms &u, int effect);
	void recordQuad(const GpuVertex verts[4], int effect, GpuBlend blend, const SDL_Rect &viewport, bool scissor, const SDL_Rect &scissorRect, uint32_t target);
	void flush();
	void ensureVertexCapacity(size_t bytes);
	void ensureIndexBuffer(size_t quads);
	SDL_GPUGraphicsPipeline *pipelineFor(int pipeline, GpuBlend blend, SDL_GPUTextureFormat format);
	SDL_GPUSampler *samplerFor(int flags);
	void releasePending();
	int screenHeight();

	SDL_GPUDevice *device;
	SDL_Window *window;
	SDL_GPUShader *vertexShader;
	SDL_GPUShader *uberShader;
	SDL_GPUShader *lightShader;
	std::map<PipelineKey, SDL_GPUGraphicsPipeline*> pipelines;
	SDL_GPUSampler *samplers[4];
	SDL_GPUTexture *dummy;

	std::vector<std::unique_ptr<GpuTexture>> textures;
	std::vector<uint32_t> freeTextures;
	std::vector<uint32_t> quarantine;
	std::vector<SDL_GPUTexture*> pendingRelease;
	std::vector<std::unique_ptr<GpuGeometry>> geometries;
	std::vector<uint32_t> freeGeometries;

	GpuTexture *target;
	uint32_t targetId;
	uint32_t boundTexture;
	int maxTexSize;

	GpuProperty<IntRect> viewport;
	GpuProperty<bool> blend;
	GpuProperty<BlendType> blendModeProp;
	GpuProperty<bool> scissorTest;
	GpuProperty<IntRect> scissorBoxProp;
	GpuProperty<Vec4> clearColor;
	GpuBlend activeBlend;
	bool blendOverridden;

	std::vector<GpuUniforms> uniformSets;
	int currentSlot;
	int blurPass;

	std::vector<LightSource> lights;
	Vec2i cameraPos;
	float ambient;

	std::vector<GpuCommand> commands;
	std::vector<GpuVertex> vertexStream;
	std::vector<std::vector<uint8_t>> uploads;
	std::vector<GpuLightParams> lightPool;

	SDL_GPUBuffer *vertexBuffer;
	size_t vertexCapacity;
	SDL_GPUBuffer *indexBuffer;
	size_t indexQuads;
	size_t neededQuads;
};
