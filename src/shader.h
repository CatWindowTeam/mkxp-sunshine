/*
** shader.h
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

#ifndef SHADER_H
#define SHADER_H

#include "etc-internal.h"
#include "gl-util.h"
#include "glstate.h"

class Shader{
public:
	void bind();
	static void unbind();

	enum Attribute{
		Position = 0,
		TexCoord = 1,
		Color = 2
	};

protected:
	Shader();
	~Shader();

	void init(const unsigned char *vert, int vertSize, const unsigned char *frag, int fragSize, const char *vertName, const char *fragName, const char *programName);
	void initFromFile(const char *vertFile, const char *fragFile, const char *programName);

	static void setVec4Uniform(GLint location, const Vec4 &vec);
	static void setTexUniform(GLint location, unsigned unitIndex, TEX::ID texture);

	GLuint vertShader, fragShader;
	GLuint program;
};

class ShaderBase : public Shader{
public:

	struct GLProjMat : public GLProperty<Vec2i>{
	private:
		void apply(const Vec2i &value);
		GLint u_mat;

		friend class ShaderBase;
	};

	/* Stack is not used (only 'set()') */
	GLProjMat projMat;

	/* Retrieves the current glState.viewport size,
	 * calculates the corresponding ortho projection matrix
	 * and loads it into the shaders uniform */
	void applyViewportProj();

	void setTexSize(const Vec2i &value);
	void setTranslation(const Vec2i &value);
	void setTime(float value);

protected:
	void init();

	GLint u_texSizeInv, u_translation;

private:
	GLint u_uTime;
};

class FlatColorShader : public ShaderBase{
public:
	FlatColorShader();

	void setColor(const Vec4 &value);

private:
	GLint u_color;
};

class SimpleShader : public ShaderBase{
public:
	SimpleShader();

	void setTexOffsetX(int value);

private:
	GLint u_texOffsetX;
};


class DynamicLightShader : public ShaderBase{
public:
	DynamicLightShader();

	void setWallMapTexture(TEX::ID texture);
	void setWallMapResolution(int x, int y);
	void setCameraPosition(int x, int y);
	void setTileMapOffset(int x, int y);
	void setLightSources(std::vector<LightSource> sources);
	void setAmbient(float power);

private:
	GLint u_wallMapTexture, u_wallMapResolution, u_cameraPosition, u_tileMapOffset, u_lightSources, u_lightSourcesCount, u_lightSourcesColors, u_ambientLight;
	Vec2i cameraPositionCache;
};

class SimpleColorShader : public ShaderBase{
public:
	SimpleColorShader();
};

class SimpleAlphaShader : public ShaderBase{
public:
	SimpleAlphaShader();
};

class SimpleSpriteShader : public ShaderBase{
public:
	SimpleSpriteShader();

	void setSpriteMat(const float value[16]);

private:
	GLint u_spriteMat;
};

class AlphaSpriteShader : public ShaderBase{
public:
	AlphaSpriteShader();

	void setSpriteMat(const float value[16]);
	void setAlpha(float value);

private:
	GLint u_spriteMat, u_alpha;
};

class TransShader : public ShaderBase{
public:
	TransShader();

	void setCurrentScene(TEX::ID tex);
	void setFrozenScene(TEX::ID tex);
	void setTransMap(TEX::ID tex);
	void setProg(float value);
	void setVague(float value);

private:
	GLint u_currentScene, u_frozenScene, u_transMap, u_prog, u_vague;
};

class SimpleTransShader : public ShaderBase{
public:
	SimpleTransShader();

	void setCurrentScene(TEX::ID tex);
	void setFrozenScene(TEX::ID tex);
	void setProg(float value);

private:
	GLint u_currentScene, u_frozenScene, u_prog;
};

class SpriteShaderBase : public ShaderBase{
public:
	void SpriteShaderInit();

	void setSpriteMat(const float value[16]);
	void setTone(const Vec4 &value);
	void setColor(const Vec4 &value);
	void setModulate(const Vec4 &value);
	void setOpacity(float value);
	void setBushDepth(float value);
	void setBushOpacity(float value);

private:
	GLint u_spriteMat, u_tone, u_opacity, u_color, u_modulate, u_bushDepth, u_bushOpacity;
};

class SpriteShader : public SpriteShaderBase{
public:
	SpriteShader();
};

class WMShader : public SpriteShaderBase{
public:
	WMShader();
};

class WaterShader : public SpriteShaderBase{
public:
	WaterShader();

