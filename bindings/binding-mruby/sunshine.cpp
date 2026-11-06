#include <mruby.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/irep.h>
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/dump.h>
#include <SDL3/SDL_revision.h>
#include "config.h"
#include "sunshine.h"
#include "sharedstate.h"
#include "meow.h"
#include "define.h"

static mrb_value SetCrashScreenData(mrb_state *mrb, mrb_value self){
	const char* data;
	mrb_get_args(mrb, "z", &data);
	SDL_snprintf(crash_message, sizeof(crash_message), "%s", data);
	show_crash_screen = true;
	return mrb_nil_value();
}

void SunshineBindingInit(mrb_state *mrb){
    struct RClass *m = mrb_define_module(mrb, "Sunshine");
    mrb_define_const(mrb, m, "SDL", mrb_str_new_lit(mrb, SDL_REVISION));
	mrb_define_const(mrb, m, "VERSION", mrb_str_new_lit(mrb, VERSION_STRING));
	mrb_define_const(mrb, m, "RENDERER", mrb_str_new_cstr(mrb, conf.renderer.c_str()));
	mrb_define_const(mrb, m, "RENDER_API", mrb_str_new_cstr(mrb, shState->render().apiName()));
	mrb_define_module_function(mrb, m, "SetCrashScreenData", SetCrashScreenData, MRB_ARGS_REQ(1));

	//detect unsupported enviroment/platform/OS	
	#ifdef defined(vita) || \
			defined(psp) || \
			defined(haiku) || \
			defined(web) || \
			defined(unix_like) || \
			defined(android) || \
			defined(TERMUX)
		mrb_define_const(mrb, m, "UNSUPPORTED_ENVIROMENT", mrb_true_value());
	#else
		mrb_define_const(mrb, m, "UNSUPPORTED_ENVIROMENT", mrb_false_value());
	#endif
}
