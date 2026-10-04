#include "sdlrender.h"
#include "render/backends.h"
#include "config.h"
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

SdlUniforms::SdlUniforms()
    : texSize(1, 1),
      color(0, 0, 0, 0),
      modulate(1, 1, 1, 1),
      opacity(1),
      aniIndex(0),
      prog(0),
      hueAdjust(0),
      gray(0),
      current(0),
      frozen(0)
{
	for (int i = 0; i < 16; ++i)
		spriteMat[i] = matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
}

static inline float clamp01(float v){
	return std::min(std::max(v, 0.0f), 1.0f);
}

static inline float mixf(float a, float b, float t){
	return a * (1.0f - t) + b * t;
}

static inline float fractf(float v){
	return v - floorf(v);
}

static void rgb2hsv(float r, float g, float b, float &h, float &s, float &v){
	float p0, p1, p2, p3;
	if (g >= b){
		p0 = g; p1 = b; p2 = 0.0f; p3 = -1.0f / 3.0f;
	}else{
		p0 = b; p1 = g; p2 = -1.0f; p3 = 2.0f / 3.0f;
	}

	float q0, q1, q2, q3;
	if (r >= p0){
		q0 = r; q1 = p1; q2 = p2; q3 = p0;
	}else{
		q0 = p0; q1 = p1; q2 = p3; q3 = r;
	}

	const float eps = 1.0e-10f;
	float d = q0 - std::min(q3, q1);

	h = fabsf(q2 + (q3 - q1) / (6.0f * d + eps));
	s = d / (q0 + eps);
	v = q0;
}

static void hsv2rgb(float h, float s, float v, float &r, float &g, float &b){
	const float k[3] = { 1.0f, 2.0f / 3.0f, 1.0f / 3.0f };
	float out[3];

	for (int i = 0; i < 3; ++i){
		float p = fabsf(fractf(h + k[i]) * 6.0f - 3.0f);
		out[i] = v * mixf(1.0f, clamp01(p - 1.0f), s);
	}

	r = out[0];
	g = out[1];
	b = out[2];
}

static SDL_FColor fcolor(float r, float g, float b, float a){
	SDL_FColor c = { r, g, b, a };
	return c;
}

SDLRender::SDLRender(const Config &conf, SDL_Renderer *renderer)
    : renderer(renderer),
      probe(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 1, 1)),
      target(0),
      boundTexture(0),
      maxTexSize(0),
      activeBlend(SdlBlend::Normal),
      uniformSets(SHADER_COUNT),
      currentSlot(SHADER_simple),
      blurPass(0),
      ambient(0)
{
	SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
	maxTexSize = (int) SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 4096);
	if (maxTexSize <= 0 || SDL_strcmp(SDL_GetRendererName(renderer), "software") == 0)
		maxTexSize = std::max(maxTexSize, 16384);

	if (conf.maxTextureSize > 0)
		maxTexSize = conf.maxTextureSize;

	viewport.current = IntRect(0, 0, conf.defScreenW, conf.defScreenH);
	blend.current = true;
	blendModeProp.current = BlendNormal;
	scissorTest.current = false;
	scissorBoxProp.current = IntRect(0, 0, conf.defScreenW, conf.defScreenH);
	clearColor.current = Vec4(0, 0, 0, 1);

	setActiveRender(this);
}

SDLRender::~SDLRender(){
	setActiveRender(0);

	if (probe)
		SDL_DestroyTexture(probe);

	if (lightMap.texture)
		SDL_DestroyTexture(lightMap.texture);

	for (size_t i = 0; i < textures.size(); ++i)
		if (textures[i] && textures[i]->texture)
			SDL_DestroyTexture(textures[i]->texture);
}

int SDLRender::maxTextureSize() const{
	return maxTexSize;
}

bool SDLRender::repeatNpotSupported() const{
	return true;
}

SdlTexture *SDLRender::texture(uint32_t id){
	if (id == 0 || id > textures.size())
		return 0;

	return textures[id-1].get();
}

