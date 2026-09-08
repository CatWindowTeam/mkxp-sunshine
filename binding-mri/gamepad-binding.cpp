#include <ruby.h>
#include <SDL3/SDL_gamepad.h>

// TODO: implement that in the engine itself, not bindings.
VALUE rb_GetGamepadType(VALUE){
    SDL_Gamepad *gamepad = SDL_GetGamepadFromPlayerIndex(0);

    if (gamepad == NULL)
        return INT2NUM(SDL_GAMEPAD_TYPE_UNKNOWN);
    
    return INT2NUM(SDL_GetGamepadType(gamepad));
}

VALUE rb_GetGamepadID(VALUE){
    SDL_Gamepad *gamepad = SDL_GetGamepadFromPlayerIndex(0);

    if (gamepad == NULL)
        return Qnil;

    return UINT2NUM(SDL_GetGamepadID(gamepad));
}

VALUE rb_GetGamepadGUID(VALUE){
    SDL_Gamepad *gamepad = SDL_GetGamepadFromPlayerIndex(0);

    if (gamepad == NULL)
        return Qnil;
    
    SDL_GUID guid = SDL_GetGamepadGUIDForID(SDL_GetGamepadID(gamepad));

    char buffer[33];
    SDL_GUIDToString(guid, buffer, sizeof(buffer));

    return rb_str_new_cstr(buffer);
}

