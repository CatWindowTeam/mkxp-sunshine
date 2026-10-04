#include "glrender.h"
#include "render/backends.h"
#include "gl-fun.h"
#include "gl-util.h"
#include "config.h"
#include "render/renderstats.h"
#include "exception.h"
#ifndef NDEBUG
	#include "gl-debug.h"
#endif
#include "quad.h"
#include "vertex.h"
#include "shader.h"
#include <assert.h>
#include <limits>
#include <cstddef>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>

static TEX::ID glTex(TexHandle h) noexcept{
	return TEX::ID(h.id);
}

static FBO::ID glFbo(FboHandle h) noexcept{
	return FBO::ID(h.id);
}

static GLenum glFormat(PixelFormat fmt){
	return fmt == PixelFormat::Luminance ? GL_LUMINANCE : GL_RGBA;
}

static GLint glInternalFormat(PixelFormat fmt){
	return fmt == PixelFormat::Luminance ? GL_LUMINANCE8 : GL_RGBA16F;
}

#define attrOffset(type, mem) ((const GLvoid*) offsetof(type, mem))

static const VertexAttribute SimpleAttribs[] ={
	{ Shader::Position, 2, GL_FLOAT, attrOffset(SVertex, pos)    },
	{ Shader::TexCoord, 2, GL_FLOAT, attrOffset(SVertex, texPos) }
};

static const VertexAttribute ColorAttribs[] ={
	{ Shader::Color,    4, GL_FLOAT, attrOffset(CVertex, color) },
	{ Shader::Position, 2, GL_FLOAT, attrOffset(CVertex, pos)   }
};

static const VertexAttribute FullAttribs[] ={
	{ Shader::Color,    4, GL_FLOAT, attrOffset(Vertex, color)  },
	{ Shader::Position, 2, GL_FLOAT, attrOffset(Vertex, pos)    },
	{ Shader::TexCoord, 2, GL_FLOAT, attrOffset(Vertex, texPos) }
};

static void fillVertexData(GLMeta::VAO &vao, VertexLayout layout){
	switch (layout){
	case VertexLayout::Simple :
		vao.attr      = SimpleAttribs;
		vao.attrCount = sizeof(SimpleAttribs) / sizeof(SimpleAttribs[0]);
		vao.vertSize  = sizeof(SVertex);
		break;
	case VertexLayout::Color :
		vao.attr      = ColorAttribs;
		vao.attrCount = sizeof(ColorAttribs) / sizeof(ColorAttribs[0]);
		vao.vertSize  = sizeof(CVertex);
		break;
	case VertexLayout::Full :
		vao.attr      = FullAttribs;
		vao.attrCount = sizeof(FullAttribs) / sizeof(FullAttribs[0]);
		vao.vertSize  = sizeof(Vertex);
		break;
	}
}

static GLenum glUsage(GeometryUsage usage){
	return usage == GeometryUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
}

GLRender::GLRender() : glStateObj(), context(SDL_GL_GetCurrentContext()){
	setActiveRender(this);
	quadIbo = IBO::gen();
	ensureQuadIndices(1);

	shaders.reset(new ShaderSet);
	effectBases[SHADER_flatColor]    = &shaders->flatColor;
	effectBases[SHADER_simple]       = &shaders->simple;
	effectBases[SHADER_simpleColor]  = &shaders->simpleColor;
	effectBases[SHADER_simpleAlpha]  = &shaders->simpleAlpha;
	effectBases[SHADER_simpleSprite] = &shaders->simpleSprite;
	effectBases[SHADER_alphaSprite]  = &shaders->alphaSprite;
	effectBases[SHADER_sprite]       = &shaders->sprite;
	effectBases[SHADER_worldMachine] = &shaders->worldMachine;
	effectBases[SHADER_water]        = &shaders->water;
	effectBases[SHADER_crt]          = &shaders->crt;
	effectBases[SHADER_tilemapWater] = &shaders->tilemapWater;
	effectBases[SHADER_plane]        = &shaders->plane;
	effectBases[SHADER_gray]         = &shaders->gray;
	effectBases[SHADER_tilemap]      = &shaders->tilemap;
	effectBases[SHADER_flashMap]     = &shaders->flashMap;
	effectBases[SHADER_trans]        = &shaders->trans;
	effectBases[SHADER_simpleTrans]  = &shaders->simpleTrans;
	effectBases[SHADER_hue]          = &shaders->hue;
	effectBases[SHADER_blt]          = &shaders->blt;
	effectBases[SHADER_simpleMatrix] = &shaders->simpleMatrix;
	effectBases[SHADER_blur]         = &shaders->blur.pass1;
	effectBases[SHADER_obscured]     = &shaders->obscured;
	effectBases[SHADER_dynamicLight] = &shaders->dynamicLight;
	currentEffect = SHADER_simple;
	current = effectBases[SHADER_simple];
	if (gl.ReleaseShaderCompiler)
		gl.ReleaseShaderCompiler();
}

