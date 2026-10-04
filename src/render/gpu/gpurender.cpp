#include "gpurender.h"
#include "render/backends.h"
#include "config.h"
#include "render/renderstats.h"
#include "exception.h"
#include "debugwriter.h"
#include "sharedstate.h"
#include "graphics.h"
#include "vertex.h"

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>
#include <algorithm>
#include <math.h>
#include <string.h>

#ifdef SUNSHINE_GPU_SHADERS
#include "uber.vert.spv.xxd"
#include "uber.frag.spv.xxd"
#include "light.frag.spv.xxd"
#endif

static SDL_GPUDevice *createdDevice = 0;
static SDL_Window *createdWindow = 0;

static const int PipelineUber = 0;
static const int PipelineLight = 1;
static const int MaxQuadsPerDraw = 16000;

static bool usesTranslation(int effect){
	switch (effect){
	case SHADER_simple :
	case SHADER_simpleColor :
	case SHADER_simpleAlpha :
	case SHADER_tilemap :
	case SHADER_tilemapWater :
	case SHADER_plane :
	case SHADER_gray :
	case SHADER_hue :
	case SHADER_blt :
	case SHADER_trans :
	case SHADER_simpleTrans :
	case SHADER_obscured :
	case SHADER_water :
	case SHADER_dynamicLight :
	case SHADER_flashMap :
		return true;

	default :
		return false;
	}
}

static bool usesSpriteMat(int effect){
	switch (effect){
	case SHADER_simpleSprite :
	case SHADER_alphaSprite :
	case SHADER_sprite :
	case SHADER_worldMachine :
	case SHADER_crt :
		return true;

	default :
		return false;
	}
}

static SDL_GPUColorTargetBlendState blendState(GpuBlend blend){
	SDL_GPUColorTargetBlendState s;
	memset(&s, 0, sizeof(s));

	s.color_blend_op = SDL_GPU_BLENDOP_ADD;
	s.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
	s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
	s.enable_blend = blend != GpuBlend::None;

	switch (blend){
	case GpuBlend::None :
		break;

	case GpuBlend::Normal :
		s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		break;

	case GpuBlend::KeepDestAlpha :
		s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
		s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		break;

	case GpuBlend::Addition :
		s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		break;

	case GpuBlend::Substraction :
		s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.color_blend_op = SDL_GPU_BLENDOP_REVERSE_SUBTRACT;
		s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
		s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		break;

	case GpuBlend::Multiply :
		s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR;
		s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
		s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
		s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		break;

	case GpuBlend::ToneAdd :
		s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
		s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		break;

	case GpuBlend::ToneSubtract :
		s.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		s.color_blend_op = SDL_GPU_BLENDOP_REVERSE_SUBTRACT;
		s.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
		s.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
		break;

	case GpuBlend::Count :
		break;
	}

	return s;
}

bool GPURender::PipelineKey::operator<(const PipelineKey &o) const{
	if (pipeline != o.pipeline)
		return pipeline < o.pipeline;

	if (blend != o.blend)
		return blend < o.blend;

	return format < o.format;
}

static SDL_GPUShader *createShader(SDL_GPUDevice *device, const unsigned char *code, size_t size, SDL_GPUShaderStage stage, Uint32 samplers){
	SDL_GPUShaderCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.code = code;
	info.code_size = size;
	info.entrypoint = "main";
	info.format = SDL_GPU_SHADERFORMAT_SPIRV;
	info.stage = stage;
	info.num_samplers = samplers;
	info.num_uniform_buffers = 1;

	SDL_GPUShader *shader = SDL_CreateGPUShader(device, &info);
	if (!shader)
		throw Exception(Exception::MKXPError, "Failed to create GPU shader: %s", SDL_GetError());

	return shader;
}

GPURender::GPURender(const Config &conf, SDL_GPUDevice *device, SDL_Window *window)
    : StatefulRender(conf),
      device(device),
      window(window),
      vertexShader(0),
      uberShader(0),
      lightShader(0),
      dummy(0),
      target(0),
      targetId(0),
      maxTexSize(conf.maxTextureSize > 0 ? conf.maxTextureSize : 16384),
      vertexBuffer(0),
      vertexCapacity(0),
      indexBuffer(0),
      indexQuads(0),
      neededQuads(1)
{
	for (int i = 0; i < 4; ++i)
		samplers[i] = 0;

#ifdef SUNSHINE_GPU_SHADERS
	vertexShader = createShader(device, uber_vert_spv, uber_vert_spv_len, SDL_GPU_SHADERSTAGE_VERTEX, 0);
	uberShader = createShader(device, uber_frag_spv, uber_frag_spv_len, SDL_GPU_SHADERSTAGE_FRAGMENT, 4);
	lightShader = createShader(device, light_frag_spv, light_frag_spv_len, SDL_GPU_SHADERSTAGE_FRAGMENT, 0);
#endif

	SDL_GPUTextureCreateInfo tinfo;
	memset(&tinfo, 0, sizeof(tinfo));
	tinfo.type = SDL_GPU_TEXTURETYPE_2D;
	tinfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	tinfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
	tinfo.width = 1;
	tinfo.height = 1;
	tinfo.layer_count_or_depth = 1;
	tinfo.num_levels = 1;
	tinfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
	dummy = SDL_CreateGPUTexture(device, &tinfo);

	SDL_GPUTransferBufferCreateInfo binfo;
	memset(&binfo, 0, sizeof(binfo));
	binfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	binfo.size = 4;
	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device, &binfo);
	Uint8 *mapped = static_cast<Uint8*>(SDL_MapGPUTransferBuffer(device, transfer, false));
	mapped[0] = mapped[1] = mapped[2] = mapped[3] = 255;
	SDL_UnmapGPUTransferBuffer(device, transfer);

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTextureTransferInfo src = { transfer, 0, 1, 1 };
	SDL_GPUTextureRegion dst;
	memset(&dst, 0, sizeof(dst));
	dst.texture = dummy;
	dst.w = 1;
	dst.h = 1;
	dst.d = 1;
	SDL_UploadToGPUTexture(copy, &src, &dst, false);
	SDL_EndGPUCopyPass(copy);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, transfer);

	const char *driverName = SDL_GetGPUDeviceDriver(device);
	apiNameStr = driverName ? driverName : "unknown";

	setActiveRender(this);
}

