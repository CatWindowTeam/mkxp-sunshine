#include "lightmap.h"

#include "sharedstate.h"
#include "bitmap.h"
#include "etc.h"
#include "etc-internal.h"
#include "util.h"
#include "signals/signal.h"

#include "gl-util.h"
#include "quad.h"
#include "shader.h"
#include "glstate.h"
#include "quadarray.h"
#include "config.h"
#include "debugwriter.h"
#include "sunshine.h"

#include <math.h>

#include <SDL3/SDL_rect.h>
#include <boost/chrono.hpp>

struct LightMapPrivate{

	Bitmap *bitmap;
	Bitmap *wallMap;

	int cameraX, cameraY, tilemapOffsetX, tilemapOffsetY;
	float ambient;
	std::vector<LightSource> staticLightSources;
	std::vector<LightSource> dynamicLightSources;
	std::vector<LightSource> gpuBuffer;

	SignalConnection bitmapUpdateConnection;

	Quad quad;

	Rect *srcRect;
	SignalConnection srcRectCon;

	IntRect sceneRect;
	Vec2i sceneOrig;

	/* Would this sprite be visible on
	 * the screen if drawn? */
	bool isVisible;

	EtcTemps tmp;

	SignalConnection prepareCon;

	LightMapPrivate()
	    : bitmap(new Bitmap(shState->graphics().width(), shState->graphics().height())),
		  wallMap(0),
		  cameraX(0),
		  cameraY(0),
		  tilemapOffsetX(0),
		  tilemapOffsetY(0),
		  ambient(255.0),
	      srcRect(&tmp.rect),
	      isVisible(false)

	{
		sceneRect.x = sceneRect.y = 0;

		updateSrcRectCon();

		prepareCon = shState->graphicsSignals.prepareDraw.Connect(*this, &LightMapPrivate::prepare);
		bitmapUpdateConnection = shState->graphicsSignals.resized.Connect([&](int w, int h){
			shState->rubyDispatcher().invoke([&, w, h]{
				bitmap = new Bitmap(w, h);
				bitmap->ensureNonMega();
				*srcRect = bitmap->rect();
				onSrcRectChange();
				quad.setPosRect(srcRect->toFloatRect());
			});
		});
		
		bitmap->ensureNonMega();
		//bitmap->fillRect(0, 0, shState->graphics().width(), shState->graphics().height(), Vec4(0, 0, 0, 1));

		staticLightSources.reserve(48);
		dynamicLightSources.reserve(16);
		gpuBuffer.reserve(64);
	}

	~LightMapPrivate(){
		srcRectCon.Disconnect();
		prepareCon.Disconnect();
		bitmapUpdateConnection.Disconnect();
	}

	void onSrcRectChange(){
		FloatRect rect = srcRect->toFloatRect();
		Vec2i bmSize;

		if (!nullOrDisposed(bitmap))
			bmSize = Vec2i(bitmap->width(), bitmap->height());

		/* Clamp the rectangle so it doesn't reach outside
		 * the bitmap bounds */
		rect.w = clamp<int>(rect.w, 0, bmSize.x-rect.x);
		rect.h = clamp<int>(rect.h, 0, bmSize.y-rect.y);

		quad.setTexRect(rect);

		quad.setPosRect(FloatRect(0, 0, rect.w, rect.h));
	}

	void updateSrcRectCon(){
		/* Cut old connection */
		srcRectCon.Disconnect();
		/* Create new one */
		srcRectCon = srcRect->valueChanged.Connect(*this, &LightMapPrivate::onSrcRectChange);
	}

	void updateVisibility(){
		isVisible = false;

		if (nullOrDisposed(bitmap))
			return;

		/* Compare sprite bounding box against the scene */
		IntRect self;
		self.setPos(Vec2i(0, 0));
		self.w = bitmap->width();
		self.h = bitmap->height();

		isVisible = SDL_HasRectIntersection(&self, &sceneRect);
	}

	void prepare(){
		updateVisibility();
	}
};

