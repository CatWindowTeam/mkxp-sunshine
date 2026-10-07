#include "statefulrender.h"
#include "config.h"
#include "sharedstate.h"
#include "graphics.h"
#include <algorithm>
#include <math.h>
#include <string.h>

EffectUniforms::EffectUniforms()
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

void multiplyMatrices(const float a[16], const float b[16], float out[16]){
	float res[16];

	for (int col = 0; col < 4; ++col)
		for (int row = 0; row < 4; ++row){
			float sum = 0;
			for (int k = 0; k < 4; ++k)
				sum += a[k * 4 + row] * b[col * 4 + k];
			res[col * 4 + row] = sum;
		}

	memcpy(out, res, sizeof(res));
}

void orthoMatrix(float out[16], int w, int h){
	const float a = 2.f / w;
	const float b = 2.f / h;
	const float c = -2.f;

	const float mat[16] = {
		 a,  0,  0,  0,
		 0,  b,  0,  0,
		 0,  0,  c,  0,
		-1, -1, -1,  1
	};

	memcpy(out, mat, sizeof(mat));
}

void perspectiveMatrix(float out[16], int w, int h, float fovDegrees){
	const float width  = (float) w;
	const float height = (float) h;

	const float camDist = (height * 0.5f) / tanf(fovDegrees * PI / 360.0f);

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

	memcpy(out, mat, sizeof(mat));
}

static BlendKind blendFromMode(BlendType mode){
	switch (mode){
	case BlendKeepDestAlpha : return BlendKind::KeepDestAlpha;
	case BlendAddition      : return BlendKind::Addition;
	case BlendSubstraction  : return BlendKind::Substraction;
	case BlendMultiply      : return BlendKind::Multiply;
	default                 : return BlendKind::Normal;
	}
}

StatefulRender::StatefulRender(const Config &conf)
    : activeBlend(BlendKind::Normal),
      boundTexture(0),
      uniformSets(SHADER_COUNT),
      currentSlot(SHADER_simple),
      blurPass(0),
      ambient(0)
{
	viewport.current = IntRect(0, 0, conf.defScreenW, conf.defScreenH);
	blend.current = true;
	blendModeProp.current = BlendNormal;
	scissorTest.current = false;
	scissorBoxProp.current = IntRect(0, 0, conf.defScreenW, conf.defScreenH);
	clearColor.current = Vec4(0, 0, 0, 1);
}

EffectUniforms &StatefulRender::uniforms(){
	return uniformSets[currentSlot];
}

void StatefulRender::resetBlend(){
	activeBlend = blendFromMode(blendModeProp.current);
}

void StatefulRender::bindTexture(TexHandle tex){
	boundTexture = tex.id;
}

void StatefulRender::unbindTexture(){
	boundTexture = 0;
}

void StatefulRender::setViewport(const IntRect &rect){
	viewport.set(rect);
}

void StatefulRender::pushViewport(const IntRect &rect){
	viewport.pushSet(rect);
}

void StatefulRender::popViewport(){
	viewport.pop();
}

void StatefulRender::refreshViewport(){
}

void StatefulRender::pushBlend(bool enabled){
	blend.pushSet(enabled);
}

void StatefulRender::popBlend(){
	blend.pop();
}

void StatefulRender::pushBlendMode(BlendType mode){
	blendModeProp.pushSet(mode);
	resetBlend();
}

void StatefulRender::popBlendMode(){
	blendModeProp.pop();
	resetBlend();
}

void StatefulRender::setBlendOverride(BlendOverride mode){
	switch (mode){
	case BlendOverride::ToneAdd :
		activeBlend = BlendKind::ToneAdd;
		break;

	case BlendOverride::ToneSubtract :
		activeBlend = BlendKind::ToneSubtract;
		break;

	case BlendOverride::Overlay :
		activeBlend = BlendKind::KeepDestAlpha;
		break;
	}
}

void StatefulRender::refreshBlendMode(){
	resetBlend();
}

