/*
** shader.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is SDL_free software: you can redistribute it and/or modify
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

#include "shader.h"
#include "sharedstate.h"
#include "glstate.h"
#include "exception.h"
#include "debugwriter.h"

#include <assert.h>
#include <string.h>

#include "common.h.xxd"
#include "sprite.frag.xxd"
#include "worldMachine.frag.xxd"
#include "water.frag.xxd"
#include "crt.frag.xxd"
#include "tilemapWater.frag.xxd"
#include "hue.frag.xxd"
#include "trans.frag.xxd"
#include "transSimple.frag.xxd"
#include "bitmapBlit.frag.xxd"
#include "plane.frag.xxd"
#include "gray.frag.xxd"
#include "flatColor.frag.xxd"
#include "simple.frag.xxd"
#include "simpleColor.frag.xxd"
#include "simpleAlpha.frag.xxd"
#include "simpleAlphaUni.frag.xxd"
#include "flashMap.frag.xxd"
#include "minimal.vert.xxd"
#include "simple.vert.xxd"
#include "simpleColor.vert.xxd"
#include "sprite.vert.xxd"
#include "tilemap.vert.xxd"
#include "blur.frag.xxd"
#include "simpleMatrix.vert.xxd"
#include "blurH.vert.xxd"
#include "blurV.vert.xxd"
#include "obscured.frag.xxd"
#include "dynamicLight.frag.xxd"

#include "meow.h"
#include "sunshine.h"

#define INIT_SHADER(vert, frag, name){ \
	Shader::init(shader_##vert##_vert, shader_##vert##_vert_len, shader_##frag##_frag, shader_##frag##_frag_len, \
	#vert, #frag, #name); \
}

#define GET_U(name) u_##name = gl.GetUniformLocation(program, #name)

static void printShaderLog(GLuint shader){
	GLint logLength;
	gl.GetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

	std::string SDL_log(logLength, '\0');
	gl.GetShaderInfoLog(shader, SDL_log.size(), 0, &SDL_log[0]);

	Debug() << "Shader SDL_log:\n" << SDL_log;
}

static void printProgramLog(GLuint program){
	GLint logLength;
	gl.GetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

	std::string SDL_log(logLength, '\0');
	gl.GetProgramInfoLog(program, SDL_log.size(), 0, &SDL_log[0]);

	Debug() << "Program SDL_log:\n" << SDL_log;
}

Shader::Shader(){
	vertShader = gl.CreateShader(GL_VERTEX_SHADER);
	fragShader = gl.CreateShader(GL_FRAGMENT_SHADER);

	program = gl.CreateProgram();
}

Shader::~Shader(){
	gl.UseProgram(0);
	gl.DeleteProgram(program);
	gl.DeleteShader(vertShader);
	gl.DeleteShader(fragShader);
}

void Shader::bind(){
	glState.program.set(program);
}

void Shader::unbind(){
	gl.ActiveTexture(GL_TEXTURE0);
	glState.program.set(0);
}

static void setupShaderSource(GLuint shader, GLenum type, const unsigned char *body, int bodySize){
	static const char glesDefine[] = "#define GLSLES\n";
	static const char fragDefine[] = "#define FRAGMENT_SHADER\n";

	const GLchar *shaderSrc[4];
	GLint shaderSrcSize[4];
	size_t i = 0;

	if (gl.glsles){
		shaderSrc[i] = glesDefine;
		shaderSrcSize[i] = sizeof(glesDefine)-1;
		++i;
	}

	if (type == GL_FRAGMENT_SHADER){
		shaderSrc[i] = fragDefine;
		shaderSrcSize[i] = sizeof(fragDefine)-1;
		++i;
	}

	shaderSrc[i] = (const GLchar*) shader_common_h;
	shaderSrcSize[i] = shader_common_h_len;
	++i;

	shaderSrc[i] = (const GLchar*) body;
	shaderSrcSize[i] = bodySize;
	++i;

	gl.ShaderSource(shader, i, shaderSrc, shaderSrcSize);
}

void Shader::init(const unsigned char *vert, int vertSize, const unsigned char *frag, int fragSize, const char *vertName, const char *fragName, const char *programName) {
	GLint success;
	
	/* Compile vertex shader */
	setupShaderSource(vertShader, GL_VERTEX_SHADER, vert, vertSize);
	gl.CompileShader(vertShader);

	gl.GetShaderiv(vertShader, GL_COMPILE_STATUS, &success);

	if (!success){
		printShaderLog(vertShader);
		crash(Exception::MKXPError, "GLSL: An error occured while compiling vertex shader '%s' in program '%s'", vertName, programName);
	}

	/* Compile fragment shader */
	setupShaderSource(fragShader, GL_FRAGMENT_SHADER, frag, fragSize);
	gl.CompileShader(fragShader);

	gl.GetShaderiv(fragShader, GL_COMPILE_STATUS, &success);

	if (!success){
		printShaderLog(fragShader);
		crash(Exception::MKXPError, "GLSL: An error occured while compiling fragment shader '%s' in program '%s'", fragName, programName);
	}

	/* Link shader program */
	gl.AttachShader(program, vertShader);
	gl.AttachShader(program, fragShader);

	gl.BindAttribLocation(program, Position, "position");
	gl.BindAttribLocation(program, TexCoord, "texCoord");
	gl.BindAttribLocation(program, Color, "color");

	gl.LinkProgram(program);

	gl.GetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success){
		printProgramLog(program);
		crash(Exception::MKXPError, "GLSL: An error occured while linking program '%s' (vertex '%s', fragment '%s')", programName, vertName, fragName);
	}
}

