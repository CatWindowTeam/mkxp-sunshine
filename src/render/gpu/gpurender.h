#pragma once
#include "render/statefulrender.h"
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

class GPURender : public StatefulRender{
public:
	GPURender(const Config &conf, SDL_GPUDevice *device, SDL_Window *window);
	~GPURender();

	int maxTextureSize() const;
	bool repeatNpotSupported() const;
	const char *apiName() const;

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
	struct FlushState;
	struct ResolvedTarget;

	struct PipelineKey{
		int pipeline;
		int blend;
		int format;

		bool operator<(const PipelineKey &o) const;
	};

	GpuTexture *texture(uint32_t id);
	void createGpuTexture(GpuTexture &tex, int w, int h);
	void pushDraw(const GpuCommand &c);
	void recordUpload(uint32_t id, int x, int y, int w, int h, std::vector<uint8_t> &data);
	void recordClear(uint32_t id, const Vec4 &color);
	GpuBlend currentBlend() const;
	void fillCommandState(GpuCommand &cmd, const EffectUniforms &u, int effect);
	void recordQuad(const GpuVertex verts[4], int effect, GpuBlend blend, const SDL_Rect &viewport, bool scissor, const SDL_Rect &scissorRect, uint32_t target);
	void flush();
	void ensureVertexCapacity(size_t bytes);
	void ensureIndexBuffer(size_t quads);
	SDL_GPUGraphicsPipeline *pipelineFor(int pipeline, GpuBlend blend, SDL_GPUTextureFormat format);
	SDL_GPUSampler *samplerFor(int flags);
	void releasePending();
	Vec2i windowSize();
	Vec2i targetSize();
	SDL_GPUTransferBuffer *createUploadBuffer(const void *data, size_t size);
	void uploadVertexStream(SDL_GPUCommandBuffer *cmd);
	void endPass(FlushState &s);
	void beginPass(FlushState &s, const ResolvedTarget &r, uint32_t id, SDL_GPULoadOp loadOp, const float *color);
	bool resolveTarget(FlushState &s, uint32_t id, ResolvedTarget &out);
	void executeUpload(FlushState &s, const GpuCommand &c);
	void executeDraw(FlushState &s, const GpuCommand &c, const ResolvedTarget &r);

	SDL_GPUDevice *device;
	SDL_Window *window;
	SDL_GPUShader *vertexShader;
	SDL_GPUShader *uberShader;
	SDL_GPUShader *lightShader;
	std::map<PipelineKey, SDL_GPUGraphicsPipeline*> pipelines;
	SDL_GPUSampler *samplers[4];
	SDL_GPUTexture *dummy;

	HandlePool<GpuTexture> textures;
	std::vector<uint32_t> quarantine;
	HandlePool<GpuGeometry> geometries;

	GpuTexture *target;
	uint32_t targetId;
	int maxTexSize;
	std::string apiNameStr;

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
