#include <ruby.h>
#include <ruby/debug.h>
#include "meow.h"
#include "debugwriter.h"

static VALUE SetLogLevel(VALUE, VALUE v) {
  loglevel = FIX2INT(v);
  return v;
}

static VALUE debug(VALUE, VALUE v) {
  if(loglevel >= 4){
  	VALUE inspected = rb_inspect(v);
  	Debug() << "[DEBUG " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
  }
  return v;
}

static VALUE Info(VALUE, VALUE v) {
  if(loglevel >= 3){
  	VALUE inspected = rb_inspect(v);
  	Debug() << "[INFO " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
  }
  return v;
}

static VALUE Warn(VALUE, VALUE v) {
  if(loglevel >= 2){
  	VALUE inspected = rb_inspect(v);
  	Debug() << "[WARN " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
  }
  return v;
}

static VALUE Error(VALUE, VALUE v) {
  if(loglevel >= 1){
  	VALUE inspected = rb_inspect(v);
  	Debug() << "[ERROR " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
  }
  return v;
}

void LoggerInit(){
    VALUE module = rb_define_module("Logger");
	rb_define_singleton_method(module, "Error", RUBY_METHOD_FUNC(Error), 1);
	rb_define_singleton_method(module, "Warn", RUBY_METHOD_FUNC(Warn), 1);
	rb_define_singleton_method(module, "Info", RUBY_METHOD_FUNC(Info), 1);
	rb_define_singleton_method(module, "Debug", RUBY_METHOD_FUNC(debug), 1);
	rb_define_singleton_method(module, "log_level=", RUBY_METHOD_FUNC(SetLogLevel), 1);
}
