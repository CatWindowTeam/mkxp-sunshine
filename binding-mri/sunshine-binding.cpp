#include <ruby.h>
#include <SDL3/SDL_version.h>

static VALUE string_clone(VALUE self){
    if (!RB_TYPE_P(self, T_STRING)) {
        rb_raise(rb_eTypeError, "wrong argument type (expected String)");
    }
    return rb_obj_clone(self);
}

void SunshineBindingInit(){
    printf("[SunshineBindingInit] Initializing Sunshine binding\n");
    VALUE module = rb_define_module("Sunshine");
    rb_const_set(module, rb_intern("SDLVersion_major"), INT2NUM(SDL_MAJOR_VERSION));
    rb_const_set(module, rb_intern("SDLVersion_minor"), INT2NUM(SDL_MINOR_VERSION));
    rb_const_set(module, rb_intern("SDLVersion_micro"), INT2NUM(SDL_MICRO_VERSION));
    printf("[SunshineBindingInit] SDL version: %i.%i.%i\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	if (!rb_respond_to(rb_cObject, rb_intern("class"))) {
	        rb_define_method(rb_cObject, "class", rb_obj_class, 0);
	}
	if (!rb_respond_to(rb_cString, rb_intern("clone"))) {
	        rb_define_method(rb_cString, "clone", RUBY_METHOD_FUNC(string_clone), 0);
	}
}
