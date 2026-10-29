#ifdef RENDER_SIMPLE

#include "sdlrender.h"
#include "render/backends.h"
#include "config.h"
#include "render/renderstats.h"
#include "exception.h"
#include "debugwriter.h"
#include "vertex.h"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_properties.h>
#include <algorithm>
#include <math.h>
#include <string.h>

static SDL_Renderer *createdRenderer = 0;

static SDL_FColor fcolor(float r, float g, float b, float a){
	SDL_FColor c = { r, g, b, a };
	return c;
}

static float clamp01(float v){
	return std::min(std::max(v, 0.0f), 1.0f);
}

static SDL_FColor applyColorKeep(SDL_FColor c, const EffectUniforms &u){
	const float keep = 1.0f - clamp01(u.color.w);
	c.r *= keep * (1.0f + std::min(std::max(u.tone.x, -1.0f), 0.0f));
	c.g *= keep * (1.0f + std::min(std::max(u.tone.y, -1.0f), 0.0f));
	c.b *= keep * (1.0f + std::min(std::max(u.tone.z, -1.0f), 0.0f));
	return c;
}

static SDL_FColor additiveColor(SDL_FColor c, const EffectUniforms &u){
	const float a = clamp01(u.color.w);
	c.r = clamp01(std::max(u.tone.x, 0.0f) + u.color.x * a);
	c.g = clamp01(std::max(u.tone.y, 0.0f) + u.color.y * a);
	c.b = clamp01(std::max(u.tone.z, 0.0f) + u.color.z * a);
	return c;
}

static float fractPart(float v){
	return v - floorf(v);
}

static void rgbToHsv(float r, float g, float b, float &h, float &s, float &v){
	const float mx = std::max(r, std::max(g, b));
	const float mn = std::min(r, std::min(g, b));
	const float d = mx - mn;

	v = mx;
	s = d / (mx + 1.0e-10f);
	h = 0;

	if (d == 0)
		return;

	if (mx == r)
		h = fmodf((g - b) / d + 6.0f, 6.0f);
	else if (mx == g)
		h = (b - r) / d + 2.0f;
	else
		h = (r - g) / d + 4.0f;

	h /= 6.0f;
}

static float hsvChannel(float h, float s, float v, float offset){
	const float p = fabsf(fractPart(h + offset) * 6.0f - 3.0f);
	return v * (1.0f - s + s * clamp01(p - 1.0f));
}

static void hsvToRgb(float h, float s, float v, float &r, float &g, float &b){
	r = hsvChannel(h, s, v, 1.0f);
	g = hsvChannel(h, s, v, 2.0f / 3.0f);
	b = hsvChannel(h, s, v, 1.0f / 3.0f);
}

static uint8_t toByte(float v){
	return (uint8_t) (clamp01(v) * 255.0f + 0.5f);
}

static void applyHue(std::vector<uint8_t> &px, float hueAdjust){
	for (size_t i = 0; i < px.size(); i += 4){
		float h, s, v, r, g, b;
		rgbToHsv(px[i] / 255.0f, px[i+1] / 255.0f, px[i+2] / 255.0f, h, s, v);
		hsvToRgb(h + hueAdjust, s, v, r, g, b);
		px[i] = toByte(r);
		px[i+1] = toByte(g);
		px[i+2] = toByte(b);
	}
}

static void applyGray(std::vector<uint8_t> &px, float amount){
	for (size_t i = 0; i < px.size(); i += 4){
		const float luma = 0.299f * px[i] + 0.587f * px[i+1] + 0.114f * px[i+2];

		for (int c = 0; c < 3; ++c)
			px[i+c] = toByte((px[i+c] + (luma - px[i+c]) * amount) / 255.0f);
	}
}

static void applyBlur(const std::vector<uint8_t> &src, std::vector<uint8_t> &dst, int w, int h, bool vertical){
	for (int y = 0; y < h; ++y){
		for (int x = 0; x < w; ++x){
			const int xa = vertical ? x : std::max(x - 1, 0);
			const int xb = vertical ? x : std::min(x + 1, w - 1);
			const int ya = vertical ? std::max(y - 1, 0) : y;
			const int yb = vertical ? std::min(y + 1, h - 1) : y;
			const uint8_t *a = &src[((size_t) ya * w + xa) * 4];
			const uint8_t *m = &src[((size_t) y * w + x) * 4];
			const uint8_t *b = &src[((size_t) yb * w + xb) * 4];
			uint8_t *out = &dst[((size_t) y * w + x) * 4];

			for (int c = 0; c < 4; ++c)
				out[c] = (uint8_t) ((a[c] + m[c] + b[c] + 1) / 3);
		}
	}
}