void StatefulRender::pushScissorTest(bool enabled){
	scissorTest.pushSet(enabled);
}

void StatefulRender::popScissorTest(){
	scissorTest.pop();
}

void StatefulRender::pushScissorBox(const IntRect &rect){
	scissorBoxProp.pushSet(rect);
}

void StatefulRender::saveScissorBox(){
	scissorBoxProp.push();
}

void StatefulRender::intersectScissorBox(const IntRect &rect){
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

void StatefulRender::popScissorBox(){
	scissorBoxProp.pop();
}

void StatefulRender::setScissorBox(const IntRect &rect){
	scissorBoxProp.set(rect);
}

const IntRect &StatefulRender::scissorBox(){
	return scissorBoxProp.current;
}

void StatefulRender::pushClearColor(const Vec4 &color){
	clearColor.pushSet(color);
}

void StatefulRender::popClearColor(){
	clearColor.pop();
}

void StatefulRender::useEffect(ShaderType effect){
	currentSlot = effect;
}

void StatefulRender::useBlurPass(int pass){
	currentSlot = SHADER_blur;
	blurPass = pass;
}

void StatefulRender::applyViewportProj(){
	orthoMatrix(uniforms().proj, viewport.current.w, viewport.current.h);
}

void StatefulRender::applyPerspectiveProj(){
	perspectiveMatrix(uniforms().proj, viewport.current.w, viewport.current.h, shState->graphics().globalFov);
}

void StatefulRender::setTexSize(const Vec2i &size){
	uniforms().texSize = size;
}

void StatefulRender::setTranslation(const Vec2i &value){
	uniforms().translation = value;
}

void StatefulRender::setTime(float value){
	uniforms().time = value;
}

void StatefulRender::setEffectTexture(EffectTexture slot, TexHandle tex){
	EffectUniforms &u = uniforms();

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

void StatefulRender::setSpriteMat(const float value[16]){
	memcpy(uniforms().spriteMat, value, sizeof(float) * 16);
}

void StatefulRender::setMatrix(const float value[16]){
	memcpy(uniforms().matrix, value, sizeof(float) * 16);
}

void StatefulRender::setTone(const Vec4 &value){
	uniforms().tone = value;
}

void StatefulRender::setColor(const Vec4 &value){
	uniforms().color = value;
}

void StatefulRender::setFlash(const Vec4 &value){
	uniforms().flash = value;
}

void StatefulRender::setModulate(const Vec4 &value){
	uniforms().modulate = value;
}

void StatefulRender::setOpacity(float value){
	uniforms().opacity = value;
}

void StatefulRender::setBushDepth(float value){
	uniforms().bushDepth = value;
}

void StatefulRender::setBushOpacity(float value){
	uniforms().bushOpacity = value;
}

void StatefulRender::setGray(float value){
	uniforms().gray = value;
}

void StatefulRender::setHueAdjust(float value){
	uniforms().hueAdjust = value;
}

void StatefulRender::setAniIndex(int value){
	uniforms().aniIndex = value;
}

void StatefulRender::setOffset(const Vec2i &value){
	uniforms().offset = value;
}

void StatefulRender::setSubRect(const FloatRect &value){
	uniforms().subRect = Vec4(value.x, value.y, value.w, value.h);
}

void StatefulRender::setProg(float value){
	uniforms().prog = value;
}

void StatefulRender::setVague(float value){
	uniforms().vague = value;
}

void StatefulRender::setWallMapResolution(int, int){
}

void StatefulRender::setCameraPosition(int x, int y){
	cameraPos = Vec2i(x, y);
}

void StatefulRender::setTileMapOffset(int, int){
}

void StatefulRender::setLightSources(const std::vector<LightSource> &sources){
	lights.clear();

	for (size_t i = 0; i < sources.size() && lights.size() < 64; ++i){
		LightSource source = sources[i];
		if (source.hasEffect())
			lights.push_back(source);
	}
}

void StatefulRender::setAmbient(float value){
	ambient = value;
}
