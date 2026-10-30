#include <mruby.h>
#include <mruby/value.h>
#include <mruby/common.h>
#include "config.h"

namespace {

  // placeholder
  static mrb_value do_nothing(mrb_state* mrb, mrb_value) {
    return mrb_nil_value();
  }

}

void graphicsBindingInit(mrb_state *mrb) {
  auto graphics = mrb_define_module(mrb, "Graphics");

  mrb_define_module_function(mrb, graphics, "window_moved", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "window_resized", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "viewport_resized", do_nothing, MRB_ARGS_OPT(69));

  mrb_define_module_function(mrb, graphics, "x", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "y", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "width", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "height", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "wait", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "fadeout", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "fadein", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "snap_to_bitmap", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "resize_screen", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "move_screen", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "update", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "delta_time", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "freeze", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "transition", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "frame_reset", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "vsync_mode=", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "fov", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "fov=", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "__reset__", do_nothing, MRB_ARGS_OPT(69));

  mrb_define_module_function(mrb, graphics, "frame_rate", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "frame_count", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "brightness", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "brightness=", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "fullscreen", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "fullscreen=", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "show_cursor", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "show_cursor=", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "smooth", do_nothing, MRB_ARGS_OPT(69));
  mrb_define_module_function(mrb, graphics, "smooth=", do_nothing, MRB_ARGS_OPT(69));

	mrb_define_const(mrb, graphics, "RESOLUTION_OVERRIDDEN", mrb_bool_value(conf.resolutionOverridden));
}