#include <mruby.h>

void FileSystemBindingInit(mrb_state *mrb){
	struct RClass *m = mrb_define_module(mrb, "File");
}