LightMap::LightMap(Viewport *viewport)
    : ViewportElement(viewport){
	p = new LightMapPrivate;
	onGeometryChange(scene->getGeometry());
	
	if (nullOrDisposed(p->bitmap))
		return;

	p->bitmap->ensureNonMega();

	*p->srcRect = p->bitmap->rect();
	p->onSrcRectChange();
	p->quad.setPosRect(p->srcRect->toFloatRect());
}

LightMap::~LightMap(){
	dispose();
}

DEF_ATTR_RD_SIMPLE(LightMap, WallMap, Bitmap*, p->wallMap)

DEF_ATTR_SIMPLE(LightMap, CameraX,        int, p->cameraX)
DEF_ATTR_SIMPLE(LightMap, CameraY,        int, p->cameraY)
DEF_ATTR_SIMPLE(LightMap, TilemapOffsetX, int, p->tilemapOffsetX)
DEF_ATTR_SIMPLE(LightMap, TilemapOffsetY, int, p->tilemapOffsetY)

DEF_ATTR_SIMPLE(LightMap, Ambient, float, p->ambient);

void LightMap::setWallMap(Bitmap *bitmap){
	guardDisposed();

	if (p->wallMap == bitmap)
		return;

	p->wallMap = bitmap;

	if (nullOrDisposed(bitmap))
		return;

	bitmap->ensureNonMega();
}

void LightMap::initDynAttribs(){
	p->srcRect = new Rect;

	p->updateSrcRectCon();
}

/* Flashable */
void LightMap::update(){
	guardDisposed();

	Flashable::update();
}

void LightMap::clearStaticLightSources(){
	p->staticLightSources.clear();
}
void LightMap::clearDynamicLightSources(){
	p->dynamicLightSources.clear();
}
void LightMap::addStaticLightSource(LightSource source){
	p->staticLightSources.push_back(source);
}
void LightMap::addDynamicLightSource(LightSource source){
	p->dynamicLightSources.push_back(source);
}
void LightMap::removeStaticLightSource(float x, float y){
	for (int i = 0; i < p->staticLightSources.size(); i++)
		if (p->staticLightSources[i].x == x && p->staticLightSources[i].y == y){
			p->staticLightSources.erase(p->staticLightSources.begin() + i);
			return;
		}
}

/* SceneElement */
void LightMap::draw(){
	if (!p->isVisible)
		return;

	if (emptyFlashFlag)
		return;

	DynamicLightShader &shader = shState->shaders().dynamicLight;
	shader.bind();
	shader.applyViewportProj();

	if (p->wallMap)
	{
		shader.setWallMapTexture(p->wallMap->getGLTypes().tex);
		shader.setWallMapResolution(p->wallMap->width(), p->wallMap->height());
	}
	shader.setCameraPosition(p->cameraX, p->cameraY);
	shader.setTileMapOffset(p->tilemapOffsetX, p->tilemapOffsetY);

	p->gpuBuffer.clear();
	p->gpuBuffer.insert(p->gpuBuffer.end(), p->staticLightSources.begin(), p->staticLightSources.end());
	p->gpuBuffer.insert(p->gpuBuffer.end(), p->dynamicLightSources.begin(), p->dynamicLightSources.end());
	shader.setLightSources(p->gpuBuffer);
	shader.setAmbient(p->ambient);

	boost::chrono::high_resolution_clock::time_point currentTime = boost::chrono::high_resolution_clock::now();
	boost::chrono::duration<float> elapsed = currentTime - startTime;
	shader.setTime(elapsed.count());

	glState.blendMode.pushSet(BlendMultiply);

	p->bitmap->bindTex(shader);

	p->quad.draw();

	glState.blendMode.pop();
}

void LightMap::onGeometryChange(const Scene::Geometry &geo){
	p->sceneRect.setSize(geo.rect.size());
	p->sceneOrig = geo.orig;
}

void LightMap::releaseResources(){
	unlink();

	delete p;
}