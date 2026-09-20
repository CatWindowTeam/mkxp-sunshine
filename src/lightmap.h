#pragma once
#include "scene.h"
#include "flashable.h"
#include "disposable.h"
#include "viewport.h"
#include "util.h"
#include "shader.h"

class Bitmap;

struct LightMapPrivate;

class LightMap : public ViewportElement, public Flashable, public Disposable{
public:
	LightMap(Viewport *viewport = 0);
	~LightMap();

	void update();

	DECL_ATTR( Bitmap,         Bitmap* )
	DECL_ATTR( WallMap,        Bitmap* )
	DECL_ATTR( CameraX,        int )
	DECL_ATTR( CameraY,        int )
	DECL_ATTR( TilemapOffsetX, int )
	DECL_ATTR( TilemapOffsetY, int )
	DECL_ATTR( Ambient,        float )

	void clearStaticLightSources();
	void clearDynamicLightSources();
	void addStaticLightSource(const LightSource source);
	void addDynamicLightSource(const LightSource source);
	void removeStaticLightSource(float x, float y);
	void initDynAttribs();

private:
	LightMapPrivate *p;

	void draw();
	void onGeometryChange(const Scene::Geometry &);

	void releaseResources();
	const char *klassName() const { return "lightmap"; }

	ABOUT_TO_ACCESS_DISP
};