GPURender::~GPURender(){
	setActiveRender(0);

	flush();
	SDL_WaitForGPUIdle(device);
	releasePending();

	for (size_t i = 0; i < textures.size(); ++i)
		if (textures.at(i) && textures.at(i)->texture)
			SDL_ReleaseGPUTexture(device, textures.at(i)->texture);

	std::map<PipelineKey, SDL_GPUGraphicsPipeline*>::iterator it;
	for (it = pipelines.begin(); it != pipelines.end(); ++it)
		SDL_ReleaseGPUGraphicsPipeline(device, it->second);

	for (int i = 0; i < 4; ++i)
		if (samplers[i])
			SDL_ReleaseGPUSampler(device, samplers[i]);

	if (vertexBuffer)
		SDL_ReleaseGPUBuffer(device, vertexBuffer);

	if (indexBuffer)
		SDL_ReleaseGPUBuffer(device, indexBuffer);

	if (dummy)
		SDL_ReleaseGPUTexture(device, dummy);

	if (vertexShader)
		SDL_ReleaseGPUShader(device, vertexShader);

	if (uberShader)
		SDL_ReleaseGPUShader(device, uberShader);

	if (lightShader)
		SDL_ReleaseGPUShader(device, lightShader);
}

int GPURender::maxTextureSize() const{
	return maxTexSize;
}

const char *GPURender::apiName() const{
	return apiNameStr.c_str();
}

bool GPURender::repeatNpotSupported() const{
	return true;
}

GpuTexture *GPURender::texture(uint32_t id){
	return textures.get(id);
}

Vec2i GPURender::windowSize(){
	int w = 0, h = 0;
	SDL_GetWindowSizeInPixels(window, &w, &h);
	return Vec2i(w, h);
}

Vec2i GPURender::targetSize(){
	if (target)
		return Vec2i(target->width, target->height);

	return windowSize();
}

void GPURender::createGpuTexture(GpuTexture &tex, int w, int h){
	tex.width = w;
	tex.height = h;

	SDL_GPUTextureCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.type = SDL_GPU_TEXTURETYPE_2D;
	info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
	info.width = w;
	info.height = h;
	info.layer_count_or_depth = 1;
	info.num_levels = 1;
	info.sample_count = SDL_GPU_SAMPLECOUNT_1;

	tex.texture = SDL_CreateGPUTexture(device, &info);
	if (!tex.texture)
		Debug() << "[GPURender] cannot create texture" << w << h << SDL_GetError();
}

void GPURender::recordClear(uint32_t id, const Vec4 &color){
	GpuCommand c;
	memset(&c, 0, sizeof(c));
	c.type = GpuCommand::Clear;
	c.target = id;
	c.clearColor[0] = color.x;
	c.clearColor[1] = color.y;
	c.clearColor[2] = color.z;
	c.clearColor[3] = color.w;
	commands.push_back(c);
}

void GPURender::recordUpload(uint32_t id, int x, int y, int w, int h, std::vector<uint8_t> &data){
	uploads.push_back(std::vector<uint8_t>());
	uploads.back().swap(data);

	GpuCommand c;
	memset(&c, 0, sizeof(c));
	c.type = GpuCommand::Upload;
	c.target = id;
	c.payload = (int) uploads.size() - 1;
	c.rect.x = x;
	c.rect.y = y;
	c.rect.w = w;
	c.rect.h = h;
	commands.push_back(c);
}

TexHandle GPURender::createTexture(int w, int h, PixelFormat){
	std::unique_ptr<GpuTexture> tex(new GpuTexture);
	createGpuTexture(*tex, w, h);

	uint32_t id = textures.add(std::move(tex));

	recordClear(id, Vec4(0, 0, 0, 0));
	return TexHandle(id);
}

void GPURender::resizeTexture(TexHandle handle, int w, int h, PixelFormat){
	GpuTexture *tex = texture(handle.id);
	if (!tex)
		return;

	flush();

	if (tex->texture)
		SDL_ReleaseGPUTexture(device, tex->texture);

	createGpuTexture(*tex, w, h);
	recordClear(handle.id, Vec4(0, 0, 0, 0));
}

void GPURender::destroyTexture(TexHandle handle){
	GpuTexture *tex = texture(handle.id);
	if (!tex)
		return;

	if (target == tex){
		target = 0;
		targetId = 0;
	}

	if (boundTexture == handle.id)
		boundTexture = 0;

	quarantine.push_back(handle.id);
}

void GPURender::releasePending(){
	for (size_t i = 0; i < quarantine.size(); ++i){
		uint32_t id = quarantine[i];
		GpuTexture *tex = textures.get(id);

		if (tex && tex->texture)
			SDL_ReleaseGPUTexture(device, tex->texture);

		textures.remove(id);
	}

	quarantine.clear();
}

void GPURender::setTextureSmooth(TexHandle handle, bool smooth){
	boundTexture = handle.id;

	if (GpuTexture *tex = texture(handle.id))
		tex->smooth = smooth;
}

void GPURender::setTextureRepeat(TexHandle handle, bool repeat){
	boundTexture = handle.id;

	if (GpuTexture *tex = texture(handle.id))
		tex->repeat = repeat;
}

void GPURender::uploadTexture(TexHandle handle, int w, int h, const void *pixels, PixelFormat fmt){
	resizeTexture(handle, w, h, fmt);
	uploadTextureRect(handle, 0, 0, w, h, pixels, fmt);
}