SDLRender::SDLRender(const Config &conf, SDL_Renderer *renderer)
    : StatefulRender(conf),
      renderer(renderer),
      target(0),
      maxTexSize(0),
      stateValid(false),
      appliedTarget(0),
      appliedClipOn(false)
{
	SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
	maxTexSize = (int) SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 4096);
	if (maxTexSize <= 0 || SDL_strcmp(SDL_GetRendererName(renderer), "software") == 0)
		maxTexSize = std::max(maxTexSize, 16384);

	const char *rendererName = SDL_GetRendererName(renderer);
	apiNameStr = rendererName ? rendererName : "unknown";

	reverseSubtract = apiNameStr != "opengl" && apiNameStr != "software";

	setActiveRender(this);
}

SDLRender::~SDLRender(){
	setActiveRender(0);

	for (size_t i = 0; i < textures.size(); ++i)
		if (textures.at(i) && textures.at(i)->texture)
			SDL_DestroyTexture(textures.at(i)->texture);
}

int SDLRender::maxTextureSize() const{
	return maxTexSize;
}

const char *SDLRender::apiName() const{
	return apiNameStr.c_str();
}

bool SDLRender::repeatNpotSupported() const{
	return true;
}

SdlTexture *SDLRender::texture(uint32_t id){
	return textures.get(id);
}

void SDLRender::createSdlTexture(SdlTexture &tex, int w, int h){
	tex.width = w;
	tex.height = h;
	tex.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);

	if (!tex.texture){
		Debug() << "[SDLRender] cannot create texture" << w << h << SDL_GetError();
		return;
	}

	SDL_SetTextureBlendMode(tex.texture, SDL_BLENDMODE_BLEND);

	SDL_Texture *previous = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, tex.texture);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, previous);
	stateValid = false;
}

TexHandle SDLRender::createTexture(int w, int h, PixelFormat){
	std::unique_ptr<SdlTexture> tex(new SdlTexture);
	createSdlTexture(*tex, w, h);

	return TexHandle(textures.add(std::move(tex)));
}

void SDLRender::resizeTexture(TexHandle handle, int w, int h, PixelFormat){
	SdlTexture *tex = texture(handle.id);
	if (!tex)
		return;

	flushIfUses(tex->texture);

	if (tex->texture)
		SDL_DestroyTexture(tex->texture);

	stateValid = false;
	createSdlTexture(*tex, w, h);
}

void SDLRender::destroyTexture(TexHandle handle){
	SdlTexture *tex = texture(handle.id);
	if (!tex)
		return;

	if (target == tex)
		target = 0;

	if (boundTexture == handle.id)
		boundTexture = 0;

	flushIfUses(tex->texture);

	if (tex->texture)
		SDL_DestroyTexture(tex->texture);

	stateValid = false;
	textures.remove(handle.id);
}

void SDLRender::setTextureSmooth(TexHandle handle, bool smooth){
	boundTexture = handle.id;

	if (SdlTexture *tex = texture(handle.id))
		tex->smooth = smooth;
}

void SDLRender::setTextureRepeat(TexHandle handle, bool repeat){
	boundTexture = handle.id;

	if (SdlTexture *tex = texture(handle.id))
		tex->repeat = repeat;
}

void SDLRender::uploadTexture(TexHandle handle, int w, int h, const void *pixels, PixelFormat fmt){
	resizeTexture(handle, w, h, fmt);
	uploadTextureRect(handle, 0, 0, w, h, pixels, fmt);
}