void Shader::initFromFile(const char *_vertFile, const char *_fragFile, const char *programName){
	std::string vertContents, fragContents;
	readFile(_vertFile, vertContents);
	readFile(_fragFile, fragContents);

	init((const unsigned char*) vertContents.c_str(), vertContents.size(),
	     (const unsigned char*) fragContents.c_str(), fragContents.size(),
	     _vertFile, _fragFile, programName);
}

void Shader::setVec4Uniform(GLint location, const Vec4 &vec) {
	gl.Uniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::setTexUniform(GLint location, unsigned unitIndex, TEX::ID texture) {
	GLenum texUnit = GL_TEXTURE0 + unitIndex;
	
	gl.ActiveTexture(texUnit);
	gl.BindTexture(GL_TEXTURE_2D, texture.gl);
	gl.Uniform1i(location, unitIndex);
	gl.ActiveTexture(GL_TEXTURE0);
}

void ShaderBase::GLProjMat::apply(const Vec2i &value) {
	/* glOrtho replacement */
	const float a = 2.f / value.x;
	const float b = 2.f / value.y;
	const float c = -2.f;

	GLfloat mat[16] = {
		 a,  0,  0,  0,
		 0,  b,  0,  0,
		 0,  0,  c,  0,
		-1, -1, -1,  1
	};

	gl.UniformMatrix4fv(u_mat, 1, GL_FALSE, mat);
}

void ShaderBase::init(){
	GET_U(texSizeInv);
	GET_U(translation);
	GET_U(uTime);

	projMat.u_mat = gl.GetUniformLocation(program, "projMat");
}

void ShaderBase::applyViewportProj(){
	const IntRect &vp = glState.viewport.get();
	projMat.set(Vec2i(vp.w, vp.h));
}

void ShaderBase::setTexSize(const Vec2i &value){
	gl.Uniform2f(u_texSizeInv, 1.f / value.x, 1.f / value.y);
}

void ShaderBase::setTranslation(const Vec2i &value){
	gl.Uniform2f(u_translation, value.x, value.y);
}

void ShaderBase::setTime(float value){
	gl.Uniform1f(u_uTime, value);
}


FlatColorShader::FlatColorShader(){
	INIT_SHADER(minimal, flatColor, FlatColorShader);

	ShaderBase::init();

	GET_U(color);
}

void FlatColorShader::setColor(const Vec4 &value){
	setVec4Uniform(u_color, value);
}


SimpleShader::SimpleShader(){
	INIT_SHADER(simple, simple, SimpleShader);

	ShaderBase::init();

	GET_U(texOffsetX);
}

void SimpleShader::setTexOffsetX(int value){
	gl.Uniform1f(u_texOffsetX, value);
}


DynamicLightShader::DynamicLightShader(){
	INIT_SHADER(simple, dynamicLight, DynamicLightShader);

	ShaderBase::init();
	
	GET_U(wallMapTexture);
	GET_U(wallMapResolution);
	GET_U(cameraPosition);
	GET_U(tileMapOffset);
	GET_U(lightSourcesCount);
	GET_U(ambientLight);
	
	u_lightSources = gl.GetUniformLocation(program, "lightSources[0]");
	u_lightSourcesColors = gl.GetUniformLocation(program, "lightSourcesColors[0]");
}

void DynamicLightShader::setWallMapTexture(TEX::ID texture){
	setTexUniform(u_wallMapTexture, 1, texture);
}

void DynamicLightShader::setWallMapResolution(int x, int y){
	gl.Uniform2f(u_wallMapResolution, x, y);
}

void DynamicLightShader::setCameraPosition(int x, int y){
	gl.Uniform2f(u_cameraPosition, x, y);
}

void DynamicLightShader::setTileMapOffset(int x, int y){
	gl.Uniform2f(u_tileMapOffset, x, y);
}

void DynamicLightShader::setLightSources(std::vector<LightSource> sources){
	int count = 0;
	for(int i = 0; i < sources.size(); i++) {
		if (count < 64) {
			if (!sources[i].hasEffect())
				continue;
			gl.Uniform4f(u_lightSources + count,
				sources[i].x,
				sources[i].y,
				sources[i].power,
				sources[i].radius
			);
			gl.Uniform4f(u_lightSourcesColors + count,
				sources[i].color.red / 255.0,
				sources[i].color.green / 255.0,
				sources[i].color.blue / 255.0,
				sources[i].color.alpha / 255.0
			);
			count++;
		}
		else {
			Debug() << "The limit of light sources has been reached! (" << sources.size() << "/ 64, clamped" << sources.size() - count << ")";
			break;
		}
	}
	gl.Uniform1i(u_lightSourcesCount, count);
}

void DynamicLightShader::setAmbient(float power){
	gl.Uniform1f(u_ambientLight, power);
}


SimpleColorShader::SimpleColorShader(){
	INIT_SHADER(simpleColor, simpleColor, SimpleColorShader);

	ShaderBase::init();
}


SimpleAlphaShader::SimpleAlphaShader(){
	INIT_SHADER(simpleColor, simpleAlpha, SimpleAlphaShader);

	ShaderBase::init();
}


SimpleSpriteShader::SimpleSpriteShader(){
	INIT_SHADER(sprite, simple, SimpleSpriteShader);

	ShaderBase::init();

	GET_U(spriteMat);
}

void SimpleSpriteShader::setSpriteMat(const float value[16]){
	gl.UniformMatrix4fv(u_spriteMat, 1, GL_FALSE, value);
}


AlphaSpriteShader::AlphaSpriteShader(){
	INIT_SHADER(sprite, simpleAlphaUni, AlphaSpriteShader);

	ShaderBase::init();

	GET_U(spriteMat);
	GET_U(alpha);
}

void AlphaSpriteShader::setSpriteMat(const float value[16]){
	gl.UniformMatrix4fv(u_spriteMat, 1, GL_FALSE, value);
}

void AlphaSpriteShader::setAlpha(float value){
	gl.Uniform1f(u_alpha, value);
}


TransShader::TransShader(){
	INIT_SHADER(simple, trans, TransShader);

	ShaderBase::init();

	GET_U(currentScene);
	GET_U(frozenScene);
	GET_U(transMap);
	GET_U(prog);
	GET_U(vague);
}

void TransShader::setCurrentScene(TEX::ID tex){
	setTexUniform(u_currentScene, 1, tex);
}

void TransShader::setFrozenScene(TEX::ID tex){
	setTexUniform(u_frozenScene, 2, tex);
}

void TransShader::setTransMap(TEX::ID tex){
	setTexUniform(u_transMap, 3, tex);
}

void TransShader::setProg(float value){
	gl.Uniform1f(u_prog, value);
}

void TransShader::setVague(float value){
	gl.Uniform1f(u_vague, value);
}


SimpleTransShader::SimpleTransShader(){
	INIT_SHADER(simple, transSimple, SimpleTransShader);

	ShaderBase::init();

	GET_U(currentScene);
	GET_U(frozenScene);
	GET_U(prog);
}

void SimpleTransShader::setCurrentScene(TEX::ID tex){
	setTexUniform(u_currentScene, 1, tex);
}

void SimpleTransShader::setFrozenScene(TEX::ID tex){
	setTexUniform(u_frozenScene, 2, tex);
}

void SimpleTransShader::setProg(float value){
	gl.Uniform1f(u_prog, value);
}


void SpriteShaderBase::SpriteShaderInit(){
	ShaderBase::init();

	GET_U(spriteMat);
	GET_U(tone);
	GET_U(color);
	GET_U(modulate);
	GET_U(opacity);
	GET_U(bushDepth);
	GET_U(bushOpacity);
}

void SpriteShaderBase::setSpriteMat(const float value[16]){
	gl.UniformMatrix4fv(u_spriteMat, 1, GL_FALSE, value);
}

void SpriteShaderBase::setTone(const Vec4 &tone){
	setVec4Uniform(u_tone, tone);
}

void SpriteShaderBase::setColor(const Vec4 &color){
	setVec4Uniform(u_color, color);
}

void SpriteShaderBase::setModulate(const Vec4 &color){
	setVec4Uniform(u_modulate, color);
}

void SpriteShaderBase::setOpacity(float value){
	gl.Uniform1f(u_opacity, value);
}

void SpriteShaderBase::setBushDepth(float value){
	gl.Uniform1f(u_bushDepth, value);
}

void SpriteShaderBase::setBushOpacity(float value){
	gl.Uniform1f(u_bushOpacity, value);
}

SpriteShader::SpriteShader(){
	INIT_SHADER(sprite, sprite, SpriteShader);
	SpriteShaderBase::SpriteShaderInit();
}

WMShader::WMShader(){
	INIT_SHADER(sprite, worldMachine, WMShader);
	SpriteShaderBase::SpriteShaderInit();
}

WaterShader::WaterShader(){
	INIT_SHADER(simple, water, WaterShader);
	SpriteShaderBase::SpriteShaderInit();
	
	GET_U(noiseTexture);
}

void WaterShader::setNoiseTexture(TEX::ID texture){
	setTexUniform(u_noiseTexture, 1, texture);
}

CRTShader::CRTShader(){
	INIT_SHADER(sprite, crt, CRTShader);
	SpriteShaderBase::SpriteShaderInit();
}

PlaneShader::PlaneShader(){
	INIT_SHADER(simple, plane, PlaneShader);

	ShaderBase::init();

	GET_U(tone);
	GET_U(color);
	GET_U(flash);
	GET_U(opacity);
}

void PlaneShader::setTone(const Vec4 &tone){
	setVec4Uniform(u_tone, tone);
}

void PlaneShader::setColor(const Vec4 &color){
	setVec4Uniform(u_color, color);
}

void PlaneShader::setFlash(const Vec4 &flash){
	setVec4Uniform(u_flash, flash);
}

void PlaneShader::setOpacity(float value){
	gl.Uniform1f(u_opacity, value);
}

GrayShader::GrayShader(){
	INIT_SHADER(simple, gray, GrayShader);

	ShaderBase::init();

	GET_U(gray);
}

void GrayShader::setGray(float value){
	gl.Uniform1f(u_gray, value);
}

TilemapShader::TilemapShader(){
	INIT_SHADER(tilemap, simple, TilemapShader);

	ShaderBase::init();

	GET_U(aniIndex);
}

void TilemapShader::setAniIndex(int value){
	gl.Uniform1f(u_aniIndex, value);
}

TilemapWaterShader::TilemapWaterShader(){
	INIT_SHADER(tilemap, tilemapWater, TilemapWaterShader);

	ShaderBase::init();

	GET_U(aniIndex);
	GET_U(offset);
	GET_U(noiseTexture);
}

void TilemapWaterShader::setAniIndex(int value){
	gl.Uniform1f(u_aniIndex, value);
}

void TilemapWaterShader::setOffset(const Vec2i &value){
	gl.Uniform2f(u_offset, value.x, value.y);
}

void TilemapWaterShader::setNoiseTexture(TEX::ID texture){
	setTexUniform(u_noiseTexture, 1, texture);
}

FlashMapShader::FlashMapShader(){
	INIT_SHADER(simpleColor, flashMap, FlashMapShader);

	ShaderBase::init();

	GET_U(alpha);
}

void FlashMapShader::setAlpha(float value){
	gl.Uniform1f(u_alpha, value);
}

HueShader::HueShader(){
	INIT_SHADER(simple, hue, HueShader);

	ShaderBase::init();

	GET_U(hueAdjust);
}

void HueShader::setHueAdjust(float value){
	gl.Uniform1f(u_hueAdjust, value);
}

SimpleMatrixShader::SimpleMatrixShader(){
	INIT_SHADER(simpleMatrix, simpleAlpha, SimpleMatrixShader);

	ShaderBase::init();

	GET_U(matrix);
}

void SimpleMatrixShader::setMatrix(const float value[16]){
	gl.UniformMatrix4fv(u_matrix, 1, GL_FALSE, value);
}

BlurShader::HPass::HPass(){
	INIT_SHADER(blurH, blur, BlurShader::HPass);

	ShaderBase::init();
}

BlurShader::VPass::VPass(){
	INIT_SHADER(blurV, blur, BlurShader::VPass);

	ShaderBase::init();
}

BltShader::BltShader(){
	INIT_SHADER(simple, bitmapBlit, BltShader);

	ShaderBase::init();

	GET_U(source);
	GET_U(destination);
	GET_U(subRect);
	GET_U(opacity);
}

void BltShader::setSource(){
	gl.Uniform1i(u_source, 0);
}

void BltShader::setDestination(const TEX::ID value){
	setTexUniform(u_destination, 1, value);
}

void BltShader::setSubRect(const FloatRect &value){
	gl.Uniform4f(u_subRect, value.x, value.y, value.w, value.h);
}

void BltShader::setOpacity(float value){
	gl.Uniform1f(u_opacity, value);
}

ObscuredShader::ObscuredShader(){
	INIT_SHADER(simple, obscured, ObscuredShader);

	ShaderBase::init();

	GET_U(obscured);
}

void ObscuredShader::setObscured(const TEX::ID value){
	setTexUniform(u_obscured, 1, value);
}
