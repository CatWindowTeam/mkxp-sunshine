#include "lightmap.h"
#include "sharedstate.h"
#include "bitmap.h"
#include "etc.h"
#include "etc-internal.h"
#include "util.h"
#include "signals/signal.h"
#include "quad.h"
#include "quadarray.h"
#include "config.h"
#include "sunshine.h"
#include "transform.h"
#include "texpool.h"
#include <math.h>
#include <SDL3/SDL_rect.h>
#include <chrono>

struct LightMapPrivate{
	Bitmap *bitmap;
	Bitmap *wallMap;

	int cameraX, cameraY, tilemapOffsetX, tilemapOffsetY, scale;
	float ambient;

	std::vector<LightSource> staticLightSources;
	std::vector<LightSource> dynamicLightSources;
	std::vector<LightSource> gpuBuffer;

	SignalConnection bitmapUpdateConnection;
	SignalConnection srcRectCon;
	SignalConnection prepareCon;
	Quad quad;
	Rect *srcRect;
	IntRect sceneRect;
	Vec2i sceneOrig;

	/* Would this sprite be visible on
	 * the screen if drawn? */
	bool isVisible;
	EtcTemps tmp;


	LightMapPrivate()
	    : wallMap(0),
		  cameraX(0),
		  cameraY(0),
		  tilemapOffsetX(0),
		  tilemapOffsetY(0),
		  ambient(255.0),
		  scale(1),
	      srcRect(&tmp.rect),
	      isVisible(false)
	{
		sceneRect.x = sceneRect.y = 0;

		updateBitmap();
		updateSrcRectCon();

		bitmapUpdateConnection = shState->graphicsSignals.resized.Connect([&](int w, int h){
			shState->rubyDispatcher().invoke([&]{
				updateBitmap();
			});
		});
		prepareCon = shState->graphicsSignals.prepareDraw.Connect(*this, &LightMapPrivate::prepare);
		
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

	void updateBitmap(){
		if (scale <= 0)
			scale = 1;
		int w = shState->graphics().width();
		int h = shState->graphics().height();
		int s_w = (w + scale - 1) / scale;
		int s_h = (h + scale - 1) / scale;
		bitmap = new Bitmap(s_w, s_h);
		bitmap->ensureNonMega();
		*srcRect = IntRect(0, 0, s_w * scale, s_h * scale);
		onSrcRectChange();
		quad.setPosRect(srcRect->toFloatRect());
	}


	// render light into bitmap
	void renderLight(){
		IRender &render = shState->render();

		Quad &renderQuad = shState->gpQuad();
		FloatRect rect(0, 0, bitmap->width(), bitmap->height());
		renderQuad.setTexPosRect(rect, rect);

		RenderTarget auxTex = shState->texPool().request(bitmap->width(), bitmap->height());

		render.pushBlend(false);
		render.pushViewport(IntRect(0, 0, bitmap->width(), bitmap->height()));

		render.bindTexture(auxTex.tex);
		render.bindRenderTarget(bitmap->getRenderTarget());

		render.useEffect(SHADER_dynamicLight);
		render.setTexSize(Vec2i(bitmap->width(), bitmap->height()));
		render.applyViewportProj();
		render.setScale(scale);

		if (wallMap){
			render.setEffectTexture(EffectTexture::WallMap, wallMap->getRenderTarget().tex);
			render.setWallMapResolution(wallMap->width(), wallMap->height());
		}
		render.setCameraPosition(cameraX, cameraY);
		render.setTileMapOffset(tilemapOffsetX, tilemapOffsetY);

		gpuBuffer.clear();
		gpuBuffer.insert(gpuBuffer.end(), staticLightSources.begin(), staticLightSources.end());
		gpuBuffer.insert(gpuBuffer.end(), dynamicLightSources.begin(), dynamicLightSources.end());
		render.setLightSources(gpuBuffer);
		render.setAmbient(ambient);

		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = currentTime - startTime;
		render.setTime(elapsed.count());

		renderQuad.draw();

		render.popViewport();
		render.popBlend();

		shState->texPool().release(auxTex);

		bitmap->callModified();
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
		renderLight();
	}
};

LightMap::LightMap(Viewport *viewport) : ViewportElement(viewport){
	p = new LightMapPrivate;
	onGeometryChange(scene->getGeometry());
	
	if (nullOrDisposed(p->bitmap))
		return;

	p->updateBitmap();
}

LightMap::~LightMap(){
	dispose();
}

DEF_ATTR_RD_SIMPLE(LightMap, WallMap, Bitmap*, p->wallMap)
DEF_ATTR_RD_SIMPLE(LightMap, Scale, int, p->scale)

DEF_ATTR_SIMPLE(LightMap, CameraX,        int,   p->cameraX)
DEF_ATTR_SIMPLE(LightMap, CameraY,        int,   p->cameraY)
DEF_ATTR_SIMPLE(LightMap, TilemapOffsetX, int,   p->tilemapOffsetX)
DEF_ATTR_SIMPLE(LightMap, TilemapOffsetY, int,   p->tilemapOffsetY)
DEF_ATTR_SIMPLE(LightMap, Ambient,        float, p->ambient);

void LightMap::setScale(int scale){
	guardDisposed();
	if (p->scale == scale)
		return;
	
	p->scale = scale;
	if (p->bitmap != nullptr && !p->bitmap->isDisposed())
	{
		p->bitmap->dispose();
		delete p->bitmap;
	}
	p->updateBitmap();
}

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
void LightMap::addStaticLightSource(const LightSource source){
	p->staticLightSources.push_back(source);
}
void LightMap::addDynamicLightSource(const LightSource source){
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

	IRender &render = shState->render();
	
	render.useEffect(SHADER_simple);
	render.applyViewportProj();
	render.setTextureSmooth(p->bitmap->getRenderTarget().tex, smooth);

	render.pushBlendMode(BlendMultiply);

	p->bitmap->bindTex();
	p->quad.draw();

	render.popBlendMode();
}

void LightMap::onGeometryChange(const Scene::Geometry &geo){
	p->sceneRect.setSize(geo.rect.size());
	p->sceneOrig = geo.orig;
}

void LightMap::releaseResources(){
	unlink();
	delete p;
}
