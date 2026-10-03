/*
** bitmap.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bitmap.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_iostream.h>
#include <pixman.h>
#include "quad.h"
#include "quadarray.h"
#include "transform.h"
#include "exception.h"
#include "config.h"
#include "meow.h"
#include "sharedstate.h"
#include "texpool.h"
#include "shader.h"
#include "filesystem.h"
#include "font.h"
#include "eventthread.h"

#define GUARD_MEGA \
	{ \
		if (p->megaSurface) \
			ErrorMsg(Exception::MKXPError, "Operation not supported for mega surfaces"); \
	}

#define OUTLINE_SIZE 1

/* Normalize (= ensure width and
 * height are positive) */
static IntRect normalizedRect(const IntRect &rect){
	IntRect norm = rect;
	if (norm.w < 0){
		norm.w = -norm.w;
		norm.x -= norm.w;
	}

	if (norm.h < 0){
		norm.h = -norm.h;
		norm.y -= norm.h;
	}

	return norm;
}

struct BitmapPrivate{
	Bitmap *self;
	IRender &render;
	RenderTarget gl;
	Font *font;

	/* "Mega surfaces" are a hack to allow Tilesets to be used
	 * whose Bitmaps don't fit into a regular texture. They're
	 * kept in RAM and will throw an error if they're used in
	 * any context other than as Tilesets */
	SDL_Surface *megaSurface;

	/* A cached version of the bitmap in client memory, for
	 * getPixel calls. Is invalidated any time the bitmap
	 * is modified */
	SDL_Surface *surface;
	SDL_PixelFormatDetails* format;

	/* The 'tainted' area describes which parts of the
	 * bitmap are not cleared, ie. don't have 0 opacity.
	 * If we're blitting / drawing text to a cleared part
	 * with full opacity, we can disregard any old contents
	 * in the texture and blit to it directly, saving
	 * ourselves the expensive blending calculation */
	pixman_region16_t tainted;

	BitmapPrivate(Bitmap *self) : self(self), render(shState->render()), megaSurface(0), surface(0){
		format = (SDL_PixelFormatDetails*)SDL_GetPixelFormatDetails(SDL_PixelFormat::SDL_PIXELFORMAT_ABGR8888);
		font = &shState->defaultFont();
		pixman_region_init(&tainted);
	}

	~BitmapPrivate(){
		pixman_region_fini(&tainted);
	}

	void allocSurface(){
		surface = SDL_CreateSurface(gl.width, gl.height, format->format);
	}

	void clearTaintedArea(){
		pixman_region_fini(&tainted);
		pixman_region_init(&tainted);
	}

	void addTaintedArea(const IntRect &rect){
		IntRect norm = normalizedRect(rect);
		pixman_region_union_rect(&tainted, &tainted, norm.x, norm.y, norm.w, norm.h);
	}

	void substractTaintedArea(const IntRect &rect){
		if (!touchesTaintedArea(rect))
			return;

		pixman_region16_t m_reg;
		pixman_region_init_rect(&m_reg, rect.x, rect.y, rect.w, rect.h);
		pixman_region_subtract(&tainted, &m_reg, &tainted);
		pixman_region_fini(&m_reg);
	}

	bool touchesTaintedArea(const IntRect &rect){
		pixman_box16_t box;
		box.x1 = rect.x;
		box.y1 = rect.y;
		box.x2 = rect.x + rect.w;
		box.y2 = rect.y + rect.h;
		
		pixman_region_overlap_t result = pixman_region_contains_rectangle(&tainted, &box);
		return result != PIXMAN_REGION_OUT;
	}

	void bindTexture(ShaderBase &shader){
		render.bindTexture(gl.tex);
		shader.setTexSize(Vec2i(gl.width, gl.height));
	}

	void bindFBO(){ render.bindRenderTarget(gl); }

	void pushSetViewport(ShaderBase &shader) const{
		render.pushViewport(IntRect(0, 0, gl.width, gl.height));
		shader.applyViewportProj();
	}

	void popViewport() const{
		render.popViewport();
	}

	void blitQuad(Quad &quad){
		render.pushBlend(false);
		quad.draw();
		render.popBlend();
	}

	void fillRect(const IntRect &rect, const Vec4 &color) {
		bindFBO();

		render.pushScissorTest(true);
		render.pushScissorBox(normalizedRect(rect));
		render.pushClearColor(color);

		render.clear();

		render.popClearColor();
		render.popScissorBox();
		render.popScissorTest();
	}

	static void ensureFormat(SDL_Surface *&surf, SDL_PixelFormat format){
		if (surf->format == format)
			return;

		SDL_Surface *surfConv = SDL_ConvertSurface(surf, format);
		SDL_DestroySurface(surf);
		surf = surfConv;
	}

