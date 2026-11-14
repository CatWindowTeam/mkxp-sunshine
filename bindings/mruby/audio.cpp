/*
** audio-binding.cpp
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

#include "sound/audio.h"
#include "sharedstate.h"
#include "audioplayback.h"
#include "binding-utils.h"


static const char *tag_to_cstr(mrb_state *mrb, mrb_value tag){
  if (mrb_symbol_p(tag)) {
    return mrb_sym_name(mrb, mrb_symbol(tag));
  }

  tag = mrb_ensure_string_type(mrb, tag);
  return mrb_str_to_cstr(mrb, tag);
}

static mrb_value rb_audio_createAudioPlayback(mrb_state *mrb, mrb_value self){
	char* path;
	int group = 0;
	bool predecode = false;
	mrb_get_args(mrb, "z|bi", &path, &predecode, &group);
    AudioPlayback* pb = shState->audio().createAudioPlayback(path, group, predecode);
    return mrb_obj_value(Data_Wrap_Struct(mrb, audioplayback_klass, &audioplayback_type, pb));
}

static mrb_value rb_audio_createGroup(mrb_state *mrb, mrb_value self){
	return mrb_fixnum_value((mrb_int)shState->audio().createGroup());
}

static mrb_value rb_audio_destroyGroup(mrb_state *mrb, mrb_value self){
	int groupId = 0;
	mrb_get_args(mrb, "i", &groupId);

	shState->audio().destroyGroup(groupId);
	return mrb_nil_value();
}

static mrb_value rb_audio_getMasterVolume(mrb_state *mrb, mrb_value self){
	return mrb_float_value(mrb, (mrb_float)shState->audio().getMasterVolume());
}

static mrb_value rb_audio_setMasterVolume(mrb_state *mrb, mrb_value self){
	float volume;
	mrb_get_args(mrb, "f", &volume);

	shState->audio().setMasterVolume(static_cast<double>(volume));
	return mrb_nil_value();
}

static mrb_value rb_audio_getGroupVolume(mrb_state *mrb, mrb_value self){
	float groupId;
	mrb_get_args(mrb, "i", &groupId);

	return mrb_float_value(mrb, (mrb_float)shState->audio().getGroupVolume(groupId));
}

static mrb_value rb_audio_setGroupVolume(mrb_state *mrb, mrb_value self){
	int groupId;
	float volume;
	mrb_get_args(mrb, "if", &groupId , &volume);

	shState->audio().setGroupVolume(groupId, volume);
	return mrb_nil_value();
}

static mrb_value rb_audio_stopGroupSounds(mrb_state *mrb, mrb_value self){
	int groupId;
	double fadeOutTime = 0.0f;
	mrb_get_args(mrb, "i|f", &groupId, &fadeOutTime);
	shState->audio().setGroupVolume(groupId, fadeOutTime);
	return mrb_nil_value();
}

static mrb_value rb_audio_setTagVolume(mrb_state *mrb, mrb_value self){
	mrb_value tag;
	float volume;
	mrb_get_args(mrb, "of", &tag, &volume);

	shState->audio().setTagVolume(tag_to_cstr(mrb, tag), volume);
	return mrb_nil_value();
}

static mrb_value rb_audio_playTagSounds(mrb_state *mrb, mrb_value self){
	mrb_value tag;
	mrb_get_args(mrb, "o", &tag);
	shState->audio().playSoundsInTag(tag_to_cstr(mrb, tag));
	return mrb_nil_value();
}

static mrb_value rb_audio_stopTagSounds(mrb_state *mrb, mrb_value self){
    mrb_value rb_tagName;
    double rb_fadeTime = 0.0;
    mrb_get_args(mrb, "of", &rb_tagName, &rb_fadeTime);
	shState->audio().stopSoundsInTag(tag_to_cstr(mrb, rb_tagName), rb_fadeTime);
	return mrb_nil_value();
}

static mrb_value rb_audio_unload(mrb_state *mrb, mrb_value self){
	const char* path;
	mrb_get_args(mrb, "z", &path);
	shState->audio().unload(path);
	return mrb_nil_value();
}

static mrb_value rb_audio_clearCache(mrb_state *mrb, mrb_value self){
	shState->audio().clearCache();
	return mrb_nil_value();
}

static mrb_value audioReset(mrb_state *mrb, mrb_value self){
	shState->audio().reset();
	return mrb_nil_value();
}

void audioBindingInit(mrb_state *mrb){
	struct RClass *module = mrb_define_module(mrb, "Audio");
	mrb_define_module_function(mrb, module, "create_sound", rb_audio_createAudioPlayback, MRB_ARGS_ARG(1,2));

	// returns id of group, returns -1 if group not created
	mrb_define_module_function(mrb, module, "create_group", rb_audio_createGroup, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "destroy_group", rb_audio_destroyGroup, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "master_volume", rb_audio_getMasterVolume, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "master_volume=", rb_audio_setMasterVolume, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "get_group_volume", rb_audio_getGroupVolume, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "set_group_volume", rb_audio_setGroupVolume, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module, "stop_group_sounds", rb_audio_stopGroupSounds, MRB_ARGS_ARG(1,1));
	mrb_define_module_function(mrb, module, "set_tag_volume", rb_audio_setTagVolume, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module, "play_tag_sounds", rb_audio_playTagSounds, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "stop_tag_sounds", rb_audio_stopTagSounds, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, module, "unload", rb_audio_unload, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, module, "clear_cache", rb_audio_clearCache, MRB_ARGS_NONE());
	mrb_define_module_function(mrb, module, "__reset__", audioReset, MRB_ARGS_NONE());
}