GLRender::~GLRender(){
	shaders.reset();
	IBO::del(quadIbo);
	setActiveRender(0);
}

int GLRender::maxTextureSize() const{
	return glStateObj.caps.maxTexSize;
}

const char *GLRender::apiName() const{
	return gl.glsles ? "opengles" : "opengl";
}

bool GLRender::repeatNpotSupported() const{
	return gl.npot_repeat;
}

TexHandle GLRender::createTexture(int w, int h, PixelFormat fmt){
	TEX::ID id = TEX::gen();
	TEX::bind(id);
	TEX::setRepeat(false);
	TEX::setSmooth(false);
	gl.TexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(fmt), w, h, 0, glFormat(fmt), GL_UNSIGNED_BYTE, 0);
	return TexHandle(id.gl);
}

void GLRender::resizeTexture(TexHandle tex, int w, int h, PixelFormat fmt){
	TEX::bind(glTex(tex));
	gl.TexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(fmt), w, h, 0, glFormat(fmt), GL_UNSIGNED_BYTE, 0);
}

void GLRender::destroyTexture(TexHandle tex){
	TEX::del(glTex(tex));
}

void GLRender::bindTexture(TexHandle tex){
	TEX::bind(glTex(tex));
}

void GLRender::unbindTexture(){
	TEX::unbind();
}

void GLRender::setTextureSmooth(TexHandle tex, const bool smooth){
	TEX::bind(glTex(tex));
	TEX::setSmooth(smooth);
}

void GLRender::setTextureRepeat(TexHandle tex, const bool repeat){
	TEX::bind(glTex(tex));
	TEX::setRepeat(repeat);
}

void GLRender::uploadTexture(TexHandle tex, int w, int h, const void *pixels, PixelFormat fmt){
	renderStatsUpload((size_t) w * h * (fmt == PixelFormat::RGBA ? 4 : 1));
	TEX::bind(glTex(tex));
	gl.TexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(fmt), w, h, 0, glFormat(fmt), GL_UNSIGNED_BYTE, pixels);
}

void GLRender::uploadTextureRect(TexHandle tex, int x, int y, int w, int h, const void *pixels, PixelFormat fmt){
	renderStatsUpload((size_t) w * h * (fmt == PixelFormat::RGBA ? 4 : 1));
	TEX::bind(glTex(tex));
	TEX::uploadSubImage(x, y, w, h, pixels, glFormat(fmt));
}

