//based on github.com/mruby/mruby/blob/master/mrbgems/mruby-io/src/file.c
#include <SDL3/SDL_platform_defines.h>
#include <SDL3/SDL_filesystem.h>
#include <mruby.h>

static mrb_value file_copy(mrb_state *mrb, mrb_value self){
  const char *oldpath;
  const char *newpath;
  mrb_get_args(mrb, "zz", &oldpath, &newpath);
  
  return SDL_CopyFile(oldpath, newpath) ? mrb_true_value() : mrb_false_value();
}

static mrb_value file_mkdir(mrb_state *mrb, mrb_value self){
  const char *path;
  mrb_get_args(mrb, "z", &path);
  
  return SDL_CreateDirectory(path) ? mrb_true_value() : mrb_false_value();
}

static mrb_value file_rmpath(mrb_state *mrb, mrb_value self){
  const char *path;
  mrb_get_args(mrb, "z", &path);
  
  return SDL_RemovePath(path) ? mrb_true_value() : mrb_false_value();
}

void FileSystemBindingInit(mrb_state *mrb){
	struct RClass *m = mrb_define_module(mrb, "File");
	mrb_define_class_method(mrb, m, "copy", file_copy, MRB_ARGS_REQ(2));
	mrb_define_class_method(mrb, m, "mkdir", file_mkdir, MRB_ARGS_REQ(1));
	mrb_define_class_method(mrb, m, "rmpath", file_rmpath, MRB_ARGS_REQ(1));
}
