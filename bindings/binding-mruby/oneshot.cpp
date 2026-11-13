#include "oneshot.h"
#include "sharedstate.h"
#include "eventthread.h"
#include <mruby.h>
#include <zlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

static mrb_value oneshotSetYesNo(mrb_state *mrb, mrb_value self){
	const char *yes;
	const char *no;
	mrb_get_args(mrb, "zz", &yes, &no);
	shState->oneshot().setYesNo(yes, no);
	return mrb_nil_value();
}

static mrb_value oneshotMsgBox(mrb_state *mrb, mrb_value self){
	int type;
	const char *body;
	const char *title = nullptr;
	mrb_get_args(mrb, "iz|z", &type, &body, &title);
	return shState->oneshot().msgbox(type, body,
		(title == nullptr) ? "" : title) ? mrb_true_value() : mrb_false_value();
}

static mrb_value oneshotTextInput(mrb_state *mrb, mrb_value self){
	const char *prompt;
	int char_limit = 100;
	const char *font = nullptr;
	mrb_get_args(mrb, "z|iz", &prompt, &char_limit, &font);
	return mrb_str_new_cstr(mrb, shState->oneshot().textinput(prompt, char_limit, (font == nullptr) ? "" : font).c_str());
}

static mrb_value oneshotResetObscured(mrb_state *mrb, mrb_value self){
	shState->oneshot().resetObscured();
	return mrb_nil_value();
}

static mrb_value oneshotObscuredCleared(mrb_state *mrb, mrb_value self){
	return shState->oneshot().obscuredCleared() ? mrb_true_value() : mrb_false_value();
}

static mrb_value oneshotAllowExit(mrb_state *mrb, mrb_value self){
	bool allowExit;
	mrb_get_args(mrb, "b", &allowExit);
	shState->oneshot().setAllowExit(allowExit);
	return mrb_nil_value();
}

static mrb_value oneshotExiting(mrb_state *mrb, mrb_value self){
	bool exiting;
	mrb_get_args(mrb, "b", &exiting);
	shState->oneshot().setExiting(exiting);
	return mrb_nil_value();
}

static mrb_value oneshotShake(mrb_state *mrb, mrb_value self){
	int absx, absy;
	SDL_GetWindowPosition(shState->rtData().window, &absx, &absy);
	int state;
	SDL_srand(time(NULL));
	for (int i = 0; i < 60; ++i) {
		int max = 60 - i;
		int x = SDL_rand(RAND_MAX) % (max * 2) - max;
		int y = SDL_rand(RAND_MAX) % (max * 2) - max;
		SDL_SetWindowPosition(shState->rtData().window, absx + x, absy + y);
	}
	return mrb_nil_value();
}

static mrb_value oneshotCRC32(mrb_state *mrb, mrb_value self){
	const char *string;
	mrb_get_args(mrb, "z", &string);
	uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, reinterpret_cast<const Bytef*>(string), sizeof(string));
	return mrb_int_value(mrb, static_cast<mrb_int>(static_cast<std::uint32_t>(crc)));
}

static mrb_value oneshotSetObscuredUpdating(mrb_state *mrb, mrb_value self){
	bool value;
	mrb_get_args(mrb, "b", &value);
	shState->oneshot().setObscuredUpdating(value);
	return mrb_nil_value();
}

void oneshotBindingInit(mrb_state *mrb){
	auto module = mrb_define_module(mrb, "Oneshot");
	auto msg = mrb_define_class_under(mrb, module, "Msg", mrb->object_class);

	// Constants
	mrb_define_const(mrb, module, "OS", mrb_str_new_cstr(mrb, shState->oneshot().os().c_str()));
	#ifdef unix_like
		mrb_define_const(mrb, module, "DE", mrb_str_new_cstr(mrb, shState->oneshot().desktopEnv.c_str()));
	#endif
	mrb_define_const(mrb, module, "USER_NAME", mrb_str_new_cstr(mrb, shState->oneshot().userName().c_str()));
	mrb_define_const(mrb, module, "SAVE_PATH", mrb_str_new_cstr(mrb, shState->oneshot().savePath().c_str()));
	mrb_define_const(mrb, module, "DOCS_PATH", mrb_str_new_cstr(mrb, shState->oneshot().docsPath().c_str()));
	mrb_define_const(mrb, module, "GAME_PATH", mrb_str_new_cstr(mrb, shState->oneshot().gamePath().c_str()));
	mrb_define_const(mrb, module, "JOURNAL", mrb_str_new_cstr(mrb, shState->oneshot().journal().c_str()));
	mrb_define_const(mrb, module, "LANG", mrb_str_new_cstr(mrb, shState->oneshot().lang().c_str()));
	mrb_define_const(mrb, msg, "INFO", mrb_int_value(mrb, Oneshot::MSG_INFO));
	mrb_define_const(mrb, msg, "YESNO", mrb_int_value(mrb, Oneshot::MSG_YESNO));
	mrb_define_const(mrb, msg, "WARN", mrb_int_value(mrb, Oneshot::MSG_WARN));
	mrb_define_const(mrb, msg, "ERR", mrb_int_value(mrb, Oneshot::MSG_ERR));

	// Functions
	mrb_define_module_function(mrb, module, "set_yes_no", oneshotSetYesNo, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module, "msgbox", oneshotMsgBox, MRB_ARGS_ARG(2,1));
	mrb_define_module_function(mrb, module, "textinput", oneshotTextInput, MRB_ARGS_ARG(1,2));
	mrb_define_module_function(mrb, module, "reset_obscured", oneshotResetObscured, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "obscured_cleared?", oneshotObscuredCleared, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "allow_exit", oneshotAllowExit, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "exiting", oneshotExiting, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "shake", oneshotShake, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "crc32", oneshotCRC32, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "obscured_updating=", oneshotSetObscuredUpdating, MRB_ARGS_REQ(1));
}
