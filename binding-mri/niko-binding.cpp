#include <ruby.h>
#include "binding-util.h"
#include "binding-types.h"


RB_METHOD(nikoPrepare){
	RB_UNUSED_PARAM;
	return Qnil;
}

RB_METHOD(nikoStart){
	RB_UNUSED_PARAM;
	return Qnil;
}

void nikoBindingInit(){
	VALUE module = rb_define_module("Niko");
	_rb_define_module_function(module, "get_ready", nikoPrepare);
	_rb_define_module_function(module, "do_your_thing", nikoStart);
}
