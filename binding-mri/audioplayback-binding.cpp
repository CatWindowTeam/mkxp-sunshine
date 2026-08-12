#include "audioplayback-binding.h"
#include "binding-util.h"
#include "debugwriter.h"

void audioplayback_free(void* ptr)
{
    AudioPlayback* pb = (AudioPlayback*)ptr;
    delete pb;
}

const rb_data_type_t audioplayback_type = { "AudioPlayback", {0, audioplayback_free, 0}, 0, 0, RUBY_TYPED_FREE_IMMEDIATELY };

VALUE audioplayback_klass;

#define PLAYBACK \
    AudioPlayback* pb; \
    TypedData_Get_Struct(self, AudioPlayback, &audioplayback_type, pb);

static VALUE rb_playbackPlay(int argc, VALUE *argv, VALUE self) {
    PLAYBACK
    switch (argc)
    {
        case 0:{
            pb->play();
            break;
        }
        case 1:{
            int loops;
            rb_get_args(argc, argv, "i", &loops RB_ARG_END);

            pb->play(loops);
            break;
        }
        default: {
            int loops;
            double volume;
            double pitch = 1.0;
            rb_get_args(argc, argv, "if|f", &loops, &volume, &pitch RB_ARG_END);
            pb->play(loops, volume, pitch);
            break;
        }
    }

    return Qnil;
}

static VALUE rb_playbackStop(VALUE self){
    PLAYBACK
    pb->stop();

    return Qnil;
}

static VALUE rb_playbackPlaying(VALUE self){
    PLAYBACK
    return rb_bool_new(pb->isPlaying());
}

static VALUE rb_playbackAddTag(int argc, VALUE *argv, VALUE self) {
    char* c_str;
    rb_get_args(argc, argv, c_str RB_ARG_END);
    PLAYBACK
    pb->addTag(c_str);

    return Qnil;
}

static VALUE rb_playbackRemoveTag(int argc, VALUE *argv, VALUE self) {
    char* c_str;
    rb_get_args(argc, argv, c_str RB_ARG_END);
    PLAYBACK
    pb->removeTag(c_str);

    return Qnil;
}

static VALUE rb_playbackClearTags(VALUE self){
    PLAYBACK
    pb->clearTags();

    return Qnil;
}

static VALUE rb_playbackSetGroup(VALUE self, VALUE groupId){
    PLAYBACK
    pb->setGroup(NUM2INT(groupId));
    return Qnil;
}

static VALUE rb_playbackGetGroup(VALUE self){
    PLAYBACK
    return INT2FIX(pb->getGroup());
}

static VALUE rb_playbackSetLoops(VALUE self, VALUE loops){
    PLAYBACK
    pb->setLoops(NUM2INT(loops));
    return Qnil;
}

static VALUE rb_playbackGetLoops(VALUE self){
    PLAYBACK
    return INT2FIX(pb->getLoops());
}

static VALUE rb_playbackSetVolume(VALUE self, VALUE loops){
    PLAYBACK
    pb->setVolume(NUM2DBL(loops));
    return Qnil;
}

static VALUE rb_playbackGetVolume(VALUE self){
    PLAYBACK
    return DBL2NUM(pb->getVolume());
}

static VALUE rb_playbackSetPitch(VALUE self, VALUE pitch){
    PLAYBACK
    pb->setPitch(NUM2DBL(pitch));
    return Qnil;
}

static VALUE rb_playbackGetPitch(VALUE self){
    PLAYBACK
    return DBL2NUM(pb->getPitch());
}

static VALUE rb_playbackSetPosition(VALUE self, VALUE position){
    PLAYBACK
    pb->setPlaybackPosition(NUM2DBL(position));
    return Qnil;
}

static VALUE rb_playbackGetPosition(VALUE self){
    PLAYBACK
    return DBL2NUM(pb->getPlaybackPosition());
}

static VALUE rb_playbackFadeIn(int argc, VALUE *argv, VALUE self) {
    PLAYBACK
    switch (argc)
    {
        case 1:{
            double time;
            rb_get_args(argc, argv, "f", &time RB_ARG_END);
            pb->fadeIn(time);
            break;
        }
        case 2:{
            double time;
            int loops;
            rb_get_args(argc, argv, "fi", &time, &loops RB_ARG_END);

            pb->fadeIn(time, loops);
            break;
        }
        default: {
            double time;
            int loops;
            double volume;
            double pitch = 1.0f;
            rb_get_args(argc, argv, "fif|f", &time, &loops, &volume, &pitch RB_ARG_END);

            pb->fadeIn(time, loops, volume, loops);
            break;
        }
    }

    return Qnil;
}

static VALUE rb_playbackFadeOut(VALUE self, VALUE time){
    PLAYBACK
    pb->fadeOut(NUM2DBL(time));
    return Qnil;
}

void audioPlaybackBindingInit(){
	audioplayback_klass = rb_define_class("AudioPlayback", rb_cObject);
    rb_undef_alloc_func(audioplayback_klass);

    rb_define_method(audioplayback_klass, "play", RUBY_METHOD_FUNC(rb_playbackPlay), -1);
    rb_define_method(audioplayback_klass, "stop", RUBY_METHOD_FUNC(rb_playbackStop), 0);

    rb_define_method(audioplayback_klass, "playing", RUBY_METHOD_FUNC(rb_playbackPlaying), 0);

    rb_define_method(audioplayback_klass, "add_tag", RUBY_METHOD_FUNC(rb_playbackAddTag), -1);
    rb_define_method(audioplayback_klass, "remove_tag", RUBY_METHOD_FUNC(rb_playbackRemoveTag), -1);
    rb_define_method(audioplayback_klass, "clear_tags", RUBY_METHOD_FUNC(rb_playbackClearTags), 0);

    rb_define_method(audioplayback_klass, "group=", RUBY_METHOD_FUNC(rb_playbackSetGroup), 1);
    rb_define_method(audioplayback_klass, "group", RUBY_METHOD_FUNC(rb_playbackGetGroup), 0);

    rb_define_method(audioplayback_klass, "loops=", RUBY_METHOD_FUNC(rb_playbackSetLoops), 1);
    rb_define_method(audioplayback_klass, "loops", RUBY_METHOD_FUNC(rb_playbackGetLoops), 0);
    rb_define_method(audioplayback_klass, "volume=", RUBY_METHOD_FUNC(rb_playbackSetVolume), 1);
    rb_define_method(audioplayback_klass, "volume", RUBY_METHOD_FUNC(rb_playbackGetVolume), 0);
    rb_define_method(audioplayback_klass, "pitch=", RUBY_METHOD_FUNC(rb_playbackSetPitch), 1);
    rb_define_method(audioplayback_klass, "pitch", RUBY_METHOD_FUNC(rb_playbackGetPitch), 0);

    rb_define_method(audioplayback_klass, "position=", RUBY_METHOD_FUNC(rb_playbackSetPosition), 1);
    rb_define_method(audioplayback_klass, "position", RUBY_METHOD_FUNC(rb_playbackGetPosition), 0);

    rb_define_method(audioplayback_klass, "fade_in", RUBY_METHOD_FUNC(rb_playbackFadeIn), -1);
    rb_define_method(audioplayback_klass, "fade_out", RUBY_METHOD_FUNC(rb_playbackFadeOut), 1);
}