void GPURender::uploadTextureRect(TexHandle handle, int x, int y, int w, int h, const void *pixels, PixelFormat fmt){
	boundTexture = handle.id;

	if (!texture(handle.id) || !pixels)
		return;

	std::vector<uint8_t> data((size_t) w * h * 4);

	if (fmt == PixelFormat::Luminance){
		const uint8_t *src = static_cast<const uint8_t*>(pixels);

		for (size_t i = 0; i < (size_t) w * h; ++i){
			data[i*4+0] = data[i*4+1] = data[i*4+2] = src[i];
			data[i*4+3] = 255;
		}
	}else{
		memcpy(data.data(), pixels, data.size());
	}

	recordUpload(handle.id, x, y, w, h, data);
}

void GPURender::uploadTextureRect(TexHandle handle, int dstX, int dstY, int w, int h, SDL_Surface *src, int srcX, int srcY){
	boundTexture = handle.id;

	if (!texture(handle.id))
		return;

	std::vector<uint8_t> data((size_t) w * h * 4);
	const uint8_t *base = static_cast<const uint8_t*>(src->pixels);

	for (int row = 0; row < h; ++row)
		memcpy(&data[(size_t) row * w * 4], base + (size_t) (srcY + row) * src->pitch + (size_t) srcX * 4, (size_t) w * 4);

	recordUpload(handle.id, dstX, dstY, w, h, data);
}

RenderTarget GPURender::createRenderTarget(int w, int h){
	TexHandle tex = createTexture(w, h, PixelFormat::RGBA);

	RenderTarget rt;
	rt.tex = tex;
	rt.fbo = FboHandle(tex.id);
	rt.width = w;
	rt.height = h;
	return rt;
}

void GPURender::resizeRenderTarget(RenderTarget &rt, int w, int h){
	resizeTexture(rt.tex, w, h, PixelFormat::RGBA);
	rt.width = w;
	rt.height = h;
}

void GPURender::destroyRenderTarget(RenderTarget &rt){
	destroyTexture(rt.tex);
}

void GPURender::bindRenderTarget(const RenderTarget &rt){
	targetId = rt.fbo.id;
	target = texture(targetId);
}

void GPURender::bindScreenTarget(){
	targetId = 0;
	target = 0;
}

GpuBlend GPURender::currentBlend() const{
	if (!blend.current)
		return GpuBlend::None;

	switch (activeBlend){
	case BlendKind::KeepDestAlpha : return GpuBlend::KeepDestAlpha;
	case BlendKind::Normal        : return GpuBlend::Normal;
	case BlendKind::Addition      : return GpuBlend::Addition;
	case BlendKind::Substraction  : return GpuBlend::Substraction;
	case BlendKind::Multiply      : return GpuBlend::Multiply;
	case BlendKind::ToneAdd       : return GpuBlend::ToneAdd;
	case BlendKind::ToneSubtract  : return GpuBlend::ToneSubtract;
	}

	return GpuBlend::Normal;
}

void GPURender::fillCommandState(GpuCommand &c, const EffectUniforms &u, int effect){
	memset(&c.vertexParams, 0, sizeof(c.vertexParams));
	memset(&c.fragParams, 0, sizeof(c.fragParams));

	if (usesSpriteMat(effect))
		multiplyMatrices(u.proj, u.spriteMat, c.vertexParams.xform);
	else if (effect == SHADER_simpleMatrix)
		multiplyMatrices(u.proj, u.matrix, c.vertexParams.xform);
	else
		memcpy(c.vertexParams.xform, u.proj, sizeof(float) * 16);

	const float invX = u.texSize.x != 0 ? 1.0f / u.texSize.x : 1.0f;
	const float invY = u.texSize.y != 0 ? 1.0f / u.texSize.y : 1.0f;

	if (usesTranslation(effect)){
		c.vertexParams.translation[0] = u.translation.x;
		c.vertexParams.translation[1] = u.translation.y;
	}

	c.vertexParams.texSizeInv[0] = invX;
	c.vertexParams.texSizeInv[1] = invY;
	c.vertexParams.offset[0] = u.offset.x;
	c.vertexParams.offset[1] = u.offset.y;
	c.vertexParams.aniIndex = u.aniIndex;
	c.vertexParams.tilemapMode = (effect == SHADER_tilemap || effect == SHADER_tilemapWater) ? 1 : 0;
	c.vertexParams.blurMode = effect == SHADER_blur ? (blurPass == 0 ? 1 : 2) : 0;

	GpuFragParams &f = c.fragParams;
	f.tone[0] = u.tone.x; f.tone[1] = u.tone.y; f.tone[2] = u.tone.z; f.tone[3] = u.tone.w;
	f.color[0] = u.color.x; f.color[1] = u.color.y; f.color[2] = u.color.z; f.color[3] = u.color.w;
	f.flash[0] = u.flash.x; f.flash[1] = u.flash.y; f.flash[2] = u.flash.z; f.flash[3] = u.flash.w;
	f.modulate[0] = u.modulate.x; f.modulate[1] = u.modulate.y; f.modulate[2] = u.modulate.z; f.modulate[3] = u.modulate.w;
	f.subRect[0] = u.subRect.x; f.subRect[1] = u.subRect.y; f.subRect[2] = u.subRect.z; f.subRect[3] = u.subRect.w;
	f.texSizeInv[0] = invX;
	f.texSizeInv[1] = invY;
	f.opacity = u.opacity;
	f.gray = u.gray;
	f.hueAdjust = u.hueAdjust;
	f.bushDepth = u.bushDepth;
	f.bushOpacity = u.bushOpacity;
	f.time = u.time;
	f.prog = u.prog;
	f.vague = u.vague;
	f.effect = effect;

	c.tex[0] = boundTexture;
	c.tex[1] = u.aux[1];
	c.tex[2] = u.aux[2];
	c.tex[3] = u.aux[3];

	for (int i = 0; i < 4; ++i){
		GpuTexture *t = texture(c.tex[i]);
		c.sampler[i] = t ? (t->smooth ? 1 : 0) | (t->repeat ? 2 : 0) : 0;
	}
}