void SDLRender::uploadTextureRect(TexHandle handle, int x, int y, int w, int h, const void *pixels, PixelFormat fmt){
	SdlTexture *tex = texture(handle.id);
	boundTexture = handle.id;

	if (!tex || !tex->texture || !pixels)
		return;

	flushIfUses(tex->texture);
	renderStatsUpload((size_t) w * h * 4);
	SDL_Rect rect = { x, y, w, h };

	if (fmt == PixelFormat::Luminance){
		const uint8_t *src = static_cast<const uint8_t*>(pixels);
		std::vector<uint8_t> expanded((size_t) w * h * 4);

		for (size_t i = 0; i < (size_t) w * h; ++i){
			expanded[i*4+0] = expanded[i*4+1] = expanded[i*4+2] = src[i];
			expanded[i*4+3] = 255;
		}

		SDL_UpdateTexture(tex->texture, &rect, expanded.data(), w * 4);
	}else{
		SDL_UpdateTexture(tex->texture, &rect, pixels, w * 4);
	}
}

void SDLRender::uploadTextureRect(TexHandle handle, int dstX, int dstY, int w, int h, SDL_Surface *src, int srcX, int srcY){
	SdlTexture *tex = texture(handle.id);
	boundTexture = handle.id;

	if (!tex || !tex->texture)
		return;

	flushIfUses(tex->texture);
	renderStatsUpload((size_t) w * h * 4);
	SDL_Rect rect = { dstX, dstY, w, h };
	const uint8_t *base = static_cast<const uint8_t*>(src->pixels) + (size_t) srcY * src->pitch + (size_t) srcX * 4;
	SDL_UpdateTexture(tex->texture, &rect, base, src->pitch);
}

RenderTarget SDLRender::createRenderTarget(int w, int h){
	TexHandle tex = createTexture(w, h, PixelFormat::RGBA);

	RenderTarget rt;
	rt.tex = tex;
	rt.fbo = FboHandle(tex.id);
	rt.width = w;
	rt.height = h;
	return rt;
}

void SDLRender::resizeRenderTarget(RenderTarget &rt, int w, int h){
	resizeTexture(rt.tex, w, h, PixelFormat::RGBA);
	rt.width = w;
	rt.height = h;
}

void SDLRender::destroyRenderTarget(RenderTarget &rt){
	destroyTexture(rt.tex);
}

void SDLRender::bindRenderTarget(const RenderTarget &rt){
	target = texture(rt.fbo.id);
}

void SDLRender::bindScreenTarget(){
	target = 0;
}

int SDLRender::screenHeight(){
	int w = 0, h = 0;
	SDL_GetCurrentRenderOutputSize(renderer, &w, &h);
	return h;
}

void SDLRender::applyStateFor(SDL_Texture *wantTarget, const SDL_Rect &vp, bool clipOn, const SDL_Rect &box){
	const bool targetChanged = !stateValid || wantTarget != appliedTarget;

	if (targetChanged){
		SDL_SetRenderTarget(renderer, wantTarget);
		appliedTarget = wantTarget;
	}

	if (targetChanged || memcmp(&vp, &appliedViewport, sizeof(vp)) != 0){
		SDL_SetRenderViewport(renderer, &vp);
		appliedViewport = vp;
	}

	if (targetChanged || clipOn != appliedClipOn || (clipOn && memcmp(&box, &appliedClip, sizeof(box)) != 0)){
		SDL_SetRenderClipRect(renderer, clipOn ? &box : 0);
		appliedClipOn = clipOn;
		appliedClip = box;
	}

	stateValid = true;
}

void SDLRender::applyState(){
	applyStateFor(target ? target->texture : 0, viewport.current, scissorTest.current, scissorBoxProp.current);
}

