/*
** input-binding.cpp
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

#include "input.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "util.h"
#include "eventthread.h"
#include "keybindings-binding.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_gamepad.h>
#include "debugwriter.h"
#include <vector>

extern void initGamepadBinding(VALUE inputModule);

static VALUE inputUpdate(VALUE self){
	shState->input().update();
	return Qnil;
}

static VALUE inputSetMouseEnabled(int argc, VALUE *argv, VALUE self){
	bool value;
	rb_get_args(argc, argv, "b", &value RB_ARG_END);
	EventThread::mouseEnabled = value;
	return Qnil;
}

static VALUE inputGetMouseEnabled(VALUE self){
	return EventThread::mouseEnabled ? Qtrue : Qfalse;
}

static VALUE inputSetGamepadEnabled(int argc, VALUE *argv, VALUE self){
	bool value;
	rb_get_args(argc, argv, "b", &value RB_ARG_END);
	EventThread::gamepadEnabled = value;
	return Qnil;
}

static VALUE inputGetGamepadEnabled(VALUE self){
	return EventThread::gamepadEnabled ? Qtrue : Qfalse;
}

static int getButtonArg(int argc, VALUE *argv){
	int num;
	rb_check_argc(argc, 1);
	if (FIXNUM_P(argv[0])){
		num = FIX2INT(argv[0]);
	}else{
		// FIXME: RMXP allows only few more types that
		// don't make sense (symbols in pre 3, floats)
		num = 0;
	}
	
	return num;
}

static VALUE inputPress(int argc, VALUE *argv, VALUE self){
	int num = getButtonArg(argc, argv);
	return rb_bool_new(shState->input().isPressed(num));
}

static VALUE inputTrigger(int argc, VALUE *argv, VALUE self){
	int num = getButtonArg(argc, argv);
	return rb_bool_new(shState->input().isTriggered(num));
}

static VALUE inputRepeat(int argc, VALUE *argv, VALUE self){
	int num = getButtonArg(argc, argv);
	return rb_bool_new(shState->input().isRepeated(num));
}

static VALUE inputDir4(VALUE self){
	return rb_fix_new(shState->input().dir4Value());
}

static VALUE inputDir8(VALUE self){
	return rb_fix_new(shState->input().dir8Value());
}

/* Non-standard extensions */
static VALUE inputMouseX(VALUE self){
	return rb_fix_new(shState->input().mouseX());
}

static VALUE inputMouseY(VALUE self){
	return rb_fix_new(shState->input().mouseY());
}

// wheel :3
static VALUE inputWheelX(VALUE self) {
	return rb_float_new(shState->input().wheelX());
}

static VALUE inputWheelY(VALUE self) {
	return rb_float_new(shState->input().wheelY());
}

static VALUE inputWheelFlipped(VALUE self) {
	return rb_bool_new(shState->input().wheelFlipped());
}

static VALUE inputQuit(VALUE self) {
	return rb_bool_new(shState->input().hasQuit());
}

// keyboard
static VALUE getKeyName(int argc, VALUE *argv, VALUE self) {
	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);
	if (key >= 0 && key < SDL_Scancode::SDL_SCANCODE_COUNT)
		return rb_utf8_str_new_cstr(SDL_GetKeyName(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(key), SDL_KMOD_NONE, false)));

	return rb_utf8_str_new_cstr(SDL_GetKeyName(SDLK_UNKNOWN));
}

static VALUE keyPress(int argc, VALUE *argv, VALUE self) {
	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);
	if (key >= 0 && key < SDL_Scancode::SDL_SCANCODE_COUNT)
		return rb_bool_new(EventThread::keyStates[key]);
	
	return rb_bool_new(false);
}

static VALUE getPressedKey(VALUE self) {
	short pressedKey = 0;
	for(; pressedKey < SDL_Scancode::SDL_SCANCODE_COUNT && !EventThread::keyStates[pressedKey]; pressedKey++);
	return pressedKey == SDL_Scancode::SDL_SCANCODE_COUNT ? Qnil : rb_fix_new(pressedKey);
}

static VALUE getKeyFromName(int argc, VALUE *argv, VALUE self) {
	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	return rb_fix_new(SDL_GetScancodeFromName(name));
}

// gamepad buttons
static VALUE getGamepadButtonName(int argc, VALUE *argv, VALUE self) {
	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);
	if (key >= 0 && key < SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT)
		return rb_utf8_str_new_cstr(SDL_GetGamepadStringForButton(static_cast<SDL_GamepadButton>(key)));

	return rb_utf8_str_new_cstr("Invalid");
}

static VALUE gamepadButtonPress(int argc, VALUE *argv, VALUE self) {
	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);
	if (key >= 0 && key < SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT)
		return rb_bool_new(EventThread::gcState.buttons[key]);
	
	return rb_bool_new(false);
}

