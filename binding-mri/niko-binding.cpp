//TODO: rewrite this shit and make crossplatform

#include "binding-util.h"
#include "binding-types.h"
#include "sharedstate.h"
#include "debugwriter.h"
#include "eventthread.h"
#include "define.h"
#include <SDL3/SDL.h>

int niko_server_thread(void *data){
	(void)data;
}

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

	//Functions
	_rb_define_module_function(module, "get_ready", nikoPrepare);
	_rb_define_module_function(module, "do_your_thing", nikoStart);
}