void SDLRender::flush(){
	if (!batch.active)
		return;

	batch.active = false;
	applyStateFor(batch.target, batch.viewport, batch.clipOn, batch.clip);

	if (batch.src){
		SDL_SetTextureBlendMode(batch.src, batch.mode);
		SDL_SetTextureScaleMode(batch.src, batch.linear ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
	}else{
		SDL_SetRenderDrawBlendMode(renderer, batch.mode);
	}

	++renderStats.drawCalls;
	renderStats.vertices += batch.list.size();
	SDL_RenderGeometry(renderer, batch.src, batch.list.data(), (int) batch.list.size(), batch.indices.data(), (int) batch.indices.size());
}

void SDLRender::flushIfUses(SDL_Texture *tex){
	if (batch.active && tex && (batch.src == tex || batch.target == tex))
		flush();
}

void SDLRender::clear(){
	flush();
	applyState();

	const Vec4 &c = clearColor.current;
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColorFloat(renderer, c.x, c.y, c.z, c.w);

	if (scissorTest.current)
		SDL_RenderFillRect(renderer, 0);
	else
		SDL_RenderClear(renderer);
}

void SDLRender::readTexture(SdlTexture &tex, std::vector<uint8_t> &out){
	flush();
	out.assign((size_t) tex.width * tex.height * 4, 0);

	if (!tex.texture)
		return;

	SDL_Texture *previous = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, tex.texture);
	SDL_Rect clip = { 0, 0, tex.width, tex.height };
	SDL_SetRenderViewport(renderer, 0);
	SDL_SetRenderClipRect(renderer, 0);
	SDL_Surface *surf = SDL_RenderReadPixels(renderer, &clip);
	SDL_SetRenderTarget(renderer, previous);
	stateValid = false;

	if (!surf)
		return;

	SDL_Surface *conv = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
	SDL_DestroySurface(surf);

	if (!conv)
		return;

	for (int y = 0; y < tex.height; ++y)
		memcpy(&out[(size_t) y * tex.width * 4], static_cast<uint8_t*>(conv->pixels) + (size_t) y * conv->pitch, (size_t) tex.width * 4);

	SDL_DestroySurface(conv);
}

SDL_Texture *SDLRender::processTexture(SdlTexture &src, int effect, const EffectUniforms &u){
	std::vector<uint8_t> px;
	readTexture(src, px);

	if (effect == SHADER_hue){
		applyHue(px, u.hueAdjust);
	}else if (effect == SHADER_gray){
		applyGray(px, u.gray);
	}else if (effect == SHADER_sprite || effect == SHADER_plane){
		applyGray(px, u.tone.w);
	}else if (effect == SHADER_blur){
		std::vector<uint8_t> out(px.size());
		applyBlur(px, out, src.width, src.height, blurPass != 0);
		px.swap(out);
	}

	SDL_Texture *tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, src.width, src.height);
	if (!tex)
		return 0;

	SDL_UpdateTexture(tex, 0, px.data(), src.width * 4);
	return tex;
}

void SDLRender::readPixels(const RenderTarget &rt, int w, int h, void *out){
	SdlTexture *tex = texture(rt.fbo.id);
	if (!tex)
		return;

	std::vector<uint8_t> data;
	readTexture(*tex, data);

	uint8_t *dst = static_cast<uint8_t*>(out);
	int cw = std::min(w, tex->width);
	int ch = std::min(h, tex->height);

	for (int y = 0; y < ch; ++y)
		memcpy(dst + (size_t) y * w * 4, &data[(size_t) y * tex->width * 4], (size_t) cw * 4);
}

GeometryHandle SDLRender::createGeometry(VertexLayout layout){
	std::unique_ptr<SdlGeometry> geom(new SdlGeometry);
	geom->layout = layout;

	return GeometryHandle(geometries.add(std::move(geom)));
}

void SDLRender::destroyGeometry(GeometryHandle geom){
	geometries.remove(geom.id);
}

void SDLRender::allocGeometry(GeometryHandle geom, size_t bytes, GeometryUsage){
	geometries.get(geom.id)->data.assign(bytes, 0);
}

void SDLRender::uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage){
	const uint8_t *src = static_cast<const uint8_t*>(data);
	geometries.get(geom.id)->data.assign(src, src + bytes);
}

void SDLRender::uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data){
	if (bytes == 0)
		return;

	std::vector<uint8_t> &buf = geometries.get(geom.id)->data;
	if (buf.size() < offset + bytes)
		buf.resize(offset + bytes);

	memcpy(&buf[offset], data, bytes);
}

void SDLRender::ensureQuadIndices(size_t){
}

static SDL_BlendMode approxSubtractBlend(){
	return SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR, SDL_BLENDOPERATION_ADD,
	                                  SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
}

SDL_BlendMode SDLRender::blendMode(bool forceBlend){
	if (!blend.current && !forceBlend)
		return SDL_BLENDMODE_NONE;

	switch (activeBlend){
	case BlendKind::Addition :
	case BlendKind::ToneAdd :
		return SDL_BLENDMODE_ADD;

	case BlendKind::Multiply :
		return SDL_BLENDMODE_MUL;

	case BlendKind::Substraction :
		if (!reverseSubtract)
			return approxSubtractBlend();

		return SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
		                                  SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);

	case BlendKind::ToneSubtract :
		if (!reverseSubtract)
			return approxSubtractBlend();

		return SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
		                                  SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);

	default :
		return SDL_BLENDMODE_BLEND;
	}
}

