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

#include "binding.h"
#include <mruby.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/irep.h>
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/dump.h>
#include <stdio.h>
#include <zlib.h>
#include <string>
#include <SDL_messagebox.h>
#include <SDL_rwops.h>
#include <SDL_timer.h>
#include "sharedstate.h"
#include "texpool.h"
#include "eventthread.h"
#include "filesystem.h"
#include "exception.h"
#include "binding-util.h"
#include "binding-types.h"
#include "mrb-ext/marshal.h"

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

void fileBindingInit(mrb_state *);
void timeBindingInit(mrb_state *);
void marshalBindingInit(mrb_state *);
void kernelBindingInit(mrb_state *);
void tableBindingInit(mrb_state *);
void etcBindingInit(mrb_state *);
void fontBindingInit(mrb_state *);
void bitmapBindingInit(mrb_state *);
void spriteBindingInit(mrb_state *);
void planeBindingInit(mrb_state *);
void viewportBindingInit(mrb_state *);
void windowBindingInit(mrb_state *);
void tilemapBindingInit(mrb_state *);
void inputBindingInit(mrb_state *);
void audioBindingInit(mrb_state *);
void graphicsBindingInit(mrb_state *);

/* From module_rpg.c */
//extern const uint8_t mrbModuleRPG[];

static void mrbBindingInit(mrb_state *mrb){
	int arena = mrb_gc_arena_save(mrb);

	/* Init standard classes */
	fileBindingInit(mrb);
	timeBindingInit(mrb);
	marshalBindingInit(mrb);
	kernelBindingInit(mrb);

	/* Init RGSS classes */
	tableBindingInit(mrb);
	etcBindingInit(mrb);
	fontBindingInit(mrb);
	bitmapBindingInit(mrb);
	spriteBindingInit(mrb);
	planeBindingInit(mrb);
	viewportBindingInit(mrb);
	windowBindingInit(mrb);
	tilemapBindingInit(mrb);

	/* Init RGSS modules */
	inputBindingInit(mrb);
	audioBindingInit(mrb);
	graphicsBindingInit(mrb);

	/* Load RPG module */
	// mrb_load_irep(mrb, mrbModuleRPG);
	mrb_gc_arena_restore(mrb, arena);
}

static const char *mrbValueString(mrb_value value){
	return mrb_string_p(value) ? RSTRING_PTR(value) : 0;
}

static void checkException(mrb_state *mrb){
	if (!mrb->exc)
		return;

	mrb_value exc = mrb_obj_value(mrb->exc);
	MrbData &mrbData = *getMrbData(mrb);

	/* Check if an actual exception occured, or just a shutdown was called */
	if (mrb_obj_class(mrb, exc) != mrbData.exc[Shutdown])
		showExcMessageBox(mrb, exc);
}

//GOVNO, надо свою абасрацкию движка
static void runCustomScript(mrb_state *mrb, mrbc_context *ctx, const char *filename){
	/* Execute custom script */
	FILE *f = fopen(filename, "rb");

	if (!f){
		static char buffer[256];
		snprintf(buffer, sizeof(buffer), "Unable to open script '%s'", filename);
		showError(buffer);
		return;
	}

	ctx->filename = strdup(filename);
	ctx->lineno = 1;

	/* Run code */
	mrb_load_file_cxt(mrb, f, ctx);

	free(ctx->filename);
	fclose(f);
}

static void runMrbFile(mrb_state *mrb, const char *filename){
	/* Execute compiled script */
	FILE *f = fopen(filename, "rb");

	if (!f){
		static char buffer[256];
		snprintf(buffer, sizeof(buffer), "Unable to open compiled script '%s'", filename);
		showError(buffer);
		return;
	}

	mrb_irep *irep = mrb_read_irep_file(mrb, f);
	if (!irep){
		static char buffer[256];
		snprintf(buffer, sizeof(buffer), "Unable to read compiled script '%s'", filename);
		showError(buffer);
		return;
	}

	RProc *proc = mrb_proc_new(mrb, irep);
	mrb_run(mrb, proc, mrb_top_self(mrb));
	fclose(f);
}

