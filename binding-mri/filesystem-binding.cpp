/*
** filesystem-binding.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <www.gnu.org/licenses/>.
*/

#include "binding-util.h"
#include "filesystem.h"
#include "debugwriter.h"
#include "util.h"
#include "define.h"
#include <ruby.h>
#include <ruby/intern.h>
#include <ruby/encoding.h>
#ifdef mkxp_android
	#include <filesystem>
#endif

static VALUE fileIntClose(VALUE self) {
    SDL_IOStream *ops = getPrivateData<SDL_IOStream>(self);
    if (!ops) {
        return Qnil;
    }

    setPrivateData(self, nullptr);
    SDL_CloseIO(ops);
    return Qnil;
}

static void fileIntFreeInstance(void *inst){
	SDL_IOStream *ops = static_cast<SDL_IOStream*>(inst);
	if (!ops)
	    return;
	SDL_CloseIO(ops);
}

DEF_TYPE_CUSTOMFREE(FileInt, fileIntFreeInstance);

VALUE fileIntForPath(const char *path, bool rubyExc){
	SDL_IOStream* ops = nullptr;
	#ifdef mkxp_android
		std::string absPath = std::filesystem::absolute(path).string();
		ops = SDL_IOFromFile(absPath.c_str(), "r");
	#else
		ops = SDL_IOFromFile(path, "r");
	#endif
	if (!ops){
		if (rubyExc) {
			rb_raise(rb_eIOError, "Cannot open file: %s", path);
		}
		return Qnil;
	}
	VALUE klass = rb_const_get(rb_cObject, rb_intern("FileInt"));
	VALUE obj = rb_obj_alloc(klass);
	setPrivateData(obj, ops);
	return obj;
}

static VALUE fileIntRead(int argc, VALUE *argv, VALUE self){
	Sint64 length = -1;

	int len = -1;
	rb_get_args(argc, argv, "i", &len);
	if (len != -1)
		length = static_cast<Sint64>(len);

	SDL_IOStream *ops = getPrivateData<SDL_IOStream>(self);
	if(!ops){
        rb_raise(rb_eIOError, "closed stream");
    }else if(length == -1){
		Sint64 cur = SDL_TellIO(ops);
		Sint64 end = SDL_GetIOSize(ops);
		length = end - cur;
//		SDL_SeekIO(ops, cur, SDL_IO_SEEK_SET);
	}

	if (length == 0)
		return Qnil;

	VALUE data = rb_str_new(0, (long)length);
	size_t bytes_read = SDL_ReadIO(ops, RSTRING_PTR(data), (size_t)length);
	return data;
}


static VALUE fileIntGetByte(VALUE self){
	SDL_IOStream *ops = getPrivateData<SDL_IOStream>(self);
	if (!ops) {
		rb_raise(rb_eIOError, "closed stream");
    }
	unsigned char byte = 0;
	size_t result = SDL_ReadIO(ops, &byte, 1);
	return (result == 1) ? INT2NUM(byte) : Qnil;
}

VALUE load_protect(VALUE marsh_and_port) {
    VALUE *arr = (VALUE *)marsh_and_port;
    VALUE marsh = arr[0];
    VALUE port  = arr[1];
    return rb_funcallv(marsh, rb_intern("load"), 1, &port);
}

VALUE kernelLoadDataInt(const char *filename, bool rubyExc){
    VALUE port = fileIntForPath(filename, rubyExc);
    if (NIL_P(port)) return Qnil;
    VALUE marsh = rb_const_get(rb_cObject, rb_intern("Marshal"));
    VALUE args[2] = { marsh, port };
    int state = 0;
    VALUE result = rb_protect(load_protect, (VALUE)args, &state);
    rb_funcallv(port, rb_intern("close"), 0, NULL);
    if (state){
		VALUE err = rb_errinfo();
		VALUE klass = rb_obj_class(err);
		VALUE message = rb_funcall(err, rb_intern("message"), 0);
		VALUE backtrace = rb_funcall(err, rb_intern("backtrace"), 0);
		rb_p(klass);
		rb_p(message);
		rb_p(backtrace);
        rb_jump_tag(state);
    }
    return result;
}

static VALUE kernelLoadData(int argc, VALUE *argv, VALUE self){
	const char *filename;
	rb_get_args(argc, argv, "z", &filename);
	return kernelLoadDataInt(filename, true);
}

static VALUE kernelSaveData(int argc, VALUE *argv, VALUE self){
	VALUE obj;
	VALUE filename;

	rb_get_args(argc, argv, "oS", &obj, &filename);
	VALUE file = rb_file_open_str(filename, "wb");
	VALUE marsh = rb_const_get(rb_cObject, rb_intern("Marshal"));
	VALUE v[] = { obj, file };
	rb_funcall2(marsh, rb_intern("dump"), ARRAY_SIZE(v), v);

	rb_io_close(file);
	return Qnil;
}

static VALUE _marshalLoad(int argc, VALUE *argv, VALUE self){
	VALUE port, proc = Qnil;
	rb_scan_args(argc, argv, "01", &port, &proc);
	return rb_marshal_load(port);
}

static VALUE binMode(VALUE self){
	return Qnil;
}

void fileIntBindingInit(){
	VALUE klass = rb_define_class("FileInt", rb_cIO);
	rb_define_alloc_func(klass, classAllocate<&FileIntType>);
    rb_define_method(klass, "read", RUBY_METHOD_FUNC(fileIntRead), -1);
    rb_define_method(klass, "getbyte", RUBY_METHOD_FUNC(fileIntGetByte), 0);
    rb_define_method(klass, "close", RUBY_METHOD_FUNC(fileIntClose), 0);
    rb_define_method(klass, "binmode", RUBY_METHOD_FUNC(binMode), 0);
	rb_define_module_function(rb_mKernel, "load_data", RUBY_METHOD_FUNC(kernelLoadData), -1);
    rb_define_module_function(rb_mKernel, "save_data", RUBY_METHOD_FUNC(kernelSaveData), -1);

	VALUE marsh = rb_const_get(rb_cObject, rb_intern("Marshal"));
	rb_define_alias(rb_singleton_class(marsh), "_mkxp_load_alias", "load");
	rb_define_module_function(marsh, "load", RUBY_METHOD_FUNC(_marshalLoad), -1);
}
