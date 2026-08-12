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
#include "binding-util.h"
#include "audioplayback-binding.h"

#define TAG2CSTR(tag) SYMBOL_P(tag) ? rb_id2name(rb_sym2id(tag)) : StringValueCStr(tag)

RB_METHOD(rb_audio_createAudioPlayback) {
	char* path;
	int group = 0;
	bool predecode = false;
	rb_get_args(argc, argv, "z|bi", &path, &predecode, &group RB_ARG_END);
	
    AudioPlayback* pb = shState->audio().createAudioPlayback(path, group, predecode);

    return TypedData_Wrap_Struct(audioplayback_klass, &audioplayback_type, pb);
}

static VALUE rb_audio_createGroup(VALUE self) {
	return INT2FIX(shState->audio().createGroup());
}

static VALUE rb_audio_destroyGroup(VALUE self, VALUE groupId) {
	shState->audio().destroyGroup(FIX2INT(groupId));
	return Qnil;
}

static VALUE rb_audio_getMasterVolume(VALUE self) {
	return DBL2NUM(shState->audio().getMasterVolume());
}

static VALUE rb_audio_setMasterVolume(VALUE self, VALUE volume) {
	shState->audio().setMasterVolume(NUM2DBL(volume));
	return Qnil;
}

static VALUE rb_audio_getGroupVolume(VALUE self, VALUE groupId) {
	return DBL2NUM(shState->audio().getGroupVolume(FIX2INT(groupId)));
}

static VALUE rb_audio_setGroupVolume(VALUE self, VALUE groupId, VALUE volume) {
	shState->audio().setGroupVolume(FIX2INT(groupId), NUM2DBL(volume));
	return Qnil;
}

RB_METHOD(rb_audio_stopGroupSounds) {
	int groupId;
	double fadeOutTime = 0.0f;
	rb_get_args(argc, argv, "i|f", &groupId, &fadeOutTime);
	shState->audio().setGroupVolume(groupId, fadeOutTime);
	return Qnil;
}

static VALUE rb_audio_setTagVolume(VALUE self, VALUE tag, VALUE volume) {
	shState->audio().setTagVolume(TAG2CSTR(tag), NUM2DBL(volume));
	return Qnil;
}

static VALUE rb_audio_playTagSounds(VALUE self, VALUE tag) {
	shState->audio().playSoundsInTag(TAG2CSTR(tag));
	return Qnil;
}

RB_METHOD(rb_audio_stopTagSounds) {
    VALUE rb_tagName;
    VALUE rb_fadeTime = DBL2NUM(0.0);
    rb_scan_args(argc, argv, "11", &rb_tagName, &rb_fadeTime);
	shState->audio().stopSoundsInTag(TAG2CSTR(rb_tagName), NUM2DBL(rb_fadeTime));
	return Qnil;
}

static VALUE rb_audio_unload(VALUE self, VALUE path) {
	shState->audio().unload(StringValueCStr(path));
	return Qnil;
}

static VALUE rb_audio_clearCache(VALUE self) {
	shState->audio().clearCache();
	return Qnil;
}

RB_METHOD(audioReset) {
	RB_UNUSED_PARAM;
	shState->audio().reset();

	return Qnil;
}

void audioBindingInit(){
	printf("[audioBindingInit] Initialing Audio binding\n");
	VALUE module = rb_define_module("Audio");

	rb_define_singleton_method(module, "create_sound", RUBY_METHOD_FUNC(rb_audio_createAudioPlayback), -1);

	// returns id of group, returns -1 if group not created
	rb_define_singleton_method(module, "create_group", RUBY_METHOD_FUNC(rb_audio_createGroup), 0);
	rb_define_singleton_method(module, "destroy_group", RUBY_METHOD_FUNC(rb_audio_destroyGroup), 1);

	rb_define_singleton_method(module, "master_volume", RUBY_METHOD_FUNC(rb_audio_getMasterVolume), 0);
	rb_define_singleton_method(module, "master_volume=", RUBY_METHOD_FUNC(rb_audio_setMasterVolume), 1);

	rb_define_singleton_method(module, "get_group_volume", RUBY_METHOD_FUNC(rb_audio_getGroupVolume), 1);
	rb_define_singleton_method(module, "set_group_volume", RUBY_METHOD_FUNC(rb_audio_setGroupVolume), 2);
	rb_define_singleton_method(module, "stop_group_sounds", RUBY_METHOD_FUNC(rb_audio_stopGroupSounds), -1);

	rb_define_singleton_method(module, "set_tag_volume", RUBY_METHOD_FUNC(rb_audio_setTagVolume), 2);
	rb_define_singleton_method(module, "play_tag_sounds", RUBY_METHOD_FUNC(rb_audio_playTagSounds), 1);
	rb_define_singleton_method(module, "stop_tag_sounds", RUBY_METHOD_FUNC(rb_audio_stopTagSounds), -1);

	rb_define_singleton_method(module, "unload", RUBY_METHOD_FUNC(rb_audio_unload), 1);
	rb_define_singleton_method(module, "clear_cache", RUBY_METHOD_FUNC(rb_audio_clearCache), 0);

	_rb_define_module_function(module, "__reset__", audioReset);
}
