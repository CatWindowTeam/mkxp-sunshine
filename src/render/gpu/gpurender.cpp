#include "gpurender.h"
#include "render/backends.h"
#include "config.h"
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

GpuUniforms::GpuUniforms()
    : texSize(1, 1),
      modulate(1, 1, 1, 1),
      subRect(0, 0, 1, 1),
      time(0),
      opacity(1),
      bushDepth(0),
      bushOpacity(1),
      gray(0),
      hueAdjust(0),
      aniIndex(0),
      prog(0),
      vague(0)
{
	for (int i = 0; i < 16; ++i)
		proj[i] = spriteMat[i] = matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;

	aux[0] = aux[1] = aux[2] = aux[3] = 0;
}

static void matMul(const float a[16], const float b[16], float out[16]){
	float res[16];

	for (int col = 0; col < 4; ++col)
		for (int row = 0; row < 4; ++row){
			float sum = 0;
			for (int k = 0; k < 4; ++k)
				sum += a[k * 4 + row] * b[col * 4 + k];
			res[col * 4 + row] = sum;
		}

	for (int i = 0; i < 16; ++i)
		out[i] = res[i];
}

static void orthoMatrix(float out[16], int w, int h){
	const float a = 2.f / w;
	const float b = 2.f / h;
	const float c = -2.f;

	const float mat[16] = {
		 a,  0,  0,  0,
		 0,  b,  0,  0,
		 0,  0,  c,  0,
		-1, -1, -1,  1
	};

	for (int i = 0; i < 16; ++i)
		out[i] = mat[i];
}