void initGamepadBinding(VALUE inputModule){
    VALUE gamepadModule = rb_define_module_under(inputModule, "GamepadType");

    rb_define_module_function(gamepadModule, "current_type", RUBY_METHOD_FUNC(rb_GetGamepadType), 0);
    rb_define_module_function(gamepadModule, "current_id", RUBY_METHOD_FUNC(rb_GetGamepadID), 0);
    rb_define_module_function(gamepadModule, "current_guid", RUBY_METHOD_FUNC(rb_GetGamepadGUID), 0);

    rb_const_set(gamepadModule, rb_intern("UNKNOWN"), INT2FIX(SDL_GAMEPAD_TYPE_UNKNOWN));
    rb_const_set(gamepadModule, rb_intern("STANDART"), INT2FIX(SDL_GAMEPAD_TYPE_STANDARD));
    rb_const_set(gamepadModule, rb_intern("XBOX360"), INT2FIX(SDL_GAMEPAD_TYPE_XBOX360));
    rb_const_set(gamepadModule, rb_intern("XBOXONE"), INT2FIX(SDL_GAMEPAD_TYPE_XBOXONE));
    rb_const_set(gamepadModule, rb_intern("PS3"), INT2FIX(SDL_GAMEPAD_TYPE_PS3));
    rb_const_set(gamepadModule, rb_intern("PS4"), INT2FIX(SDL_GAMEPAD_TYPE_PS4));
    rb_const_set(gamepadModule, rb_intern("PS5"), INT2FIX(SDL_GAMEPAD_TYPE_PS5));
    rb_const_set(gamepadModule, rb_intern("SWITCH_PRO"), INT2FIX(SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO));
    rb_const_set(gamepadModule, rb_intern("JOYCON_LEFT"), INT2FIX(SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT));
    rb_const_set(gamepadModule, rb_intern("JOYCON_RIGHT"), INT2FIX(SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT));
    rb_const_set(gamepadModule, rb_intern("JOYCON_PAIR"), INT2FIX(SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR));
    rb_const_set(gamepadModule, rb_intern("GAMECUBE"), INT2FIX(SDL_GAMEPAD_TYPE_GAMECUBE));

    VALUE gamepadButtonsModule = rb_define_module_under(inputModule, "GamepadButton");
    
    rb_const_set(gamepadButtonsModule, rb_intern("INVALID"), INT2FIX(SDL_GAMEPAD_BUTTON_INVALID));
    rb_const_set(gamepadButtonsModule, rb_intern("SOUTH"), INT2FIX(SDL_GAMEPAD_BUTTON_SOUTH));
    rb_const_set(gamepadButtonsModule, rb_intern("EAST"), INT2FIX(SDL_GAMEPAD_BUTTON_EAST));
    rb_const_set(gamepadButtonsModule, rb_intern("WEST"), INT2FIX(SDL_GAMEPAD_BUTTON_WEST));
    rb_const_set(gamepadButtonsModule, rb_intern("NORTH"), INT2FIX(SDL_GAMEPAD_BUTTON_NORTH));
    rb_const_set(gamepadButtonsModule, rb_intern("BACK"), INT2FIX(SDL_GAMEPAD_BUTTON_BACK));
    rb_const_set(gamepadButtonsModule, rb_intern("GUIDE"), INT2FIX(SDL_GAMEPAD_BUTTON_GUIDE));
    rb_const_set(gamepadButtonsModule, rb_intern("START"), INT2FIX(SDL_GAMEPAD_BUTTON_START));
    rb_const_set(gamepadButtonsModule, rb_intern("LEFT_STICK"), INT2FIX(SDL_GAMEPAD_BUTTON_LEFT_STICK));
    rb_const_set(gamepadButtonsModule, rb_intern("RIGHT_STICK"), INT2FIX(SDL_GAMEPAD_BUTTON_RIGHT_STICK));
    rb_const_set(gamepadButtonsModule, rb_intern("LEFT_SHOULDER"), INT2FIX(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER));
    rb_const_set(gamepadButtonsModule, rb_intern("RIGHT_SHOULDER"), INT2FIX(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER));
    rb_const_set(gamepadButtonsModule, rb_intern("DPAD_UP"), INT2FIX(SDL_GAMEPAD_BUTTON_DPAD_UP));
    rb_const_set(gamepadButtonsModule, rb_intern("DPAD_DOWN"), INT2FIX(SDL_GAMEPAD_BUTTON_DPAD_DOWN));
    rb_const_set(gamepadButtonsModule, rb_intern("DPAD_LEFT"), INT2FIX(SDL_GAMEPAD_BUTTON_DPAD_LEFT));
    rb_const_set(gamepadButtonsModule, rb_intern("DPAD_RIGHT"), INT2FIX(SDL_GAMEPAD_BUTTON_DPAD_RIGHT));
    rb_const_set(gamepadButtonsModule, rb_intern("MISC1"), INT2FIX(SDL_GAMEPAD_BUTTON_MISC1));
    rb_const_set(gamepadButtonsModule, rb_intern("RIGHT_PADDLE1"), INT2FIX(SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1));
    rb_const_set(gamepadButtonsModule, rb_intern("LEFT_PADDLE1"), INT2FIX(SDL_GAMEPAD_BUTTON_LEFT_PADDLE1));
    rb_const_set(gamepadButtonsModule, rb_intern("RIGHT_PADDLE2"), INT2FIX(SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2));
    rb_const_set(gamepadButtonsModule, rb_intern("LEFT_PADDLE2"), INT2FIX(SDL_GAMEPAD_BUTTON_LEFT_PADDLE2));
    rb_const_set(gamepadButtonsModule, rb_intern("TOUCHPAD"), INT2FIX(SDL_GAMEPAD_BUTTON_TOUCHPAD));
    rb_const_set(gamepadButtonsModule, rb_intern("MISC2"), INT2FIX(SDL_GAMEPAD_BUTTON_MISC2));
    rb_const_set(gamepadButtonsModule, rb_intern("MISC3"), INT2FIX(SDL_GAMEPAD_BUTTON_MISC3));
    rb_const_set(gamepadButtonsModule, rb_intern("MISC4"), INT2FIX(SDL_GAMEPAD_BUTTON_MISC4));
    rb_const_set(gamepadButtonsModule, rb_intern("MISC5"), INT2FIX(SDL_GAMEPAD_BUTTON_MISC5));
    rb_const_set(gamepadButtonsModule, rb_intern("MISC6"), INT2FIX(SDL_GAMEPAD_BUTTON_MISC6));

    VALUE gamepadAxisModule = rb_define_module_under(inputModule, "GamepadAxis");
    
    rb_const_set(gamepadAxisModule, rb_intern("INVALID"), INT2FIX(SDL_GAMEPAD_AXIS_INVALID));
    rb_const_set(gamepadAxisModule, rb_intern("LEFTX"), INT2FIX(SDL_GAMEPAD_AXIS_LEFTX));
    rb_const_set(gamepadAxisModule, rb_intern("LEFTY"), INT2FIX(SDL_GAMEPAD_AXIS_LEFTY));
    rb_const_set(gamepadAxisModule, rb_intern("RIGHTX"), INT2FIX(SDL_GAMEPAD_AXIS_RIGHTX));
    rb_const_set(gamepadAxisModule, rb_intern("RIGHTY"), INT2FIX(SDL_GAMEPAD_AXIS_RIGHTY));
    rb_const_set(gamepadAxisModule, rb_intern("LEFT_TRIGGER"), INT2FIX(SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
    rb_const_set(gamepadAxisModule, rb_intern("RIGHT_TRIGGER"), INT2FIX(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));
}