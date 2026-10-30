#include "debugwriter.h"
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/common.h>

namespace {

  static mrb_value mrb_puts(mrb_state* mrb, mrb_value) {
    const char* str;
    mrb_get_args(mrb, "z", &str);
    Debug() << str;
    return mrb_nil_value();
  }

}

void mkxpBindingInit(mrb_state *mrb) {
  auto mkxp = mrb_define_module(mrb, "MKXP");
  mrb_define_module_function(mrb, mkxp, "puts", mrb_puts, MRB_ARGS_REQ(1));
}