static VALUE getPressedGamepadButton(VALUE self) {
	short pressedKey = 0;
	for(; pressedKey < SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT && !EventThread::gcState.buttons[pressedKey]; pressedKey++);
	return pressedKey == SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT ? Qnil : rb_fix_new(pressedKey);
}

static VALUE getGamepadButtonFromName(int argc, VALUE *argv, VALUE self) {
	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	return rb_fix_new(SDL_GetGamepadButtonFromString(name));
}

// gamepad axes
static VALUE getGamepadAxisName(int argc, VALUE *argv, VALUE self) {
	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);
	if (key >= 0 && key < SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT)
		return rb_utf8_str_new_cstr(SDL_GetGamepadStringForAxis(static_cast<SDL_GamepadAxis>(key)));

	return rb_utf8_str_new_cstr("Invalid");
}

static VALUE getGamepadAxisPressure(int argc, VALUE *argv, VALUE self) {
	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);
	if (key >= 0 && key < SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT)
		return rb_fix_new(EventThread::gcState.axes[key]);
	
	return rb_fix_new(0);
}

static VALUE getActiveGamepadAxis(int argc, VALUE *argv, VALUE self) {
	int deadzone = 16000;
	rb_get_args(argc, argv, "|i", &deadzone RB_ARG_END);
	short pressedKey = 0;
	for(; pressedKey < SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT && (EventThread::gcState.axes[pressedKey] <= -32768 || std::abs(EventThread::gcState.axes[pressedKey]) < deadzone); pressedKey++);
	return pressedKey == SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT ? Qnil : rb_fix_new(pressedKey);
}

static VALUE getGamepadAxisFromName(int argc, VALUE *argv, VALUE self) {
	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	return rb_fix_new(SDL_GetGamepadAxisFromString(name));
}

static VALUE setBinding(VALUE self, VALUE rb_arr, VALUE rb_target){
    Check_Type(rb_arr, T_ARRAY);
    int target = FIX2INT(rb_target);
    long count = RARRAY_LEN(rb_arr);
    std::vector<SourceDesc> result;
    result.reserve(count);

    for (size_t i = 0; i < count; ++i){
        VALUE rb_binding = rb_ary_entry(rb_arr, i);
		if (!rb_typeddata_is_kind_of(rb_binding, &sourceDesc_type))
    		continue;
    		
        SourceDesc* src;
        TypedData_Get_Struct(rb_binding, SourceDesc, &sourceDesc_type, src);
        result.push_back(*src);
    }

	shState->input().setBinding(result, static_cast<Input::ButtonCode>(target));
    return Qnil;
}

static VALUE setLED(VALUE self, VALUE r, VALUE g, VALUE b) {
    if (gc != nullptr) {
        if (!SDL_SetGamepadLED(gc, NUM2INT(r), NUM2INT(g), NUM2INT(b))) {
			Debug() << "failed to set gamepad led for gamepad " << SDL_GetGamepadName(gc) << ": " << SDL_GetError();
		}
    }

    return Qnil;
}

static VALUE rumble(VALUE self, VALUE low_frequency_rumble, VALUE high_frequency_rumble, VALUE duration_ms) {
    Uint16 low_freq = NUM2DBL(low_frequency_rumble) * 65535;
    Uint16 high_freq = NUM2DBL(high_frequency_rumble) * 65535;
    if (gc != nullptr) {
        if (!SDL_RumbleGamepad(gc, low_freq, high_freq, NUM2INT(duration_ms))) {
			Debug() << "failed to rumble gamepad " << SDL_GetGamepadName(gc) << ": " << SDL_GetError();
		}
    }

    return Qnil;
}

struct{
	const char *str;
	Input::ButtonCode val;
}
static buttonCodes[] = {
	{ "NONE",        Input::None        },
	{ "DOWN",        Input::Down        },
	{ "LEFT",        Input::Left        },
	{ "RIGHT",       Input::Right       },
	{ "UP",          Input::Up          },
	{ "ACTION",      Input::Action      },
	{ "CANCEL",      Input::Cancel      },
	{ "MENU",        Input::Menu        },
	{ "ITEMS",       Input::Items       },
	{ "RUN",         Input::Run         },
	{ "DEACTIVATE",  Input::Deactivate  },
	{ "DEBUGACTION", Input::DebugAction },
	{ "L",           Input::L           },
	{ "R",           Input::R           },
	{ "F5",          Input::F5          },
	{ "F6",          Input::F6          },
	{ "F7",          Input::F7          },
	{ "F8",          Input::F8          },
	{ "F9",          Input::F9          },
	{ "MOUSELEFT",   Input::MouseLeft   },
	{ "MOUSEMIDDLE", Input::MouseMiddle },
	{ "MOUSERIGHT",  Input::MouseRight  },
};