void GPURender::pushDraw(const GpuCommand &c){
	if (!commands.empty() && c.pipeline == PipelineUber){
		GpuCommand &last = commands.back();

		if (last.type == GpuCommand::Draw && last.pipeline == PipelineUber
		    && last.target == c.target && last.blend == c.blend
		    && last.scissorOn == c.scissorOn
		    && last.firstVertex + last.quadCount * 4 == c.firstVertex
		    && last.quadCount + c.quadCount <= (uint32_t) MaxQuadsPerDraw
		    && memcmp(&last.viewport, &c.viewport, sizeof(SDL_Rect)) == 0
		    && memcmp(&last.scissor, &c.scissor, sizeof(SDL_Rect)) == 0
		    && memcmp(&last.vertexParams, &c.vertexParams, sizeof(c.vertexParams)) == 0
		    && memcmp(&last.fragParams, &c.fragParams, sizeof(c.fragParams)) == 0
		    && memcmp(last.tex, c.tex, sizeof(c.tex)) == 0
		    && memcmp(last.sampler, c.sampler, sizeof(c.sampler)) == 0){
			last.quadCount += c.quadCount;
			return;
		}
	}

	commands.push_back(c);
}

void GPURender::recordQuad(const GpuVertex verts[4], int effect, GpuBlend blendKind, const SDL_Rect &vp, bool scissor, const SDL_Rect &scissorRect, uint32_t targetTexture){
	GpuCommand c;
	memset(&c, 0, sizeof(c));
	c.type = GpuCommand::Draw;
	c.target = targetTexture;
	c.pipeline = PipelineUber;
	c.blend = blendKind;
	c.viewport = vp;
	c.scissorOn = scissor;
	c.scissor = scissorRect;
	c.firstVertex = (uint32_t) vertexStream.size();
	c.quadCount = 1;
	fillCommandState(c, uniforms(), effect);

	for (int i = 0; i < 4; ++i)
		vertexStream.push_back(verts[i]);

	pushDraw(c);
}

void GPURender::clear(){
	const Vec4 &col = clearColor.current;

	if (!scissorTest.current){
		recordClear(targetId, col);
		return;
	}

	const Vec2i size = targetSize();

	GpuCommand c;
	memset(&c, 0, sizeof(c));
	c.type = GpuCommand::Draw;
	c.target = targetId;
	c.pipeline = PipelineUber;
	c.blend = GpuBlend::None;
	c.viewport = SDL_Rect{ 0, 0, size.x, size.y };
	c.scissorOn = true;
	c.scissor = scissorBoxProp.current;
	c.firstVertex = (uint32_t) vertexStream.size();
	c.quadCount = 1;

	orthoMatrix(c.vertexParams.xform, size.x, size.y);
	c.vertexParams.texSizeInv[0] = c.vertexParams.texSizeInv[1] = 1.0f;
	c.fragParams.effect = SHADER_flatColor;
	c.fragParams.color[0] = col.x;
	c.fragParams.color[1] = col.y;
	c.fragParams.color[2] = col.z;
	c.fragParams.color[3] = col.w;

	GpuVertex v[4];
	memset(v, 0, sizeof(v));
	v[1].x = size.x;
	v[2].x = size.x; v[2].y = size.y;
	v[3].y = size.y;

	for (int i = 0; i < 4; ++i){
		v[i].r = col.x; v[i].g = col.y; v[i].b = col.z; v[i].a = col.w;
		vertexStream.push_back(v[i]);
	}

	pushDraw(c);
}

void GPURender::readPixels(const RenderTarget &rt, int w, int h, void *out){
	GpuTexture *tex = texture(rt.fbo.id);
	if (!tex || !tex->texture)
		return;

	flush();

	const Uint32 size = (Uint32) ((size_t) tex->width * tex->height * 4);

	SDL_GPUTransferBufferCreateInfo binfo;
	memset(&binfo, 0, sizeof(binfo));
	binfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
	binfo.size = size;
	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device, &binfo);

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(cmd);

	SDL_GPUTextureRegion region;
	memset(&region, 0, sizeof(region));
	region.texture = tex->texture;
	region.w = tex->width;
	region.h = tex->height;
	region.d = 1;

	SDL_GPUTextureTransferInfo info = { transfer, 0, (Uint32) tex->width, (Uint32) tex->height };
	SDL_DownloadFromGPUTexture(copy, &region, &info);
	SDL_EndGPUCopyPass(copy);

	SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
	SDL_WaitForGPUFences(device, true, &fence, 1);
	SDL_ReleaseGPUFence(device, fence);

	const uint8_t *src = static_cast<const uint8_t*>(SDL_MapGPUTransferBuffer(device, transfer, false));
	uint8_t *dst = static_cast<uint8_t*>(out);
	const int cw = std::min(w, tex->width);
	const int ch = std::min(h, tex->height);

	for (int y = 0; y < ch; ++y)
		memcpy(dst + (size_t) y * w * 4, src + (size_t) y * tex->width * 4, (size_t) cw * 4);

	SDL_UnmapGPUTransferBuffer(device, transfer);
	SDL_ReleaseGPUTransferBuffer(device, transfer);
}

GeometryHandle GPURender::createGeometry(VertexLayout layout){
	std::unique_ptr<GpuGeometry> geom(new GpuGeometry);
	geom->layout = layout;

	return GeometryHandle(geometries.add(std::move(geom)));
}

void GPURender::destroyGeometry(GeometryHandle geom){
	geometries.remove(geom.id);
}

