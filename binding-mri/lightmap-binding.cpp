#include "lightmap.h"
#include "disposable-binding.h"
#include "flashable-binding.h"
#include "sceneelement-binding.h"
#include "viewportelement-binding.h"
#include "binding-util.h"

DEF_TYPE(LightMap);

RB_METHOD(lightmapInitialize){
	LightMap *s = viewportElementInitialize<LightMap>(argc, argv, self);

	setPrivateData(self, s);

	/* Wrap property objects */
	s->initDynAttribs();

	return self;
}

DEF_PROP_OBJ_REF(LightMap, Bitmap, WallMap, "wallmap")

DEF_PROP_I(LightMap, CameraX)
DEF_PROP_I(LightMap, CameraY)
DEF_PROP_I(LightMap, TilemapOffsetX)
DEF_PROP_I(LightMap, TilemapOffsetY)

static VALUE clearStaticSources(VALUE self) {
	LightMap *k = getPrivateData<LightMap>(self);
	k->clearStaticLightSources();
	return Qnil;
}

static VALUE clearDynamicSources(VALUE self) {
	LightMap *k = getPrivateData<LightMap>(self);
	k->clearDynamicLightSources();
	return Qnil;
}

static VALUE addStaticSource(VALUE self, VALUE rb_x, VALUE rb_y, VALUE rb_power, VALUE rb_radius, VALUE rb_color) {
	LightMap *k = getPrivateData<LightMap>(self);
	Color *color;
	color = getPrivateDataCheck<Color>(rb_color, ColorType);
	k->addStaticLightSource( LightSource (
		NUM2DBL(rb_x),
		NUM2DBL(rb_y),
		NUM2DBL(rb_power),
		NUM2DBL(rb_radius),
		*color
	));
	return Qnil;
}

static VALUE addDynamicSource(VALUE self, VALUE rb_x, VALUE rb_y, VALUE rb_power, VALUE rb_radius, VALUE rb_color) {
	LightMap *k = getPrivateData<LightMap>(self);
	Color *color;
	color = getPrivateDataCheck<Color>(rb_color, ColorType);
	k->addDynamicLightSource( LightSource (
		NUM2DBL(rb_x),
		NUM2DBL(rb_y),
		NUM2DBL(rb_power),
		NUM2DBL(rb_radius),
		*color
	));
	return Qnil;
}

static VALUE setAmbient(VALUE self, VALUE var){
	LightMap *k = getPrivateData<LightMap>(self);
	k->setAmbient(NUM2DBL(var));
	return Qnil;
}

void lightmapBindingInit(){
	VALUE klass = rb_define_class("LightMap", rb_cObject);
	rb_define_alloc_func(klass, classAllocate<&LightMapType>);

	disposableBindingInit     <LightMap>(klass);
	flashableBindingInit      <LightMap>(klass);
	viewportElementBindingInit<LightMap>(klass);

	_rb_define_method(klass, "initialize", lightmapInitialize);

	INIT_PROP_BIND( LightMap, WallMap, "wallmap" );
	INIT_PROP_BIND( LightMap, CameraX, "camera_x" );
	INIT_PROP_BIND( LightMap, CameraY, "camera_y" );
	INIT_PROP_BIND( LightMap, TilemapOffsetX, "tilemap_offset_x" );
	INIT_PROP_BIND( LightMap, TilemapOffsetY, "tilemap_offset_y" );
	
	rb_define_method(klass, "clear_static_sources", clearStaticSources, 0);
	rb_define_method(klass, "clear_dynamic_sources", clearDynamicSources, 0);
	rb_define_method(klass, "add_static_source", addStaticSource, 5);
	rb_define_method(klass, "add_dynamic_source", addDynamicSource, 5);
	rb_define_method(klass, "ambient=", setAmbient, 1);
}