static elementsN(buttonCodes);

void inputBindingInit(){
	VALUE module = rb_define_module("Input");
	initGamepadBinding(module);

	// mkxp's input
	rb_define_module_function(module, "update", RUBY_METHOD_FUNC(inputUpdate), 0);
	rb_define_module_function(module, "mouse_enabled=", RUBY_METHOD_FUNC(inputSetMouseEnabled), -1);
	rb_define_module_function(module, "mouse_enabled?", RUBY_METHOD_FUNC(inputGetMouseEnabled), 0);
	rb_define_module_function(module, "gamepad_enabled=", RUBY_METHOD_FUNC(inputSetGamepadEnabled), -1);
	rb_define_module_function(module, "gamepad_enabled?", RUBY_METHOD_FUNC(inputGetGamepadEnabled), 0);
	rb_define_module_function(module, "press?", RUBY_METHOD_FUNC(inputPress), -1);
	rb_define_module_function(module, "trigger?", RUBY_METHOD_FUNC(inputTrigger), -1);
	rb_define_module_function(module, "repeat?", RUBY_METHOD_FUNC(inputRepeat), -1);
	rb_define_module_function(module, "dir4", RUBY_METHOD_FUNC(inputDir4), 0);
	rb_define_module_function(module, "dir8", RUBY_METHOD_FUNC(inputDir8), 0);

	// keyboard keys
	rb_define_module_function(module, "key_name", RUBY_METHOD_FUNC(getKeyName), -1);
	rb_define_module_function(module, "key_press?", RUBY_METHOD_FUNC(keyPress), -1);
	rb_define_module_function(module, "pressed_key", RUBY_METHOD_FUNC(getPressedKey), 0);
	rb_define_module_function(module, "key_from_name", RUBY_METHOD_FUNC(getKeyFromName), -1);
	rb_const_set(module, rb_intern("KEYS_COUNT"), SDL_Scancode::SDL_SCANCODE_COUNT - 1);

	// gamepad buttons
	rb_define_module_function(module, "c_button_name", RUBY_METHOD_FUNC(getGamepadButtonName), -1);
	rb_define_module_function(module, "c_button_press?", RUBY_METHOD_FUNC(gamepadButtonPress), -1);
	rb_define_module_function(module, "pressed_c_button", RUBY_METHOD_FUNC(getPressedGamepadButton), 0);
	rb_define_module_function(module, "c_button_from_name", RUBY_METHOD_FUNC(getGamepadButtonFromName), -1);
	rb_const_set(module, rb_intern("GAMEPAD_BUTTONS_COUNT"), SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT - 1);

	// gamepad axes
	rb_define_module_function(module, "c_axis_name", RUBY_METHOD_FUNC(getGamepadAxisName), -1);
	rb_define_module_function(module, "c_axis_pressure", RUBY_METHOD_FUNC(getGamepadAxisPressure), -1);
	rb_define_module_function(module, "active_c_axis", RUBY_METHOD_FUNC(getActiveGamepadAxis), -1);
	rb_define_module_function(module, "c_axis_from_name", RUBY_METHOD_FUNC(getGamepadAxisFromName), -1);
	rb_const_set(module, rb_intern("GAMEPAD_AXIS_COUNT"), SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT - 1);
	rb_define_module_function(module, "set_binding", RUBY_METHOD_FUNC(setBinding), 2);
	
	// haptic
	rb_define_singleton_method(module, "set_led", RUBY_METHOD_FUNC(setLED), 3);
    rb_define_singleton_method(module, "vibrate", RUBY_METHOD_FUNC(rumble), 3);

	// mouse
	rb_define_module_function(module, "mouse_x", RUBY_METHOD_FUNC(inputMouseX), 0);
	rb_define_module_function(module, "mouse_y", RUBY_METHOD_FUNC(inputMouseY), 0);

	// wheel support :P
	rb_define_module_function(module, "wheel_x", RUBY_METHOD_FUNC(inputWheelX), 0);
	rb_define_module_function(module, "wheel_y", RUBY_METHOD_FUNC(inputWheelY), 0);
	rb_define_module_function(module, "wheel_flipped", RUBY_METHOD_FUNC(inputWheelFlipped), 0);
	rb_define_module_function(module, "quit?", RUBY_METHOD_FUNC(inputQuit), 0);
	for (size_t i = 0; i < buttonCodesN; ++i){
		ID sym = rb_intern(buttonCodes[i].str);
		VALUE val = INT2FIX(buttonCodes[i].val);
		rb_const_set(module, sym, val);
	}
}
