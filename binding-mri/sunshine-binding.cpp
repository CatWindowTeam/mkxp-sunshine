#include <ruby.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_locale.h>
#include "security.h"
#include "eventthread.h"
#include "sunshine.h"
#include "meow.h"
#include "sharedstate.h"

static VALUE sunshineSetCrashPrivacy(VALUE, VALUE v) {
  is_privacy_crashdump_enabled = RTEST(v);
  return v;
}

static VALUE sunshineSetWallpaperMode(VALUE, VALUE v) {
  std::string s(StringValueCStr(v));
  shState->config().wallpaperMode = s;
  return v;
}

static VALUE sunshineSetHint(VALUE, VALUE h, VALUE v) {
  return SDL_SetHint(StringValueCStr(h), StringValueCStr(v));
}

static VALUE obj_clone(VALUE self){
    return rb_obj_clone(self);
}

static VALUE a_last(VALUE self){
    return rb_ary_entry(self, -1);
}

static VALUE int_times(VALUE self) {
    long n = NUM2LONG(self);

    if (!rb_block_given_p()) {
        ID id_to_enum = rb_intern("to_enum");
        VALUE sym = ID2SYM(rb_intern("times"));
        return rb_funcall(self, id_to_enum, 1, sym);
    }

    if (n <= 0) return self;

    for (long i = 0; i < n; ++i)
        rb_yield(LONG2NUM(i));

    return self;
}

void SunshineBindingInit(){
    VALUE module = rb_define_module("Sunshine");
	//SDL versions, for debug info in main menu and other shi idkkkkkkkk
    rb_const_set(module, rb_intern("SDLVersion_major"), INT2NUM(SDL_MAJOR_VERSION));
    rb_const_set(module, rb_intern("SDLVersion_minor"), INT2NUM(SDL_MINOR_VERSION));
    rb_const_set(module, rb_intern("SDLVersion_micro"), INT2NUM(SDL_MICRO_VERSION));
	rb_const_set(module, rb_intern("SECURITYSTATE"), rb_str_new_cstr(securitystate));
	rb_const_set(module, rb_intern("VERSION"), rb_str_new_cstr(VERSION_STRING));
	rb_const_set(module, rb_intern("WARNS"), rb_str_new_cstr(warns.c_str()));
	#ifdef DEVBUILD
		rb_const_set(module, rb_intern("DEVBUILD"), Qtrue);
	#else
		rb_const_set(module, rb_intern("DEVBUILD"), Qfalse);
	#endif
	rb_define_singleton_method(module, "crash_privacy=", RUBY_METHOD_FUNC(sunshineSetCrashPrivacy), 1);
	rb_define_singleton_method(module, "wallpaper_mode=", RUBY_METHOD_FUNC(sunshineSetWallpaperMode), 1);
	rb_define_singleton_method(module, "set_sdl_hint", RUBY_METHOD_FUNC(sunshineSetHint), 2);
	if (!rb_respond_to(rb_cObject, rb_intern("class")))
	    rb_define_method(rb_cObject, "class", rb_obj_class, 0);

	if (!rb_respond_to(rb_cObject, rb_intern("clone")))
	    rb_define_method(rb_cObject, "clone", RUBY_METHOD_FUNC(obj_clone), 0);

	if (!rb_respond_to(rb_cInteger, rb_intern("times")))
	    rb_define_method(rb_cInteger, "times", RUBY_METHOD_FUNC(int_times), 0);
	SDL_Locale** Locale = SDL_GetPreferredLocales(NULL);
	if(Locale == nullptr){
		Debug() << "Failed to detect prefered Locale, using english";
		rb_const_set(module, rb_intern("P_LOCALE"), rb_str_new_cstr("en"));
	}else{
		rb_const_set(module, rb_intern("P_LOCALE"), rb_str_new_cstr(Locale[0]->language));
	}
}
