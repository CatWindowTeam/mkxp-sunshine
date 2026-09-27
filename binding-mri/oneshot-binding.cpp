#include "oneshot.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "eventthread.h"
#include <zlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

static VALUE oneshotSetYesNo(int argc, VALUE *argv, VALUE self){
	const char *yes;
	const char *no;
	rb_get_args(argc, argv, "zz", &yes, &no RB_ARG_END);
	shState->oneshot().setYesNo(yes, no);
	return Qnil;
}

static VALUE oneshotMsgBox(int argc, VALUE *argv, VALUE self){
	int type;
	VALUE body;
	VALUE title = Qnil;
	rb_get_args(argc, argv, "iS|S", &type, &body, &title RB_ARG_END);
	std::string bodyStr = std::string(RSTRING_PTR(body), RSTRING_LEN(body));
	std::string titleStr = (title == Qnil) ? "" : std::string(RSTRING_PTR(title), RSTRING_LEN(title));
	return rb_bool_new(shState->oneshot().msgbox(type, bodyStr.c_str(), titleStr.c_str()));
}

static VALUE oneshotTextInput(int argc, VALUE *argv, VALUE self){
	VALUE prompt;
	int char_limit = 100;
	VALUE font = Qnil;
	rb_get_args(argc, argv, "S|iS", &prompt, &char_limit, &font RB_ARG_END);
	std::string promptStr = std::string(RSTRING_PTR(prompt), RSTRING_LEN(prompt));
	std::string fontStr = (font == Qnil) ? "" : std::string(RSTRING_PTR(font), RSTRING_LEN(font));
	return rb_str_new2(shState->oneshot().textinput(promptStr.c_str(), char_limit, fontStr.c_str()).c_str());
}

static VALUE oneshotResetObscured(VALUE self){
	shState->oneshot().resetObscured();
	return Qnil;
}

static VALUE oneshotObscuredCleared(VALUE self){
	return shState->oneshot().obscuredCleared() ? Qtrue : Qfalse;
}

static VALUE oneshotAllowExit(int argc, VALUE *argv, VALUE self){
	bool allowExit;
	rb_get_args(argc, argv, "b", &allowExit RB_ARG_END);
	shState->oneshot().setAllowExit(allowExit);
	return Qnil;
}

static VALUE oneshotExiting(int argc, VALUE *argv, VALUE self){
	bool exiting;
	rb_get_args(argc, argv, "b", &exiting RB_ARG_END);
	shState->oneshot().setExiting(exiting);
	return Qnil;
}

static VALUE oneshotShake(VALUE self){
	int absx, absy;
	SDL_GetWindowPosition(shState->rtData().window, &absx, &absy);
	int state;
	SDL_srand(time(NULL));
	for (int i = 0; i < 60; ++i) {
		int max = 60 - i;
		int x = SDL_rand(RAND_MAX) % (max * 2) - max;
		int y = SDL_rand(RAND_MAX) % (max * 2) - max;
		SDL_SetWindowPosition(shState->rtData().window, absx + x, absy + y);
		rb_eval_string_protect("sleep 0.02", &state);
	}
	return Qnil;
}

static VALUE oneshotCRC32(int argc, VALUE *argv, VALUE self){
	VALUE string;
	rb_get_args(argc, argv, "S", &string RB_ARG_END);
	uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, reinterpret_cast<const Bytef*>(RSTRING_PTR(string)), RSTRING_LEN(string));
	return UINT2NUM(static_cast<std::uint32_t>(crc));
}

static VALUE oneshotSetObscuredUpdating(VALUE self, VALUE rb_bool){
	bool value;
	rb_bool_arg(rb_bool, &value);
	shState->oneshot().setObscuredUpdating(value);
	return Qnil;
}

void oneshotBindingInit(){
	VALUE module = rb_define_module("Oneshot");
	VALUE msg = rb_define_module_under(module, "Msg");

	// Constants
	rb_const_set(module, rb_intern("OS"), rb_str_new2(shState->oneshot().os().c_str()));
	#ifdef unix_like
		rb_const_set(module, rb_intern("DE"), rb_str_new2(shState->oneshot().desktopEnv.c_str()));
	#endif
	rb_const_set(module, rb_intern("USER_NAME"), rb_str_new2(shState->oneshot().userName().c_str()));
	rb_const_set(module, rb_intern("SAVE_PATH"), rb_str_new2(shState->oneshot().savePath().c_str()));
	rb_const_set(module, rb_intern("DOCS_PATH"), rb_str_new2(shState->oneshot().docsPath().c_str()));
	rb_const_set(module, rb_intern("GAME_PATH"), rb_str_new2(shState->oneshot().gamePath().c_str()));
	rb_const_set(module, rb_intern("JOURNAL"), rb_str_new2(shState->oneshot().journal().c_str()));
	rb_const_set(module, rb_intern("LANG"), rb_str_new2(shState->oneshot().lang().c_str()));
	rb_const_set(msg, rb_intern("INFO"), INT2FIX(Oneshot::MSG_INFO));
	rb_const_set(msg, rb_intern("YESNO"), INT2FIX(Oneshot::MSG_YESNO));
	rb_const_set(msg, rb_intern("WARN"), INT2FIX(Oneshot::MSG_WARN));
	rb_const_set(msg, rb_intern("ERR"), INT2FIX(Oneshot::MSG_ERR));

	// Functions
	rb_define_module_function(module, "set_yes_no", RUBY_METHOD_FUNC(oneshotSetYesNo), -1);
	rb_define_module_function(module, "msgbox", RUBY_METHOD_FUNC(oneshotMsgBox), -1);
	rb_define_module_function(module, "textinput", RUBY_METHOD_FUNC(oneshotTextInput), -1);
	rb_define_module_function(module, "reset_obscured", RUBY_METHOD_FUNC(oneshotResetObscured), 0);
	rb_define_module_function(module, "obscured_cleared?", RUBY_METHOD_FUNC(oneshotObscuredCleared), 0);
	rb_define_module_function(module, "allow_exit", RUBY_METHOD_FUNC(oneshotAllowExit), -1);
	rb_define_module_function(module, "exiting", RUBY_METHOD_FUNC(oneshotExiting), -1);
	rb_define_module_function(module, "shake", RUBY_METHOD_FUNC(oneshotShake), 0);
	rb_define_module_function(module, "crc32", RUBY_METHOD_FUNC(oneshotCRC32), -1);
	rb_define_module_function(module, "obscured_updating=", RUBY_METHOD_FUNC(oneshotSetObscuredUpdating), 1);
}
