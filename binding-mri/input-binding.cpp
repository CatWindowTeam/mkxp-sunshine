/*
** input-binding.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is SDL_free software: you can redistribute it and/or modify
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
#include "exception.h"
#include "binding-util.h"
#include "util.h"
#include "eventthread.h"

#include "keybindings-binding.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_gamepad.h>
#include <cstdio>
#include <vector>

RB_METHOD(inputUpdate){
	RB_UNUSED_PARAM;

	shState->input().update();

	return Qnil;
}

static int getButtonArg(int argc, VALUE *argv){
	int num;

	rb_check_argc(argc, 1);

	if (FIXNUM_P(argv[0])){
		num = FIX2INT(argv[0]);
	}
	//else if (SYMBOL_P(argv[0]) && rgssVer >= 3){
	//	VALUE symHash = getRbData()->buttoncodeHash;
	//	num = FIX2INT(rb_hash_lookup2(symHash, argv[0], INT2FIX(Input::None)));
	//}
	else{
		// FIXME: RMXP allows only few more types that
		// don't make sense (symbols in pre 3, floats)
		num = 0;
	}

	return num;
}

RB_METHOD(inputPress){
	RB_UNUSED_PARAM;
	int num = getButtonArg(argc, argv);
	return rb_bool_new(shState->input().isPressed(num));
}

RB_METHOD(inputTrigger){
	RB_UNUSED_PARAM;
	int num = getButtonArg(argc, argv);
	return rb_bool_new(shState->input().isTriggered(num));
}

RB_METHOD(inputRepeat){
	RB_UNUSED_PARAM;
	int num = getButtonArg(argc, argv);
	return rb_bool_new(shState->input().isRepeated(num));
}

RB_METHOD(inputDir4){
	RB_UNUSED_PARAM;
	return rb_fix_new(shState->input().dir4Value());
}

RB_METHOD(inputDir8){
	RB_UNUSED_PARAM;
	return rb_fix_new(shState->input().dir8Value());
}

/* Non-standard extensions */
RB_METHOD(inputMouseX){
	RB_UNUSED_PARAM;
	return rb_fix_new(shState->input().mouseX());
}

RB_METHOD(inputMouseY){
	RB_UNUSED_PARAM;
	return rb_fix_new(shState->input().mouseY());
}

// wheel :3
RB_METHOD(inputWheelX) {
	RB_UNUSED_PARAM;
	return rb_float_new(shState->input().wheelX());
}

RB_METHOD(inputWheelY) {
	RB_UNUSED_PARAM;
	return rb_float_new(shState->input().wheelY());
}

RB_METHOD(inputWheelFlipped) {
	RB_UNUSED_PARAM;
	return rb_bool_new(shState->input().wheelFlipped());
}

RB_METHOD(inputQuit) {
	RB_UNUSED_PARAM;
	return rb_bool_new(shState->input().hasQuit());
}

// keyboard
RB_METHOD(getKeyName) {
	RB_UNUSED_PARAM;

	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);

	if (key >= 0 && key < SDL_Scancode::SDL_SCANCODE_COUNT)
		return rb_utf8_str_new_cstr(SDL_GetKeyName(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(key), SDL_KMOD_NONE, false)));

	return rb_utf8_str_new_cstr(SDL_GetKeyName(SDLK_UNKNOWN));
}

RB_METHOD(keyPress) {
	RB_UNUSED_PARAM;

	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);

	if (key >= 0 && key < SDL_Scancode::SDL_SCANCODE_COUNT)
		return rb_bool_new(EventThread::keyStates[key]);
	
	return rb_bool_new(false);
}

RB_METHOD(getPressedKey) {
	RB_UNUSED_PARAM;
	
	short pressedKey = 0;
	for(; pressedKey < SDL_Scancode::SDL_SCANCODE_COUNT && !EventThread::keyStates[pressedKey]; pressedKey++);
	return pressedKey == SDL_Scancode::SDL_SCANCODE_COUNT ? Qnil : rb_fix_new(pressedKey);
}

RB_METHOD(getKeyFromName) {
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	return rb_fix_new(SDL_GetScancodeFromName(name));
}

// gamepad buttons
RB_METHOD(getGamepadButtonName) {
	RB_UNUSED_PARAM;

	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);

	if (key >= 0 && key < SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT)
		return rb_utf8_str_new_cstr(SDL_GetGamepadStringForButton(static_cast<SDL_GamepadButton>(key)));

	return rb_utf8_str_new_cstr("Invalid");
}

RB_METHOD(gamepadButtonPress) {
	RB_UNUSED_PARAM;

	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);

	if (key >= 0 && key < SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT)
		return rb_bool_new(EventThread::gcState.buttons[key]);
	
	return rb_bool_new(false);
}

RB_METHOD(getPressedGamepadButton) {
	RB_UNUSED_PARAM;

	short pressedKey = 0;
	for(; pressedKey < SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT && !EventThread::gcState.buttons[pressedKey]; pressedKey++);
	return pressedKey == SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT ? Qnil : rb_fix_new(pressedKey);
}

RB_METHOD(getGamepadButtonFromName) {
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	return rb_fix_new(SDL_GetGamepadButtonFromString(name));
}

