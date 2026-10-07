#pragma once
#include "render/irender.h"
#include "glstate.h"
#include "gl-meta.h"
#include "shader.h"
#include <memory>
#include <vector>
#include "sharedstate.h"

typedef void *GLContextHandle;

class GLRender : public IRender{
public:
	GLRender();
	~GLRender();

	GLState &state() { return glStateObj; }

	int maxTextureSize() const;
	bool repeatNpotSupported() const;
	const char *apiName() const;

	TexHandle createTexture(int w, int h, PixelFormat fmt);
	void resizeTexture(TexHandle tex, int w, int h, PixelFormat fmt);
	void destroyTexture(TexHandle tex);
	void bindTexture(TexHandle tex);
	void unbindTexture();
	void setTextureSmooth(TexHandle tex, const bool smooth);
	void setTextureRepeat(TexHandle tex, const bool repeat);
	void uploadTexture(TexHandle tex, int w, int h, const void *pixels, PixelFormat fmt);
	void uploadTextureRect(TexHandle tex, int x, int y, int w, int h, const void *pixels, PixelFormat fmt);
	void uploadTextureRect(TexHandle tex, int dstX, int dstY, int w, int h, SDL_Surface *src, int srcX, int srcY);

	RenderTarget createRenderTarget(const int w, const int h);
	void resizeRenderTarget(RenderTarget &target, const int w, const int h);
	void destroyRenderTarget(RenderTarget &target);
	void bindRenderTarget(const RenderTarget &target);
	void bindScreenTarget();
	void clear();
	void readPixels(const RenderTarget &target, int w, int h, void *out);

	void setViewport(const IntRect &rect);
	void pushViewport(const IntRect &rect);
	void popViewport();
	void refreshViewport();

	void pushBlend(const bool enabled);
	void popBlend();

	void pushBlendMode(BlendType mode);
	void popBlendMode();
	void setBlendOverride(BlendOverride mode);
	void refreshBlendMode();

	void pushScissorTest(const bool enabled);
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
	void setTime(const float value);
	void setEffectTexture(EffectTexture slot, TexHandle tex);
	void setSpriteMat(const float value[16]);
	void setMatrix(const float value[16]);
	void setTone(const Vec4 &value);
	void setColor(const Vec4 &value);
	void setFlash(const Vec4 &value);
	void setModulate(const Vec4 &value);
	void setOpacity(const float value);
	void setBushDepth(const float value);
	void setBushOpacity(const float value);
	void setGray(const float value);
	void setHueAdjust(const float value);
	void setAniIndex(const int value);
	void setOffset(const Vec2i &value);
	void setSubRect(const FloatRect &value);
	void setProg(const float value);
	void setVague(const float value);
	void setWallMapResolution(const int x, const int y);
	void setCameraPosition(const int x, const int y);
	void setTileMapOffset(const int x, const int y);
	void setLightSources(const std::vector<LightSource> &sources);
	void setAmbient(const float value);
	void setScale(const float value);

	void beginBlit(const RenderTarget &target);
	void beginBlitScreen(const Vec2i &size);
	void blitSource(const RenderTarget &source);
	void blitRect(const IntRect &src, const Vec2i &dstPos);
	void blitRect(const IntRect &src, const IntRect &dst, const bool smooth);
	void endBlit();

	void swapWindow(SDL_Window *window);
	void suspendContext(SDL_Window *window);
	void resumeContext(SDL_Window *window);

private:
	void beginBlitTo(FboHandle fbo, const Vec2i &size);

	struct Geometry{
		VBO::ID vbo;
		GLMeta::VAO vao;
	};

	GLState glStateObj;
	GLContextHandle context;
	std::vector<Geometry> geometries;
	std::vector<uint32_t> freeGeometries;
	IBO::ID quadIbo;
	std::vector<uint16_t> quadIndices;

	std::unique_ptr<ShaderSet> shaders;
	ShaderBase *effectBases[SHADER_COUNT];
	ShaderType currentEffect;
	ShaderBase *current;

	SpriteShaderBase &spriteBase();
};

#define glState static_cast<GLRender&>(activeRender()).state()