void SDLRender::submit(SdlTexture *tex, const Vertices &v, SDL_BlendMode mode, bool smooth){
	if (mode == SDL_BLENDMODE_INVALID || v.list.empty())
		return;

	SDL_Texture *src = 0;
	bool linear = false;

	if (tex){
		if (!tex->texture)
			return;

		src = tex->texture;
		linear = smooth || tex->smooth;
	}

	SDL_Texture *dst = target ? target->texture : 0;
	const SDL_Rect vp = viewport.current;
	const bool clipOn = scissorTest.current;
	const SDL_Rect box = scissorBoxProp.current;

	const bool same = batch.active && src == batch.src && dst == batch.target && mode == batch.mode &&
	                  linear == batch.linear && memcmp(&vp, &batch.viewport, sizeof(vp)) == 0 &&
	                  clipOn == batch.clipOn && (!clipOn || memcmp(&box, &batch.clip, sizeof(box)) == 0);

	if (!same){
		flush();
		batch.active = true;
		batch.src = src;
		batch.target = dst;
		batch.mode = mode;
		batch.linear = linear;
		batch.viewport = vp;
		batch.clipOn = clipOn;
		batch.clip = box;
		batch.list.clear();
		batch.indices.clear();
	}

	const int base = (int) batch.list.size();
	batch.list.insert(batch.list.end(), v.list.begin(), v.list.end());

	for (size_t i = 0; i < v.indices.size(); ++i)
		batch.indices.push_back(base + v.indices[i]);
}

static SDL_Vertex lerpVertex(const SDL_Vertex &a, const SDL_Vertex &b, float t){
	SDL_Vertex out;
	out.position.x = a.position.x + (b.position.x - a.position.x) * t;
	out.position.y = a.position.y + (b.position.y - a.position.y) * t;
	out.tex_coord.x = a.tex_coord.x + (b.tex_coord.x - a.tex_coord.x) * t;
	out.tex_coord.y = a.tex_coord.y + (b.tex_coord.y - a.tex_coord.y) * t;
	out.color.r = a.color.r + (b.color.r - a.color.r) * t;
	out.color.g = a.color.g + (b.color.g - a.color.g) * t;
	out.color.b = a.color.b + (b.color.b - a.color.b) * t;
	out.color.a = a.color.a + (b.color.a - a.color.a) * t;
	return out;
}

static void appendQuadIndices(std::vector<int> &indices, int base);

static void appendBushQuad(std::vector<SDL_Vertex> &list, std::vector<int> &indices, const SDL_Vertex q[4], float opacity){
	const int base = (int) list.size();

	for (int i = 0; i < 4; ++i){
		SDL_Vertex vert = q[i];
		vert.color.a *= opacity;
		list.push_back(vert);
	}

	appendQuadIndices(indices, base);
}

static void applyBushDepth(std::vector<SDL_Vertex> &list, std::vector<int> &indices, float depth, float opacity){
	std::vector<SDL_Vertex> src;
	src.swap(list);
	indices.clear();

	for (size_t i = 0; i + 3 < src.size(); i += 4){
		const SDL_Vertex *q = &src[i];
		const float y0 = q[0].tex_coord.y;
		const float y3 = q[3].tex_coord.y;
		const bool aboveStart = y0 < depth;
		const bool aboveEnd = y3 < depth;

		if (aboveStart && aboveEnd){
			appendBushQuad(list, indices, q, 1.0f);
		}else if (!aboveStart && !aboveEnd){
			appendBushQuad(list, indices, q, opacity);
		}else{
			const float t = (depth - y0) / (y3 - y0);
			const SDL_Vertex p = lerpVertex(q[0], q[3], t);
			const SDL_Vertex r = lerpVertex(q[1], q[2], t);
			const SDL_Vertex startPart[4] = { q[0], q[1], r, p };
			const SDL_Vertex endPart[4] = { p, r, q[2], q[3] };

			appendBushQuad(list, indices, startPart, aboveStart ? 1.0f : opacity);
			appendBushQuad(list, indices, endPart, aboveEnd ? 1.0f : opacity);
		}
	}
}