void GLRender::uploadTextureRect(TexHandle tex, int dstX, int dstY, int w, int h, SDL_Surface *src, int srcX, int srcY){
	renderStatsUpload((size_t) w * h * 4);
	TEX::bind(glTex(tex));
	if (gl.unpack_subimage){
		gl.PixelStorei(GL_UNPACK_ROW_LENGTH, src->w);
		gl.PixelStorei(GL_UNPACK_SKIP_PIXELS, srcX);
		gl.PixelStorei(GL_UNPACK_SKIP_ROWS, srcY);
		TEX::uploadSubImage(dstX, dstY, w, h, src->pixels, GL_RGBA);
		gl.PixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		gl.PixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		gl.PixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	}else{
		SDL_Surface *tmp = SDL_CreateSurface(w, h, src->format);
		SDL_Rect srcRect = { srcX, srcY, w, h };
		SDL_BlitSurface(src, &srcRect, tmp, 0);
		TEX::uploadSubImage(dstX, dstY, w, h, tmp->pixels, GL_RGBA);
		SDL_DestroySurface(tmp);
	}
}

RenderTarget GLRender::createRenderTarget(const int w, const int h){
	TEXFBO obj;
	TEXFBO::init(obj);
	TEXFBO::allocEmpty(obj, w, h);
	TEXFBO::linkFBO(obj);

	RenderTarget target;
	target.tex = TexHandle(obj.tex.gl);
	target.fbo = FboHandle(obj.fbo.gl);
	target.width = obj.width;
	target.height = obj.height;
	return target;
}

void GLRender::resizeRenderTarget(RenderTarget &target, const int w, const int h){
	TEX::bind(glTex(target.tex));
	TEX::allocEmpty(w, h);
	target.width = w;
	target.height = h;
}

void GLRender::destroyRenderTarget(RenderTarget &target){
	FBO::del(glFbo(target.fbo));
	TEX::del(glTex(target.tex));
}

void GLRender::bindRenderTarget(const RenderTarget &target){
	FBO::bind(glFbo(target.fbo));
}

void GLRender::bindScreenTarget(){
	FBO::unbind();
}

void GLRender::clear(){
	FBO::clear();
}

void GLRender::readPixels(const RenderTarget &target, int w, int h, void *out){
	FBO::bind(glFbo(target.fbo));
	glStateObj.viewport.pushSet(IntRect(0, 0, w, h));
	gl.ReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, out);
	glStateObj.viewport.pop();
}

void GLRender::setViewport(const IntRect &rect){
	glStateObj.viewport.set(rect);
}

void GLRender::pushViewport(const IntRect &rect){
	glStateObj.viewport.pushSet(rect);
}

void GLRender::popViewport(){
	glStateObj.viewport.pop();
}

void GLRender::refreshViewport(){
	glStateObj.viewport.refresh();
}

void GLRender::pushBlend(const bool enabled){
	glStateObj.blend.pushSet(enabled);
}

void GLRender::popBlend(){
	glStateObj.blend.pop();
}

void GLRender::pushBlendMode(BlendType mode){
	glStateObj.blendMode.pushSet(mode);
}

void GLRender::popBlendMode(){
	glStateObj.blendMode.pop();
}

void GLRender::setBlendOverride(BlendOverride mode){
	switch (mode){
	case BlendOverride::ToneAdd :
		gl.BlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
		gl.BlendEquation(GL_FUNC_ADD);
		break;

	case BlendOverride::ToneSubtract :
		gl.BlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
		gl.BlendEquation(GL_FUNC_REVERSE_SUBTRACT);
		break;

	case BlendOverride::Overlay :
		gl.BlendEquation(GL_FUNC_ADD);
		gl.BlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
		break;
	}
}

void GLRender::refreshBlendMode(){
	glStateObj.blendMode.refresh();
}

void GLRender::pushScissorTest(const bool enabled){
	glStateObj.scissorTest.pushSet(enabled);
}

void GLRender::popScissorTest(){
	glStateObj.scissorTest.pop();
}

void GLRender::pushScissorBox(const IntRect &rect){
	glStateObj.scissorBox.pushSet(rect);
}

void GLRender::saveScissorBox(){
	glStateObj.scissorBox.push();
}

void GLRender::intersectScissorBox(const IntRect &rect){
	glStateObj.scissorBox.setIntersect(rect);
}

void GLRender::popScissorBox(){
	glStateObj.scissorBox.pop();
}

