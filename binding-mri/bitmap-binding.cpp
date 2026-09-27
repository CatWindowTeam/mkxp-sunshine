/*
** bitmap-binding.cpp
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

#include "bitmap.h"
#include "font.h"
#include "exception.h"
#include "disposable-binding.h"
#include "binding-util.h"
#include "binding-types.h"

DEF_TYPE(Bitmap);
static const char *objAsStringPtr(VALUE obj){
	VALUE str = rb_obj_as_string(obj);
	return RSTRING_PTR(str);
}

void bitmapInitProps(Bitmap *b, VALUE self){
	/* Wrap properties */
	VALUE fontKlass = rb_const_get(rb_cObject, rb_intern("Font"));
	VALUE fontObj = rb_obj_alloc(fontKlass);
	rb_obj_call_init(fontObj, 0, 0);
	Font *font = getPrivateData<Font>(fontObj);
	b->setInitFont(font);
	rb_iv_set(self, "font", fontObj);
}

static VALUE bitmapInitialize(int argc, VALUE *argv, VALUE self){
	Bitmap *b = 0;
	if (argc == 1){
		char *filename;
		rb_get_args(argc, argv, "z", &filename RB_ARG_END);
		GUARD_EXC( b = new Bitmap(filename); )
	}else{
		int width, height;
		rb_get_args(argc, argv, "ii", &width, &height RB_ARG_END);
		GUARD_EXC( b = new Bitmap(width, height); )
	}

	setPrivateData(self, b);
	bitmapInitProps(b, self);
	return self;
}

static VALUE bitmapWidth(VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	int value = 0;
	GUARD_EXC( value = b->width(); );
	return INT2FIX(value);
}

static VALUE bitmapHeight(VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	int value = 0;
	GUARD_EXC( value = b->height(); );
	return INT2FIX(value);
}

static VALUE bitmapRect(VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	IntRect rect;
	GUARD_EXC( rect = b->rect(); );
	Rect *r = new Rect(rect);
	return wrapObject(r, RectType);
}

static VALUE bitmapBlt(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);

	int x, y;
	VALUE srcObj;
	VALUE srcRectObj;
	int opacity = 255;
	Bitmap *src;
	Rect *srcRect;

	rb_get_args(argc, argv, "iioo|i", &x, &y, &srcObj, &srcRectObj, &opacity RB_ARG_END);
	src = getPrivateDataCheck<Bitmap>(srcObj, BitmapType);
	srcRect = getPrivateDataCheck<Rect>(srcRectObj, RectType);
	GUARD_EXC( b->blt(x, y, *src, srcRect->toIntRect(), opacity); );
	return self;
}

static VALUE bitmapStretchBlt(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	VALUE destRectObj;
	VALUE srcObj;
	VALUE srcRectObj;
	int opacity = 255;
	Bitmap *src;
	Rect *destRect, *srcRect;

	rb_get_args(argc, argv, "ooo|i", &destRectObj, &srcObj, &srcRectObj, &opacity RB_ARG_END);
	src = getPrivateDataCheck<Bitmap>(srcObj, BitmapType);
	destRect = getPrivateDataCheck<Rect>(destRectObj, RectType);
	srcRect = getPrivateDataCheck<Rect>(srcRectObj, RectType);

	GUARD_EXC( b->stretchBlt(destRect->toIntRect(), *src, srcRect->toIntRect(), opacity); );
	return self;
}

static VALUE bitmapFillRect(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	VALUE colorObj;
	Color *color;

	if (argc == 2){
		VALUE rectObj;
		Rect *rect;
		rb_get_args(argc, argv, "oo", &rectObj, &colorObj RB_ARG_END);
		rect = getPrivateDataCheck<Rect>(rectObj, RectType);
		color = getPrivateDataCheck<Color>(colorObj, ColorType);
		GUARD_EXC( b->fillRect(rect->toIntRect(), color->norm); );
	}else{
		int x, y, width, height;
		rb_get_args(argc, argv, "iiiio", &x, &y, &width, &height, &colorObj RB_ARG_END);
		color = getPrivateDataCheck<Color>(colorObj, ColorType);
		GUARD_EXC( b->fillRect(x, y, width, height, color->norm); );
	}
	return self;
}

static VALUE bitmapClear(VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	GUARD_EXC( b->clear(); )
	return self;
}

