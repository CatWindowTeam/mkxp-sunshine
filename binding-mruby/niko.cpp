#include <mruby.h>
//TODO: rewrite it

static mrb_value nikoPrepare(mrb_state *mrb, mrb_value self){ return mrb_nil_value(); }
static mrb_value nikoStart(mrb_state *mrb, mrb_value self){ return mrb_nil_value(); }

void nikoBindingInit(mrb_state *mrb){
	struct RClass *m = mrb_define_module(mrb, "niko");
	mrb_define_module_function(mrb, m, "get_ready", nikoPrepare, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, m, "do_your_thing", nikoStart, MRB_ARGS_NONE());
}