static size_t layoutStride(VertexLayout layout){
	switch (layout){
	case VertexLayout::Simple : return sizeof(SVertex);
	case VertexLayout::Color  : return sizeof(CVertex);
	case VertexLayout::Full   : return sizeof(Vertex);
	}

	return sizeof(Vertex);
}

static void convertVertices(VertexLayout layout, const uint8_t *src, size_t count, GpuVertex *dst){
	const size_t stride = layoutStride(layout);

	for (size_t i = 0; i < count; ++i){
		GpuVertex &out = dst[i];
		const uint8_t *p = src + i * stride;

		switch (layout){
		case VertexLayout::Simple :
			{
				SVertex v;
				memcpy(&v, p, sizeof(v));
				out.x = v.pos.x; out.y = v.pos.y; out.u = v.texPos.x; out.v = v.texPos.y;
				out.r = 0; out.g = 0; out.b = 0; out.a = 1;
				break;
			}

		case VertexLayout::Color :
			{
				CVertex v;
				memcpy(&v, p, sizeof(v));
				out.x = v.pos.x; out.y = v.pos.y; out.u = 0; out.v = 0;
				out.r = v.color.x; out.g = v.color.y; out.b = v.color.z; out.a = v.color.w;
				break;
			}

		case VertexLayout::Full :
			{
				Vertex v;
				memcpy(&v, p, sizeof(v));
				out.x = v.pos.x; out.y = v.pos.y; out.u = v.texPos.x; out.v = v.texPos.y;
				out.r = v.color.x; out.g = v.color.y; out.b = v.color.z; out.a = v.color.w;
				break;
			}
		}
	}
}

void GPURender::allocGeometry(GeometryHandle geom, size_t bytes, GeometryUsage){
	GpuGeometry &g = *geometries.get(geom.id);
	GpuVertex zero;
	memset(&zero, 0, sizeof(zero));
	zero.a = 1;
	g.vertices.assign(bytes / layoutStride(g.layout), zero);
}

void GPURender::uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage){
	GpuGeometry &g = *geometries.get(geom.id);
	const size_t count = bytes / layoutStride(g.layout);
	g.vertices.resize(count);
	convertVertices(g.layout, static_cast<const uint8_t*>(data), count, g.vertices.data());
}

void GPURender::uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data){
	GpuGeometry &g = *geometries.get(geom.id);
	const size_t stride = layoutStride(g.layout);
	const size_t first = offset / stride;
	const size_t count = bytes / stride;

	if (g.vertices.size() < first + count){
		GpuVertex zero;
		memset(&zero, 0, sizeof(zero));
		zero.a = 1;
		g.vertices.resize(first + count, zero);
	}

	convertVertices(g.layout, static_cast<const uint8_t*>(data), count, &g.vertices[first]);
}

void GPURender::ensureQuadIndices(size_t quadCount){
	neededQuads = std::max(neededQuads, std::min<size_t>(quadCount, MaxQuadsPerDraw));
}

void GPURender::drawQuads(GeometryHandle geom, size_t firstQuad, size_t quadCount){
	const GpuGeometry &g = *geometries.get(geom.id);
	const int effect = currentSlot;
	const EffectUniforms &u = uniforms();

	while (quadCount > 0){
		size_t chunk = std::min<size_t>(quadCount, MaxQuadsPerDraw);

		if ((firstQuad + chunk) * 4 > g.vertices.size())
			break;

		GpuCommand c;
		memset(&c, 0, sizeof(c));
		c.type = GpuCommand::Draw;
		c.target = targetId;
		c.pipeline = effect == SHADER_dynamicLight ? PipelineLight : PipelineUber;
		c.blend = currentBlend();
		c.viewport = viewport.current;
		c.scissorOn = scissorTest.current;
		c.scissor = scissorBoxProp.current;
		c.firstVertex = (uint32_t) vertexStream.size();
		c.quadCount = (uint32_t) chunk;
		fillCommandState(c, u, effect);

		if (effect == SHADER_dynamicLight){
			GpuLightParams lp;
			memset(&lp, 0, sizeof(lp));
			lp.texSizeInv[0] = c.fragParams.texSizeInv[0];
			lp.texSizeInv[1] = c.fragParams.texSizeInv[1];
			lp.cameraPos[0] = cameraPos.x;
			lp.cameraPos[1] = cameraPos.y;
			lp.ambient = ambient;
			lp.count = (int) std::min<size_t>(lights.size(), 64);

			for (int i = 0; i < lp.count; ++i){
				const LightSource &s = lights[i];
				lp.sources[i][0] = s.x;
				lp.sources[i][1] = s.y;
				lp.sources[i][2] = s.power;
				lp.sources[i][3] = s.radius;
				lp.colors[i][0] = (float) (s.color.red / 255.0);
				lp.colors[i][1] = (float) (s.color.green / 255.0);
				lp.colors[i][2] = (float) (s.color.blue / 255.0);
				lp.colors[i][3] = (float) (s.color.alpha / 255.0);
			}

			lightPool.push_back(lp);
			c.payload = (int) lightPool.size() - 1;
		}

		vertexStream.insert(vertexStream.end(), g.vertices.begin() + firstQuad * 4, g.vertices.begin() + (firstQuad + chunk) * 4);
		pushDraw(c);

		firstQuad += chunk;
		quadCount -= chunk;
	}
}

void GPURender::beginBlit(const RenderTarget &rt){
	targetId = rt.fbo.id;
	target = texture(targetId);
	viewport.pushSet(IntRect(0, 0, rt.width, rt.height));
	useEffect(SHADER_simple);
	applyViewportProj();
	setTranslation(Vec2i());
}

void GPURender::beginBlitScreen(const Vec2i &size){
	targetId = 0;
	target = 0;
	viewport.pushSet(IntRect(0, 0, size.x, size.y));
	useEffect(SHADER_simple);
	applyViewportProj();
	setTranslation(Vec2i());
}