void GLRender::setScissorBox(const IntRect &rect){
	glStateObj.scissorBox.set(rect);
}

const IntRect &GLRender::scissorBox(){
	return glStateObj.scissorBox.get();
}

void GLRender::pushClearColor(const Vec4 &color){
	glStateObj.clearColor.pushSet(color);
}

void GLRender::popClearColor(){
	glStateObj.clearColor.pop();
}

GeometryHandle GLRender::createGeometry(VertexLayout layout){
	Geometry geom;
	geom.vbo = VBO::gen();
	fillVertexData(geom.vao, layout);
	geom.vao.vbo = geom.vbo;
	geom.vao.ibo = quadIbo;
	GLMeta::vaoInit(geom.vao);

	uint32_t index;
	if (freeGeometries.empty()){
		geometries.push_back(geom);
		index = geometries.size();
	}else{
		index = freeGeometries.back();
		freeGeometries.pop_back();
		geometries[index-1] = geom;
	}

	return GeometryHandle(index);
}

void GLRender::destroyGeometry(GeometryHandle geom){
	Geometry &g = geometries[geom.id-1];
	GLMeta::vaoFini(g.vao);
	VBO::del(g.vbo);
	freeGeometries.push_back(geom.id);
}

void GLRender::allocGeometry(GeometryHandle geom, size_t bytes, GeometryUsage usage){
	VBO::bind(geometries[geom.id-1].vbo);
	VBO::allocEmpty(bytes, glUsage(usage));
	VBO::unbind();
}

void GLRender::uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage usage){
	VBO::bind(geometries[geom.id-1].vbo);
	VBO::uploadData(bytes, data, glUsage(usage));
	VBO::unbind();
}

void GLRender::uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data){
	VBO::bind(geometries[geom.id-1].vbo);
	VBO::uploadSubData(offset, bytes, data);
	VBO::unbind();
}

void GLRender::ensureQuadIndices(size_t quadCount){
	assert(quadCount*6 < std::numeric_limits<uint16_t>::max());
	if (quadIndices.size() >= quadCount*6)
		return;

	size_t startInd = quadIndices.size() / 6;
	quadIndices.reserve(quadCount*6);
	for (size_t i = startInd; i < quadCount; ++i){
		static const uint16_t indTemp[] = { 0, 1, 2, 2, 3, 0 };
		for (size_t j = 0; j < 6; ++j)
			quadIndices.push_back(i * 4 + indTemp[j]);
	}

	IBO::bind(quadIbo);
	IBO::uploadData(quadIndices.size() * sizeof(uint16_t), quadIndices.data());
	IBO::unbind();
}

void GLRender::drawQuads(GeometryHandle geom, size_t firstQuad, size_t quadCount){
	GLMeta::VAO &vao = geometries[geom.id-1].vao;
	GLMeta::vaoBind(vao);

	++renderStats.drawCalls;
	renderStats.vertices += quadCount * 4;
	const char *offset = (const char*) 0 + firstQuad * 6 * sizeof(uint16_t);
	gl.DrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_SHORT, offset);
	GLMeta::vaoUnbind(vao);
}

void GLRender::beginBlitTo(FboHandle fbo, const Vec2i &size){
	FBO::bind(glFbo(fbo));
	glStateObj.viewport.pushSet(IntRect(0, 0, size.x, size.y));

	useEffect(SHADER_simple);
	applyViewportProj();
	setTranslation(Vec2i());
}

SpriteShaderBase &GLRender::spriteBase(){
	return *static_cast<SpriteShaderBase*>(current);
}

void GLRender::useEffect(ShaderType effect){
	currentEffect = effect;
	current = effectBases[effect];
	current->bind();
	if (effect == SHADER_blt)
		shaders->blt.setSource();
}

void GLRender::useBlurPass(int pass){
	currentEffect = SHADER_blur;
	current = pass == 0 ? static_cast<ShaderBase*>(&shaders->blur.pass1) : static_cast<ShaderBase*>(&shaders->blur.pass2);
	current->bind();
}