static void appendQuadIndices(std::vector<int> &indices, int base){
	static const int pattern[6] = { 0, 1, 2, 2, 3, 0 };

	for (int i = 0; i < 6; ++i)
		indices.push_back(base + pattern[i]);
}

void SDLRender::drawTextureQuad(SdlTexture *tex, const SDL_Vertex corners[4], SDL_BlendMode mode, bool smooth){
	Vertices v;
	for (int i = 0; i < 4; ++i)
		v.list.push_back(corners[i]);

	appendQuadIndices(v.indices, 0);
	submit(tex, v, mode, smooth);
}

void SDLRender::drawTransition(const Vertices &v){
	const EffectUniforms &u = uniforms();
	SdlTexture *frozen = texture(u.aux[2]);
	SdlTexture *current = texture(u.aux[1]);

	if (frozen){
		Vertices copy = v;
		for (size_t i = 0; i < copy.list.size(); ++i)
			copy.list[i].color = fcolor(1, 1, 1, 1);
		submit(frozen, copy, SDL_BLENDMODE_NONE, false);
	}

	if (current){
		Vertices copy = v;
		for (size_t i = 0; i < copy.list.size(); ++i)
			copy.list[i].color = fcolor(1, 1, 1, std::min(std::max(u.prog, 0.0f), 1.0f));
		submit(current, copy, SDL_BLENDMODE_BLEND, false);
	}
}

static void transformPoint(const float m[16], float x, float y, float &ox, float &oy){
	ox = m[0] * x + m[4] * y + m[12];
	oy = m[1] * x + m[5] * y + m[13];
}

