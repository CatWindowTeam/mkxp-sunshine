#include <mruby.h>
#include <mruby/value.h>
#include <mruby/common.h>

namespace {

  // placeholder
  static mrb_value do_nothing(mrb_state* mrb, mrb_value) {
    return mrb_nil_value();
  }

}

void oneshotBindingInit(mrb_state *mrb) {
  auto oneshot = mrb_define_module(mrb, "Oneshot");

  mrb_define_class_method(mrb, oneshot, "exiting", do_nothing, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, oneshot, "allow_exit", do_nothing, MRB_ARGS_OPT(1));
}