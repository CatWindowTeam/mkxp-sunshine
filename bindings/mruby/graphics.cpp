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
#include "config.h"
#include "debugwriter.h"
#include "define.h"
#include <mruby.h>

static mrb_value graphicsUpdate(mrb_state *mrb, mrb_value self){
	shState->graphics().update();
	return mrb_nil_value();
}

static mrb_value graphicsDeltaTime(mrb_state *mrb, mrb_value self){
	return mrb_float_value(mrb, (mrb_float)shState->graphics().deltaTime());
}

static mrb_value graphicsFreeze(mrb_state *mrb, mrb_value self){
	shState->graphics().freeze();
	return mrb_nil_value();
}

static mrb_value graphicsTransition(mrb_state *mrb, mrb_value self){
	int duration = 8;
	const char *filename = "";
	int vague = 40;
	mrb_get_args(mrb, "|izi", &duration, &filename, &vague);
	shState->graphics().transition(duration, filename, vague);
	return mrb_nil_value();
}

static mrb_value graphicsFrameReset(mrb_state *mrb, mrb_value self){
	shState->graphics().frameReset();
	return mrb_nil_value();
}

#define DEF_GRA_PROP_I(PropName) \
	static mrb_value graphics##Get##PropName(mrb_state *mrb, mrb_value self){ \
		return mrb_fixnum_value((mrb_int)shState->graphics().get##PropName()); \
	} \
	static mrb_value graphics##Set##PropName(mrb_state *mrb, mrb_value self){ \
		int value; \
		mrb_get_args(mrb, "i", &value); \
		shState->graphics().set##PropName(value); \
		return mrb_fixnum_value((mrb_int)value); \
	}

#define DEF_GRA_PROP_B(PropName) \
	static mrb_value graphics##Get##PropName(mrb_state *mrb, mrb_value self){ \
		return shState->graphics().get##PropName() ? mrb_true_value() : mrb_false_value(); \
	} \
	static mrb_value graphics##Set##PropName(mrb_state *mrb, mrb_value self){ \
		bool value; \
		mrb_get_args(mrb, "b", &value); \
		shState->graphics().set##PropName(value); \
		return value ? mrb_true_value() : mrb_false_value(); \
	}

static mrb_value graphicsPosX(mrb_state *mrb, mrb_value self){
	return mrb_fixnum_value((mrb_int)shState->graphics().x());
}

static mrb_value graphicsPosY(mrb_state *mrb, mrb_value self){
	return mrb_fixnum_value((mrb_int)shState->graphics().y());
}

static mrb_value graphicsWidth(mrb_state *mrb, mrb_value self){
	return mrb_fixnum_value((mrb_int)shState->graphics().width());
}

static mrb_value graphicsHeight(mrb_state *mrb, mrb_value self){
	return mrb_fixnum_value((mrb_int)shState->graphics().height());
}

static mrb_value graphicsWait(mrb_state *mrb, mrb_value self){
	int duration;
	mrb_get_args(mrb, "i", &duration);
	shState->graphics().wait(duration);
	return mrb_nil_value();
}

static mrb_value graphicsSetVsync(mrb_state *mrb, mrb_value self){
	int vsync;
	mrb_get_args(mrb, "i", &vsync);
	shState->graphics().setVsync(vsync);
	return mrb_nil_value();
}

static mrb_value graphicsFadeout(mrb_state *mrb, mrb_value self){
	int duration;
	mrb_get_args(mrb, "i", &duration);
	shState->graphics().fadeout(duration);
	return mrb_nil_value();
}

static mrb_value graphicsFadein(mrb_state *mrb, mrb_value self){
	int duration;
	mrb_get_args(mrb, "i", &duration);
	shState->graphics().fadein(duration);
	return mrb_nil_value();
}

static mrb_value graphicsResizeScreen(mrb_state *mrb, mrb_value self){
	int width, height;
	bool emitSignal = true;
	mrb_get_args(mrb, "ii|b", &width, &height, &emitSignal);
	shState->graphics().resizeScreen(width, height, emitSignal);
	return mrb_nil_value();
}

static mrb_value graphicsMoveScreen(mrb_state *mrb, mrb_value self){
	int x, y;
	mrb_get_args(mrb, "ii", &x, &y);
	shState->graphics().moveScreen(x, y);
	return mrb_nil_value();
}

static mrb_value graphicsReset(mrb_state *mrb, mrb_value self){
	shState->graphics().reset();
	return mrb_nil_value();
}

DEF_GRA_PROP_I(FrameRate)
DEF_GRA_PROP_I(FrameCount)
DEF_GRA_PROP_I(Brightness)

DEF_GRA_PROP_B(Fullscreen)
DEF_GRA_PROP_B(ShowCursor)
DEF_GRA_PROP_B(Smooth)