static void runRMXPScripts(mrb_state *mrb, mrbc_context *ctx){
	if (conf.game.scripts.empty()){
		WarnMsg("No game scripts specified (missing Game.ini?)");
		return;
	}

	if (!shState->fileSystem().exists(scriptPack.c_str())){
		WarnMsg(std::string("Unable to open '") + scriptPack + "'");
		return;
	}

	/* We use a secondary util state to unmarshal the scripts */
	mrb_state *scriptMrb = mrb_open();
	SDL_RWops ops;

	shState->fileSystem().openReadRaw(ops, scriptPack.c_str());
	mrb_value scriptArray = mrb_nil_value();
	std::string readError;

	try{
		scriptArray = marshalLoadInt(scriptMrb, &ops);
	}catch(const Exception &e){
		readError = std::string(": ") + e.msg;
	}

	SDL_RWclose(&ops);
	if (!mrb_array_p(scriptArray)){
		WarnMsg(std::string("Failed to read script data") + readError);
		mrb_close(scriptMrb);
		return;
	}

	int scriptCount = mrb_ary_len(scriptMrb, scriptArray);
	std::string decodeBuffer;
	decodeBuffer.resize(0x1000);

	for (int i = 0; i < scriptCount; ++i){
		mrb_value script = mrb_ary_entry(scriptArray, i);

		mrb_value scriptChksum = mrb_ary_entry(script, 0);
		mrb_value scriptName   = mrb_ary_entry(script, 1);
		mrb_value scriptString = mrb_ary_entry(script, 2);

		(void) scriptChksum;

		int result = Z_OK;
		unsigned long bufferLen;

		while (true){
			unsigned char *bufferPtr = reinterpret_cast<unsigned char*>(const_cast<char*>(decodeBuffer.c_str()));
			unsigned char *sourcePtr = reinterpret_cast<unsigned char*>(RSTRING_PTR(scriptString));

			bufferLen = decodeBuffer.length();
			result = uncompress(bufferPtr, &bufferLen, sourcePtr, RSTRING_LEN(scriptString));
			bufferPtr[bufferLen] = '\0';
			if (result != Z_BUF_ERROR)
				break;

			decodeBuffer.resize(decodeBuffer.size()*2);
		}

		if (result != Z_OK){
			static char buffer[256];
			snprintf(buffer, sizeof(buffer), "Error decoding script %d: '%s'", i, RSTRING_PTR(scriptName));
			WarnMsg(buffer);
			break;
		}

		ctx->filename = RSTRING_PTR(scriptName);
		ctx->lineno = 1;

		int ai = mrb_gc_arena_save(mrb);

		/* Execute code */
		mrb_load_nstring_cxt(mrb, decodeBuffer.c_str(), bufferLen, ctx);
		mrb_gc_arena_restore(mrb, ai);
		if (mrb->exc)
			break;
	}

	mrb_close(scriptMrb);
}

static void mrbBindingExecute(){
	mrb_state *mrb = mrb_open();
	shState->setBindingData(mrb);

	MrbData mrbData(mrb);
	mrb->ud = &mrbData;
	mrbBindingInit(mrb);

	mrbc_context *ctx = mrbc_context_new(mrb);
	ctx->capture_errors = 1;

	const Config &conf = shState->rtData().config;
	const std::string &customScript = conf.customScript;

	if (!customScript.empty())
		runCustomScript(mrb, ctx, customScript.c_str());
	else
		runRMXPScripts(mrb, ctx);

	checkException(mrb);

	shState->rtData().rqTermAck.set();
	shState->texPool().disable();

	mrbc_context_free(mrb, ctx);
	mrb_close(mrb);
}

static void mrbBindingTerminate(){
	mrb_state *mrb = static_cast<mrb_state*>(shState->bindingData());
	MrbData *data = static_cast<MrbData*>(mrb->ud);

	mrb_raise(mrb, data->exc[Shutdown], "");
}

static void mrbBindingReset(){
	// No idea how to do this with mruby yet
}