void SDLRender::drawQuads(GeometryHandle geom, size_t firstQuad, size_t quadCount){
	const SdlGeometry &g = *geometries.get(geom.id);
	const EffectUniforms &u = uniforms();
	const int effect = currentSlot;

	if (effect == SHADER_dynamicLight)
		return;

	size_t stride = 0;
	switch (g.layout){
	case VertexLayout::Simple : stride = sizeof(SVertex); break;
	case VertexLayout::Color  : stride = sizeof(CVertex); break;
	case VertexLayout::Full   : stride = sizeof(Vertex);  break;
	}

	const bool isTransition = effect == SHADER_trans || effect == SHADER_simpleTrans;
	const bool untextured = effect == SHADER_flatColor || effect == SHADER_simpleColor || effect == SHADER_flashMap;

	SdlTexture *bound = texture(boundTexture);
	if (!untextured && !isTransition && (!bound || !bound->texture))
		return;

	float texW = 1, texH = 1;
	if (isTransition){
		texW = u.texSize.x;
		texH = u.texSize.y;
	}else if (bound){
		texW = bound->width;
		texH = bound->height;
	}

	bool spritePos = false;
	bool translated = true;
	const float *mat = 0;

	switch (effect){
	case SHADER_simpleSprite :
	case SHADER_alphaSprite :
	case SHADER_sprite :
	case SHADER_worldMachine :
	case SHADER_crt :
		spritePos = true;
		mat = u.spriteMat;
		translated = false;
		break;

	case SHADER_simpleMatrix :
		spritePos = true;
		mat = u.matrix;
		translated = false;
		break;

	case SHADER_flatColor :
	case SHADER_blur :
		translated = false;
		break;

	default :
		break;
	}

	Vertices &v = scratch;
	v.list.clear();
	v.indices.clear();
	v.list.reserve(quadCount * 4);
	v.indices.reserve(quadCount * 6);

	for (size_t q = firstQuad; q < firstQuad + quadCount; ++q){
		if ((q * 4 + 4) * stride > g.data.size())
			break;

		for (int i = 0; i < 4; ++i){
			const uint8_t *src = &g.data[(q * 4 + i) * stride];
			Vec2 pos;
			Vec2 texPos;
			Vec4 vcolor(0, 0, 0, 1);

			switch (g.layout){
			case VertexLayout::Simple :
				{
					SVertex sv;
					memcpy(&sv, src, sizeof(sv));
					pos = sv.pos;
					texPos = sv.texPos;
					break;
				}

			case VertexLayout::Color :
				{
					CVertex cv;
					memcpy(&cv, src, sizeof(cv));
					pos = cv.pos;
					vcolor = cv.color;
					break;
				}

			case VertexLayout::Full :
				{
					Vertex fv;
					memcpy(&fv, src, sizeof(fv));
					pos = fv.pos;
					texPos = fv.texPos;
					vcolor = fv.color;
					break;
				}
			}

			float px = pos.x;
			float py = pos.y;

			if (spritePos){
				transformPoint(mat, pos.x, pos.y, px, py);
			}else if (translated){
				px += u.translation.x;
				py += u.translation.y;
			}

			float tx = texPos.x;
			float ty = texPos.y;

			if (effect == SHADER_tilemap || effect == SHADER_tilemapWater){
				if (tx <= 96.0f && ty <= 128.0f * 7.0f)
					tx += u.aniIndex * 32.0f * 3.0f;
			}

			SDL_FColor color = fcolor(1, 1, 1, 1);

			switch (effect){
			case SHADER_flatColor :
				color = fcolor(u.color.x, u.color.y, u.color.z, u.color.w);
				break;

			case SHADER_simpleColor :
				color = fcolor(vcolor.x, vcolor.y, vcolor.z, vcolor.w);
				break;

			case SHADER_flashMap :
				color = fcolor(vcolor.x * u.opacity, vcolor.y * u.opacity, vcolor.z * u.opacity, 1.0f);
				break;

			case SHADER_simpleAlpha :
			case SHADER_simpleMatrix :
				color = fcolor(1, 1, 1, vcolor.w);
				break;

			case SHADER_alphaSprite :
			case SHADER_blt :
				color = fcolor(1, 1, 1, u.opacity);
				break;

			case SHADER_plane :
				color = applyColorKeep(fcolor(1, 1, 1, u.opacity), u);
				break;

			case SHADER_sprite :
				color = applyColorKeep(fcolor(u.modulate.x, u.modulate.y, u.modulate.z, u.modulate.w * u.opacity), u);
				break;

			case SHADER_worldMachine :
			case SHADER_crt :
			case SHADER_water :
				color = fcolor(u.modulate.x, u.modulate.y, u.modulate.z, u.modulate.w * u.opacity);
				break;

			default :
				break;
			}

			SDL_Vertex vert;
			vert.position.x = px;
			vert.position.y = py;
			vert.color = color;
			vert.tex_coord.x = tx / texW;
			vert.tex_coord.y = ty / texH;
			v.list.push_back(vert);
		}

		appendQuadIndices(v.indices, (int) ((q - firstQuad) * 4));
	}

	if (v.list.empty())
		return;

	if (isTransition){
		drawTransition(v);
		return;
	}

	if (effect == SHADER_sprite && u.bushDepth < 1.0f && u.bushOpacity < 1.0f)
		applyBushDepth(v.list, v.indices, u.bushDepth, u.bushOpacity);

	if (untextured){
		const bool tone = activeBlend == BlendKind::ToneAdd || activeBlend == BlendKind::ToneSubtract;
		if (tone){
			for (size_t i = 0; i < v.list.size(); ++i)
				v.list[i].color.a = 1.0f;
		}

		submit(0, v, blendMode(false), false);
		return;
	}

	const bool grayed = (effect == SHADER_sprite || effect == SHADER_plane) && u.tone.w > 0;
	const bool cpuEffect = effect == SHADER_hue || effect == SHADER_gray || effect == SHADER_blur || grayed;
	SdlTexture processed;

	if (cpuEffect){
		processed.texture = processTexture(*bound, effect, u);
		if (!processed.texture)
			return;

		processed.width = bound->width;
		processed.height = bound->height;
		processed.smooth = bound->smooth;
		bound = &processed;
	}

	const SDL_BlendMode mode = blendMode(effect == SHADER_blt);
	submit(bound, v, mode, false);

	const bool toned = effect == SHADER_sprite || effect == SHADER_plane;
	const bool hasAdd = u.tone.x > 0 || u.tone.y > 0 || u.tone.z > 0 || u.color.w > 0;

	if (toned && hasAdd && mode != SDL_BLENDMODE_NONE){
		for (size_t i = 0; i < v.list.size(); ++i)
			v.list[i].color = additiveColor(v.list[i].color, u);

		submit(bound, v, SDL_BLENDMODE_ADD, false);
	}

	if (cpuEffect){
		flush();
		SDL_DestroyTexture(processed.texture);
	}
}

