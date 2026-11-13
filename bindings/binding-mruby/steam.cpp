#ifdef STEAM
	#include "steam.h"
#endif
#include "sharedstate.h"
#include <mruby.h>

static mrb_value steamEnabled(mrb_state *mrb, mrb_value self){
	#ifdef STEAM
		return mrb_true_value();
	#else
		return mrb_false_value();
	#endif
}

static mrb_value steamUnlock(mrb_state *mrb, mrb_value self){
	#ifdef STEAM
		const char *name;
		mrb_get_args(mrb, "z", &name);
		shState->steam().unlock(name);
	#endif
	return mrb_nil_value();
}

static mrb_value steamLock(mrb_state *mrb, mrb_value self){
	#ifdef STEAM
		const char *name;
		mrb_get_args(mrb, "z", &name);
		shState->steam().lock(name);
	#endif
	return mrb_nil_value();
}

static mrb_value steamUnlocked(mrb_state *mrb, mrb_value self){
	#ifdef STEAM
		const char *name;
		mrb_get_args(mrb, "z", &name);
		return shState->steam().isUnlocked(name) ? mrb_true_value() : mrb_false_value();
	#else
		return mrb_false_value();
	#endif
}

void steamBindingInit(mrb_state *mrb){
    struct RClass *module = mrb_define_module(mrb, "Steam");

	/* Constants */
	#ifdef STEAM
		mrb_define_const(mrb, module, "VERSION", mrb_str_new_lit(mrb, shState->steam().userName().c_str()));
		if(shState->steam().lang().empty())
			mrb_define_const(mrb, module, "LANG", mrb_nil_value());
		else
			mrb_define_const(mrb, module, "LANG", mrb_str_new_lit(mrb, shState->steam().lang().c_str()));
	#else
		mrb_define_const(mrb, module, "USER_NAME", mrb_nil_value());
		mrb_define_const(mrb, module, "LANG", mrb_nil_value());
	#endif

	/* Functions */	
	mrb_define_module_function(mrb, module, "enabled?", steamEnabled, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "unlock", steamUnlock, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "lock", steamLock, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "unlocked?", steamUnlocked, MRB_ARGS_REQ(1));
}
