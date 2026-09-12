/*
** graphics-binding.cpp
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

#include "graphics.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "binding-util.h"
#include "binding-types.h"
#include "exception.h"
#include "config.h"
#include "debugwriter.h"
#include "signals/rubydispatcher.h"

#include "signalconnection-binding.h"
#include "define.h"

#include <vector>
#include <cstring>

#ifdef mkxp_android
#include "input.h"
#include "eventthread.h"

void androidSetTouchControlsVisible(bool visible);

static bool androidSceneIs(VALUE sceneVal, const char *name){
	if (NIL_P(sceneVal))
		return false;
	VALUE sceneClass = rb_obj_class(sceneVal);
	VALUE sceneName = rb_class_name(sceneClass);
	return strcmp(StringValueCStr(sceneName), name) == 0;
}

static bool androidShouldShowTouchControls(){
	VALUE sceneVal = rb_gv_get("$scene");
	if (!androidSceneIs(sceneVal, "Scene_Map"))
		return false;

	VALUE gameTemp = rb_gv_get("$game_temp");
	if (!NIL_P(gameTemp) && rb_respond_to(gameTemp, rb_intern("message_window_showing"))){
		VALUE showing = rb_funcall(gameTemp, rb_intern("message_window_showing"), 0);
		if (RTEST(showing))
			return false;
	}

	VALUE gameSystem = rb_gv_get("$game_system");
	if (!NIL_P(gameSystem) && rb_respond_to(gameSystem, rb_intern("map_interpreter"))){
		VALUE interpreter = rb_funcall(gameSystem, rb_intern("map_interpreter"), 0);
		if (!NIL_P(interpreter) && rb_respond_to(interpreter, rb_intern("running?"))){
			VALUE running = rb_funcall(interpreter, rb_intern("running?"), 0);
			if (RTEST(running))
				return false;
		}
	}

	return true;
}

static void androidHandleTitleClick(){
	if (!EventThread::leftClickEdge())
		return;

	VALUE sceneVal = rb_gv_get("$scene");
	if (!androidSceneIs(sceneVal, "Scene_Title"))
		return;

	VALUE cursorPos = rb_iv_get(sceneVal, "@cursor_pos");
	if (NIL_P(cursorPos))
		return;

	VALUE sceneClass = rb_obj_class(sceneVal);
	int menuX = NUM2INT(rb_const_get(sceneClass, rb_intern("MENU_X")));
	int menuY = NUM2INT(rb_const_get(sceneClass, rb_intern("MENU_Y")));
	int entryH = NUM2INT(rb_const_get(sceneClass, rb_intern("ENTRY_HEIGHT")));

	int gw = shState->graphics().width();
	int gh = shState->graphics().height();

	int mx = shState->input().mouseX();
	int my = shState->input().mouseY();

	int menuLeft = gw - menuX;
	int menuTop = gh - menuY;

	if (mx < menuLeft || mx >= menuLeft + menuX || my < menuTop)
		return;

	int row = (my - menuTop) / entryH;

	int maxIndex = 2;
	VALUE gameSwitches = rb_gv_get("$game_switches");
	if (!NIL_P(gameSwitches) && rb_respond_to(gameSwitches, rb_intern("[]"))){
		VALUE sw160 = rb_funcall(gameSwitches, rb_intern("[]"), 1, INT2FIX(160));
		VALUE sw152 = rb_funcall(gameSwitches, rb_intern("[]"), 1, INT2FIX(152));
		if (RTEST(sw160) && RTEST(sw152))
			maxIndex = 3;
	}

	if (row < 0 || row > maxIndex)
		return;

	rb_iv_set(sceneVal, "@cursor_pos", rb_float_new((double)row));
}
#endif

RB_METHOD(graphicsUpdate){
	RB_UNUSED_PARAM;

	shState->graphics().update();

#ifdef mkxp_android
	androidSetTouchControlsVisible(androidShouldShowTouchControls());
	androidHandleTitleClick();
#endif

	return Qnil;
}

RB_METHOD(graphicsFreeze){
	RB_UNUSED_PARAM;

	shState->graphics().freeze();

	return Qnil;
}

RB_METHOD(graphicsTransition){
	RB_UNUSED_PARAM;

	int duration = 8;
	const char *filename = "";
	int vague = 40;

	rb_get_args(argc, argv, "|izi", &duration, &filename, &vague RB_ARG_END);

	GUARD_EXC( shState->graphics().transition(duration, filename, vague); )

	return Qnil;
}

RB_METHOD(graphicsFrameReset){
	RB_UNUSED_PARAM;

	shState->graphics().frameReset();

	return Qnil;
}

#define DEF_GRA_PROP_I(PropName) \
	RB_METHOD(graphics##Get##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_fix_new(shState->graphics().get##PropName()); \
	} \
	RB_METHOD(graphics##Set##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		int value; \
		rb_get_args(argc, argv, "i", &value RB_ARG_END); \
		shState->graphics().set##PropName(value); \
		return INT2FIX(value); \
	}

#define DEF_GRA_PROP_B(PropName) \
	RB_METHOD(graphics##Get##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		return rb_bool_new(shState->graphics().get##PropName()); \
	} \
	RB_METHOD(graphics##Set##PropName) \
	{ \
		RB_UNUSED_PARAM; \
		bool value; \
		rb_get_args(argc, argv, "b", &value RB_ARG_END); \
		shState->graphics().set##PropName(value); \
		return rb_bool_new(value); \
	}
	

RB_METHOD(graphicsPosX){
	RB_UNUSED_PARAM;
	return INT2FIX(shState->graphics().x());
}

RB_METHOD(graphicsPosY){
	RB_UNUSED_PARAM;
	return INT2FIX(shState->graphics().y());
}

RB_METHOD(graphicsWidth){
	RB_UNUSED_PARAM;
	return INT2FIX(shState->graphics().width());
}

RB_METHOD(graphicsHeight){
	RB_UNUSED_PARAM;
	return INT2FIX(shState->graphics().height());
}

RB_METHOD(graphicsWait){
	RB_UNUSED_PARAM;

	int duration;
	rb_get_args(argc, argv, "i", &duration RB_ARG_END);

	shState->graphics().wait(duration);

	return Qnil;
}

RB_METHOD(graphicsSetVsync){
	RB_UNUSED_PARAM;
	int xuinia_ebania;
	rb_get_args(argc, argv, "i", &xuinia_ebania RB_ARG_END);
	shState->graphics().setVsync(xuinia_ebania);

	return Qnil;
}

RB_METHOD(graphicsFadeout){
	RB_UNUSED_PARAM;

	int duration;
	rb_get_args(argc, argv, "i", &duration RB_ARG_END);

	shState->graphics().fadeout(duration);

	return Qnil;
}

RB_METHOD(graphicsFadein){
	RB_UNUSED_PARAM;

	int duration;
	rb_get_args(argc, argv, "i", &duration RB_ARG_END);

	shState->graphics().fadein(duration);

	return Qnil;
}

void bitmapInitProps(Bitmap *b, VALUE self);

RB_METHOD(graphicsSnapToBitmap){
	RB_UNUSED_PARAM;

	Bitmap *result = 0;
	GUARD_EXC( result = shState->graphics().snapToBitmap(); );

	VALUE obj = wrapObject(result, BitmapType);
	bitmapInitProps(result, obj);

	return obj;
}

RB_METHOD(graphicsResizeScreen){
	RB_UNUSED_PARAM;

	int width, height;
	bool emitSignal = true;
	rb_get_args(argc, argv, "ii|b", &width, &height, &emitSignal RB_ARG_END);

	shState->graphics().resizeScreen(width, height, emitSignal);

	return Qnil;
}

RB_METHOD(graphicsMoveScreen){
	RB_UNUSED_PARAM;

	int x, y;
	rb_get_args(argc, argv, "ii", &x, &y RB_ARG_END);

	shState->graphics().moveScreen(x, y);

	return Qnil;
}

RB_METHOD(graphicsReset){
	RB_UNUSED_PARAM;

	shState->graphics().reset();

	return Qnil;
}

DEF_GRA_PROP_I(FrameRate)
DEF_GRA_PROP_I(FrameCount)
DEF_GRA_PROP_I(Brightness)

DEF_GRA_PROP_B(Fullscreen)
DEF_GRA_PROP_B(ShowCursor)
DEF_GRA_PROP_B(Smooth)
DEF_GRA_PROP_B(Frameskip)

#define INIT_GRA_PROP_BIND(PropName, prop_name_s) \
{ \
	_rb_define_module_function(module, prop_name_s, graphics##Get##PropName); \
	_rb_define_module_function(module, prop_name_s "=", graphics##Set##PropName); \
}

static VALUE graphicsWindowMoved(VALUE){
	RUBY_CONNECTION
	conn->connection = shState->windowSignals.moved.Connect([conn](int x, int y){
		shState->rubyDispatcher().invoke([conn, x, y]{
			if (NIL_P(conn->proc)){
				Debug() << "Unable to call non-existent proc! (Graphics.window_moved connection, disconnecting)";
				conn->connection.Disconnect();
				return;
			}
			rb_funcall(conn->proc, rb_intern("call"), 2, INT2NUM(x), INT2NUM(y));
		});
	});
	return TypedData_Wrap_Struct(rb_cRubyConnection, &rubyConnection_type, conn);
}

static VALUE graphicsWindowResized(VALUE){
	RUBY_CONNECTION
	conn->connection = shState->windowSignals.resized.Connect([conn](int w, int h){
		shState->rubyDispatcher().invoke([conn, w, h]{
			if (NIL_P(conn->proc)){
				Debug() << "Unable to call non-existent proc! (Graphics.window_resized connection, disconnecting)";
				conn->connection.Disconnect();
				return;
			}
			rb_funcall(conn->proc, rb_intern("call"), 2, INT2NUM(w), INT2NUM(h));
		});
	});
	return TypedData_Wrap_Struct(rb_cRubyConnection, &rubyConnection_type, conn);
}

static VALUE graphicsViewportResized(VALUE){
	RUBY_CONNECTION
	conn->connection = shState->graphicsSignals.resized.Connect([conn](int w, int h){
		//shState->rubyDispatcher().invoke([conn, w, h]{
			if (NIL_P(conn->proc)){
				Debug() << "Unable to call non-existent proc! (Graphics.viewport_resized connection, disconnecting)";
				conn->connection.Disconnect();
				return;
			}
			rb_funcall(conn->proc, rb_intern("call"), 2, INT2NUM(w), INT2NUM(h));
		//});
	});
	return TypedData_Wrap_Struct(rb_cRubyConnection, &rubyConnection_type, conn);
}

static VALUE graphicsGetFOV(VALUE) {
	return DBL2NUM(shState->graphics().globalFov);
}

static VALUE graphicsSetFOV(VALUE, VALUE fov) {
	shState->graphics().globalFov = NUM2DBL(fov);
	return Qnil;
}

void graphicsBindingInit(){
	VALUE module = rb_define_module("Graphics");

	// Signals

	rb_define_module_function(module, "window_moved", RUBY_METHOD_FUNC(graphicsWindowMoved), 0);
	rb_define_module_function(module, "window_resized", RUBY_METHOD_FUNC(graphicsWindowResized), 0);
	rb_define_module_function(module, "viewport_resized", RUBY_METHOD_FUNC(graphicsViewportResized), 0);


	rb_define_module_function(module, "fov", RUBY_METHOD_FUNC(graphicsGetFOV), 0);
	rb_define_module_function(module, "fov=", RUBY_METHOD_FUNC(graphicsSetFOV), 1);

	// Functions
	_rb_define_module_function(module, "x", graphicsPosX);
	_rb_define_module_function(module, "y", graphicsPosY);
	_rb_define_module_function(module, "width", graphicsWidth);
	_rb_define_module_function(module, "height", graphicsHeight);
	_rb_define_module_function(module, "wait", graphicsWait);
	_rb_define_module_function(module, "fadeout", graphicsFadeout);
	_rb_define_module_function(module, "fadein", graphicsFadein);
	_rb_define_module_function(module, "snap_to_bitmap", graphicsSnapToBitmap);
	_rb_define_module_function(module, "resize_screen", graphicsResizeScreen);
	_rb_define_module_function(module, "move_screen", graphicsMoveScreen);
	_rb_define_module_function(module, "update", graphicsUpdate);
	_rb_define_module_function(module, "freeze", graphicsFreeze);
	_rb_define_module_function(module, "transition", graphicsTransition);
	_rb_define_module_function(module, "frame_reset", graphicsFrameReset);
	_rb_define_module_function(module, "vsync_mode=", graphicsSetVsync);
	_rb_define_module_function(module, "setVsync", graphicsSetVsync);
	_rb_define_module_function(module, "__reset__", graphicsReset);

	// Variables
	INIT_GRA_PROP_BIND( FrameRate,  "frame_rate"  );
	INIT_GRA_PROP_BIND( FrameCount, "frame_count" );
	INIT_GRA_PROP_BIND( Brightness, "brightness" );
	INIT_GRA_PROP_BIND( Fullscreen, "fullscreen"  );
	INIT_GRA_PROP_BIND( ShowCursor, "show_cursor" );
	INIT_GRA_PROP_BIND( Smooth,     "smooth"      );
	INIT_GRA_PROP_BIND( Frameskip,  "frameskip"   );
	
	const Config &conf = shState->rtData().config;
	rb_define_const(module, "RESOLUTION_OVERRIDDEN", rb_bool_new(conf.resolutionOverridden));
}
