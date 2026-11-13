//dummy backend, you can use it as reference

#pragma once
#include <mruby.h>

static mrb_value wallpaperSet(mrb_state *mrb, mrb_value self){
	return mrb_nil_value();
}

static mrb_value wallpaperReset(mrb_state *mrb, mrb_value self){
	return mrb_nil_value();
}
