/*
** window-binding.cpp
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

#include "window.h"
#include "disposable-binding.h"
#include "viewportelement-binding.h"
#include "binding-util.h"
#include "define.h"

#ifdef mkxp_android
#include "sharedstate.h"
#include "input.h"
#include "eventthread.h"

static void androidHandleSelectableClick(VALUE self, Window *w){
	VALUE itemMax = rb_iv_get(self, "@item_max");
	VALUE columnMax = rb_iv_get(self, "@column_max");
	VALUE active = rb_iv_get(self, "@active");

	if (!FIXNUM_P(itemMax) || !FIXNUM_P(columnMax) || !RTEST(active))
		return;

	if (!EventThread::leftClickEdge())
		return;

	int itemMaxI = FIX2INT(itemMax);
	int columnMaxI = FIX2INT(columnMax);
	if (itemMaxI <= 0 || columnMaxI <= 0)
		return;

	int wx = w->getX(), wy = w->getY();
	int ww = w->getWidth(), wh = w->getHeight();
	int oy = w->getOY();
	int mx = shState->input().mouseX();
	int my = shState->input().mouseY();

	if (mx < wx || mx >= wx + ww || my < wy || my >= wy + wh)
		return;

	int cursorWidth = ww / columnMaxI - 32;
	if (cursorWidth <= 0)
		return;

	int lx = mx - wx - 16;
	int ly = my - wy - 16 + oy;
	if (lx < 0 || ly < 0)
		return;

	int col = lx / (cursorWidth + 32);
	int row = ly / 32;
	int clickIndex = row * columnMaxI + col;

	if (col < 0 || col >= columnMaxI || clickIndex < 0 || clickIndex >= itemMaxI)
		return;

	VALUE curIndex = rb_iv_get(self, "@index");
	if (!FIXNUM_P(curIndex) || FIX2INT(curIndex) != clickIndex)
		rb_funcall(self, rb_intern("index="), 1, INT2FIX(clickIndex));
}
#endif

DEF_TYPE(Window);

RB_METHOD(windowInitialize){
	Window *w = viewportElementInitialize<Window>(argc, argv, self);

	setPrivateData(self, w);

	w->initDynAttribs();

	wrapProperty(self, &w->getCursorRect(), "cursor_rect", RectType);

	return self;
}

RB_METHOD(windowUpdate){
	RB_UNUSED_PARAM;
	Window *w = getPrivateData<Window>(self);

	w->update();

#ifdef mkxp_android
	androidHandleSelectableClick(self, w);
#endif

	return Qnil;
}

DEF_PROP_OBJ_REF(Window, Bitmap, Windowskin, "windowskin")
DEF_PROP_OBJ_REF(Window, Bitmap, Contents,   "contents")
DEF_PROP_OBJ_VAL(Window, Rect,   CursorRect, "cursor_rect")

DEF_PROP_B(Window, Stretch)
DEF_PROP_B(Window, Active)
DEF_PROP_B(Window, Pause)

DEF_PROP_I(Window, X)
DEF_PROP_I(Window, Y)
DEF_PROP_I(Window, Width)
DEF_PROP_I(Window, Height)
DEF_PROP_I(Window, OX)
DEF_PROP_I(Window, OY)
DEF_PROP_I(Window, Opacity)
DEF_PROP_I(Window, BackOpacity)
DEF_PROP_I(Window, ContentsOpacity)


void windowBindingInit(){
	VALUE klass = rb_define_class("Window", rb_cObject);
	rb_define_alloc_func(klass, classAllocate<&WindowType>);

	disposableBindingInit     <Window>(klass);
	viewportElementBindingInit<Window>(klass);

	_rb_define_method(klass, "initialize", windowInitialize);
	_rb_define_method(klass, "update",     windowUpdate);
	INIT_PROP_BIND( Window, Windowskin,      "windowskin"       );
	INIT_PROP_BIND( Window, Contents,        "contents"         );
	INIT_PROP_BIND( Window, Stretch,         "stretch"          );
	INIT_PROP_BIND( Window, CursorRect,      "cursor_rect"      );
	INIT_PROP_BIND( Window, Active,          "active"           );
	INIT_PROP_BIND( Window, Pause,           "pause"            );
	INIT_PROP_BIND( Window, X,               "x"                );
	INIT_PROP_BIND( Window, Y,               "y"                );
	INIT_PROP_BIND( Window, Width,           "width"            );
	INIT_PROP_BIND( Window, Height,          "height"           );
	INIT_PROP_BIND( Window, OX,              "ox"               );
	INIT_PROP_BIND( Window, OY,              "oy"               );
	INIT_PROP_BIND( Window, Opacity,         "opacity"          );
	INIT_PROP_BIND( Window, BackOpacity,     "back_opacity"     );
	INIT_PROP_BIND( Window, ContentsOpacity, "contents_opacity" );
}
