#pragma once

//    Name            In ruby const name
#define SHADER_LIST(X) \
	X(flatColor,      Flat) \
	X(simple,         Simple) \
	X(simpleColor,    SimpleColor) \
	X(simpleAlpha,    SimpleAlpha) \
	X(simpleSprite,   SimpleSprite) \
	X(alphaSprite,    AlphaSprite) \
	X(sprite,         Sprite) \
	X(worldMachine,   WorldMachine) \
	X(water,          Water) \
	X(crt,            CRT) \
	X(tilemapWater,   TilemapWater) \
	X(plane,          Plane) \
	X(gray,           Gray) \
	X(tilemap,        TileMap) \
	X(flashMap,       Flash) \
	X(trans,          Trans) \
	X(simpleTrans,    SimpleTrans) \
	X(hue,            Hue) \
	X(blt,            Blt) \
	X(simpleMatrix,   SimpleMatrix) \
	X(blur,           Blur) \
	X(obscured,       Obscured) \
	X(dynamicLight,   DynamicLight)

enum ShaderType{
#define DECLARE_ENUM(name, rb) SHADER_##name,
	SHADER_LIST(DECLARE_ENUM)
#undef DECLARE_ENUM
	SHADER_COUNT
};

enum class EffectTexture{
	Noise,
	Obscured,
	Current,
	Frozen,
	TransMap,
	Destination,
	WallMap
};
