#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_iostream.h>
#include <mruby.h>
#include <physfs.h>
#include "meow.h"

static mrb_value file_read(mrb_state *mrb, mrb_value self){
    const char *path;
    mrb_get_args(mrb, "z", &path);

    PHYSFS_File *file = PHYSFS_openRead(path);
    if(!file){
        return mrb_nil_value();
    }

    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    if(size < 0 || (PHYSFS_uint64)size >= (PHYSFS_uint64)SIZE_MAX || (PHYSFS_uint64)size > (PHYSFS_uint64)MRB_INT_MAX){
        PHYSFS_close(file);
        return mrb_nil_value();
    }

    char *buffer = (char*)SDL_malloc((size_t)size + 1);
    if(!buffer){
        PHYSFS_close(file);
        return mrb_nil_value();
    }

    PHYSFS_sint64 total = 0;
    while(total < size) {
        PHYSFS_uint64 remaining = (PHYSFS_uint64)(size - total);
        PHYSFS_sint64 n = PHYSFS_readBytes(file, buffer + total, remaining);
        if (n <= 0) {
            SDL_free(buffer);
            PHYSFS_close(file);
            return mrb_nil_value();
        }

        total += n;
    }

    buffer[total] = '\0';
    PHYSFS_close(file);
    mrb_value result = mrb_str_new(mrb, buffer, (mrb_int)total);
    SDL_free(buffer);
    return result;
}

static mrb_value file_write(mrb_state *mrb, mrb_value self){
	const char* path;
	mrb_value data;
	mrb_get_args(mrb, "zS", &path, &data);
	SDL_IOStream *file = SDL_IOFromFile(path, "wb");
	if(file == nullptr){
		return mrb_false_value();	
	}else{
		SDL_WriteIO(file, (void*)RSTRING_PTR(data), RSTRING_LEN(data));
		SDL_CloseIO(file);
		return mrb_true_value();
	}
}

/* MiniFS::File.copy(source, destination) */
static mrb_value file_copy(mrb_state *mrb, mrb_value self){
  mrb_value source, destination;
  mrb_get_args(mrb, "SS", &source, &destination);

  /* TODO: реализовать */
  return mrb_nil_value();
}

/* MiniFS::File.delete(path) */
static mrb_value file_delete(mrb_state *mrb, mrb_value self){
  mrb_value path;
  mrb_get_args(mrb, "S", &path);

  /* TODO: реализовать */
  return mrb_nil_value();
}

/* MiniFS::Dir.entries(path) */
static mrb_value dir_entries(mrb_state *mrb, mrb_value self){
  mrb_value path;
  mrb_get_args(mrb, "S", &path);

  /* TODO: реализовать */
  return mrb_nil_value();
}

/* MiniFS::Dir.mkdir(path) */
static mrb_value dir_mkdir(mrb_state *mrb, mrb_value self){
  mrb_value path;
  mrb_get_args(mrb, "S", &path);

  /* TODO: реализовать */
  return mrb_nil_value();
}

/* MiniFS::Dir.rmdir(path) */
static mrb_value dir_rmdir(mrb_state *mrb, mrb_value self){
  mrb_value path;
  mrb_get_args(mrb, "S", &path);

  /* TODO: реализовать */
  return mrb_nil_value();
}

void FileSystemBindingInit(mrb_state *mrb){
  struct RClass *file = mrb_define_module(mrb, "File");
  struct RClass *dir = mrb_define_module(mrb, "Dir");

  mrb_define_module_function(mrb, file, "read", file_read, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, file, "write", file_write, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, file, "copy", file_copy, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, file, "delete", file_delete, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, dir, "entries", dir_entries, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, dir, "mkdir", dir_mkdir, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, dir, "rmdir", dir_rmdir, MRB_ARGS_REQ(1));
}
