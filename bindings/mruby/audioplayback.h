#pragma once
#include "sound/audioplayback.h"
#include <mruby.h>

extern void audioplayback_free(mrb_state *mrb, void* ptr);
extern const mrb_data_type audioplayback_type;
extern struct RClass *audioplayback_klass;
