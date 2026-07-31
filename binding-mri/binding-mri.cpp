/*
** binding-mri.cpp
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
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "binding.h"
#include "binding-util.h"
#include "ruby/internal/eval.h"
#include "ruby/internal/memory.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "filesystem.h"
#include "util.h"
#include "sdl-util.h"
#include "debugwriter.h"
#include "graphics.h"
#include "audio.h"
#include "boost-hash.h"
#include "meow.h"
#include "sunshine.h"
#include "modloader.h"

#include <ruby-3.4.0/ruby/internal/gc.h>
#include <ruby.h>
#include <ruby/debug.h>
#include <ruby/encoding.h>
#undef inline
#include <assert.h>
#include <string>
#include <zlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <SDL3/SDL_filesystem.h>

typedef struct {
    uint64_t start_ns;
} ud_t;

//TODO rewrtire profiler bcz ChatGPT bcz WHERE IS DOCS ABOUT RUBY C API TRACEPOINT 
static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static const char *val_to_cstr(VALUE v, const char *fallback) {
    if (NIL_P(v)) return fallback;
    VALUE s = rb_funcall(v, rb_intern("to_s"), 0);
    if (!RB_TYPE_P(s, T_STRING)) return fallback;
    return RSTRING_PTR(s);
}

static void tp_cb(VALUE tpval, void *data) {
    ud_t *ud = (ud_t *)data;

    rb_trace_arg_t *trace_arg = rb_tracearg_from_tracepoint(tpval);
    if (!trace_arg) return;

    VALUE path   = rb_tracearg_path(trace_arg);
    VALUE lineno = rb_tracearg_lineno(trace_arg);
    VALUE defined_class = rb_tracearg_defined_class(trace_arg);
    VALUE method_id = rb_tracearg_method_id(trace_arg);
    rb_event_flag_t ev = rb_tracearg_event_flag(trace_arg);

    const char *path_s = val_to_cstr(path, "<nil>");
    const char *class_s = val_to_cstr(defined_class, "<nil>");

    const char *meth_s = "<nil>";
    if (!NIL_P(method_id)) {
        const char *mn = rb_id2name(SYM2ID(method_id));
        if (mn) meth_s = mn;
    }

    long line = NIL_P(lineno) ? 0 : FIX2LONG(lineno);

    if (ev == RUBY_EVENT_CALL) {
        ud->start_ns = now_ns();
    } else if (ev == RUBY_EVENT_RETURN) {
        uint64_t end_ns = now_ns();
        uint64_t dur_ns = end_ns - ud->start_ns;
        printf("[PROFILER] %s %s:%ld class=%s dur_ns=%" PRIu64 "\n", meth_s, path_s, line, class_s, dur_ns);
    }
}

extern const char binding_mri_module_rpg1_rb[];
extern const int binding_mri_module_rpg1_rb_len;

static void mriBindingExecute();
static void mriBindingTerminate();
static void mriBindingReset();

ScriptBinding scriptBindingImpl = {
	mriBindingExecute,
	mriBindingTerminate,
	mriBindingReset
};

ScriptBinding *scriptBinding = &scriptBindingImpl;

void tableBindingInit();
void etcBindingInit();
void fontBindingInit();
void bitmapBindingInit();
void spriteBindingInit();
void viewportBindingInit();
void planeBindingInit();
void windowBindingInit();
void tilemapBindingInit();
void windowVXBindingInit();
void tilemapVXBindingInit();
void TimeBindingInit();
void SignalConnectionBindingInit();

void inputBindingInit();
void audioBindingInit();
void graphicsBindingInit();

void fileIntBindingInit();

void journalBindingInit();
void wallpaperBindingInit();
#ifdef unix_like
void wallpaperBindingTerminate();
#endif
void nikoBindingInit();
void oneshotBindingInit();
void SunshineBindingInit();
void steamBindingInit();
void shaderBindingInit();
void ModLoaderBindingInit();
void keybindingsBindingInit();
void lightmapBindingInit();

RB_METHOD(mriPrint);
RB_METHOD(mriP);
RB_METHOD(mkxpDataDirectory);
RB_METHOD(mkxpPuts);
RB_METHOD(mkxpRawKeyStates);
RB_METHOD(mkxpMouseInWindow);

RB_METHOD(mriRgssMain);
RB_METHOD(mriRgssStop);
RB_METHOD(_kernelCaller);

static void mriBindingInit(){
	printf("[mriBindingInit] Loading bindings...\n");
	tableBindingInit();
	etcBindingInit();
	fontBindingInit();
	bitmapBindingInit();
	spriteBindingInit();
	viewportBindingInit();
	planeBindingInit();
	windowBindingInit();
	tilemapBindingInit();
	TimeBindingInit();
	SignalConnectionBindingInit();
	inputBindingInit();
	audioBindingInit();
	graphicsBindingInit();
	fileIntBindingInit();
	journalBindingInit();
	wallpaperBindingInit();
	nikoBindingInit();
	oneshotBindingInit();
    SunshineBindingInit();
	steamBindingInit();
	shaderBindingInit();
	ModLoader();
	ModLoaderBindingInit();
	keybindingsBindingInit();
	lightmapBindingInit();

	_rb_define_module_function(rb_mKernel, "rgss_main", mriRgssMain);
	_rb_define_module_function(rb_mKernel, "rgss_stop", mriRgssStop);
	_rb_define_module_function(rb_mKernel, "msgbox",    mriPrint);
	_rb_define_module_function(rb_mKernel, "msgbox_p",  mriP);
	_rb_define_module_function(rb_mKernel, "print", mriPrint);
	_rb_define_module_function(rb_mKernel, "p",     mriP);
	rb_define_alias(rb_singleton_class(rb_mKernel), "_mkxp_kernel_caller_alias", "caller");
	_rb_define_module_function(rb_mKernel, "caller", _kernelCaller);

	char *script = (char*)SDL_malloc(binding_mri_module_rpg1_rb_len + 1);
	SDL_memcpy(script, binding_mri_module_rpg1_rb, binding_mri_module_rpg1_rb_len);
	script[binding_mri_module_rpg1_rb_len] = '\0';

	rb_eval_string(script);

	SDL_free(script);

	VALUE mod = rb_define_module("MKXP");
	_rb_define_module_function(mod, "data_directory", mkxpDataDirectory);
	_rb_define_module_function(mod, "puts", mkxpPuts);
	_rb_define_module_function(mod, "raw_key_states", mkxpRawKeyStates);
	_rb_define_module_function(mod, "mouse_in_window", mkxpMouseInWindow);

}

static void printP(int argc, VALUE *argv, const char *convMethod, const char *sep){
	VALUE dispString = rb_str_buf_new(128);
	ID conv = rb_intern(convMethod);

	for (int i = 0; i < argc; ++i){
		VALUE str = rb_funcall2(argv[i], conv, 0, NULL);
		rb_str_buf_append(dispString, str);

		if (i < argc)
			rb_str_buf_cat2(dispString, sep);
	}

	shState->eThread().showMessageBox(RSTRING_PTR(dispString));
}

RB_METHOD(mriPrint){
	RB_UNUSED_PARAM;
	printP(argc, argv, "to_s", "");
	return Qnil;
}

RB_METHOD(mriP){
	RB_UNUSED_PARAM;
	printP(argc, argv, "inspect", "\n");
	return Qnil;
}

RB_METHOD(mkxpDataDirectory){
	RB_UNUSED_PARAM;
	const std::string &path = shState->config().customDataPath;
	const char *s = path.empty() ? "." : path.c_str();

	return rb_str_new_cstr(s);
}

RB_METHOD(mkxpPuts){
	RB_UNUSED_PARAM;
	const char *str;
	rb_get_args(argc, argv, "z", &str RB_ARG_END);
	Debug() << str;

	return Qnil;
}

RB_METHOD(mkxpRawKeyStates){
	RB_UNUSED_PARAM;

	VALUE str = rb_str_new(0, sizeof(EventThread::keyStates));
	SDL_memcpy(RSTRING_PTR(str), EventThread::keyStates, sizeof(EventThread::keyStates));

	return str;
}

RB_METHOD(mkxpMouseInWindow){
	RB_UNUSED_PARAM;
	return rb_bool_new(EventThread::mouseState.inWindow);
}

static VALUE rgssMainCb(VALUE block){
	rb_funcall2(block, rb_intern("call"), 0, 0);
	return Qnil;
}

static VALUE rgssMainRescue(VALUE arg, VALUE exc){
	VALUE *excRet = (VALUE*) arg;
	*excRet = exc;

	return Qnil;
}

static void processReset(){
	shState->graphics().reset();
	shState->audio().reset();

	shState->rtData().rqReset.clear();
	shState->graphics().repaintWait(shState->rtData().rqResetFinish, false);
}

static VALUE rgssMainCb_wrapper(VALUE data){ return rgssMainCb(data); }

static VALUE rgssMainRescue_wrapper(VALUE data, VALUE ex){ return rgssMainRescue(data, ex); }

RB_METHOD(mriRgssMain){
	RB_UNUSED_PARAM;
	while (true){
		VALUE exc = Qnil;

		rb_rescue2(rgssMainCb_wrapper, rb_block_proc(),
           rgssMainRescue_wrapper, (VALUE)&exc,
           rb_eException, (VALUE)0);

		if (NIL_P(exc))
			break;

		if (rb_obj_class(exc) == getRbData()->exc[Reset]){
			processReset();
		}else{
			rb_exc_raise(exc);
		}
	}

	return Qnil;
}

RB_METHOD(mriRgssStop){
	RB_UNUSED_PARAM;
	while (true)
		shState->graphics().update();

	return Qnil;
}

RB_METHOD(_kernelCaller){
	RB_UNUSED_PARAM;

	VALUE trace = rb_funcall2(rb_mKernel, rb_intern("_mkxp_kernel_caller_alias"), 0, 0);

	if (!RB_TYPE_P(trace, RUBY_T_ARRAY))
		return trace;

	long len = RARRAY_LEN(trace);

	if (len < 2)
		return trace;

	/* Remove useless "ruby:1:in 'eval'" */
	rb_ary_pop(trace);

	/* Also remove trace of this helper function */
	rb_ary_shift(trace);

	len -= 2;

	if (len == 0)
		return trace;

	/* RMXP does this, not sure if specific or 1.8 related */
	VALUE args[] = { rb_str_new_cstr(":in `<main>'"), rb_str_new_cstr("") };
	rb_funcall2(rb_ary_entry(trace, len-1), rb_intern("gsub!"), 2, args);

	return trace;
}