// gamepad axes
RB_METHOD(getGamepadAxisName) {
	RB_UNUSED_PARAM;

	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);

	if (key >= 0 && key < SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT)
		return rb_utf8_str_new_cstr(SDL_GetGamepadStringForAxis(static_cast<SDL_GamepadAxis>(key)));

	return rb_utf8_str_new_cstr("Invalid");
}

RB_METHOD(getGamepadAxisPressure) {
	RB_UNUSED_PARAM;

	int key = 0;
	rb_get_args(argc, argv, "i", &key RB_ARG_END);

	if (key >= 0 && key < SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT)
		return rb_fix_new(EventThread::gcState.axes[key]);
	
	return rb_fix_new(0);
}

RB_METHOD(getActiveGamepadAxis) {
	RB_UNUSED_PARAM;

	int deadzone = 16000;
	rb_get_args(argc, argv, "|i", &deadzone RB_ARG_END);

	short pressedKey = 0;
	for(; pressedKey < SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT && (EventThread::gcState.axes[pressedKey] <= -32768 || std::abs(EventThread::gcState.axes[pressedKey]) < deadzone); pressedKey++);
	return pressedKey == SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT ? Qnil : rb_fix_new(pressedKey);
}

RB_METHOD(getGamepadAxisFromName) {
	RB_UNUSED_PARAM;

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

    for (size_t i = 0; i < count; ++i)
    {
        VALUE rb_binding = rb_ary_entry(rb_arr, i);

		//printf("bind\n");
		if (!rb_typeddata_is_kind_of(rb_binding, &sourceDesc_type))
    		continue;
		//printf("bind ok\n");

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
			printf("failed to set gamepad led for gamepad %s: %s\n", SDL_GetGamepadName(gc), SDL_GetError());
		}
    }

    return Qnil;
}

static VALUE rumble(VALUE self, VALUE low_frequency_rumble, VALUE high_frequency_rumble, VALUE duration_ms) {
    Uint16 low_freq = NUM2DBL(low_frequency_rumble) * 65535;
    Uint16 high_freq = NUM2DBL(high_frequency_rumble) * 65535;
    
    if (gc != nullptr) {
		printf("low freq: %d, high freq: %d, duration: %d", low_freq, high_freq, NUM2INT(duration_ms));
        if (!SDL_RumbleGamepad(gc, low_freq, high_freq, NUM2INT(duration_ms))) {
			printf("failed to rumble gamepad %s: %s\n", SDL_GetGamepadName(gc), SDL_GetError());
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
	printf("[inputBindingInit] Initializing Input binding\n");
	VALUE module = rb_define_module("Input");

	// mkxp's input
	_rb_define_module_function(module, "update", inputUpdate);
	_rb_define_module_function(module, "press?", inputPress);
	_rb_define_module_function(module, "trigger?", inputTrigger);
	_rb_define_module_function(module, "repeat?", inputRepeat);
	_rb_define_module_function(module, "dir4", inputDir4);
	_rb_define_module_function(module, "dir8", inputDir8);

	// keyboard keys
	_rb_define_module_function(module, "key_name", getKeyName);
	_rb_define_module_function(module, "key_press?", keyPress);
	_rb_define_module_function(module, "pressed_key", getPressedKey);
	_rb_define_module_function(module, "key_from_name", getKeyFromName);
	rb_const_set(module, rb_intern("KEYS_COUNT"), SDL_Scancode::SDL_SCANCODE_COUNT - 1);

	// gamepad buttons
	_rb_define_module_function(module, "c_button_name", getGamepadButtonName);
	_rb_define_module_function(module, "c_button_press?", gamepadButtonPress);
	_rb_define_module_function(module, "pressed_c_button", getPressedGamepadButton);
	_rb_define_module_function(module, "c_button_from_name", getGamepadButtonFromName);
	rb_const_set(module, rb_intern("GAMEPAD_BUTTONS_COUNT"), SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT - 1);

	// gamepad axes
	_rb_define_module_function(module, "c_axis_name", getGamepadAxisName);
	_rb_define_module_function(module, "c_axis_pressure", getGamepadAxisPressure);
	_rb_define_module_function(module, "active_c_axis", getActiveGamepadAxis);
	_rb_define_module_function(module, "c_axis_from_name", getGamepadAxisFromName);
	rb_const_set(module, rb_intern("GAMEPAD_AXIS_COUNT"), SDL_GamepadAxis::SDL_GAMEPAD_AXIS_COUNT - 1);

	rb_define_module_function(module, "set_binding", setBinding, 2);
	
	// haptic
	rb_define_singleton_method(module, "set_led", setLED, 3);
    rb_define_singleton_method(module, "vibrate", rumble, 3);

	// mouse
	_rb_define_module_function(module, "mouse_x", inputMouseX);
	_rb_define_module_function(module, "mouse_y", inputMouseY);

	// wheel support :P
	_rb_define_module_function(module, "wheel_x", inputWheelX);
	_rb_define_module_function(module, "wheel_y", inputWheelY);
	_rb_define_module_function(module, "wheel_flipped", inputWheelFlipped);

	_rb_define_module_function(module, "quit?", inputQuit);

	for (size_t i = 0; i < buttonCodesN; ++i){
		ID sym = rb_intern(buttonCodes[i].str);
		VALUE val = INT2FIX(buttonCodes[i].val);

		rb_const_set(module, sym, val);
	}
}