#define INIT_GRA_PROP_BIND(PropName, prop_name_s) \
{ \
	mrb_define_module_function(mrb, module, prop_name_s, graphics##Get##PropName, MRB_ARGS_NONE()); \
	mrb_define_module_function(mrb, module, prop_name_s "=", graphics##Set##PropName, MRB_ARGS_REQ(1)); \
}

//static mrb_value graphicsWindowMoved(mrb_value self){
//	RUBY_CONNECTION
//	conn->connection = shState->windowSignals.moved.Connect([conn](int x, int y){
//		shState->rubyDispatcher().invoke([conn, x, y]{
//			if (NIL_P(conn->proc)){
//				Debug() << "Unable to call non-existent proc! (Graphics.window_moved connection, disconnecting)";
//				conn->connection.Disconnect();
//				return;
//			}
//			rb_funcall(conn->proc, rb_intern("call"), 2, INT2NUM(x), INT2NUM(y));
//		});
//	});
//	return TypedData_Wrap_Struct(rb_cRubyConnection, &rubyConnection_type, conn);
//}

//static mrb_value graphicsWindowResized(mrb_value self){
//	RUBY_CONNECTION
//	conn->connection = shState->windowSignals.resized.Connect([conn](int w, int h){
//		shState->rubyDispatcher().invoke([conn, w, h]{
//			if (NIL_P(conn->proc)){
//				Debug() << "Unable to call non-existent proc! (Graphics.window_resized connection, disconnecting)";
//				conn->connection.Disconnect();
//				return;
//			}
//			rb_funcall(conn->proc, rb_intern("call"), 2, INT2NUM(w), INT2NUM(h));
//		});
//	});
//	return TypedData_Wrap_Struct(rb_cRubyConnection, &rubyConnection_type, conn);
//}

//static VALUE graphicsViewportResized(mrb_value self){
//	RUBY_CONNECTION
//	conn->connection = shState->graphicsSignals.resized.Connect([conn](int w, int h){
//		if (!conn || NIL_P(conn->proc)){
//			Debug() << "Unable to call non-existent proc! (Graphics.viewport_resized connection, disconnecting)";
//			conn->connection.Disconnect();
//			return;
//		}
//		rb_funcall(conn->proc, rb_intern("call"), 2, INT2NUM(w), INT2NUM(h));
//	});
//	return TypedData_Wrap_Struct(rb_cRubyConnection, &rubyConnection_type, conn);
//}

static mrb_value graphicsGetFOV(mrb_state *mrb, mrb_value self) {
	return mrb_float_value(mrb, (mrb_float)shState->graphics().globalFov);
}

static mrb_value graphicsSetFOV(mrb_state *mrb, mrb_value self){
	float fov = 0;
	mrb_get_args(mrb, "f", &fov);
	shState->graphics().globalFov = (double)fov;
	return mrb_nil_value();
}

void graphicsBindingInit(mrb_state *mrb){
	struct RClass *module = mrb_define_module(mrb, "Graphics");

	// Signals
	//rb_define_module_function(module, "window_moved", RUBY_METHOD_FUNC(graphicsWindowMoved), 0);
	//rb_define_module_function(module, "window_resized", RUBY_METHOD_FUNC(graphicsWindowResized), 0);
	//rb_define_module_function(module, "viewport_resized", RUBY_METHOD_FUNC(graphicsViewportResized), 0);

	// Functions
	mrb_define_module_function(mrb, module, "x", graphicsPosX, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "y", graphicsPosY, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "width", graphicsWidth, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "height", graphicsHeight, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "wait", graphicsWait, MRB_ARGS_OPT(1));
	mrb_define_module_function(mrb, module, "fadeout", graphicsFadeout, MRB_ARGS_OPT(1));
	mrb_define_module_function(mrb, module, "fadein", graphicsFadein, MRB_ARGS_OPT(1));
	//rb_define_module_function(m, "snap_to_bitmap", graphicsSnapToBitmap, 0);
	mrb_define_module_function(mrb, module, "resize_screen", graphicsResizeScreen, MRB_ARGS_ARG(2,1));
	mrb_define_module_function(mrb, module, "move_screen", graphicsMoveScreen, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module, "update", graphicsUpdate, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "delta_time", graphicsDeltaTime, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "freeze", graphicsFreeze, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "transition", graphicsTransition, MRB_ARGS_OPT(3));
	mrb_define_module_function(mrb, module, "frame_reset", graphicsFrameReset, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "vsync_mode=", graphicsSetVsync, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module, "fov", graphicsGetFOV, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "fov=", graphicsSetFOV, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "__reset__", graphicsReset, MRB_ARGS_NONE());

	// Variables
	INIT_GRA_PROP_BIND( FrameRate,  "frame_rate"  );
	INIT_GRA_PROP_BIND( FrameCount, "frame_count" );
	INIT_GRA_PROP_BIND( Brightness, "brightness" );
	INIT_GRA_PROP_BIND( Fullscreen, "fullscreen"  );
	INIT_GRA_PROP_BIND( ShowCursor, "show_cursor" );
	INIT_GRA_PROP_BIND( Smooth,     "smooth"      );
	mrb_define_const(mrb, module, "RESOLUTION_OVERRIDDEN", conf.resolutionOverridden ? mrb_true_value() : mrb_false_value());
}