void GLRender::applyViewportProj(){
	current->applyViewportProj();
}

void GLRender::applyPerspectiveProj(){
	current->applyPerspectiveProj();
}

void GLRender::setTexSize(const Vec2i &size){
	current->setTexSize(size);
}

void GLRender::setTranslation(const Vec2i &value){
	current->setTranslation(value);
}

void GLRender::setTime(const float value){
	current->setTime(value);
}

void GLRender::setEffectTexture(EffectTexture slot, TexHandle tex){
	switch (currentEffect){
	case SHADER_water :
		shaders->water.setNoiseTexture(tex);
		break;

	case SHADER_tilemapWater :
		shaders->tilemapWater.setNoiseTexture(tex);
		break;

	case SHADER_obscured :
		shaders->obscured.setObscured(tex);
		break;

	case SHADER_blt :
		shaders->blt.setDestination(tex);
		break;

	case SHADER_dynamicLight :
		shaders->dynamicLight.setWallMapTexture(tex);
		break;

	case SHADER_trans :
		if (slot == EffectTexture::Current)
			shaders->trans.setCurrentScene(tex);
		else if (slot == EffectTexture::Frozen)
			shaders->trans.setFrozenScene(tex);
		else
			shaders->trans.setTransMap(tex);
		break;

	case SHADER_simpleTrans :
		if (slot == EffectTexture::Current)
			shaders->simpleTrans.setCurrentScene(tex);
		else
			shaders->simpleTrans.setFrozenScene(tex);
		break;

	default :
		break;
	}
}

void GLRender::setSpriteMat(const float value[16]){
	switch (currentEffect){
	case SHADER_simpleSprite :
		shaders->simpleSprite.setSpriteMat(value);
		break;

	case SHADER_alphaSprite :
		shaders->alphaSprite.setSpriteMat(value);
		break;

	default :
		spriteBase().setSpriteMat(value);
		break;
	}
}

void GLRender::setMatrix(const float value[16]){
	shaders->simpleMatrix.setMatrix(value);
}

void GLRender::setTone(const Vec4 &value){
	if (currentEffect == SHADER_plane)
		shaders->plane.setTone(value);
	else
		spriteBase().setTone(value);
}

void GLRender::setColor(const Vec4 &value){
	switch (currentEffect){
	case SHADER_flatColor :
		shaders->flatColor.setColor(value);
		break;

	case SHADER_plane :
		shaders->plane.setColor(value);
		break;

	default :
		spriteBase().setColor(value);
		break;
	}
}

void GLRender::setFlash(const Vec4 &value){
	shaders->plane.setFlash(value);
}

void GLRender::setModulate(const Vec4 &value){
	spriteBase().setModulate(value);
}

void GLRender::setOpacity(const float value){
	switch (currentEffect){
	case SHADER_alphaSprite :
		shaders->alphaSprite.setAlpha(value);
		break;

	case SHADER_flashMap :
		shaders->flashMap.setAlpha(value);
		break;

	case SHADER_plane :
		shaders->plane.setOpacity(value);
		break;

	case SHADER_blt :
		shaders->blt.setOpacity(value);
		break;

	default :
		spriteBase().setOpacity(value);
		break;
	}
}

void GLRender::setBushDepth(const float value){
	spriteBase().setBushDepth(value);
}

void GLRender::setBushOpacity(const float value){
	spriteBase().setBushOpacity(value);
}

void GLRender::setGray(const float value){
	shaders->gray.setGray(value);
}

void GLRender::setHueAdjust(const float value){
	shaders->hue.setHueAdjust(value);
}

void GLRender::setAniIndex(const int value){
	if (currentEffect == SHADER_tilemapWater)
		shaders->tilemapWater.setAniIndex(value);
	else
		shaders->tilemap.setAniIndex(value);
}

void GLRender::setOffset(const Vec2i &value){
	shaders->tilemapWater.setOffset(value);
}

void GLRender::setSubRect(const FloatRect &value){
	shaders->blt.setSubRect(value);
}

