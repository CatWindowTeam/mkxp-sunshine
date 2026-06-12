#include <ruby.h>
#include <SDL3/SDL_version.h>
#include <limits.h>
#include "security.h"

//Просто на C реализуем методы мне в падлу ебаться со статической линковкой и прочим дерьмом.
//Аминь.

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

    for (long i = 0; i < n; ++i) {
        rb_yield(LONG2NUM(i));
    }

    return self;
}

void SunshineBindingInit(){
    printf("[SunshineBindingInit] Initializing Sunshine binding\n");
    VALUE module = rb_define_module("Sunshine");
	//SDL versions, for debug info in main menu and other shi idkkkkkkkk
    rb_const_set(module, rb_intern("SDLVersion_major"), INT2NUM(SDL_MAJOR_VERSION));
    rb_const_set(module, rb_intern("SDLVersion_minor"), INT2NUM(SDL_MINOR_VERSION));
    rb_const_set(module, rb_intern("SDLVersion_micro"), INT2NUM(SDL_MICRO_VERSION));
	rb_const_set(module, rb_intern("SECURITYSTATE"), rb_str_new_cstr(securitystate));

    //если методы доступны то просто не перезаписываем их
	if (!rb_respond_to(rb_cObject, rb_intern("class"))) {
	        rb_define_method(rb_cObject, "class", rb_obj_class, 0);
	}
	//сразу для всех обьектов
	if (!rb_respond_to(rb_cObject, rb_intern("clone"))) {
	        rb_define_method(rb_cObject, "clone", RUBY_METHOD_FUNC(obj_clone), 0);
	}

	if (!rb_respond_to(rb_cInteger, rb_intern("times"))) {
	    rb_define_method(rb_cInteger, "times", RUBY_METHOD_FUNC(int_times), 0);
	}
	if (!rb_respond_to(rb_cArray, rb_intern("last"))) {
	    rb_define_method(rb_cArray, "last", RUBY_METHOD_FUNC(a_last), 0);
	}
}