	void setNoiseTexture(TEX::ID texture);

private:
	GLint u_noiseTexture;
};

class CRTShader : public SpriteShaderBase{
public:
	CRTShader();
};

class TilemapWaterShader : public ShaderBase{
public:
	TilemapWaterShader();

	void setAniIndex(int value);
	void setOffset(const Vec2i &value);
	void setNoiseTexture(TEX::ID texture);

private:
	GLint u_aniIndex, u_offset, u_noiseTexture;
};

class PlaneShader : public ShaderBase{
public:
	PlaneShader();

	void setTone(const Vec4 &value);
	void setColor(const Vec4 &value);
	void setFlash(const Vec4 &value);
	void setOpacity(float value);

private:
	GLint u_tone, u_color, u_flash, u_opacity;
};

class GrayShader : public ShaderBase{
public:
	GrayShader();

	void setGray(float value);

private:
	GLint u_gray;
};

class TilemapShader : public ShaderBase{
public:
	TilemapShader();

	void setAniIndex(int value);

private:
	GLint u_aniIndex;
};

class FlashMapShader : public ShaderBase{
public:
	FlashMapShader();

	void setAlpha(float value);

private:
	GLint u_alpha;
};

class HueShader : public ShaderBase{
public:
	HueShader();

	void setHueAdjust(float value);

private:
	GLint u_hueAdjust;
};

class SimpleMatrixShader : public ShaderBase{
public:
	SimpleMatrixShader();

	void setMatrix(const float value[16]);

private:
	GLint u_matrix;
};

/* Gaussian blur */
struct BlurShader{
	class HPass : public ShaderBase{
	public:
		HPass();
	};

	class VPass : public ShaderBase{
	public:
		VPass();
	};

	HPass pass1;
	VPass pass2;
};

class TilemapVXShader : public ShaderBase{
public:
	TilemapVXShader();

	void setAniOffset(const Vec2 &value);

private:
	GLint u_aniOffset;
};

/* Bitmap blit */
class BltShader : public ShaderBase{
public:
	BltShader();

	void setSource();
	void setDestination(const TEX::ID value);
	void setDestCoorF(const Vec2 &value);
	void setSubRect(const FloatRect &value);
	void setOpacity(float value);

private:
	GLint u_source, u_destination, u_subRect, u_opacity;
};

/* Obscured graphic */
class ObscuredShader : public ShaderBase{
public:
	ObscuredShader();

	void setObscured(const TEX::ID value);

private:
	GLint u_obscured;
};

/* Global object containing all available shaders */

//    Name            C++ Class           In ruby const name
#define SHADER_LIST(X) \
	X(flatColor,      FlatColorShader,    Flat) \
	X(simple,         SimpleShader,       Simple) \
	X(simpleColor,    SimpleColorShader,  SimpleColor) \
	X(simpleAlpha,    SimpleAlphaShader,  SimpleAlpha) \
	X(simpleSprite,   SimpleSpriteShader, SimpleSprite) \
	X(alphaSprite,    AlphaSpriteShader,  AlphaSprite) \
	X(sprite,         SpriteShader,       Sprite) \
	X(worldMachine,   WMShader,           WorldMachine) \
	X(water,	      WaterShader,        Water) \
	X(crt,	          CRTShader,          CRT) \
	X(tilemapWater,	  TilemapWaterShader, TilemapWater) \
	X(plane,          PlaneShader,        Plane) \
	X(gray,           GrayShader,         Gray) \
	X(tilemap,        TilemapShader,      TileMap) \
	X(flashMap,       FlashMapShader,     Flash) \
	X(trans,          TransShader,        Trans) \
	X(simpleTrans,    SimpleTransShader,  SimpleTrans) \
	X(hue,            HueShader,          Hue) \
	X(blt,            BltShader,          Blt) \
	X(simpleMatrix,   SimpleMatrixShader, SimpleMatrix) \
	X(blur,           BlurShader,         Blur) \
	X(obscured,       ObscuredShader,     Obscured) \
	X(dynamicLight,   DynamicLightShader, DynamicLight)

struct ShaderSet{
	#define DECLARE_SHADER(name, type, rb) type name;
		SHADER_LIST(DECLARE_SHADER)
	#undef DECLARE_SHADER
};
enum ShaderType{
#define DECLARE_ENUM(name, type, rb) SHADER_##name,
	SHADER_LIST(DECLARE_ENUM)
#undef DECLARE_ENUM
	SHADER_COUNT
};

#endif // SHADER_H
