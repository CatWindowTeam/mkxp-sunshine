#include <mruby.h>
#include <SDL3/SDL_gamepad.h>

//TODO

static mrb_value rb_GetGamepadType(mrb_state *mrb, mrb_value self){
    SDL_Gamepad *gamepad = SDL_GetGamepadFromPlayerIndex(0);
    if(gamepad == nullptr)
        return mrb_fixnum_value((mrb_int)SDL_GAMEPAD_TYPE_UNKNOWN);
    
    return mrb_fixnum_value((mrb_int)SDL_GetGamepadType(gamepad));
}

static mrb_value rb_GetGamepadID(mrb_state *mrb, mrb_value self){
    SDL_Gamepad *gamepad = SDL_GetGamepadFromPlayerIndex(0);
    if (gamepad == nullptr)
        return mrb_nil_value();

    return mrb_fixnum_value((mrb_int)SDL_GetGamepadID(gamepad));
}

static mrb_value rb_GetGamepadGUID(mrb_state *mrb, mrb_value self){
    SDL_Gamepad *gamepad = SDL_GetGamepadFromPlayerIndex(0);
    if(gamepad == nullptr)
        return mrb_nil_value();
    
    SDL_GUID guid = SDL_GetGamepadGUIDForID(SDL_GetGamepadID(gamepad));

    char buffer[33];
    SDL_GUIDToString(guid, buffer, sizeof(buffer));
    return mrb_str_new_cstr(mrb, buffer);
}