void GPURender::blitSource(const RenderTarget &source){
	setTexSize(Vec2i(source.width, source.height));
	boundTexture = source.tex.id;
}

void GPURender::blitRect(const IntRect &src, const Vec2i &dstPos){
	blitRect(src, IntRect(dstPos.x, dstPos.y, src.w, src.h), false);
}

void GPURender::blitRect(const IntRect &src, const IntRect &dst, bool smooth){
	GpuTexture *srcTex = texture(boundTexture);
	if (!srcTex || !srcTex->texture)
		return;

	float left = dst.x;
	float width = dst.w;
	float top = dst.y;
	float height = dst.h;
	bool flipV = false;

	if (!target){
		int screenH = windowSize().y;

		if (dst.h < 0){
			top = screenH - dst.y;
			height = -dst.h;
		}else{
			top = screenH - (dst.y + dst.h);
			height = dst.h;
			flipV = true;
		}
	}

	float v0 = src.y;
	float v1 = src.y + src.h;

	if (flipV)
		std::swap(v0, v1);

	GpuVertex verts[4];
	memset(verts, 0, sizeof(verts));
	verts[0].x = left;         verts[0].y = top;          verts[0].u = src.x;         verts[0].v = v0;
	verts[1].x = left + width; verts[1].y = top;          verts[1].u = src.x + src.w; verts[1].v = v0;
	verts[2].x = left + width; verts[2].y = top + height; verts[2].u = src.x + src.w; verts[2].v = v1;
	verts[3].x = left;         verts[3].y = top + height; verts[3].u = src.x;         verts[3].v = v1;

	for (int i = 0; i < 4; ++i)
		verts[i].r = verts[i].g = verts[i].b = verts[i].a = 1.0f;

	const bool wasSmooth = srcTex->smooth;
	srcTex->smooth = wasSmooth || smooth;

	recordQuad(verts, SHADER_simple, GpuBlend::None, viewport.current, scissorTest.current, scissorBoxProp.current, targetId);

	srcTex->smooth = wasSmooth;
}

void GPURender::endBlit(){
	viewport.pop();
}

SDL_GPUSampler *GPURender::samplerFor(int flags){
	if (!samplers[flags]){
		SDL_GPUSamplerCreateInfo info;
		memset(&info, 0, sizeof(info));
		const SDL_GPUFilter filter = (flags & 1) ? SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST;
		const SDL_GPUSamplerAddressMode mode = (flags & 2) ? SDL_GPU_SAMPLERADDRESSMODE_REPEAT : SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		info.min_filter = filter;
		info.mag_filter = filter;
		info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
		info.address_mode_u = mode;
		info.address_mode_v = mode;
		info.address_mode_w = mode;
		samplers[flags] = SDL_CreateGPUSampler(device, &info);
	}

	return samplers[flags];
}

SDL_GPUGraphicsPipeline *GPURender::pipelineFor(int kind, GpuBlend blendKind, SDL_GPUTextureFormat format){
	PipelineKey key;
	key.pipeline = kind;
	key.blend = (int) blendKind;
	key.format = (int) format;

	std::map<PipelineKey, SDL_GPUGraphicsPipeline*>::iterator found = pipelines.find(key);
	if (found != pipelines.end())
		return found->second;

	SDL_GPUVertexBufferDescription vbd;
	memset(&vbd, 0, sizeof(vbd));
	vbd.slot = 0;
	vbd.pitch = sizeof(GpuVertex);
	vbd.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

	SDL_GPUVertexAttribute attrs[3];
	memset(attrs, 0, sizeof(attrs));
	attrs[0].location = 0; attrs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2; attrs[0].offset = offsetof(GpuVertex, x);
	attrs[1].location = 1; attrs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2; attrs[1].offset = offsetof(GpuVertex, u);
	attrs[2].location = 2; attrs[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; attrs[2].offset = offsetof(GpuVertex, r);

	SDL_GPUColorTargetDescription ctd;
	memset(&ctd, 0, sizeof(ctd));
	ctd.format = format;
	ctd.blend_state = blendState(blendKind);

	SDL_GPUGraphicsPipelineCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.vertex_shader = vertexShader;
	info.fragment_shader = kind == PipelineLight ? lightShader : uberShader;
	info.vertex_input_state.vertex_buffer_descriptions = &vbd;
	info.vertex_input_state.num_vertex_buffers = 1;
	info.vertex_input_state.vertex_attributes = attrs;
	info.vertex_input_state.num_vertex_attributes = 3;
	info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
	info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
	info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
	info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
	info.target_info.color_target_descriptions = &ctd;
	info.target_info.num_color_targets = 1;

	SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, &info);
	if (!pipeline)
		Debug() << "[GPURender] cannot create pipeline" << SDL_GetError();

	pipelines[key] = pipeline;
	return pipeline;
}

struct GPURender::FlushState{
	SDL_GPUCommandBuffer *cmd;
	SDL_GPURenderPass *pass;
	uint32_t passTarget;
	SDL_GPUGraphicsPipeline *lastPipeline;
	SDL_GPUTexture *swapTexture;
	Uint32 swapW;
	Uint32 swapH;
	bool swapTried;
	SDL_GPUTextureFormat swapFormat;
};

struct GPURender::ResolvedTarget{
	SDL_GPUTexture *tex;
	int w;
	int h;
	SDL_GPUTextureFormat format;
};

SDL_GPUTransferBuffer *GPURender::createUploadBuffer(const void *data, size_t size){
	SDL_GPUTransferBufferCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	info.size = (Uint32) size;

	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device, &info);
	memcpy(SDL_MapGPUTransferBuffer(device, transfer, false), data, size);
	SDL_UnmapGPUTransferBuffer(device, transfer);
	return transfer;
}