static VALUE bitmapGetPixel(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	int x, y;
	rb_get_args(argc, argv, "ii", &x, &y RB_ARG_END);
	Color value;
	GUARD_EXC( value = b->getPixel(x, y); );
	Color *color = new Color(value);
	return wrapObject(color, ColorType);
}

static VALUE bitmapSetPixel(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	int x, y;
	VALUE colorObj;
	Color *color;
	rb_get_args(argc, argv, "iio", &x, &y, &colorObj RB_ARG_END);
	color = getPrivateDataCheck<Color>(colorObj, ColorType);
	GUARD_EXC( b->setPixel(x, y, *color); );
	return self;
}

static VALUE bitmapHueChange(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	int hue;
	rb_get_args(argc, argv, "i", &hue RB_ARG_END);
	GUARD_EXC( b->hueChange(hue); );
	return self;
}

static VALUE bitmapDrawText(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	const char *str;
	int align = Bitmap::Left;
	if (argc == 2 || argc == 3){
		VALUE rectObj;
		Rect *rect;
		rb_get_args(argc, argv, "oz|i", &rectObj, &str, &align RB_ARG_END);
		rect = getPrivateDataCheck<Rect>(rectObj, RectType);
		GUARD_EXC( b->drawText(rect->toIntRect(), str, align); );
	}else{
		int x, y, width, height;
		rb_get_args(argc, argv, "iiiiz|i", &x, &y, &width, &height, &str, &align RB_ARG_END);
		GUARD_EXC( b->drawText(x, y, width, height, str, align); );
	}
	return self;
}

static VALUE bitmapTextSize(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	const char *str;
	rb_get_args(argc, argv, "z", &str RB_ARG_END);
	IntRect value;
	GUARD_EXC( value = b->textSize(str); );
	Rect *rect = new Rect(value);
	return wrapObject(rect, RectType);
}

DEF_PROP_OBJ_VAL(Bitmap, Font, Font, "font")
static VALUE bitmapGradientFillRect(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	VALUE color1Obj, color2Obj, color3Obj, color4Obj;
	Color *color1, *color2, *color3, *color4;
	bool vertical = false;
	if (argc == 3 || argc == 4){
		VALUE rectObj;
		Rect *rect;

		rb_get_args(argc, argv, "ooo|b", &rectObj, &color1Obj, &color2Obj, &vertical RB_ARG_END);
		rect = getPrivateDataCheck<Rect>(rectObj, RectType);
		color1 = getPrivateDataCheck<Color>(color1Obj, ColorType);
		color2 = getPrivateDataCheck<Color>(color2Obj, ColorType);
		GUARD_EXC( b->gradientFillRect(rect->toIntRect(), color1->norm, color2->norm, vertical); );
	}else if (argc == 6 || argc == 7){
		int x, y, width, height;
		rb_get_args(argc, argv, "iiiioo|b", &x, &y, &width, &height, &color1Obj, &color2Obj, &vertical RB_ARG_END);
		color1 = getPrivateDataCheck<Color>(color1Obj, ColorType);
		color2 = getPrivateDataCheck<Color>(color2Obj, ColorType);
		GUARD_EXC( b->gradientFillRect(x, y, width, height, color1->norm, color2->norm, vertical); );
	}else if (argc == 5){
		VALUE rectObj;
		Rect *rect;

		rb_get_args(argc, argv, "ooooo", &rectObj, &color1Obj, &color2Obj, &color3Obj, &color4Obj RB_ARG_END);
		rect = getPrivateDataCheck<Rect>(rectObj, RectType);
		color1 = getPrivateDataCheck<Color>(color1Obj, ColorType);
		color2 = getPrivateDataCheck<Color>(color2Obj, ColorType);
		color3 = getPrivateDataCheck<Color>(color3Obj, ColorType);
		color4 = getPrivateDataCheck<Color>(color4Obj, ColorType);
		GUARD_EXC( b->gradientFillRect(rect->toIntRect(), color1->norm, color2->norm, color3->norm, color4->norm); );
	}else{
		int x, y, width, height;
		rb_get_args(argc, argv, "iiiioooo", &x, &y, &width, &height, &color1Obj, &color2Obj, &color3Obj, &color4Obj RB_ARG_END);
		color1 = getPrivateDataCheck<Color>(color1Obj, ColorType);
		color2 = getPrivateDataCheck<Color>(color2Obj, ColorType);
		color3 = getPrivateDataCheck<Color>(color3Obj, ColorType);
		color4 = getPrivateDataCheck<Color>(color4Obj, ColorType);
		GUARD_EXC( b->gradientFillRect(x, y, width, height, color1->norm, color2->norm, color3->norm, color4->norm); );
	}
	return self;
}