static VALUE newStringUTF8(const char *string, long length){
	return rb_enc_str_new(string, length, rb_utf8_encoding());
}

struct evalArg{
	VALUE string;
	VALUE filename;
};

static VALUE evalHelper(evalArg *arg){
	// crashes with Game_Battler_1 for some reason
	VALUE argv[] = { arg->string, Qnil, arg->filename };
	return rb_funcall2(Qnil, rb_intern("eval"), ARRAY_SIZE(argv), argv);
}

static VALUE evalString(VALUE string, VALUE filename, int *state){
	evalArg arg = { string, filename };
	return rb_protect((VALUE (*)(VALUE))evalHelper, (VALUE)&arg, state);
}

static void runCustomScript(const std::string &filename){
	std::string scriptData;

	if (!readFileSDL(filename.c_str(), scriptData)){
		crash(Exception::MEOW, "Unable to open %s", filename);
		return;
	}

	evalString(newStringUTF8(scriptData.c_str(), scriptData.size()), newStringUTF8(filename.c_str(), filename.size()), NULL);
}

VALUE kernelLoadDataInt(const char *filename, bool rubyExc);

struct BacktraceData{
	/* Maps: Ruby visible filename, To: Actual script name */
	BoostHash<std::string, std::string> scriptNames;
};

