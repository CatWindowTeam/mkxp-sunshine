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

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/irep.h>
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/dump.h>
#include <string>
#include <SDL3/SDL_iostream.h>
#include <unistd.h>
#include "binding.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "filesystem.h"
#include "meow.h"

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
mrb_state *mrb;

void exceptions_binding(mrb_state *mrb);
void SunshineBindingInit(mrb_state *mrb);
void nikoBindingInit(mrb_state *mrb);
void journalBindingInit(mrb_state *mrb);

static void mrbBindingInit(mrb_state *mrb){
	int arena = mrb_gc_arena_save(mrb);
	exceptions_binding(mrb);
	SunshineBindingInit(mrb);
	nikoBindingInit(mrb);
	journalBindingInit(mrb);
	mrb_gc_arena_restore(mrb, arena);
}

static const char *mrbValueString(mrb_value value){
	return mrb_string_p(value) ? RSTRING_PTR(value) : 0;
}

static void checkException(mrb_state *mrb){}

static void mrbBindingExecute(){
	try{
		mrb = mrb_open();
		mrbBindingInit(mrb);
		//mrb_ccontext *cxt = mrb_ccontext_new(mrb);	
		//runRMXPScripts(mrb, ctx);
		//checkException(mrb);
		//mrb_ccontext_free(mrb, cxt);
		shState->rtData().rqTermAck.set();
		mrb_close(mrb);
	}catch(...){
		show_crash_screen = true;
		shState->rtData().rqTermAck.set();
	}
}

static void mrbBindingTerminate(){
	//mrb_raise(mrb, data->exc[Shutdown], "");
}

static void mrbBindingReset(){}

static void mrbBindingGC(){
	mrb_full_gc(mrb);
}