	void onModified(bool freeSurface = true){
		if (surface && freeSurface){
			SDL_DestroySurface(surface);
			surface = 0;
		}

		self->modified();
	}
};

struct BitmapOpenHandler : FileSystem::OpenHandler{
	SDL_Surface *surf;
	BitmapOpenHandler() : surf(0){}

	bool tryRead(SDL_IOStream* &ops, const char *ext){
		surf = IMG_LoadTyped_IO(ops, 1, ext);
		return surf != 0;
	}
};

Bitmap::Bitmap(const char *filename){
	BitmapOpenHandler handler;
	shState->fileSystem().openRead(handler, filename);
	SDL_Surface *imgSurf = handler.surf;

	if (!imgSurf)
		ErrorMsg(Exception::SDLError, "Error loading image '%s': %s", filename, SDL_GetError());

	p->ensureFormat(imgSurf, SDL_PIXELFORMAT_ABGR8888);
	if (imgSurf->w > shState->render().maxTextureSize() || imgSurf->h > shState->render().maxTextureSize()){
		/* Mega surface */
		p = new BitmapPrivate(this);
		p->megaSurface = imgSurf;
		SDL_SetSurfaceBlendMode(p->megaSurface, SDL_BLENDMODE_NONE);
	}else{
		/* Regular surface */
		RenderTarget tex;

		try{
			tex = shState->texPool().request(imgSurf->w, imgSurf->h);
		}catch (const Exception &e){
			SDL_DestroySurface(imgSurf);
			throw e;
		}

		p = new BitmapPrivate(this);
		p->gl = tex;

		p->render.uploadTexture(p->gl.tex, p->gl.width, p->gl.height, imgSurf->pixels);

		SDL_DestroySurface(imgSurf);
	}
	p->addTaintedArea(rect());
}

Bitmap::Bitmap(SDL_IOStream *src){
	SDL_Surface *imgSurf = IMG_Load_IO(src, false);
	p->ensureFormat(imgSurf, SDL_PIXELFORMAT_ABGR8888);
	if (imgSurf->w > shState->render().maxTextureSize() || imgSurf->h > shState->render().maxTextureSize()){
		/* Mega surface */
		p = new BitmapPrivate(this);
		p->megaSurface = imgSurf;
		SDL_SetSurfaceBlendMode(p->megaSurface, SDL_BLENDMODE_NONE);
	}else{
		/* Regular surface */
		RenderTarget tex;
		try{
			tex = shState->texPool().request(imgSurf->w, imgSurf->h);
		}catch (const Exception &e){
			SDL_DestroySurface(imgSurf);
			throw e;
		}

		p = new BitmapPrivate(this);
		p->gl = tex;
		p->render.uploadTexture(p->gl.tex, p->gl.width, p->gl.height, imgSurf->pixels);
		SDL_DestroySurface(imgSurf);
	}
	p->addTaintedArea(rect());
}

Bitmap::Bitmap(int width, int height){
	if (width <= 0 || height <= 0)
		ErrorMsg(Exception::RGSSError, "failed to create bitmap"); 

	RenderTarget tex = shState->texPool().request(width, height);
	p = new BitmapPrivate(this);
	p->gl = tex;
	clear();
}

Bitmap::Bitmap(const Bitmap &other){
	other.ensureNonMega();
	p = new BitmapPrivate(this);
	p->gl = shState->texPool().request(other.width(), other.height());
	blt(0, 0, other, rect());
}

Bitmap::~Bitmap(){
	dispose();
}

int Bitmap::width() const{
	guardDisposed();
	if (p->megaSurface)
		return p->megaSurface->w;

	return p->gl.width;
}

int Bitmap::height() const{
	guardDisposed();

	if (p->megaSurface)
		return p->megaSurface->h;

	return p->gl.height;
}

IntRect Bitmap::rect() const{
	guardDisposed();
	return IntRect(0, 0, width(), height());
}

void Bitmap::blt(int x, int y, const Bitmap &source, IntRect rect, int opacity){
	if (source.isDisposed())
		return;

	// FIXME: RGSS allows the source rect to both lie outside
	// the bitmap rect and be inverted in both directions;
	// clamping only covers a subset of these cases (and
	// doesn't fix anything for a direct stretch_blt call).

	/* Clamp rect to source bitmap size */
	if (rect.x + rect.w > source.width())
		rect.w = source.width() - rect.x;

	if (rect.y + rect.h > source.height())
		rect.h = source.height() - rect.y;

	stretchBlt(IntRect(x, y, rect.w, rect.h),
	           source, rect, opacity);
}