#define SCRIPT_SECTION_FMT "Section%03ld"

static void runRMXPScripts(BacktraceData &btData){
	const Config &conf = shState->rtData().config;
	const std::string &scriptPack = conf.game.scripts;
	
	if (!shState->fileSystem().exists(scriptPack.c_str())){
		crash(Exception::MEOW, "Unable to open '%s'", scriptPack.c_str());
		return;
	}

	VALUE scriptArray;

	/* We checked if Scripts.rxdata exists, but something might
	 * still go wrong */
	try{
		scriptArray = kernelLoadDataInt(scriptPack.c_str(), false);
	}catch (const Exception &e){
		crash(Exception::MEOW, "Failed to read script data: %s", e.msg);
		return;
	}

	if (!RB_TYPE_P(scriptArray, RUBY_T_ARRAY)){
		crash(Exception::MEOW, "Failed to read script data");
		return;
	}
	
	rb_gv_set("$RGSS_SCRIPTS", scriptArray);	
	long scriptCount = RARRAY_LEN(scriptArray);
	std::string decodeBuffer;
	decodeBuffer.resize(0x1000);

	for (long i = 0; i < scriptCount; ++i){
		VALUE script = rb_ary_entry(scriptArray, i);

		if (!RB_TYPE_P(script, RUBY_T_ARRAY))
			continue;

		VALUE scriptName = rb_ary_entry(script, 1);
		VALUE scriptString = rb_ary_entry(script, 2);

		int result = Z_OK;
		unsigned long bufferLen;

		while (true){
			unsigned char *bufferPtr = reinterpret_cast<unsigned char*>(const_cast<char*>(decodeBuffer.c_str()));
			const unsigned char *sourcePtr = reinterpret_cast<const unsigned char*>(RSTRING_PTR(scriptString));

			bufferLen = decodeBuffer.length();

			result = uncompress(bufferPtr, &bufferLen, sourcePtr, RSTRING_LEN(scriptString));

			bufferPtr[bufferLen] = '\0';

			if (result != Z_BUF_ERROR)
				break;

			decodeBuffer.resize(decodeBuffer.size()*2);
		}

		if (result != Z_OK){
			crash(Exception::MEOW, "Error decoding script %ld: '%s'\n", i, RSTRING_PTR(scriptName));
			break;
		}
		rb_ary_store(script, 3, rb_str_new_cstr(decodeBuffer.c_str()));
	}

	/* Execute preloaded scripts */
	for (std::set<std::string>::iterator i = conf.preloadScripts.begin();
	     i != conf.preloadScripts.end(); ++i)
		runCustomScript(*i);

	VALUE exc = rb_gv_get("$!");
	if (exc != Qnil)
		return;

	while (true){
		for (long i = 0; i < scriptCount; ++i){
			VALUE script = rb_ary_entry(scriptArray, i);
			VALUE scriptDecoded = rb_ary_entry(script, 3);
			VALUE string = newStringUTF8(RSTRING_PTR(scriptDecoded), RSTRING_LEN(scriptDecoded));

			rb_gc_register_address(&string);

			VALUE fname;
			const char *scriptName = RSTRING_PTR(rb_ary_entry(script, 1));
			char buf[512];
			int len;

			len = SDL_snprintf(buf, sizeof(buf), "%03ld:%s", i, scriptName);

			fname = newStringUTF8(buf, len);
			rb_gc_register_address(&fname);

			btData.scriptNames.insert(buf, scriptName);
			int state;
			evalString(string, fname, &state);

			rb_gc_unregister_address(&fname);
			rb_gc_unregister_address(&string);

			if (state)
				break;
		}

		VALUE exc = rb_gv_get("$!");
		if (rb_obj_class(exc) != getRbData()->exc[Reset])
			break;

		processReset();
	}
}

