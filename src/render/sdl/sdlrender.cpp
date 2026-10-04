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
#include <string.h>

static SDL_Renderer *createdRenderer = 0;

static SDL_FColor fcolor(float r, float g, float b, float a){
	SDL_FColor c = { r, g, b, a };
	return c;
}

SDLRender::SDLRender(const Config &conf, SDL_Renderer *renderer)
    : StatefulRender(conf),
      renderer(renderer),
      target(0),
      maxTexSize(0)
{
	SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
	maxTexSize = (int) SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 4096);
	if (maxTexSize <= 0 || SDL_strcmp(SDL_GetRendererName(renderer), "software") == 0)
		maxTexSize = std::max(maxTexSize, 16384);

	if (conf.maxTextureSize > 0)
		maxTexSize = conf.maxTextureSize;

	const char *rendererName = SDL_GetRendererName(renderer);
	apiNameStr = rendererName ? rendererName : "unknown";

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
	std::vector<uint8_t> &buf = geometries.get(geom.id)->data;
	if (buf.size() < offset + bytes)
		buf.resize(offset + bytes);

	memcpy(&buf[offset], data, bytes);
}

void SDLRender::ensureQuadIndices(size_t){
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
	case BlendKind::ToneSubtract :
		return SDL_BLENDMODE_INVALID;

	default :
		return SDL_BLENDMODE_BLEND;
	}
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

	++renderStats.drawCalls;
	renderStats.vertices += v.list.size();
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

	if (untextured){
		const bool tone = activeBlend == BlendKind::ToneAdd || activeBlend == BlendKind::ToneSubtract;
		if (tone){
			for (size_t i = 0; i < v.list.size(); ++i)
				v.list[i].color.a = 1.0f;
		}

		submit(0, v, blendMode(false), false);
		return;
	}

	submit(bound, v, blendMode(effect == SHADER_blt), false);
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
	renderStatsBeginSwap();
	SDL_SetRenderTarget(renderer, 0);
	SDL_RenderPresent(renderer);
	renderStatsEndSwap();
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