SdlUniforms &SDLRender::uniforms(){
	return uniformSets[currentSlot];
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
}

TexHandle SDLRender::createTexture(int w, int h, PixelFormat){
	std::unique_ptr<SdlTexture> tex(new SdlTexture);
	createSdlTexture(*tex, w, h);

	uint32_t id;
	if (freeTextures.empty()){
		textures.push_back(std::move(tex));
		id = textures.size();
	}else{
		id = freeTextures.back();
		freeTextures.pop_back();
		textures[id-1] = std::move(tex);
	}

	return TexHandle(id);
}

void SDLRender::resizeTexture(TexHandle handle, int w, int h, PixelFormat){
	SdlTexture *tex = texture(handle.id);
	if (!tex)
		return;

	if (tex->texture)
		SDL_DestroyTexture(tex->texture);

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

	if (tex->texture)
		SDL_DestroyTexture(tex->texture);

	textures[handle.id-1].reset();
	freeTextures.push_back(handle.id);
}

void SDLRender::bindTexture(TexHandle tex){
	boundTexture = tex.id;
}

void SDLRender::unbindTexture(){
	boundTexture = 0;
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

void SDLRender::applyState(){
	SDL_SetRenderTarget(renderer, target ? target->texture : 0);

	SDL_Rect vp = viewport.current;
	SDL_SetRenderViewport(renderer, &vp);

	if (scissorTest.current){
		SDL_Rect box = scissorBoxProp.current;
		SDL_SetRenderClipRect(renderer, &box);
	}else{
		SDL_SetRenderClipRect(renderer, 0);
	}
}

void SDLRender::clear(){
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

void SDLRender::setViewport(const IntRect &rect){
	viewport.set(rect);
}

void SDLRender::pushViewport(const IntRect &rect){
	viewport.pushSet(rect);
}

void SDLRender::popViewport(){
	viewport.pop();
}

void SDLRender::refreshViewport(){
}

void SDLRender::pushBlend(bool enabled){
	blend.pushSet(enabled);
}

void SDLRender::popBlend(){
	blend.pop();
}

static SdlBlend blendFromMode(BlendType mode){
	switch (mode){
	case BlendKeepDestAlpha : return SdlBlend::KeepDestAlpha;
	case BlendAddition      : return SdlBlend::Addition;
	case BlendSubstraction  : return SdlBlend::Substraction;
	case BlendMultiply      : return SdlBlend::Multiply;
	default                 : return SdlBlend::Normal;
	}
}

void SDLRender::pushBlendMode(BlendType mode){
	blendModeProp.pushSet(mode);
	activeBlend = blendFromMode(mode);
}

void SDLRender::popBlendMode(){
	blendModeProp.pop();
	activeBlend = blendFromMode(blendModeProp.current);
}

void SDLRender::setBlendOverride(BlendOverride mode){
	switch (mode){
	case BlendOverride::ToneAdd :
		activeBlend = SdlBlend::ToneAdd;
		break;

	case BlendOverride::ToneSubtract :
		activeBlend = SdlBlend::ToneSubtract;
		break;

	case BlendOverride::Overlay :
		activeBlend = SdlBlend::KeepDestAlpha;
		break;
	}
}

void SDLRender::refreshBlendMode(){
	activeBlend = blendFromMode(blendModeProp.current);
}

void SDLRender::pushScissorTest(bool enabled){
	scissorTest.pushSet(enabled);
}

void SDLRender::popScissorTest(){
	scissorTest.pop();
}

void SDLRender::pushScissorBox(const IntRect &rect){
	scissorBoxProp.pushSet(rect);
}

void SDLRender::saveScissorBox(){
	scissorBoxProp.push();
}

void SDLRender::intersectScissorBox(const IntRect &rect){
	const IntRect &cur = scissorBoxProp.current;
	int x0 = std::max(cur.x, rect.x);
	int y0 = std::max(cur.y, rect.y);
	int x1 = std::min(cur.x + cur.w, rect.x + rect.w);
	int y1 = std::min(cur.y + cur.h, rect.y + rect.h);

	if (x1 <= x0 || y1 <= y0)
		scissorBoxProp.set(IntRect(0, 0, 0, 0));
	else
		scissorBoxProp.set(IntRect(x0, y0, x1 - x0, y1 - y0));
}

void SDLRender::popScissorBox(){
	scissorBoxProp.pop();
}

void SDLRender::setScissorBox(const IntRect &rect){
	scissorBoxProp.set(rect);
}

const IntRect &SDLRender::scissorBox(){
	return scissorBoxProp.current;
}

void SDLRender::pushClearColor(const Vec4 &color){
	clearColor.pushSet(color);
}

void SDLRender::popClearColor(){
	clearColor.pop();
}

GeometryHandle SDLRender::createGeometry(VertexLayout layout){
	std::unique_ptr<SdlGeometry> geom(new SdlGeometry);
	geom->layout = layout;

	uint32_t id;
	if (freeGeometries.empty()){
		geometries.push_back(std::move(geom));
		id = geometries.size();
	}else{
		id = freeGeometries.back();
		freeGeometries.pop_back();
		geometries[id-1] = std::move(geom);
	}

	return GeometryHandle(id);
}

void SDLRender::destroyGeometry(GeometryHandle geom){
	geometries[geom.id-1].reset();
	freeGeometries.push_back(geom.id);
}

void SDLRender::allocGeometry(GeometryHandle geom, size_t bytes, GeometryUsage){
	geometries[geom.id-1]->data.assign(bytes, 0);
}

void SDLRender::uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage){
	const uint8_t *src = static_cast<const uint8_t*>(data);
	geometries[geom.id-1]->data.assign(src, src + bytes);
}

void SDLRender::uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data){
	std::vector<uint8_t> &buf = geometries[geom.id-1]->data;
	if (buf.size() < offset + bytes)
		buf.resize(offset + bytes);

	memcpy(&buf[offset], data, bytes);
}