static void showExc(VALUE exc, const BacktraceData &btData){
	VALUE bt = rb_funcall2(exc, rb_intern("backtrace"), 0, NULL);
	VALUE msg = rb_funcall2(exc, rb_intern("message"), 0, NULL);
	VALUE bt0 = rb_ary_entry(bt, 0);
	VALUE name = rb_class_path(rb_obj_class(exc));
	VALUE ds = rb_sprintf("%" PRIsVALUE ": %" PRIsVALUE " (%" PRIsVALUE ")", bt0, exc, name);
	/* omit "useless" last entry (from ruby:1:in `eval') */
	for (long i = 1, btlen = RARRAY_LEN(bt) - 1; i < btlen; ++i)
		rb_str_catf(ds, "\n\tfrom %" PRIsVALUE, rb_ary_entry(bt, i));
	Debug() << StringValueCStr(ds);

	char *s = RSTRING_PTR(bt0);

	char line[16];
	std::string file(512, '\0');

	char *p = s + SDL_strlen(s);
	char *e;

	while (p != s)
		if (*--p == ':')
			break;

	e = p;

	while (p != s)
		if (*--p == ':')
			break;

	/* s         p  e
	 * SectionXXX:YY: in 'blabla' */

	*e = '\0';
	strncpy(line, *p ? p+1 : p, sizeof(line));
	line[sizeof(line)-1] = '\0';
	*e = ':';
	e = p;

	/* s         e
	 * SectionXXX:YY: in 'blabla' */

	*e = '\0';
	strncpy(&file[0], s, file.size());
	*e = ':';

	/* Shrink to fit */
	file.resize(SDL_strlen(file.c_str()));
	file = btData.scriptNames.value(file, file);

	crash(Exception::MEOW, "Script '%s' line %s: %s occured.\n\n%s", file.c_str(), line, RSTRING_PTR(name), RSTRING_PTR(msg));
	exit(0);
}

