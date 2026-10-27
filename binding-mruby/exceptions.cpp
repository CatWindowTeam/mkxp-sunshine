#include <mruby.h>
#include <mruby/error.h>

void exceptions_binding(mrb_state *mrb){
  mrb_define_class(mrb, "Shutdown", mrb_class_get(mrb, "StandardError"));
}