void Bitmap::stretchBlt(const IntRect &destRect, const Bitmap &source, const IntRect &sourceRect, int opacity) {
	guardDisposed();
	GUARD_MEGA;
	if (source.isDisposed())
		return;

	opacity = clamp(opacity, 0, 255);
	if (opacity == 0)
		return;

	SDL_Surface *srcSurf = source.megaSurface();
	if (srcSurf && shState->config().subImageFix){
		// Blit from software surface, for broken GL drivers
		Vec2i gpTexSize;
		shState->ensureTexSize(sourceRect.w, sourceRect.h, gpTexSize);
		TexHandle globalTex = shState->bindTex();

		p->render.uploadTextureRect(globalTex, 0, 0, sourceRect.w, sourceRect.h, srcSurf, sourceRect.x, sourceRect.y);

		SimpleShader &shader = shState->shaders().simple;
		shader.bind();
		shader.setTranslation(Vec2i());
		shader.setTexSize(gpTexSize);

		p->pushSetViewport(shader);
		p->bindFBO();

		Quad &quad = shState->gpQuad();
		quad.setTexRect(FloatRect(0, 0, sourceRect.w, sourceRect.h));
		quad.setPosRect(destRect);

		p->blitQuad(quad);
		p->popViewport();
		p->addTaintedArea(destRect);
		p->onModified();
		return;
	}else if (srcSurf){
		// Blit from software surface
		// Don't do transparent blits for now
		if (opacity < 255)
			source.ensureNonMega();

		SDL_Rect srcRect = sourceRect;
		SDL_Rect dstRect = destRect;
		SDL_Rect btmRect = { 0, 0, width(), height() };
		SDL_Rect bltRect;
		if (SDL_GetRectIntersection(&btmRect, &dstRect, &bltRect) != true)
			return;

		int bpp;
		Uint32 rMask, gMask, bMask, aMask;
		SDL_GetMasksForPixelFormat(SDL_PIXELFORMAT_ABGR8888, &bpp, &rMask, &gMask, &bMask, &aMask);
		SDL_Surface *blitTemp = SDL_CreateSurface(destRect.w, destRect.h, SDL_GetPixelFormatForMasks(bpp, rMask, gMask, bMask, aMask));

		SDL_BlitSurfaceScaled(srcSurf, &srcRect, blitTemp, NULL, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

		if (bltRect.w == dstRect.w && bltRect.h == dstRect.h){
			// Dest rectangle lies within bounding box
			p->render.uploadTextureRect(p->gl.tex, destRect.x, destRect.y,
			                            destRect.w, destRect.h,
			                            blitTemp->pixels);
		}else{
			// Clipped blit
			p->render.uploadTextureRect(p->gl.tex, bltRect.x, bltRect.y, bltRect.w, bltRect.h,
			                            blitTemp, bltRect.x - dstRect.x, bltRect.y - dstRect.y);
		}

		SDL_DestroySurface(blitTemp);
		p->onModified();
		return;
	}

	if (opacity == 255 && !p->touchesTaintedArea(destRect)){
		// Fast blit
		p->render.beginBlit(p->gl);
		p->render.blitSource(source.p->gl);
		p->render.blitRect(sourceRect, destRect);
		p->render.endBlit();
	}else{
		/* Fragment pipeline */
		float normOpacity = (float) opacity / 255.0f;

		RenderTarget &gpTex = shState->gpTexFBO(destRect.w, destRect.h);

		p->render.beginBlit(gpTex);
		p->render.blitSource(p->gl);
		p->render.blitRect(destRect, Vec2i());
		p->render.endBlit();

		FloatRect bltSubRect((float) sourceRect.x / source.width(),
		                     (float) sourceRect.y / source.height(),
		                     ((float) source.width() / sourceRect.w) * ((float) destRect.w / gpTex.width),
		                     ((float) source.height() / sourceRect.h) * ((float) destRect.h / gpTex.height));

		BltShader &shader = shState->shaders().blt;
		shader.bind();
		shader.setDestination(gpTex.tex);
		shader.setSubRect(bltSubRect);
		shader.setOpacity(normOpacity);

		Quad &quad = shState->gpQuad();
		quad.setTexPosRect(sourceRect, destRect);
		quad.setColor(Vec4(1, 1, 1, normOpacity));

		source.p->bindTexture(shader);
		p->bindFBO();
		p->pushSetViewport(shader);
		p->blitQuad(quad);
		p->popViewport();
	}

	p->addTaintedArea(destRect);
	p->onModified();
}

void Bitmap::fillRect(int x, int y, int width, int height, const Vec4 &color) {
	fillRect(IntRect(x, y, width, height), color);
}

void Bitmap::fillRect(const IntRect &rect, const Vec4 &color){
	guardDisposed();
	GUARD_MEGA;
	p->fillRect(rect, color);
	if (color.w == 0)
		/* Clear op */
		p->substractTaintedArea(rect);
	else
		/* Fill op */
		p->addTaintedArea(rect);

	p->onModified();
}

void Bitmap::gradientFillRect(int x, int y, int width, int height, const Vec4 &color1, const Vec4 &color2, bool vertical) {
	gradientFillRect(IntRect(x, y, width, height), color1, color2, vertical);
}

void Bitmap::gradientFillRect(const IntRect &rect, const Vec4 &color1, const Vec4 &color2, bool vertical) {
	guardDisposed();
	GUARD_MEGA;

	SimpleColorShader &shader = shState->shaders().simpleColor;
	shader.bind();
	shader.setTranslation(Vec2i());

	Quad &quad = shState->gpQuad();

	if (vertical){
		quad.vert[0].color = color1;
		quad.vert[1].color = color1;
		quad.vert[2].color = color2;
		quad.vert[3].color = color2;
	}else{
		quad.vert[0].color = color1;
		quad.vert[3].color = color1;
		quad.vert[1].color = color2;
		quad.vert[2].color = color2;
	}

	quad.setPosRect(rect);
	p->bindFBO();
	p->pushSetViewport(shader);
	p->blitQuad(quad);
	p->popViewport();
	p->addTaintedArea(rect);
	p->onModified();
}

void Bitmap::gradientFillRect(int x, int y, int width, int height, const Vec4 &color1, const Vec4 &color2, const Vec4 &color3, const Vec4 &color4) {
	gradientFillRect(IntRect(x, y, width, height), color1, color2);
}

void Bitmap::gradientFillRect(const IntRect &rect, const Vec4 &color1, const Vec4 &color2, const Vec4 &color3, const Vec4 &color4) {
	guardDisposed();
	GUARD_MEGA;

	SimpleColorShader &shader = shState->shaders().simpleColor;
	shader.bind();
	shader.setTranslation(Vec2i());

	Quad &quad = shState->gpQuad();

	quad.vert[0].color = color1;
	quad.vert[1].color = color2;
	quad.vert[2].color = color3;
	quad.vert[3].color = color4;

	quad.setPosRect(rect);

	p->bindFBO();
	p->pushSetViewport(shader);
	p->blitQuad(quad);
	p->popViewport();
	p->addTaintedArea(rect);
	p->onModified();
}


void Bitmap::clearRect(int x, int y, int width, int height){
	clearRect(IntRect(x, y, width, height));
}

void Bitmap::clearRect(const IntRect &rect){
	guardDisposed();
	GUARD_MEGA;
	p->fillRect(rect, Vec4());
	p->onModified();
}

void Bitmap::blur(){
	guardDisposed();
	GUARD_MEGA;

	Quad &quad = shState->gpQuad();
	FloatRect rect(0, 0, width(), height());
	quad.setTexPosRect(rect, rect);

	RenderTarget auxTex = shState->texPool().request(width(), height());

	BlurShader &shader = shState->shaders().blur;
	BlurShader::HPass &pass1 = shader.pass1;
	BlurShader::VPass &pass2 = shader.pass2;

	p->render.pushBlend(false);
	p->render.pushViewport(IntRect(0, 0, width(), height()));

	p->render.bindTexture(p->gl.tex);
	p->render.bindRenderTarget(auxTex);

	pass1.bind();
	pass1.setTexSize(Vec2i(width(), height()));
	pass1.applyViewportProj();

	quad.draw();

	p->render.bindTexture(auxTex.tex);
	p->bindFBO();

	pass2.bind();
	pass2.setTexSize(Vec2i(width(), height()));
	pass2.applyViewportProj();

	quad.draw();

	p->render.popViewport();
	p->render.popBlend();

	shState->texPool().release(auxTex);

	p->onModified();
}

void Bitmap::radialBlur(int angle, int divisions){
	guardDisposed();
	GUARD_MEGA;

	angle     = clamp<int>(angle, 0, 359);
	divisions = clamp<int>(divisions, 2, 100);

	const int _width = width();
	const int _height = height();

	float angleStep = (float) angle / (divisions-1);
	float opacity   = 1.0f / divisions;
	float baseAngle = -((float) angle / 2);

	ColorQuadArray qArray;
	qArray.resize(5);

	std::vector<Vertex> &vert = qArray.vertices;

	int i = 0;

	/* Center */
	FloatRect texRect(0, 0, _width, _height);
	FloatRect posRect(0, 0, _width, _height);

	i += Quad::setTexPosRect(&vert[i*4], texRect, posRect);

	/* Upper */
	posRect = FloatRect(0, 0, _width, -_height);

	i += Quad::setTexPosRect(&vert[i*4], texRect, posRect);

	/* Lower */
	posRect = FloatRect(0, _height*2, _width, -_height);

	i += Quad::setTexPosRect(&vert[i*4], texRect, posRect);

	/* Left */
	posRect = FloatRect(0, 0, -_width, _height);

	i += Quad::setTexPosRect(&vert[i*4], texRect, posRect);

	/* Right */
	posRect = FloatRect(_width*2, 0, -_width, _height);

	i += Quad::setTexPosRect(&vert[i*4], texRect, posRect);

	for (int i = 0; i < 4*5; ++i)
		vert[i].color = Vec4(1, 1, 1, opacity);

	qArray.commit();

	RenderTarget newTex = shState->texPool().request(_width, _height);

	p->render.bindRenderTarget(newTex);

	p->render.pushClearColor(Vec4());
	p->render.clear();

	Transform trans;
	trans.setOrigin(Vec2(_width / 2.0f, _height / 2.0f));
	trans.setPosition(Vec2(_width / 2.0f, _height / 2.0f));

	p->render.pushBlendMode(BlendAddition);

	SimpleMatrixShader &shader = shState->shaders().simpleMatrix;
	shader.bind();

	p->bindTexture(shader);
	p->render.setTextureSmooth(p->gl.tex, true);

	p->pushSetViewport(shader);

	for (int i = 0; i < divisions; ++i){
		trans.setRotation(baseAngle + i*angleStep);
		shader.setMatrix(trans.getMatrix());
		qArray.draw();
	}

	p->popViewport();

	p->render.setTextureSmooth(p->gl.tex, false);

	p->render.popBlendMode();
	p->render.popClearColor();

	shState->texPool().release(p->gl);
	p->gl = newTex;

	p->onModified();
}

void Bitmap::clear(){
	guardDisposed();
	GUARD_MEGA;
	p->bindFBO();
	p->render.pushClearColor(Vec4());
	p->render.clear();
	p->render.popClearColor();
	p->clearTaintedArea();
	p->onModified();
}

static uint32_t &getPixelAt(SDL_Surface *surf, SDL_PixelFormatDetails *form, int x, int y){
	size_t offset = x * form->bytes_per_pixel + y * surf->pitch;
	uint8_t *bytes = (uint8_t*) surf->pixels + offset;
	return *((uint32_t*) bytes);
}

Color Bitmap::getPixel(int x, int y) const{
	guardDisposed();
	GUARD_MEGA;
	if (x < 0 || y < 0 || x >= width() || y >= height())
		return Vec4();

	if (!p->surface){
		p->allocSurface();
		p->render.readPixels(p->gl, width(), height(), p->surface->pixels);
	}

	uint32_t pixel = getPixelAt(p->surface, p->format, x, y);

	return Color((pixel >> p->format->Rshift) & 0xFF,
	             (pixel >> p->format->Gshift) & 0xFF,
	             (pixel >> p->format->Bshift) & 0xFF,
	             (pixel >> p->format->Ashift) & 0xFF);
}

void Bitmap::setPixel(int x, int y, const Color &color){
	guardDisposed();
	GUARD_MEGA;

    if (x < 0 || y < 0 || x >= width() || y >= height()) {
		Debug() << "Invalid pixel position " << x << " " << y << " in bitmap " << width() << "x" << height();
        return;
	}

	uint8_t pixel[] = {
		(uint8_t) clamp<double>(color.red,   0, 255),
		(uint8_t) clamp<double>(color.green, 0, 255),
		(uint8_t) clamp<double>(color.blue,  0, 255),
		(uint8_t) clamp<double>(color.alpha, 0, 255)
	};

	p->render.uploadTextureRect(p->gl.tex, x, y, 1, 1, &pixel);

	p->addTaintedArea(IntRect(x, y, 1, 1));

	/* Setting just a single pixel is no reason to throw away the
	 * whole cached surface; we can just apply the same change */

	if (p->surface){
		uint32_t &surfPixel = getPixelAt(p->surface, p->format, x, y);
		surfPixel = SDL_MapSurfaceRGBA(p->surface, pixel[0], pixel[1], pixel[2], pixel[3]);
	}

	p->onModified(false);
}

void Bitmap::hueChange(int hue){
	guardDisposed();
	GUARD_MEGA;
	if ((hue % 360) == 0)
		return;

	RenderTarget newTex = shState->texPool().request(width(), height());
	FloatRect texRect(rect());

	Quad &quad = shState->gpQuad();
	quad.setTexPosRect(texRect, texRect);
	quad.setColor(Vec4(1, 1, 1, 1));

	HueShader &shader = shState->shaders().hue;
	shader.bind();
	/* Shader expects normalized value */
	shader.setHueAdjust(wrapRange(hue, 0, 359) / 360.0f);

	p->render.bindRenderTarget(newTex);
	p->pushSetViewport(shader);
	p->bindTexture(shader);
	p->blitQuad(quad);
	p->popViewport();
	p->render.unbindTexture();
	shState->texPool().release(p->gl);
	p->gl = newTex;
	p->onModified();
}

void Bitmap::drawText(int x, int y, int width, int height, const char *str, int align) {
	drawText(IntRect(x, y, width, height), str, align);
}

static std::string fixupString(const char *str){
	std::string s(str);

	/* RMXP actually draws LF as a "missing gylph" box,
	 * but since we might have accidentally converted CRs
	 * to LFs when editing scripts on a Unix OS, treat them
	 * as white space too */
	for (size_t i = 0; i < s.size(); ++i)
		if (s[i] == '\r' || s[i] == '\n')
			s[i] = ' ';

	return s;
}

static void applyShadow(SDL_Surface *&in, const SDL_PixelFormatDetails* fm, const SDL_Color &c){
	SDL_Surface *out = SDL_CreateSurface(in->w+1, in->h+1, SDL_GetPixelFormatForMasks(fm->bits_per_pixel, fm->Rmask, fm->Gmask, fm->Bmask, fm->Amask));

	float fr = c.r / 255.0f;
	float fg = c.g / 255.0f;
	float fb = c.b / 255.0f;

	/* We allocate an output surface one pixel wider and higher than the input,
	 * (implicitly) blit a copy of the input with RGB values set to black into
	 * it with x/y offset by 1, then blend the input surface over it at origin
	 * (0,0) using the bitmap blit equation (see shader/bitmapBlit.frag) */

	for (int y = 0; y < in->h+1; ++y)
		for (int x = 0; x < in->w+1; ++x){
			/* src: input pixel, shd: shadow pixel */
			uint32_t src = 0, shd = 0;

			/* Output pixel location */
			uint32_t *outP = ((uint32_t*) ((uint8_t*) out->pixels + y*out->pitch)) + x;

			if (y < in->h && x < in->w)
				src = ((uint32_t*) ((uint8_t*) in->pixels + y*in->pitch))[x];

			if (y > 0 && x > 0)
				shd = ((uint32_t*) ((uint8_t*) in->pixels + (y-1)*in->pitch))[x-1];

			/* Set shadow pixel RGB values to 0 (black) */
			shd &= (uint32_t)fm->Amask;

			if (x == 0 || y == 0){
				*outP = src;
				continue;
			}

			if (x == in->w || y == in->h){
				*outP = shd;
				continue;
			}

			/* Input and shadow alpha values */
			uint8_t srcA, shdA;
			srcA = (src & fm->Amask) >> fm->Ashift;
			shdA = (shd & fm->Amask) >> fm->Ashift;

			if (srcA == 255 || shdA == 0){
				*outP = src;
				continue;
			}

			if (srcA == 0 && shdA == 0){
				*outP = 0;
				continue;
			}

			float fSrcA = srcA / 255.0f;
			float fShdA = shdA / 255.0f;

			/* Because opacity == 1, co1 == fSrcA */
			float co2 = fShdA * (1.0f - fSrcA);
			/* Result alpha */
			float fa = fSrcA + co2;
			/* Temp value to simplify arithmetic below */
			float co3 = fSrcA / fa;

			/* Result colors */
			uint8_t r, g, b, a;

			r = clamp<float>(fr * co3, 0, 1) * 255.0f;
			g = clamp<float>(fg * co3, 0, 1) * 255.0f;
			b = clamp<float>(fb * co3, 0, 1) * 255.0f;
			a = clamp<float>(fa, 0, 1) * 255.0f;

			*outP = SDL_MapSurfaceRGBA(out, r, g, b, a);
		}

	/* Store new surface in the input pointer */
	SDL_DestroySurface(in);
	in = out;
}

void Bitmap::drawText(const IntRect &rect, const char *str, int align){
	guardDisposed();
	GUARD_MEGA;

	std::string fixed = fixupString(str);
	str = fixed.c_str();

	if (*str == '\0')
		return;

	if (str[0] == ' ' && str[1] == '\0')
		return;

	TTF_Font *font = p->font->getSdlFont();
	const Color &fontColor = p->font->getColor();
	const Color &outColor = p->font->getOutColor();

	SDL_Color c = fontColor.toSDLColor();
	c.a = 255;

	float txtAlpha = fontColor.norm.w;

	SDL_Surface *txtSurf;

	if (conf.solidFonts)
		txtSurf = TTF_RenderText_Solid(font, str, SDL_strlen(str), c);
	else
		txtSurf = TTF_RenderText_Blended(font, str, SDL_strlen(str), c);

	p->ensureFormat(txtSurf, SDL_PIXELFORMAT_ABGR8888);

	int rawTxtSurfH = txtSurf->h;

	if (p->font->getShadow())
		applyShadow(txtSurf, p->format, c);

	/* outline using TTF_Outline and blending it together with SDL_BlitSurface
	 * FIXME: outline is forced to have the same opacity as the font color */
	if (p->font->getOutline()){
		SDL_Color co = outColor.toSDLColor();
		co.a = 255;
		SDL_Surface *outline;
		/* set the next font render to render the outline */
		TTF_SetFontOutline(font, OUTLINE_SIZE);
		if (conf.solidFonts)
			outline = TTF_RenderText_Solid(font, str, SDL_strlen(str), co);
		else
			outline = TTF_RenderText_Blended(font, str, SDL_strlen(str), co);

		p->ensureFormat(outline, SDL_PIXELFORMAT_ABGR8888);
		SDL_Rect outRect = {OUTLINE_SIZE, OUTLINE_SIZE, txtSurf->w, txtSurf->h};

		SDL_SetSurfaceBlendMode(txtSurf, SDL_BLENDMODE_BLEND);
		SDL_BlitSurface(txtSurf, NULL, outline, &outRect);
		SDL_DestroySurface(txtSurf);
		txtSurf = outline;
		/* reset outline to 0 */
		TTF_SetFontOutline(font, 0);
	}

	int alignX = rect.x;

	switch (align){
	default:
	case Left :
		break;

	case Center :
		alignX += (rect.w - txtSurf->w) / 2;
		break;

	case Right :
		alignX += rect.w - txtSurf->w;
		break;
	}

	if (alignX < rect.x)
		alignX = rect.x;

	int alignY = rect.y + (rect.h - rawTxtSurfH) / 2;
	float squeeze = (float) rect.w / txtSurf->w;

	if (squeeze > 1)
		squeeze = 1;

	FloatRect posRect(alignX, alignY, txtSurf->w * squeeze, txtSurf->h);

	Vec2i gpTexSize;
	shState->ensureTexSize(txtSurf->w, txtSurf->h, gpTexSize);

	bool fastBlit = false;
	if (fastBlit){
		if (squeeze == 1.0f && !shState->config().subImageFix){
			/* Even faster: upload directly to bitmap texture.
			 * We have to make sure the posRect lies within the texture
			 * boundaries or texSubImage will generate errors.
			 * If it partly lies outside bounds we have to upload
			 * the clipped visible part of it. */
			SDL_Rect btmRect;
			btmRect.x = btmRect.y = 0;
			btmRect.w = width();
			btmRect.h = height();

			SDL_Rect txtRect;
			txtRect.x = posRect.x;
			txtRect.y = posRect.y;
			txtRect.w = posRect.w;
			txtRect.h = posRect.h;

			SDL_Rect inters;

			/* If we have no intersection at all,
			 * there's nothing to upload to begin with */
			if (SDL_GetRectIntersection(&btmRect, &txtRect, &inters)){
				bool subImage = false;
				int subSrcX = 0, subSrcY = 0;
				if (inters.w != txtRect.w || inters.h != txtRect.h){
					/* Clip the text surface */
					subSrcX = inters.x - txtRect.x;
					subSrcY = inters.y - txtRect.y;
					subImage = true;

					posRect.x = inters.x;
					posRect.y = inters.y;
					posRect.w = inters.w;
					posRect.h = inters.h;
				}

				if (!subImage){
					p->render.uploadTextureRect(p->gl.tex, posRect.x, posRect.y, posRect.w, posRect.h, txtSurf->pixels);
				}else{
					p->render.uploadTextureRect(p->gl.tex, posRect.x, posRect.y, posRect.w, posRect.h, txtSurf, subSrcX, subSrcY);
				}
			}
		}else{
			/* Squeezing involved: need to use intermediary TexFBO */
			RenderTarget &gpTF = shState->gpTexFBO(txtSurf->w, txtSurf->h);
			p->render.uploadTextureRect(gpTF.tex, 0, 0, txtSurf->w, txtSurf->h, txtSurf->pixels);
			p->render.beginBlit(p->gl);
			p->render.blitSource(gpTF);
			p->render.blitRect(IntRect(0, 0, txtSurf->w, txtSurf->h), posRect, true);
			p->render.endBlit();
		}
	}else{
		/* Aquire a partial copy of the destination
		 * buffer we're about to render to */
		RenderTarget &gpTex2 = shState->gpTexFBO(posRect.w, posRect.h);
		p->render.beginBlit(gpTex2);
		p->render.blitSource(p->gl);
		p->render.blitRect(posRect, Vec2i());
		p->render.endBlit();

		FloatRect bltRect(0, 0, (float) (gpTexSize.x * squeeze) / gpTex2.width, (float) gpTexSize.y / gpTex2.height);

		BltShader &shader = shState->shaders().blt;
		shader.bind();
		shader.setTexSize(gpTexSize);
		shader.setSource();
		shader.setDestination(gpTex2.tex);
		shader.setSubRect(bltRect);
		shader.setOpacity(txtAlpha);

		TexHandle globalTex = shState->bindTex();
		p->render.uploadTextureRect(globalTex, 0, 0, txtSurf->w, txtSurf->h, txtSurf->pixels);
		p->render.setTextureSmooth(globalTex, true);

		Quad &quad = shState->gpQuad();
		quad.setTexRect(FloatRect(0, 0, txtSurf->w, txtSurf->h));
		quad.setPosRect(posRect);

		p->bindFBO();
		p->pushSetViewport(shader);
		p->blitQuad(quad);
		p->popViewport();
	}

	SDL_DestroySurface(txtSurf);
	p->addTaintedArea(posRect);
	p->onModified();
}

/* http://www.lemoda.net/c/utf8-to-ucs2/index.html */
static uint16_t utf8_to_ucs2(const char *_input, const char **end_ptr){
	const unsigned char *input = reinterpret_cast<const unsigned char*>(_input);
	*end_ptr = _input;
	if (input[0] == 0)
		return -1;

	if (input[0] < 0x80){
		*end_ptr = _input + 1;
		return input[0];
	}

	if ((input[0] & 0xE0) == 0xE0){
		if (input[1] == 0 || input[2] == 0)
			return -1;
		*end_ptr = _input + 3;
		return (input[0] & 0x0F)<<12 |
		       (input[1] & 0x3F)<<6  |
		       (input[2] & 0x3F);
	}

	if ((input[0] & 0xC0) == 0xC0){
		if (input[1] == 0)
			return -1;
		*end_ptr = _input + 2;
		return (input[0] & 0x1F)<<6  |
		       (input[1] & 0x3F);
	}

	return -1;
}

//TODO: replace with https://wiki.libsdl.org/SDL3_ttf/TTF_GetStringSize
IntRect Bitmap::textSize(const char *str){
	guardDisposed();
	GUARD_MEGA;
	TTF_Font *font = p->font->getSdlFont();
	std::string fixed = fixupString(str);
	str = fixed.c_str();

	// i don't know if its right migration, i didn't find any other way
	TTF_Text* text = TTF_CreateText(NULL, font, str, SDL_strlen(str));
	int w, h;
	TTF_GetTextSize(text, &w, &h);
	TTF_DestroyText(text);

	/* If str is one character long, *endPtr == 0 */
	const char *endPtr;
	uint16_t ucs2 = utf8_to_ucs2(str, &endPtr);

	/* For cursive characters, returning the advance
	 * as width yields better results */
	if (p->font->getItalic() && *endPtr == '\0')
		TTF_GetGlyphMetrics(font, ucs2, 0, 0, 0, 0, &w);

	return IntRect(0, 0, w, h);
}

DEF_ATTR_RD_SIMPLE(Bitmap, Font, Font&, *p->font)

void Bitmap::setFont(Font &value) {
	*p->font = value;
}

void Bitmap::setInitFont(Font *value) {
	p->font = value;
}

RenderTarget &Bitmap::getRenderTarget() {
	return p->gl;
}

SDL_Surface *Bitmap::megaSurface() const {
	return p->megaSurface;
}

void Bitmap::ensureNonMega() const{
	if (isDisposed())
		return;

	GUARD_MEGA;
}

void Bitmap::bindTex(ShaderBase &shader) {
	p->bindTexture(shader);
}

void Bitmap::taintArea(const IntRect &rect) {
	p->addTaintedArea(rect);
}

void Bitmap::releaseResources(){
	if (p->megaSurface)
		SDL_DestroySurface(p->megaSurface);
	else
		shState->texPool().release(p->gl);
	delete p;
}