void GPURender::ensureVertexCapacity(size_t bytes){
	if (vertexBuffer && vertexCapacity >= bytes)
		return;

	size_t capacity = std::max<size_t>(bytes, 1 << 20);

	if (vertexBuffer)
		SDL_ReleaseGPUBuffer(device, vertexBuffer);

	SDL_GPUBufferCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	info.size = (Uint32) capacity;
	vertexBuffer = SDL_CreateGPUBuffer(device, &info);
	vertexCapacity = capacity;
}

void GPURender::ensureIndexBuffer(size_t quads){
	if (indexBuffer && indexQuads >= quads)
		return;

	size_t count = std::max<size_t>(quads, 256);
	std::vector<uint16_t> indices(count * 6);

	for (size_t i = 0; i < count; ++i){
		static const uint16_t pattern[6] = { 0, 1, 2, 2, 3, 0 };
		for (int j = 0; j < 6; ++j)
			indices[i * 6 + j] = (uint16_t) (i * 4 + pattern[j]);
	}

	if (indexBuffer)
		SDL_ReleaseGPUBuffer(device, indexBuffer);

	const Uint32 bytes = (Uint32) (indices.size() * sizeof(uint16_t));

	SDL_GPUBufferCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
	info.size = bytes;
	indexBuffer = SDL_CreateGPUBuffer(device, &info);
	indexQuads = count;

	SDL_GPUTransferBuffer *transfer = createUploadBuffer(indices.data(), bytes);

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTransferBufferLocation src = { transfer, 0 };
	SDL_GPUBufferRegion dst = { indexBuffer, 0, bytes };
	SDL_UploadToGPUBuffer(copy, &src, &dst, false);
	SDL_EndGPUCopyPass(copy);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, transfer);
}

void GPURender::uploadVertexStream(SDL_GPUCommandBuffer *cmd){
	if (vertexStream.empty())
		return;

	const size_t bytes = vertexStream.size() * sizeof(GpuVertex);
	ensureVertexCapacity(bytes);

	SDL_GPUTransferBuffer *transfer = createUploadBuffer(vertexStream.data(), bytes);

	SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTransferBufferLocation src = { transfer, 0 };
	SDL_GPUBufferRegion dst = { vertexBuffer, 0, (Uint32) bytes };
	SDL_UploadToGPUBuffer(copy, &src, &dst, true);
	SDL_EndGPUCopyPass(copy);
	SDL_ReleaseGPUTransferBuffer(device, transfer);
}

void GPURender::endPass(FlushState &s){
	if (s.pass)
		SDL_EndGPURenderPass(s.pass);

	s.pass = 0;
	s.lastPipeline = 0;
}

void GPURender::beginPass(FlushState &s, const ResolvedTarget &r, uint32_t id, SDL_GPULoadOp loadOp, const float *color){
	endPass(s);

	SDL_GPUColorTargetInfo info;
	memset(&info, 0, sizeof(info));
	info.texture = r.tex;
	info.load_op = loadOp;
	info.store_op = SDL_GPU_STOREOP_STORE;

	if (color){
		info.clear_color.r = color[0];
		info.clear_color.g = color[1];
		info.clear_color.b = color[2];
		info.clear_color.a = color[3];
	}

	s.pass = SDL_BeginGPURenderPass(s.cmd, &info, 1, 0);
	s.passTarget = id;
}

bool GPURender::resolveTarget(FlushState &s, uint32_t id, ResolvedTarget &out){
	if (id == 0){
		if (!s.swapTried){
			s.swapTried = true;
			SDL_WaitAndAcquireGPUSwapchainTexture(s.cmd, window, &s.swapTexture, &s.swapW, &s.swapH);
		}

		if (!s.swapTexture)
			return false;

		out.tex = s.swapTexture;
		out.w = (int) s.swapW;
		out.h = (int) s.swapH;
		out.format = s.swapFormat;
		return true;
	}

	GpuTexture *t = texture(id);
	if (!t || !t->texture)
		return false;

	out.tex = t->texture;
	out.w = t->width;
	out.h = t->height;
	out.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	return true;
}

void GPURender::executeUpload(FlushState &s, const GpuCommand &c){
	endPass(s);

	GpuTexture *tex = texture(c.target);
	if (!tex || !tex->texture)
		return;

	const std::vector<uint8_t> &data = uploads[c.payload];
	renderStatsUpload(data.size());
	SDL_GPUTransferBuffer *transfer = createUploadBuffer(data.data(), data.size());

	SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(s.cmd);
	SDL_GPUTextureTransferInfo src = { transfer, 0, (Uint32) c.rect.w, (Uint32) c.rect.h };
	SDL_GPUTextureRegion dst;
	memset(&dst, 0, sizeof(dst));
	dst.texture = tex->texture;
	dst.x = c.rect.x;
	dst.y = c.rect.y;
	dst.w = c.rect.w;
	dst.h = c.rect.h;
	dst.d = 1;
	SDL_UploadToGPUTexture(copy, &src, &dst, false);
	SDL_EndGPUCopyPass(copy);
	SDL_ReleaseGPUTransferBuffer(device, transfer);
}