static void mriBindingExecute(){
	/* Normally only a ruby executable would do a sysinit,
	 * but not doing it will lead to crashes due to closed
	 * stdio streams on some platforms (eg. Windows) */
	int argc = 0;
	char **argv = 0;
	//options_argv3[] = "--jit"
	char options_argv1[] = "oneshot", options_argv2[] = "-ev";
	char* options_argv[] = {options_argv1, options_argv2, NULL};
	ruby_sysinit(&argc, &argv);
	RUBY_INIT_STACK;
	ruby_init();
	ruby_init_loadpath();
	rb_enc_set_default_external(rb_enc_from_encoding(rb_utf8_encoding()));
	Config &conf = shState->rtData().config;
	if(conf.debugMode){
		setvbuf(stdout, NULL, _IOFBF, 1<<20);
		ud_t ud;
		rb_event_flag_t flags = 0;
		flags |= RUBY_EVENT_CALL;
		flags |= RUBY_EVENT_RETURN;
		flags |= RUBY_EVENT_RAISE;
		VALUE tp = rb_tracepoint_new(Qnil, flags, tp_cb, &ud);
		rb_tracepoint_enable(tp);		
	}

	if (!conf.rubyLoadpaths.empty()){
		/* Setup custom load paths */
		VALUE lpaths = rb_gv_get("$:");
		for (size_t i = 0; i < conf.rubyLoadpaths.size(); ++i){
			std::string &path = conf.rubyLoadpaths[i];

			VALUE pathv = rb_str_new(path.c_str(), path.size());
			rb_ary_push(lpaths, pathv);
		}
	}
	RbData *rbData = new RbData();
	shState->setBindingData(rbData);
	BacktraceData btData;

	mriBindingInit();
	runRMXPScripts(btData);

	VALUE exc = rb_errinfo();
	if (!NIL_P(exc) && !rb_obj_is_kind_of(exc, rb_eSystemExit))
		showExc(exc, btData);

	shState->rtData().rqTermAck.set();
}

static void mriBindingTerminate(){
	rb_raise(rb_eSystemExit, " ");
#ifdef unix_like
	wallpaperBindingTerminate();
#endif
}

static void mriBindingReset(){
	rb_raise(getRbData()->exc[Reset], " ");
}