static void perspectiveMatrix(float out[16], int w, int h, float fov){
	const float width  = (float) w;
	const float height = (float) h;

	const float camDist = (height * 0.5f) / tanf(fov * PI / 360.0f);

	const float nearPlane = 0.01f;
	const float farPlane  = 10000.0f;

	const float sx = 2.0f * camDist / width;
	const float sy = 2.0f * camDist / height;

	const float A = (farPlane + nearPlane) / (farPlane - nearPlane);
	const float B = (2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

	const float mat[16] = {
		      sx,        0,               0,       0,
		       0,       sy,               0,       0,
		       0,        0,              -A,      -1,
		-camDist, -camDist, A * camDist - B, camDist
	};

	for (int i = 0; i < 16; ++i)
		out[i] = mat[i];
}

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
    : device(device),
      window(window),
      vertexShader(0),
      uberShader(0),
      lightShader(0),
      dummy(0),
      target(0),
      targetId(0),
      boundTexture(0),
      maxTexSize(conf.maxTextureSize > 0 ? conf.maxTextureSize : 16384),
      activeBlend(GpuBlend::Normal),
      blendOverridden(false),
      uniformSets(SHADER_COUNT + 1),
      currentSlot(SHADER_simple),
      blurPass(0),
      ambient(0),
      vertexBuffer(0),
      vertexCapacity(0),
      indexBuffer(0),
      indexQuads(0),
      neededQuads(1)
{
	for (int i = 0; i < 4; ++i)
		samplers[i] = 0;

	viewport.current = IntRect(0, 0, conf.defScreenW, conf.defScreenH);
	blend.current = true;
	blendModeProp.current = BlendNormal;
	scissorTest.current = false;
	scissorBoxProp.current = IntRect(0, 0, conf.defScreenW, conf.defScreenH);
	clearColor.current = Vec4(0, 0, 0, 1);

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

	setActiveRender(this);
}

GPURender::~GPURender(){
	setActiveRender(0);

	flush();
	SDL_WaitForGPUIdle(device);
	releasePending();

	for (size_t i = 0; i < textures.size(); ++i)
		if (textures[i] && textures[i]->texture)
			SDL_ReleaseGPUTexture(device, textures[i]->texture);

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

bool GPURender::repeatNpotSupported() const{
	return true;
}

GpuTexture *GPURender::texture(uint32_t id){
	if (id == 0 || id > textures.size())
		return 0;

	return textures[id-1].get();
}

GpuUniforms &GPURender::uniforms(){
	return uniformSets[currentSlot];
}

int GPURender::screenHeight(){
	int w = 0, h = 0;
	SDL_GetWindowSizeInPixels(window, &w, &h);
	return h;
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

	uint32_t id;
	if (freeTextures.empty()){
		textures.push_back(std::move(tex));
		id = textures.size();
	}else{
		id = freeTextures.back();
		freeTextures.pop_back();
		textures[id-1] = std::move(tex);
	}

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
		GpuTexture *tex = textures[id-1].get();

		if (tex && tex->texture)
			SDL_ReleaseGPUTexture(device, tex->texture);

		textures[id-1].reset();
		freeTextures.push_back(id);
	}

	quarantine.clear();
}

void GPURender::bindTexture(TexHandle tex){
	boundTexture = tex.id;
}

void GPURender::unbindTexture(){
	boundTexture = 0;
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

	return activeBlend;
}

static GpuBlend blendFromMode(BlendType mode){
	switch (mode){
	case BlendKeepDestAlpha : return GpuBlend::KeepDestAlpha;
	case BlendAddition      : return GpuBlend::Addition;
	case BlendSubstraction  : return GpuBlend::Substraction;
	case BlendMultiply      : return GpuBlend::Multiply;
	default                 : return GpuBlend::Normal;
	}
}

void GPURender::fillCommandState(GpuCommand &c, const GpuUniforms &u, int effect){
	memset(&c.vertexParams, 0, sizeof(c.vertexParams));
	memset(&c.fragParams, 0, sizeof(c.fragParams));

	if (usesSpriteMat(effect))
		matMul(u.proj, u.spriteMat, c.vertexParams.xform);
	else if (effect == SHADER_simpleMatrix)
		matMul(u.proj, u.matrix, c.vertexParams.xform);
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

	commands.push_back(c);
}

void GPURender::clear(){
	const Vec4 &col = clearColor.current;

	if (!scissorTest.current){
		recordClear(targetId, col);
		return;
	}

	int tw = target ? target->width : 0;
	int th = target ? target->height : screenHeight();

	if (!target){
		int w = 0, h = 0;
		SDL_GetWindowSizeInPixels(window, &w, &h);
		tw = w;
	}

	GpuCommand c;
	memset(&c, 0, sizeof(c));
	c.type = GpuCommand::Draw;
	c.target = targetId;
	c.pipeline = PipelineUber;
	c.blend = GpuBlend::None;
	c.viewport.x = 0;
	c.viewport.y = 0;
	c.viewport.w = tw;
	c.viewport.h = th;
	c.scissorOn = true;
	c.scissor = scissorBoxProp.current;
	c.firstVertex = (uint32_t) vertexStream.size();
	c.quadCount = 1;

	orthoMatrix(c.vertexParams.xform, tw, th);
	c.vertexParams.texSizeInv[0] = c.vertexParams.texSizeInv[1] = 1.0f;
	c.fragParams.effect = SHADER_flatColor;
	c.fragParams.color[0] = col.x;
	c.fragParams.color[1] = col.y;
	c.fragParams.color[2] = col.z;
	c.fragParams.color[3] = col.w;

	GpuVertex v[4];
	memset(v, 0, sizeof(v));
	v[0].x = 0;  v[0].y = 0;
	v[1].x = tw; v[1].y = 0;
	v[2].x = tw; v[2].y = th;
	v[3].x = 0;  v[3].y = th;

	for (int i = 0; i < 4; ++i){
		v[i].r = col.x; v[i].g = col.y; v[i].b = col.z; v[i].a = col.w;
		vertexStream.push_back(v[i]);
	}

	commands.push_back(c);
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

void GPURender::setViewport(const IntRect &rect){
	viewport.set(rect);
}

void GPURender::pushViewport(const IntRect &rect){
	viewport.pushSet(rect);
}

void GPURender::popViewport(){
	viewport.pop();
}

void GPURender::refreshViewport(){
}

void GPURender::pushBlend(bool enabled){
	blend.pushSet(enabled);
}

void GPURender::popBlend(){
	blend.pop();
}

void GPURender::pushBlendMode(BlendType mode){
	blendModeProp.pushSet(mode);
	activeBlend = blendFromMode(mode);
}

void GPURender::popBlendMode(){
	blendModeProp.pop();
	activeBlend = blendFromMode(blendModeProp.current);
}

void GPURender::setBlendOverride(BlendOverride mode){
	switch (mode){
	case BlendOverride::ToneAdd :
		activeBlend = GpuBlend::ToneAdd;
		break;

	case BlendOverride::ToneSubtract :
		activeBlend = GpuBlend::ToneSubtract;
		break;

	case BlendOverride::Overlay :
		activeBlend = GpuBlend::KeepDestAlpha;
		break;
	}
}

void GPURender::refreshBlendMode(){
	activeBlend = blendFromMode(blendModeProp.current);
}

void GPURender::pushScissorTest(bool enabled){
	scissorTest.pushSet(enabled);
}

void GPURender::popScissorTest(){
	scissorTest.pop();
}

void GPURender::pushScissorBox(const IntRect &rect){
	scissorBoxProp.pushSet(rect);
}

void GPURender::saveScissorBox(){
	scissorBoxProp.push();
}

void GPURender::intersectScissorBox(const IntRect &rect){
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

void GPURender::popScissorBox(){
	scissorBoxProp.pop();
}

void GPURender::setScissorBox(const IntRect &rect){
	scissorBoxProp.set(rect);
}

const IntRect &GPURender::scissorBox(){
	return scissorBoxProp.current;
}

void GPURender::pushClearColor(const Vec4 &color){
	clearColor.pushSet(color);
}

void GPURender::popClearColor(){
	clearColor.pop();
}

GeometryHandle GPURender::createGeometry(VertexLayout layout){
	std::unique_ptr<GpuGeometry> geom(new GpuGeometry);
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

void GPURender::destroyGeometry(GeometryHandle geom){
	geometries[geom.id-1].reset();
	freeGeometries.push_back(geom.id);
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
	GpuGeometry &g = *geometries[geom.id-1];
	GpuVertex zero;
	memset(&zero, 0, sizeof(zero));
	zero.a = 1;
	g.vertices.assign(bytes / layoutStride(g.layout), zero);
}

void GPURender::uploadGeometry(GeometryHandle geom, size_t bytes, const void *data, GeometryUsage){
	GpuGeometry &g = *geometries[geom.id-1];
	const size_t count = bytes / layoutStride(g.layout);
	g.vertices.resize(count);
	convertVertices(g.layout, static_cast<const uint8_t*>(data), count, g.vertices.data());
}

void GPURender::uploadGeometryRange(GeometryHandle geom, size_t offset, size_t bytes, const void *data){
	GpuGeometry &g = *geometries[geom.id-1];
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
	const GpuGeometry &g = *geometries[geom.id-1];
	const int effect = currentSlot;
	const GpuUniforms &u = uniforms();

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
		commands.push_back(c);

		firstQuad += chunk;
		quadCount -= chunk;
	}
}

void GPURender::useEffect(ShaderType effect){
	currentSlot = effect;
}

void GPURender::useBlurPass(int pass){
	currentSlot = SHADER_blur;
	blurPass = pass;
}

void GPURender::applyViewportProj(){
	orthoMatrix(uniforms().proj, viewport.current.w, viewport.current.h);
}

void GPURender::applyPerspectiveProj(){
	perspectiveMatrix(uniforms().proj, viewport.current.w, viewport.current.h, shState->graphics().globalFov);
}

void GPURender::setTexSize(const Vec2i &size){
	uniforms().texSize = size;
}

void GPURender::setTranslation(const Vec2i &value){
	uniforms().translation = value;
}

void GPURender::setTime(float value){
	uniforms().time = value;
}

void GPURender::setEffectTexture(EffectTexture slot, TexHandle tex){
	GpuUniforms &u = uniforms();

	switch (slot){
	case EffectTexture::Noise :
	case EffectTexture::Obscured :
	case EffectTexture::Destination :
	case EffectTexture::Current :
		u.aux[1] = tex.id;
		break;

	case EffectTexture::Frozen :
		u.aux[2] = tex.id;
		break;

	case EffectTexture::TransMap :
		u.aux[3] = tex.id;
		break;

	case EffectTexture::WallMap :
		break;
	}
}

void GPURender::setSpriteMat(const float value[16]){
	memcpy(uniforms().spriteMat, value, sizeof(float) * 16);
}

void GPURender::setMatrix(const float value[16]){
	memcpy(uniforms().matrix, value, sizeof(float) * 16);
}

void GPURender::setTone(const Vec4 &value){
	uniforms().tone = value;
}

void GPURender::setColor(const Vec4 &value){
	uniforms().color = value;
}

void GPURender::setFlash(const Vec4 &value){
	uniforms().flash = value;
}

void GPURender::setModulate(const Vec4 &value){
	uniforms().modulate = value;
}

void GPURender::setOpacity(float value){
	uniforms().opacity = value;
}

void GPURender::setBushDepth(float value){
	uniforms().bushDepth = value;
}

void GPURender::setBushOpacity(float value){
	uniforms().bushOpacity = value;
}

void GPURender::setGray(float value){
	uniforms().gray = value;
}

void GPURender::setHueAdjust(float value){
	uniforms().hueAdjust = value;
}

void GPURender::setAniIndex(int value){
	uniforms().aniIndex = value;
}

void GPURender::setOffset(const Vec2i &value){
	uniforms().offset = value;
}

void GPURender::setSubRect(const FloatRect &value){
	uniforms().subRect = Vec4(value.x, value.y, value.w, value.h);
}

void GPURender::setProg(float value){
	uniforms().prog = value;
}

void GPURender::setVague(float value){
	uniforms().vague = value;
}

void GPURender::setWallMapResolution(int, int){
}

void GPURender::setCameraPosition(int x, int y){
	cameraPos = Vec2i(x, y);
}

void GPURender::setTileMapOffset(int, int){
}

void GPURender::setLightSources(const std::vector<LightSource> &sources){
	lights.clear();

	for (size_t i = 0; i < sources.size() && lights.size() < 64; ++i){
		LightSource source = sources[i];
		if (!source.hasEffect())
			continue;

		lights.push_back(source);
	}
}

void GPURender::setAmbient(float value){
	ambient = value;
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
	if (smooth)
		srcTex->smooth = true;

	recordQuad(verts, SHADER_simple, GpuBlend::None, viewport.current, scissorTest.current, scissorBoxProp.current, targetId);

	srcTex->smooth = wasSmooth && !smooth ? wasSmooth : false;
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

	SDL_GPUBufferCreateInfo info;
	memset(&info, 0, sizeof(info));
	info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
	info.size = (Uint32) (indices.size() * sizeof(uint16_t));
	indexBuffer = SDL_CreateGPUBuffer(device, &info);
	indexQuads = count;

	SDL_GPUTransferBufferCreateInfo binfo;
	memset(&binfo, 0, sizeof(binfo));
	binfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	binfo.size = info.size;
	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device, &binfo);
	memcpy(SDL_MapGPUTransferBuffer(device, transfer, false), indices.data(), info.size);
	SDL_UnmapGPUTransferBuffer(device, transfer);

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTransferBufferLocation src = { transfer, 0 };
	SDL_GPUBufferRegion dst = { indexBuffer, 0, info.size };
	SDL_UploadToGPUBuffer(copy, &src, &dst, false);
	SDL_EndGPUCopyPass(copy);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, transfer);
}

void GPURender::flush(){
	if (commands.empty()){
		releasePending();
		return;
	}

	ensureIndexBuffer(neededQuads);

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);

	if (!vertexStream.empty()){
		const size_t bytes = vertexStream.size() * sizeof(GpuVertex);
		ensureVertexCapacity(bytes);

		SDL_GPUTransferBufferCreateInfo binfo;
		memset(&binfo, 0, sizeof(binfo));
		binfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		binfo.size = (Uint32) bytes;
		SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device, &binfo);
		memcpy(SDL_MapGPUTransferBuffer(device, transfer, false), vertexStream.data(), bytes);
		SDL_UnmapGPUTransferBuffer(device, transfer);

		SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(cmd);
		SDL_GPUTransferBufferLocation src = { transfer, 0 };
		SDL_GPUBufferRegion dst = { vertexBuffer, 0, (Uint32) bytes };
		SDL_UploadToGPUBuffer(copy, &src, &dst, true);
		SDL_EndGPUCopyPass(copy);
		SDL_ReleaseGPUTransferBuffer(device, transfer);
	}

	SDL_GPURenderPass *pass = 0;
	uint32_t passTarget = 0;
	SDL_GPUGraphicsPipeline *lastPipeline = 0;
	SDL_GPUTexture *swapTexture = 0;
	Uint32 swapW = 0, swapH = 0;
	bool swapTried = false;
	const SDL_GPUTextureFormat swapFormat = SDL_GetGPUSwapchainTextureFormat(device, window);

	struct Resolved{
		SDL_GPUTexture *tex;
		int w, h;
		SDL_GPUTextureFormat format;
	};

	for (size_t i = 0; i < commands.size(); ++i){
		const GpuCommand &c = commands[i];

		if (c.type == GpuCommand::Upload){
			if (pass){
				SDL_EndGPURenderPass(pass);
				pass = 0;
			}

			GpuTexture *tex = texture(c.target);
			if (!tex || !tex->texture)
				continue;

			const std::vector<uint8_t> &data = uploads[c.payload];

			SDL_GPUTransferBufferCreateInfo binfo;
			memset(&binfo, 0, sizeof(binfo));
			binfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
			binfo.size = (Uint32) data.size();
			SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device, &binfo);
			memcpy(SDL_MapGPUTransferBuffer(device, transfer, false), data.data(), data.size());
			SDL_UnmapGPUTransferBuffer(device, transfer);

			SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(cmd);
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
			continue;
		}

		Resolved r;
		memset(&r, 0, sizeof(r));

		if (c.target == 0){
			if (!swapTried){
				swapTried = true;
				SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapTexture, &swapW, &swapH);
			}

			if (!swapTexture)
				continue;

			r.tex = swapTexture;
			r.w = (int) swapW;
			r.h = (int) swapH;
			r.format = swapFormat;
		}else{
			GpuTexture *t = texture(c.target);
			if (!t || !t->texture)
				continue;

			r.tex = t->texture;
			r.w = t->width;
			r.h = t->height;
			r.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		}

		if (c.type == GpuCommand::Clear){
			if (pass)
				SDL_EndGPURenderPass(pass);

			SDL_GPUColorTargetInfo ci;
			memset(&ci, 0, sizeof(ci));
			ci.texture = r.tex;
			ci.load_op = SDL_GPU_LOADOP_CLEAR;
			ci.store_op = SDL_GPU_STOREOP_STORE;
			ci.clear_color.r = c.clearColor[0];
			ci.clear_color.g = c.clearColor[1];
			ci.clear_color.b = c.clearColor[2];
			ci.clear_color.a = c.clearColor[3];
			ci.cycle = false;
			pass = SDL_BeginGPURenderPass(cmd, &ci, 1, 0);
			passTarget = c.target;
			lastPipeline = 0;
			continue;
		}

		if (!pass || passTarget != c.target){
			if (pass)
				SDL_EndGPURenderPass(pass);

			SDL_GPUColorTargetInfo ci;
			memset(&ci, 0, sizeof(ci));
			ci.texture = r.tex;
			ci.load_op = SDL_GPU_LOADOP_LOAD;
			ci.store_op = SDL_GPU_STOREOP_STORE;
			pass = SDL_BeginGPURenderPass(cmd, &ci, 1, 0);
			passTarget = c.target;
			lastPipeline = 0;
		}

		if (c.viewport.w <= 0 || c.viewport.h <= 0)
			continue;

		SDL_GPUGraphicsPipeline *pipeline = pipelineFor(c.pipeline, c.blend, r.format);
		if (!pipeline)
			continue;

		if (pipeline != lastPipeline){
			SDL_BindGPUGraphicsPipeline(pass, pipeline);
			lastPipeline = pipeline;

			SDL_GPUBufferBinding vb = { vertexBuffer, 0 };
			SDL_BindGPUVertexBuffers(pass, 0, &vb, 1);
			SDL_GPUBufferBinding ib = { indexBuffer, 0 };
			SDL_BindGPUIndexBuffer(pass, &ib, SDL_GPU_INDEXELEMENTSIZE_16BIT);
		}

		SDL_PushGPUVertexUniformData(cmd, 0, &c.vertexParams, sizeof(c.vertexParams));

		if (c.pipeline == PipelineLight){
			SDL_PushGPUFragmentUniformData(cmd, 0, &lightPool[c.payload], sizeof(GpuLightParams));
		}else{
			SDL_PushGPUFragmentUniformData(cmd, 0, &c.fragParams, sizeof(c.fragParams));

			SDL_GPUTextureSamplerBinding bindings[4];
			for (int s = 0; s < 4; ++s){
				GpuTexture *t = texture(c.tex[s]);
				bindings[s].texture = (t && t->texture) ? t->texture : dummy;
				bindings[s].sampler = samplerFor(c.sampler[s]);
			}

			SDL_BindGPUFragmentSamplers(pass, 0, bindings, 4);
		}

		SDL_GPUViewport vp = { (float) c.viewport.x, (float) c.viewport.y, (float) c.viewport.w, (float) c.viewport.h, 0.0f, 1.0f };
		SDL_SetGPUViewport(pass, &vp);

		SDL_Rect scissor = { 0, 0, r.w, r.h };
		if (c.scissorOn){
			scissor.x = std::max(c.scissor.x, 0);
			scissor.y = std::max(c.scissor.y, 0);
			scissor.w = std::max(std::min(c.scissor.x + c.scissor.w, r.w) - scissor.x, 0);
			scissor.h = std::max(std::min(c.scissor.y + c.scissor.h, r.h) - scissor.y, 0);
		}
		SDL_SetGPUScissor(pass, &scissor);

		SDL_DrawGPUIndexedPrimitives(pass, c.quadCount * 6, 1, 0, (Sint32) c.firstVertex, 0);
	}

	if (pass)
		SDL_EndGPURenderPass(pass);

	SDL_SubmitGPUCommandBuffer(cmd);

	commands.clear();
	vertexStream.clear();
	uploads.clear();
	lightPool.clear();
	neededQuads = 1;
	releasePending();
}

void GPURender::swapWindow(SDL_Window *){
	flush();
}

void GPURender::suspendContext(SDL_Window *){
}

void GPURender::resumeContext(SDL_Window *){
}

class GPURenderContext : public IRenderContext{
public:
	GPURenderContext(SDL_Window *window)
	    : window(window)
	{
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

	bool claimed = SDL_ClaimWindowForGPUDevice(probe, window);
	if (claimed)
		SDL_ReleaseWindowFromGPUDevice(probe, window);

	SDL_DestroyGPUDevice(probe);
	return claimed;
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