void SDLRender::ensureQuadIndices(size_t){
}

SDL_BlendMode SDLRender::composeOrFallback(SDL_BlendFactor srcColor, SDL_BlendFactor dstColor, SDL_BlendOperation colorOp,
                                           SDL_BlendFactor srcAlpha, SDL_BlendFactor dstAlpha, SDL_BlendOperation alphaOp,
                                           SDL_BlendMode fallback){
	SDL_BlendMode mode = SDL_ComposeCustomBlendMode(srcColor, dstColor, colorOp, srcAlpha, dstAlpha, alphaOp);

	std::map<SDL_BlendMode, bool>::iterator found = customSupport.find(mode);
	if (found == customSupport.end())
		found = customSupport.insert(std::make_pair(mode, probe && SDL_SetTextureBlendMode(probe, mode))).first;

	return found->second ? mode : fallback;
}

SDL_BlendMode SDLRender::blendMode(bool forceBlend){
	if (!blend.current && !forceBlend)
		return SDL_BLENDMODE_NONE;

	switch (activeBlend){
	case SdlBlend::Normal :
		return SDL_BLENDMODE_BLEND;

	case SdlBlend::Addition :
		return SDL_BLENDMODE_ADD;

	case SdlBlend::Multiply :
		return SDL_BLENDMODE_MUL;

	case SdlBlend::KeepDestAlpha :
		return composeOrFallback(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD,
		                         SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD, SDL_BLENDMODE_BLEND);

	case SdlBlend::Substraction :
		return composeOrFallback(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
		                         SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD, SDL_BLENDMODE_INVALID);

	case SdlBlend::ToneAdd :
		return SDL_BLENDMODE_ADD;

	case SdlBlend::ToneSubtract :
		return composeOrFallback(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
		                         SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD, SDL_BLENDMODE_INVALID);
	}

	return SDL_BLENDMODE_BLEND;
}

