#include <ruby.h>
#include <ruby/debug.h>
#include "meow.h"
#include "debugwriter.h"

static VALUE loggerSetLogLevel(VALUE, VALUE v) {
	loglevel = FIX2INT(v);
	return v;
}

static VALUE loggerDebug(VALUE, VALUE v) {
	if(loglevel >= 4){
		VALUE inspected = rb_obj_as_string(v);
		Debug() << "[DEBUG " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
	}
	return v;
}

static VALUE loggerInfo(VALUE, VALUE v) {
	if(loglevel >= 3){
		VALUE inspected = rb_obj_as_string(v);
		Debug() << "[INFO " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
	}
	return v;
}

static VALUE loggerWarn(VALUE, VALUE v) {
	if(loglevel >= 2){
		VALUE inspected = rb_obj_as_string(v);
		Debug() << "[WARN " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
	}
	return v;
}

static VALUE loggerError(VALUE, VALUE v) {
	if(loglevel >= 1){
		VALUE inspected = rb_obj_as_string(v);
		Debug() << "[ERROR " << rb_sourcefile() << ":" << rb_sourceline() << "] " << StringValueCStr(inspected);
	}
	return v;
}

void LoggerInit(){
	VALUE module = rb_define_module("Logger");
	rb_define_singleton_method(module, "error", RUBY_METHOD_FUNC(loggerError), 1);
	rb_define_singleton_method(module, "warn", RUBY_METHOD_FUNC(loggerWarn), 1);
	rb_define_singleton_method(module, "info", RUBY_METHOD_FUNC(loggerInfo), 1);
	rb_define_singleton_method(module, "debug", RUBY_METHOD_FUNC(loggerDebug), 1);
	rb_define_singleton_method(module, "log_level=", RUBY_METHOD_FUNC(loggerSetLogLevel), 1);
}