void GLRender::setProg(const float value){
	if (currentEffect == SHADER_trans)
		shaders->trans.setProg(value);
	else
		shaders->simpleTrans.setProg(value);
}

void GLRender::setVague(const float value){
	shaders->trans.setVague(value);
}

void GLRender::setWallMapResolution(const int x, const int y){
	shaders->dynamicLight.setWallMapResolution(x, y);
}

void GLRender::setCameraPosition(const int x, const int y){
	shaders->dynamicLight.setCameraPosition(x, y);
}

void GLRender::setTileMapOffset(const int x, const int y){
	shaders->dynamicLight.setTileMapOffset(x, y);
}

void GLRender::setLightSources(const std::vector<LightSource> &sources){
	shaders->dynamicLight.setLightSources(sources);
}

void GLRender::setAmbient(const float value){
	shaders->dynamicLight.setAmbient(value);
}

void GLRender::beginBlit(const RenderTarget &target){
	beginBlitTo(target.fbo, Vec2i(target.width, target.height));
}

void GLRender::beginBlitScreen(const Vec2i &size){
	beginBlitTo(FboHandle(0), size);
}

void GLRender::blitSource(const RenderTarget &source){
	setTexSize(Vec2i(source.width, source.height));
	TEX::bind(glTex(source.tex));
}

void GLRender::blitRect(const IntRect &src, const Vec2i &dstPos){
	blitRect(src, IntRect(dstPos.x, dstPos.y, src.w, src.h), false);
}

void GLRender::blitRect(const IntRect &src, const IntRect &dst, const bool smooth){
	if (smooth)
		TEX::setSmooth(true);

	glStateObj.blend.pushSet(false);
	Quad &quad = shState->gpQuad();
	quad.setTexPosRect(src, dst);
	quad.draw();
	glStateObj.blend.pop();
	if (smooth)
		TEX::setSmooth(false);
}

void GLRender::endBlit(){
	glStateObj.viewport.pop();
}

void GLRender::swapWindow(SDL_Window *window){
	renderStatsBeginSwap();
	SDL_GL_SwapWindow(window);
	renderStatsEndSwap();
}

void GLRender::suspendContext(SDL_Window *window){
	SDL_GL_MakeCurrent(window, 0);
}

void GLRender::resumeContext(SDL_Window *window){
	SDL_GL_MakeCurrent(window, static_cast<SDL_GLContext>(context));
}

class GLRenderContext final : public IRenderContext{
public:
	GLRenderContext(SDL_Window *window) : context(SDL_GL_CreateContext(window)){
		if (!context)
			throw Exception(Exception::MKXPError, "Failed to create OpenGL context");

		try{
			initGLFunctions();
		}catch(...){
			SDL_GL_DestroyContext(context);
			throw;
		}

		if (!conf.enableBlitting)
			gl.BlitFramebuffer = 0;

		gl.ClearColor(0, 0, 0, 1);
		gl.Clear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);

		#ifndef NDEBUG
			debugLogger.reset(new GLDebugLogger);
		#endif
	}

	~GLRenderContext(){
		#ifndef NDEBUG
			debugLogger.reset();
		#endif
		SDL_GL_DestroyContext(context);
	}

private:
	SDL_GLContext context;
	#ifndef NDEBUG
		std::unique_ptr<GLDebugLogger> debugLogger;
	#endif
};

uint64_t glWindowFlags(){
	return SDL_WINDOW_OPENGL;
}

void glSetupWindowAttributes(){
	#if mkxp_android
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	#ifndef NDEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif
}

IRenderContext *createGLRenderContext(SDL_Window *window){
	return new GLRenderContext(window);
}

IRender *createGLRender(const Config &){
	return new GLRender();
}

bool glProbe(SDL_Window *window){
	SDL_GLContext probe = SDL_GL_CreateContext(window);
	if (!probe)
		return false;

	SDL_GL_DestroyContext(probe);
	return true;
}
