/*
** binding-mruby.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 - 2021 Amaryllis Kulla <ancurio@mapleshrine.eu>
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

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <mruby.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/irep.h>
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/dump.h>
#include <stdexcept>
#include <zlib.h>
#include <SDL3/SDL_iostream.h>
#include <unistd.h>
#include "SDL3/SDL_stdinc.h"
#include "binding.h"
#include "debugwriter.h"
#include <mruby/error.h>
#include <mruby/value.h>
#include <mruby/object.h>
#include "external/mruby/include/mruby/compile.h"
#include "graphics.h"
#include "meow.h"
#include "mruby/boxing_word.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "util.h"

#include "data.h"
//#include "test_scripts.h"

#include "module_rpg.mrb.xxd"

static void mrbBindingExecute();
static void mrbBindingTerminate();
static void mrbBindingReset();
static void mrbBindingGC();

ScriptBinding scriptBindingImpl = {
    mrbBindingExecute,
    mrbBindingTerminate,
    mrbBindingReset,
    mrbBindingGC
};

ScriptBinding *scriptBinding = &scriptBindingImpl;

void exceptions_binding(mrb_state *mrb);
void SunshineBindingInit(mrb_state *mrb);
void nikoBindingInit(mrb_state *mrb);
void journalBindingInit(mrb_state *mrb);
void oneshotBindingInit(mrb_state *mrb);
void graphicsBindingInit(mrb_state *mrb);
void wallpaperBindingInit(mrb_state *mrb);
void mkxpBindingInit(mrb_state *mrb);

const char* stringify_object(mrb_state *mrb, mrb_value *obj) {
	if (mrb_string_p(*obj))
		return mrb_string_value_cstr(mrb, obj);

	auto sym = mrb_intern_lit(mrb, "to_s");

	if (mrb_respond_to(mrb, *obj, sym)) {
		auto val = mrb_funcall(mrb, *obj, "to_s", 0);
		if (mrb->exc) {
			mrb_clear_error(mrb);
			return nullptr;
		}

		if (mrb_string_p(*obj))
			return mrb_string_value_cstr(mrb, &val);
	}

	return nullptr;
}

static void mrbBindingInit(mrb_state *mrb){
	int arena = mrb_gc_arena_save(mrb);
	exceptions_binding(mrb);
	SunshineBindingInit(mrb);
	nikoBindingInit(mrb);
	journalBindingInit(mrb);
	oneshotBindingInit(mrb);
	graphicsBindingInit(mrb);
	wallpaperBindingInit(mrb);
	mkxpBindingInit(mrb);
	mrb_gc_arena_restore(mrb, arena);
}

static const char *mrbValueString(mrb_value value){
	return mrb_string_p(value) ? RSTRING_PTR(value) : 0;
}

static void checkException(mrb_state *mrb) {

}

static void mrbBindingExecute() {
	mrb_state *mrb = mrb_open();
	shState->setBindingData(mrb);

	mrb_ccontext *cxt = mrb_ccontext_new(mrb);
	try {
		mrb->ud = SDL_calloc(1, sizeof(MRBBindingData));
		if (!mrb->ud)
			throw std::runtime_error("failed to allocate binding data");

		mrb_load_irep_buf_cxt(mrb, module_rpg_mrb, module_rpg_mrb_len, cxt);
		if (mrb->exc) {
			mrb_print_backtrace(mrb);
			throw std::runtime_error("failed to load RPG module bytecode");
		}

		MRBBindingData* bindingData = static_cast<MRBBindingData*>(mrb->ud);
		mrbBindingInit(mrb);

		throw std::runtime_error("script loading not implemented yet.")
		/*
		for (auto script : scripts) {
			mrb_ccontext_filename(mrb, cxt, script.name);
			mrb_load_string_cxt(mrb, script.script, cxt);

			if (mrb->exc) {
				if (mrb->exc->c == bindingData->shudownExc)
					break;

				mrb_print_backtrace(mrb);
				throw std::runtime_error("uncaught exception in ruby");
			}
		}
		*/
	} catch (const std::exception& e) {
		const char* what = e.what();

		size_t len = clamp(SDL_strlen(what), (size_t)0, sizeof(crash_message) - 1);

		SDL_memset(crash_message, 0, sizeof(crash_message));
		if (len != 0) {
			SDL_memcpy(crash_message, what, len);
		} else {
			const char what[] = "(empty error message)";
			SDL_memcpy(crash_message, what, sizeof(what));
		}

		show_crash_screen = true;
	}

	if (mrb->ud) {
		SDL_free(mrb->ud);
		mrb->ud = nullptr;
	}

	mrb_ccontext_free(mrb, cxt);

	shState->rtData().rqTermAck.set();

	shState->setBindingData(nullptr);
	mrb_close(mrb);
}

static void mrbBindingTerminate() {
	mrb_state* mrb = static_cast<mrb_state*>(shState->bindingData());
	if (mrb)
		mrb_raise(mrb, ((MRBBindingData*)mrb->ud)->shudownExc, "termination requested");
}

static void mrbBindingReset(){}

static void mrbBindingGC() {
	mrb_state* mrb = static_cast<mrb_state*>(shState->bindingData());
	if (mrb)
		mrb_full_gc(mrb);
}