void SDLRender::beginBlit(const RenderTarget &rt){
	target = texture(rt.fbo.id);
	viewport.pushSet(IntRect(0, 0, rt.width, rt.height));
	useEffect(SHADER_simple);
	setTranslation(Vec2i());
}

void SDLRender::beginBlitScreen(const Vec2i &size){
	target = 0;
	viewport.pushSet(IntRect(0, 0, size.x, size.y));
	useEffect(SHADER_simple);
	setTranslation(Vec2i());
}

void SDLRender::blitSource(const RenderTarget &source){
	setTexSize(Vec2i(source.width, source.height));
	boundTexture = source.tex.id;
}

void SDLRender::blitRect(const IntRect &src, const Vec2i &dstPos){
	blitRect(src, IntRect(dstPos.x, dstPos.y, src.w, src.h), false);
}

void SDLRender::blitRect(const IntRect &src, const IntRect &dst, bool smooth){
	SdlTexture *srcTex = texture(boundTexture);
	if (!srcTex || !srcTex->texture)
		return;

	float left = dst.x;
	float width = dst.w;
	float top = dst.y;
	float height = dst.h;
	bool flipV = false;

	if (!target){
		int screenH = screenHeight();

		if (dst.h < 0){
			top = screenH - dst.y;
			height = -dst.h;
		}else{
			top = screenH - (dst.y + dst.h);
			height = dst.h;
			flipV = true;
		}
	}

	const float u0 = (float) src.x / srcTex->width;
	const float u1 = (float) (src.x + src.w) / srcTex->width;
	float v0 = (float) src.y / srcTex->height;
	float v1 = (float) (src.y + src.h) / srcTex->height;

	if (flipV)
		std::swap(v0, v1);

	SDL_Vertex corners[4];
	const SDL_FColor white = fcolor(1, 1, 1, 1);
	corners[0].position.x = left;         corners[0].position.y = top;          corners[0].tex_coord.x = u0; corners[0].tex_coord.y = v0;
	corners[1].position.x = left + width; corners[1].position.y = top;          corners[1].tex_coord.x = u1; corners[1].tex_coord.y = v0;
	corners[2].position.x = left + width; corners[2].position.y = top + height; corners[2].tex_coord.x = u1; corners[2].tex_coord.y = v1;
	corners[3].position.x = left;         corners[3].position.y = top + height; corners[3].tex_coord.x = u0; corners[3].tex_coord.y = v1;

	for (int i = 0; i < 4; ++i)
		corners[i].color = white;

	drawTextureQuad(srcTex, corners, SDL_BLENDMODE_NONE, smooth);
}

void SDLRender::endBlit(){
	viewport.pop();
}

void SDLRender::swapWindow(SDL_Window *){
	flush();
	renderStatsBeginSwap();
	SDL_SetRenderTarget(renderer, 0);
	stateValid = false;
	SDL_RenderPresent(renderer);
	renderStatsEndSwap();
}

void SDLRender::suspendContext(SDL_Window *){}
void SDLRender::resumeContext(SDL_Window *){}

class SDLRenderContext : public IRenderContext{
public:
	SDLRenderContext(SDL_Window *window){
		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);
		SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, conf.syncToRefreshrate ? 1 : 0);

		renderer = SDL_CreateRendererWithProperties(props);
		SDL_DestroyProperties(props);

		if(!renderer)
			throw Exception(Exception::MKXPError, "Failed to create SDL renderer: %s", SDL_GetError());

		Debug() << "[SDLRender] driver:" << SDL_GetRendererName(renderer);
		createdRenderer = renderer;
	}

	~SDLRenderContext(){
		createdRenderer = 0;
		SDL_DestroyRenderer(renderer);
	}

private:
	SDL_Renderer *renderer;
};

uint64_t sdlWindowFlags(){
	return 0;
}

void sdlSetupWindowAttributes(){
}

IRenderContext *createSDLRenderContext(SDL_Window *window){
	return new SDLRenderContext(window);
}

IRender *createSDLRender(const Config &conf){
	return new SDLRender(conf, createdRenderer);
}

bool sdlProbe(SDL_Window *window){
	SDL_Renderer *probe = SDL_CreateRenderer(window, 0);
	if(!probe)
		return false;

	SDL_DestroyRenderer(probe);
	return true;
}

#endif
