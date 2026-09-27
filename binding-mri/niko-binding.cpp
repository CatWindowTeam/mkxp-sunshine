#include <ruby.h>

//TOOD: rewrite it

static VALUE nikoPrepare(VALUE self){ return Qnil; }
static VALUE nikoStart(VALUE self){ return Qnil; }

void nikoBindingInit(){
	VALUE module = rb_define_module("Niko");
	rb_define_module_function(module, "get_ready", RUBY_METHOD_FUNC(nikoPrepare), -1);
	rb_define_module_function(module, "do_your_thing", RUBY_METHOD_FUNC(nikoStart), -1);
}
