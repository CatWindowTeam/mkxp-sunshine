#ifdef STEAM
	#include "steam.h"
#endif
#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "debugwriter.h"

static VALUE steamEnabled(VALUE self){
	#ifdef STEAM
		return Qtrue;
	#else
		return Qfalse;
	#endif
}

static VALUE steamUnlock(int argc, VALUE *argv, VALUE self){
	#ifdef STEAM
		const char *name;
		rb_get_args(argc, argv, "z", &name RB_ARG_END);
		shState->steam().unlock(name);
	#endif
	return Qnil;
}

static VALUE steamLock(int argc, VALUE *argv, VALUE self){
	#ifdef STEAM
		const char *name;
		rb_get_args(argc, argv, "z", &name RB_ARG_END);
		shState->steam().lock(name);
	#endif
	return Qnil;
}

static VALUE steamUnlocked(int argc, VALUE *argv, VALUE self){
	#ifdef STEAM
		const char *name;
		rb_get_args(argc, argv, "z", &name RB_ARG_END);
		return shState->steam().isUnlocked(name) ? Qtrue : Qfalse;
	#else
		return Qfalse;
	#endif
}

void steamBindingInit(){
    VALUE module = rb_define_module("Steam");

	/* Constants */
	#ifdef STEAM
		rb_const_set(module, rb_intern("USER_NAME"), rb_str_new2(shState->steam().userName().c_str()));
		if(shState->steam().lang().empty())
			rb_const_set(module, rb_intern("LANG"), Qnil);
		else
			rb_const_set(module, rb_intern("LANG"), rb_str_new2(shState->steam().lang().c_str()));
	#else
		rb_const_set(module, rb_intern("USER_NAME"), Qnil);
		rb_const_set(module, rb_intern("LANG"), Qnil);
	#endif

	/* Functions */
	rb_define_module_function(module, "enabled?", RUBY_METHOD_FUNC(steamEnabled), 0);
    rb_define_module_function(module, "unlock", RUBY_METHOD_FUNC(steamUnlock), -1);
	rb_define_module_function(module, "lock", RUBY_METHOD_FUNC(steamLock), -1);
	rb_define_module_function(module, "unlocked?", RUBY_METHOD_FUNC(steamUnlocked), -1);
}