void GPURender::executeDraw(FlushState &s, const GpuCommand &c, const ResolvedTarget &r){
	if (!s.pass || s.passTarget != c.target)
		beginPass(s, r, c.target, SDL_GPU_LOADOP_LOAD, 0);

	if (c.viewport.w <= 0 || c.viewport.h <= 0)
		return;

	SDL_GPUGraphicsPipeline *pipeline = pipelineFor(c.pipeline, c.blend, r.format);
	if (!pipeline)
		return;

	if (pipeline != s.lastPipeline){
		SDL_BindGPUGraphicsPipeline(s.pass, pipeline);
		s.lastPipeline = pipeline;

		SDL_GPUBufferBinding vb = { vertexBuffer, 0 };
		SDL_BindGPUVertexBuffers(s.pass, 0, &vb, 1);
		SDL_GPUBufferBinding ib = { indexBuffer, 0 };
		SDL_BindGPUIndexBuffer(s.pass, &ib, SDL_GPU_INDEXELEMENTSIZE_16BIT);
	}

	SDL_PushGPUVertexUniformData(s.cmd, 0, &c.vertexParams, sizeof(c.vertexParams));

	if (c.pipeline == PipelineLight){
		SDL_PushGPUFragmentUniformData(s.cmd, 0, &lightPool[c.payload], sizeof(GpuLightParams));
	}else{
		SDL_PushGPUFragmentUniformData(s.cmd, 0, &c.fragParams, sizeof(c.fragParams));

		SDL_GPUTextureSamplerBinding bindings[4];
		for (int i = 0; i < 4; ++i){
			GpuTexture *t = texture(c.tex[i]);
			bindings[i].texture = (t && t->texture) ? t->texture : dummy;
			bindings[i].sampler = samplerFor(c.sampler[i]);
		}

		SDL_BindGPUFragmentSamplers(s.pass, 0, bindings, 4);
	}

	SDL_GPUViewport vp = { (float) c.viewport.x, (float) c.viewport.y, (float) c.viewport.w, (float) c.viewport.h, 0.0f, 1.0f };
	SDL_SetGPUViewport(s.pass, &vp);

	SDL_Rect scissor = { 0, 0, r.w, r.h };
	if (c.scissorOn){
		scissor.x = std::max(c.scissor.x, 0);
		scissor.y = std::max(c.scissor.y, 0);
		scissor.w = std::max(std::min(c.scissor.x + c.scissor.w, r.w) - scissor.x, 0);
		scissor.h = std::max(std::min(c.scissor.y + c.scissor.h, r.h) - scissor.y, 0);
	}
	SDL_SetGPUScissor(s.pass, &scissor);

	++renderStats.drawCalls;
	renderStats.vertices += c.quadCount * 4;
	SDL_DrawGPUIndexedPrimitives(s.pass, c.quadCount * 6, 1, 0, (Sint32) c.firstVertex, 0);
}

void GPURender::flush(){
	if (commands.empty()){
		releasePending();
		return;
	}

	ensureIndexBuffer(neededQuads);

	FlushState s;
	memset(&s, 0, sizeof(s));
	s.cmd = SDL_AcquireGPUCommandBuffer(device);
	s.swapFormat = SDL_GetGPUSwapchainTextureFormat(device, window);

	uploadVertexStream(s.cmd);

	for (size_t i = 0; i < commands.size(); ++i){
		const GpuCommand &c = commands[i];

		if (c.type == GpuCommand::Upload){
			executeUpload(s, c);
			continue;
		}

		ResolvedTarget r;
		if (!resolveTarget(s, c.target, r))
			continue;

		if (c.type == GpuCommand::Clear)
			beginPass(s, r, c.target, SDL_GPU_LOADOP_CLEAR, c.clearColor);
		else
			executeDraw(s, c, r);
	}

	endPass(s);
	SDL_SubmitGPUCommandBuffer(s.cmd);

	commands.clear();
	vertexStream.clear();
	uploads.clear();
	lightPool.clear();
	neededQuads = 1;
	releasePending();
}

void GPURender::swapWindow(SDL_Window *){
	renderStatsBeginSwap();
	flush();
	renderStatsEndSwap();
}

void GPURender::suspendContext(SDL_Window *){
}

void GPURender::resumeContext(SDL_Window *){
}

static SDL_GPUDevice *probedDevice = 0;

class GPURenderContext : public IRenderContext{
public:
	GPURenderContext(SDL_Window *window)
	    : window(window)
	{
		device = probedDevice;
		probedDevice = 0;

		if (!device)
			device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, 0);

		if (!device)
			throw Exception(Exception::MKXPError, "Failed to create GPU device: %s", SDL_GetError());

		if (!SDL_ClaimWindowForGPUDevice(device, window)){
			SDL_DestroyGPUDevice(device);
			throw Exception(Exception::MKXPError, "Failed to claim window for GPU device: %s", SDL_GetError());
		}

		SDL_GPUPresentMode mode = SDL_GPU_PRESENTMODE_VSYNC;
		if (!(conf.syncToRefreshrate && conf.fixedFramerate == 0) && SDL_WindowSupportsGPUPresentMode(device, window, SDL_GPU_PRESENTMODE_IMMEDIATE))
			mode = SDL_GPU_PRESENTMODE_IMMEDIATE;

		SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, mode);

		Debug() << "[GPURender] driver:" << SDL_GetGPUDeviceDriver(device);
		createdDevice = device;
		createdWindow = window;
	}

	~GPURenderContext(){
		createdDevice = 0;
		createdWindow = 0;
		SDL_ReleaseWindowFromGPUDevice(device, window);
		SDL_DestroyGPUDevice(device);
	}

private:
	SDL_GPUDevice *device;
	SDL_Window *window;
};

uint64_t gpuWindowFlags(){
	return 0;
}

void gpuSetupWindowAttributes(){
}

bool gpuProbe(SDL_Window *window){
#ifdef SUNSHINE_GPU_SHADERS
	if (!SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_SPIRV, 0))
		return false;

	SDL_GPUDevice *probe = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, 0);
	if (!probe)
		return false;

	if (!SDL_ClaimWindowForGPUDevice(probe, window)){
		SDL_DestroyGPUDevice(probe);
		return false;
	}

	SDL_ReleaseWindowFromGPUDevice(probe, window);

	if (probedDevice)
		SDL_DestroyGPUDevice(probedDevice);

	probedDevice = probe;
	return true;
#else
	(void) window;
	return false;
#endif
}

IRenderContext *createGPURenderContext(SDL_Window *window){
	return new GPURenderContext(window);
}

IRender *createGPURender(const Config &conf){
	return new GPURender(conf, createdDevice, createdWindow);
}
