#include "glrender.h"
#include "gl-fun.h"
#include "gl-util.h"
#include "config.h"
#include "quad.h"
#include "vertex.h"
#include "shader.h"

#include <assert.h>
#include <limits>
#include <cstddef>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>

static TEX::ID glTex(TexHandle h){
	return TEX::ID(h.id);
}

static FBO::ID glFbo(FboHandle h){
	return FBO::ID(h.id);
}

static GLenum glFormat(PixelFormat fmt){
	return fmt == PixelFormat::Luminance ? GL_LUMINANCE : GL_RGBA;
}

static GLint glInternalFormat(PixelFormat fmt){
	return fmt == PixelFormat::Luminance ? GL_LUMINANCE8 : GL_RGBA16F;
}

static IRender *activeInstance = 0;

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

GLRender::GLRender(const Config &conf)
    : glStateObj(conf),
      context(SDL_GL_GetCurrentContext())
{
	activeInstance = this;
	quadIbo = IBO::gen();
	ensureQuadIndices(1);
}

GLRender::~GLRender(){
	IBO::del(quadIbo);
	activeInstance = 0;
}

IRender &activeRender(){
	return *activeInstance;
}

int GLRender::maxTextureSize() const{
	return glStateObj.caps.maxTexSize;
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

void GLRender::setTextureSmooth(TexHandle tex, bool smooth){
	TEX::bind(glTex(tex));
	TEX::setSmooth(smooth);
}

void GLRender::setTextureRepeat(TexHandle tex, bool repeat){
	TEX::bind(glTex(tex));
	TEX::setRepeat(repeat);
}

void GLRender::uploadTexture(TexHandle tex, int w, int h, const void *pixels, PixelFormat fmt){
	TEX::bind(glTex(tex));
	gl.TexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(fmt), w, h, 0, glFormat(fmt), GL_UNSIGNED_BYTE, pixels);
}

void GLRender::uploadTextureRect(TexHandle tex, int x, int y, int w, int h, const void *pixels, PixelFormat fmt){
	TEX::bind(glTex(tex));
	TEX::uploadSubImage(x, y, w, h, pixels, glFormat(fmt));
}

void GLRender::uploadTextureRect(TexHandle tex, int dstX, int dstY, int w, int h, SDL_Surface *src, int srcX, int srcY){
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

RenderTarget GLRender::createRenderTarget(int w, int h){
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

void GLRender::resizeRenderTarget(RenderTarget &target, int w, int h){
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

void GLRender::pushBlend(bool enabled){
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

void GLRender::pushScissorTest(bool enabled){
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

	const char *offset = (const char*) 0 + firstQuad * 6 * sizeof(uint16_t);
	gl.DrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_SHORT, offset);
	GLMeta::vaoUnbind(vao);
}

void GLRender::beginBlitTo(FboHandle fbo, const Vec2i &size){
	FBO::bind(glFbo(fbo));
	glStateObj.viewport.pushSet(IntRect(0, 0, size.x, size.y));

	SimpleShader &shader = shState->shaders().simple;
	shader.bind();
	shader.applyViewportProj();
	shader.setTranslation(Vec2i());
}

void GLRender::beginBlit(const RenderTarget &target){
	beginBlitTo(target.fbo, Vec2i(target.width, target.height));
}

void GLRender::beginBlitScreen(const Vec2i &size){
	beginBlitTo(FboHandle(0), size);
}

void GLRender::blitSource(const RenderTarget &source){
	SimpleShader &shader = shState->shaders().simple;
	shader.setTexSize(Vec2i(source.width, source.height));
	TEX::bind(glTex(source.tex));
}

void GLRender::blitRect(const IntRect &src, const Vec2i &dstPos){
	blitRect(src, IntRect(dstPos.x, dstPos.y, src.w, src.h), false);
}

void GLRender::blitRect(const IntRect &src, const IntRect &dst, bool smooth){
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
	SDL_GL_SwapWindow(window);
}

void GLRender::suspendContext(SDL_Window *window){
	SDL_GL_MakeCurrent(window, 0);
}

void GLRender::resumeContext(SDL_Window *window){
	SDL_GL_MakeCurrent(window, static_cast<SDL_GLContext>(context));
}

IRender *createRender(const Config &conf){
	return new GLRender(conf);
}
