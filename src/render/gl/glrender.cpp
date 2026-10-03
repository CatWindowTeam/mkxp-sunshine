#include "glrender.h"
#include "gl-fun.h"
#include "gl-util.h"
#include "config.h"
#include "quad.h"
#include "shader.h"

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

GLRender::GLRender(const Config &conf)
    : glStateObj(conf),
      context(SDL_GL_GetCurrentContext())
{}

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