void SDLRender::submit(SdlTexture *tex, const Vertices &v, SDL_BlendMode mode, bool smooth){
	if (mode == SDL_BLENDMODE_INVALID || v.list.empty())
		return;

	applyState();

	SDL_Texture *sdlTex = 0;

	if (tex){
		if (!tex->texture)
			return;

		sdlTex = tex->texture;
		SDL_SetTextureBlendMode(sdlTex, mode);
		SDL_SetTextureScaleMode(sdlTex, (smooth || tex->smooth) ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
	}else{
		SDL_SetRenderDrawBlendMode(renderer, mode);
	}

	SDL_RenderGeometry(renderer, sdlTex, v.list.data(), (int) v.list.size(), v.indices.data(), (int) v.indices.size());
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

void SDLRender::drawFilteredCopy(SdlTexture &src, const Vertices &v, int filter){
	std::vector<uint8_t> data;
	readTexture(src, data);

	const SdlUniforms &u = uniforms();

	for (size_t i = 0; i < data.size(); i += 4){
		float r = data[i+0] / 255.0f;
		float g = data[i+1] / 255.0f;
		float b = data[i+2] / 255.0f;

		if (filter == SHADER_hue){
			float h, s, val;
			rgb2hsv(r, g, b, h, s, val);
			h += u.hueAdjust;
			hsv2rgb(h, s, val, r, g, b);
		}else{
			float l = r * .299f + g * .587f + b * .114f;
			r = mixf(r, l, u.gray);
			g = mixf(g, l, u.gray);
			b = mixf(b, l, u.gray);
		}

		data[i+0] = (uint8_t) (clamp01(r) * 255.0f + 0.5f);
		data[i+1] = (uint8_t) (clamp01(g) * 255.0f + 0.5f);
		data[i+2] = (uint8_t) (clamp01(b) * 255.0f + 0.5f);
	}

	SdlTexture scratch;
	createSdlTexture(scratch, src.width, src.height);
	if (!scratch.texture)
		return;

	SDL_Rect rect = { 0, 0, src.width, src.height };
	SDL_UpdateTexture(scratch.texture, &rect, data.data(), src.width * 4);
	scratch.smooth = src.smooth;

	submit(&scratch, v, blendMode(false), false);
	SDL_FlushRenderer(renderer);
	SDL_DestroyTexture(scratch.texture);
}

void SDLRender::drawBlur(SdlTexture &src, const Vertices &v){
	const float dx = blurPass == 0 ? 1.0f / src.width : 0.0f;
	const float dy = blurPass == 0 ? 0.0f : 1.0f / src.height;
	const float weights[3] = { 1.0f, 0.5f, 1.0f / 3.0f };
	const float offsets[3] = { 0.0f, -1.0f, 1.0f };

	for (int pass = 0; pass < 3; ++pass){
		Vertices copy = v;

		for (size_t i = 0; i < copy.list.size(); ++i){
			copy.list[i].tex_coord.x += dx * offsets[pass];
			copy.list[i].tex_coord.y += dy * offsets[pass];
			copy.list[i].color = fcolor(1, 1, 1, weights[pass]);
		}

		submit(&src, copy, pass == 0 ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND, false);
	}
}

void SDLRender::drawTransition(const Vertices &v){
	const SdlUniforms &u = uniforms();
	SdlTexture *frozen = texture(u.frozen);
	SdlTexture *current = texture(u.current);

	if (frozen){
		Vertices copy = v;
		for (size_t i = 0; i < copy.list.size(); ++i)
			copy.list[i].color = fcolor(1, 1, 1, 1);
		submit(frozen, copy, SDL_BLENDMODE_NONE, false);
	}

	if (current){
		Vertices copy = v;
		for (size_t i = 0; i < copy.list.size(); ++i)
			copy.list[i].color = fcolor(1, 1, 1, clamp01(u.prog));
		submit(current, copy, SDL_BLENDMODE_BLEND, false);
	}
}

void SDLRender::drawLightMap(const Vertices &v, const SdlTexture &bound){
	const int w = bound.width;
	const int h = bound.height;

	if (w <= 0 || h <= 0)
		return;

	if (!lightMap.texture || lightMap.width != w || lightMap.height != h){
		if (lightMap.texture)
			SDL_DestroyTexture(lightMap.texture);

		createSdlTexture(lightMap, w, h);
		if (!lightMap.texture)
			return;
	}

	const float amb = ambient / 255.0f;
	std::vector<float> light((size_t) w * h * 3, amb);

	for (size_t i = 0; i < lights.size(); ++i){
		const LightSource &src = lights[i];
		const float cx = (src.x - cameraPos.x / 32.0f) * 32.0f + 16.0f;
		const float cy = (src.y - cameraPos.y / 32.0f) * 32.0f + 16.0f;
		const float reach = 32.0f * src.radius;
		const float exponent = (float) (src.color.alpha / 255.0) * 2.0f;
		const float strength = src.power / 255.0f;
		const float cr = (float) (src.color.red / 255.0) * strength;
		const float cg = (float) (src.color.green / 255.0) * strength;
		const float cb = (float) (src.color.blue / 255.0) * strength;

		const int x0 = std::max(0, (int) floorf(cx - reach));
		const int x1 = std::min(w - 1, (int) ceilf(cx + reach));
		const int y0 = std::max(0, (int) floorf(cy - reach));
		const int y1 = std::min(h - 1, (int) ceilf(cy + reach));

		for (int y = y0; y <= y1; ++y){
			const float dy = y + 0.5f - cy;

			for (int x = x0; x <= x1; ++x){
				const float dx = x + 0.5f - cx;
				const float t = 1.0f - sqrtf(dx * dx + dy * dy) / reach;

				if (t <= 0.0f)
					continue;

				const float f = exponent == 2.0f ? t * t : powf(t, exponent);
				float *px = &light[((size_t) y * w + x) * 3];
				px[0] += cr * f;
				px[1] += cg * f;
				px[2] += cb * f;
			}
		}
	}

	std::vector<uint8_t> bytes((size_t) w * h * 4);

	for (size_t i = 0; i < (size_t) w * h; ++i){
		bytes[i*4+0] = (uint8_t) (clamp01(light[i*3+0]) * 255.0f + 0.5f);
		bytes[i*4+1] = (uint8_t) (clamp01(light[i*3+1]) * 255.0f + 0.5f);
		bytes[i*4+2] = (uint8_t) (clamp01(light[i*3+2]) * 255.0f + 0.5f);
		bytes[i*4+3] = 255;
	}

	SDL_Rect rect = { 0, 0, w, h };
	SDL_UpdateTexture(lightMap.texture, &rect, bytes.data(), w * 4);

	Vertices copy = v;
	for (size_t i = 0; i < copy.list.size(); ++i)
		copy.list[i].color = fcolor(1, 1, 1, 1);

	submit(&lightMap, copy, blendMode(false), false);
}

static void transformPoint(const float m[16], float x, float y, float &ox, float &oy){
	ox = m[0] * x + m[4] * y + m[12];
	oy = m[1] * x + m[5] * y + m[13];
}

void SDLRender::drawQuads(GeometryHandle geom, size_t firstQuad, size_t quadCount){
	const SdlGeometry &g = *geometries[geom.id-1];
	const SdlUniforms &u = uniforms();
	const int effect = currentSlot;

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

	Vertices v;
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
			case SHADER_plane :
			case SHADER_blt :
				color = fcolor(1, 1, 1, u.opacity);
				break;

			case SHADER_sprite :
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

	if (effect == SHADER_dynamicLight){
		drawLightMap(v, *bound);
		return;
	}

	if (untextured){
		const bool tone = activeBlend == SdlBlend::ToneAdd || activeBlend == SdlBlend::ToneSubtract;
		if (tone){
			for (size_t i = 0; i < v.list.size(); ++i)
				v.list[i].color.a = 1.0f;
		}

		submit(0, v, blendMode(false), false);
		return;
	}

	if (effect == SHADER_blur){
		drawBlur(*bound, v);
		return;
	}

	if (effect == SHADER_hue || effect == SHADER_gray){
		drawFilteredCopy(*bound, v, effect);
		return;
	}

	submit(bound, v, blendMode(effect == SHADER_blt), false);
}

void SDLRender::useEffect(ShaderType effect){
	currentSlot = effect;
}

void SDLRender::useBlurPass(int pass){
	currentSlot = SHADER_blur;
	blurPass = pass;
}

void SDLRender::applyViewportProj(){
}

void SDLRender::applyPerspectiveProj(){
}

void SDLRender::setTexSize(const Vec2i &size){
	uniforms().texSize = size;
}

void SDLRender::setTranslation(const Vec2i &value){
	uniforms().translation = value;
}

void SDLRender::setTime(float){
}

void SDLRender::setEffectTexture(EffectTexture slot, TexHandle tex){
	SdlUniforms &u = uniforms();

	if (slot == EffectTexture::Current)
		u.current = tex.id;
	else if (slot == EffectTexture::Frozen)
		u.frozen = tex.id;
}

void SDLRender::setSpriteMat(const float value[16]){
	memcpy(uniforms().spriteMat, value, sizeof(float) * 16);
}

void SDLRender::setMatrix(const float value[16]){
	memcpy(uniforms().matrix, value, sizeof(float) * 16);
}

void SDLRender::setTone(const Vec4 &){
}

void SDLRender::setColor(const Vec4 &value){
	uniforms().color = value;
}

void SDLRender::setFlash(const Vec4 &){
}

void SDLRender::setModulate(const Vec4 &value){
	uniforms().modulate = value;
}

void SDLRender::setOpacity(float value){
	uniforms().opacity = value;
}

void SDLRender::setBushDepth(float){
}

void SDLRender::setBushOpacity(float){
}

void SDLRender::setGray(float value){
	uniforms().gray = value;
}

void SDLRender::setHueAdjust(float value){
	uniforms().hueAdjust = value;
}

void SDLRender::setAniIndex(int value){
	uniforms().aniIndex = value;
}

void SDLRender::setOffset(const Vec2i &){
}

void SDLRender::setSubRect(const FloatRect &){
}

void SDLRender::setProg(float value){
	uniforms().prog = value;
}

void SDLRender::setVague(float){
}

void SDLRender::setWallMapResolution(int, int){
}

void SDLRender::setCameraPosition(int x, int y){
	cameraPos = Vec2i(x, y);
}

void SDLRender::setTileMapOffset(int, int){
}

void SDLRender::setLightSources(const std::vector<LightSource> &sources){
	lights.clear();

	for (size_t i = 0; i < sources.size() && lights.size() < 64; ++i){
		LightSource source = sources[i];
		if (!source.hasEffect())
			continue;

		lights.push_back(source);
	}
}

void SDLRender::setAmbient(float value){
	ambient = value;
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
	SDL_SetRenderTarget(renderer, 0);
	SDL_RenderPresent(renderer);
}

void SDLRender::suspendContext(SDL_Window *){
}

void SDLRender::resumeContext(SDL_Window *){
}

class SDLRenderContext : public IRenderContext{
public:
	SDLRenderContext(SDL_Window *window){
		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);

		if (!conf.sdlRenderDriver.empty())
			SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, conf.sdlRenderDriver.c_str());

		SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, (conf.syncToRefreshrate && conf.fixedFramerate == 0) ? 1 : 0);

		renderer = SDL_CreateRendererWithProperties(props);
		SDL_DestroyProperties(props);

		if (!renderer)
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
	SDL_Renderer *probe = SDL_CreateRenderer(window, conf.sdlRenderDriver.empty() ? 0 : conf.sdlRenderDriver.c_str());
	if (!probe)
		return false;

	SDL_DestroyRenderer(probe);
	return true;
}
