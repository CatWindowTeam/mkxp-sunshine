#include <mruby.h>
#include <mruby/value.h>
#include <mruby/common.h>

namespace {

  // placeholder
  static mrb_value do_nothing(mrb_state* mrb, mrb_value) {
    return mrb_nil_value();
  }

}

void wallpaperBindingInit(mrb_state *mrb) {
  auto oneshot = mrb_define_module(mrb, "Wallpaper");

  mrb_define_module_function(mrb, oneshot, "reset", do_nothing, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, oneshot, "set", do_nothing, MRB_ARGS_OPT(1));
}