static VALUE bitmapClearRect(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	if (argc == 1){
		VALUE rectObj;
		Rect *rect;
		rb_get_args(argc, argv, "o", &rectObj RB_ARG_END);
		rect = getPrivateDataCheck<Rect>(rectObj, RectType);
		GUARD_EXC( b->clearRect(rect->toIntRect()); );
	}else{
		int x, y, width, height;
		rb_get_args(argc, argv, "iiii", &x, &y, &width, &height RB_ARG_END);
		GUARD_EXC( b->clearRect(x, y, width, height); );
	}
	return self;
}

static VALUE bitmapBlur(VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	b->blur();
	return Qnil;
}

static VALUE bitmapRadialBlur(int argc, VALUE *argv, VALUE self){
	Bitmap *b = getPrivateData<Bitmap>(self);
	int angle, divisions;
	rb_get_args(argc, argv, "ii", &angle, &divisions RB_ARG_END);
	b->radialBlur(angle, divisions);
	return Qnil;
}

static VALUE bitmapInitializeCopy(int argc, VALUE *argv, VALUE self){
	rb_check_argc(argc, 1);
	VALUE origObj = argv[0];
	if (!OBJ_INIT_COPY(self, origObj))
		return self;

	Bitmap *orig = getPrivateData<Bitmap>(origObj);
	Bitmap *b = 0;
	GUARD_EXC( b = new Bitmap(*orig); );
	bitmapInitProps(b, self);
	b->setFont(orig->getFont());
	setPrivateData(self, b);
	return self;
}


void bitmapBindingInit(){
	VALUE klass = rb_define_class("Bitmap", rb_cObject);
	rb_define_alloc_func(klass, classAllocate<&BitmapType>);
	disposableBindingInit<Bitmap>(klass);
	rb_define_method(klass, "initialize", RUBY_METHOD_FUNC(bitmapInitialize), -1);
	rb_define_method(klass, "initialize_copy", RUBY_METHOD_FUNC(bitmapInitializeCopy), -1);
	rb_define_method(klass, "width", RUBY_METHOD_FUNC(bitmapWidth), 0);
	rb_define_method(klass, "height", RUBY_METHOD_FUNC(bitmapHeight), 0);
	rb_define_method(klass, "rect", RUBY_METHOD_FUNC(bitmapRect), 0);
	rb_define_method(klass, "blt", RUBY_METHOD_FUNC(bitmapBlt), -1);
	rb_define_method(klass, "stretch_blt", RUBY_METHOD_FUNC(bitmapStretchBlt), -1);
	rb_define_method(klass, "fill_rect", RUBY_METHOD_FUNC(bitmapFillRect), -1);
	rb_define_method(klass, "clear", RUBY_METHOD_FUNC(bitmapClear), 0);
	rb_define_method(klass, "get_pixel", RUBY_METHOD_FUNC(bitmapGetPixel), -1);
	rb_define_method(klass, "set_pixel", RUBY_METHOD_FUNC(bitmapSetPixel), -1);
	rb_define_method(klass, "hue_change", RUBY_METHOD_FUNC(bitmapHueChange), -1);
	rb_define_method(klass, "draw_text", RUBY_METHOD_FUNC(bitmapDrawText), -1);
	rb_define_method(klass, "text_size", RUBY_METHOD_FUNC(bitmapTextSize), -1);
	rb_define_method(klass, "gradient_fill_rect", RUBY_METHOD_FUNC(bitmapGradientFillRect), -1);
	rb_define_method(klass, "clear_rect", RUBY_METHOD_FUNC(bitmapClearRect), -1);
	rb_define_method(klass, "blur", RUBY_METHOD_FUNC(bitmapBlur), 0);
	rb_define_method(klass, "radial_blur", RUBY_METHOD_FUNC(bitmapRadialBlur), -1);
	INIT_PROP_BIND(Bitmap, Font, "font");
}
