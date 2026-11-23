#include "audioplayback.h"
#include "binding-utils.h"
#include "logger.h"

void audioplayback_free(mrb_state *mrb, void* ptr){
    AudioPlayback* pb = (AudioPlayback*)ptr;
    delete pb;
}

const mrb_data_type audioplayback_type = {"AudioPlayback", audioplayback_free};
struct RClass *audioplayback_klass;

static mrb_value rb_playbackPlay(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    mrb_int loops = 0;
    mrb_float volume = 1.0;
    mrb_float pitch = 1.0;
    mrb_int argc = mrb_get_args(mrb, "|iff", &loops, &volume, &pitch);

    switch(argc){
        case 0:{
            pb->play();
            break;
        }
      	case 1:{
            int loops;
            mrb_get_args(mrb, "i", &loops);
            pb->play(loops);
            break;
        }
        default: {
            int loops;
            double volume;
            double pitch = 1.0;
            mrb_get_args(mrb, "if|f", &loops, &volume, &pitch);
            pb->play(loops, volume, pitch);
            break;
        }
    }

    return mrb_nil_value();
}

static mrb_value rb_playbackStop(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->stop();
    return mrb_nil_value();
}

static mrb_value rb_playbackPlaying(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return pb->isPlaying() ? mrb_true_value() : mrb_false_value();
}

static mrb_value rb_playbackLength(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_fixnum_value((mrb_int)pb->getLength());
}

static mrb_value rb_playbackLengthNormalized(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_float_value(mrb, (mrb_float)pb->getLengthNormalized());
}

static mrb_value rb_playbackAddTag(mrb_state *mrb, mrb_value self){
    char* c_str;
    mrb_get_args(mrb, "z", &c_str);
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->addTag(c_str);
    return mrb_nil_value();
}

static mrb_value rb_playbackRemoveTag(mrb_state *mrb, mrb_value self){
    char* c_str;
    mrb_get_args(mrb, "z", &c_str);
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->removeTag(c_str);
    return mrb_nil_value();
}

static mrb_value rb_playbackClearTags(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->clearTags();
    return mrb_nil_value();
}

static mrb_value rb_playbackSetGroup(mrb_state *mrb, mrb_value self){
	int groupId;
	mrb_get_args(mrb, "i", &groupId);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->setGroup(groupId);
    return mrb_nil_value();
}

static mrb_value rb_playbackGetGroup(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_fixnum_value((mrb_int)pb->getGroup());
}

static mrb_value rb_audio_getPath(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
	return mrb_str_new_cstr(mrb, pb->getPath().c_str());
}

static mrb_value rb_playbackSetLoops(mrb_state *mrb, mrb_value self){
	int loops;
	mrb_get_args(mrb, "i", &loops);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->setLoops(loops);
    return mrb_nil_value();
}

static mrb_value rb_playbackGetLoops(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_fixnum_value((mrb_int)pb->getLoops());
}

static mrb_value rb_playbackSetVolume(mrb_state *mrb, mrb_value self){
	int loops;
	mrb_get_args(mrb, "i", &loops);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->setVolume(loops);
    return mrb_nil_value();
}

static mrb_value rb_playbackGetVolume(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_float_value(mrb, (mrb_float)pb->getVolume());
}

static mrb_value rb_playbackSetPitch(mrb_state *mrb, mrb_value self){
	int pitch;
	mrb_get_args(mrb, "i", &pitch);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->setPitch(pitch);
    return mrb_nil_value();
}

static mrb_value rb_playbackGetPitch(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_float_value(mrb, (mrb_float)pb->getPitch());
}

static mrb_value rb_playbackSetStartSample(mrb_state *mrb, mrb_value self){
	int pitch;
	mrb_get_args(mrb, "i", &pitch);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->startSample = static_cast<long>(pitch);
    return mrb_nil_value();
}

static mrb_value rb_playbackGetStartSample(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_fixnum_value((mrb_int)pb->startSample);
}

static mrb_value rb_playbackSetMaxSample(mrb_state *mrb, mrb_value self){
	int samples;
	mrb_get_args(mrb, "i", &samples);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->maxSample = static_cast<long>(samples);
    return mrb_nil_value();
}

static mrb_value rb_playbackGetMaxSample(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_fixnum_value((mrb_int)pb->maxSample);
}

static mrb_value rb_playbackSetPosition(mrb_state *mrb, mrb_value self){
	mrb_float position;
	mrb_get_args(mrb, "f", &position);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->setPlaybackPosition(static_cast<double>(position));
    return mrb_nil_value();
}

static mrb_value rb_playbackGetPosition(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    return mrb_float_value(mrb, (mrb_float)pb->getPlaybackPosition());
}

static mrb_value rb_playbackFadeIn(mrb_state *mrb, mrb_value self){
    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
	mrb_float time;
	mrb_int loops;
	mrb_float volume;
	mrb_float pitch = 1.0;

    mrb_int argc = mrb_get_args(mrb, "f|iff", &time, &loops, &volume, &pitch);
    switch(argc){
        case 1:{
            pb->fadeIn(time);
            break;
        }
        case 2:{
            pb->fadeIn(time, loops);
            break;
        }
        default: {
            pb->fadeIn(time, loops, volume, pitch);
            break;
        }
    }

    return mrb_nil_value();
}

static mrb_value rb_playbackFadeOut(mrb_state *mrb, mrb_value self){
	mrb_float time;
	mrb_get_args(mrb, "f", &time);

    AudioPlayback *pb = DATA_GET_PTR(mrb, self, &audioplayback_type, AudioPlayback);
    pb->fadeOut(static_cast<double>(time));
    return mrb_nil_value();
}

void audioPlaybackBindingInit(mrb_state *mrb){
	audioplayback_klass = mrb_define_module(mrb, "AudioPlayback");
    MRB_UNDEF_ALLOCATOR(audioplayback_klass);
    mrb_define_method(mrb, audioplayback_klass, "play", rb_playbackPlay, MRB_ARGS_OPT(3));
    mrb_define_method(mrb, audioplayback_klass, "stop", rb_playbackStop, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "playing", rb_playbackPlaying, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "length", rb_playbackLength, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "length_normalized", rb_playbackLengthNormalized, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "add_tag", rb_playbackAddTag, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "remove_tag", rb_playbackRemoveTag, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "clear_tags", rb_playbackClearTags, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "group=", rb_playbackSetGroup, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "group", rb_playbackGetGroup, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "path", rb_audio_getPath, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "loops=", rb_playbackSetLoops, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "loops", rb_playbackGetLoops, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "volume=", rb_playbackSetVolume, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "volume", rb_playbackGetVolume, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "pitch=", rb_playbackSetPitch, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "pitch", rb_playbackGetPitch, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "start_frame=", rb_playbackSetStartSample, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "start_frame", rb_playbackGetStartSample, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "max_frame=", rb_playbackSetMaxSample, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "max_frame", rb_playbackGetMaxSample, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "position=", rb_playbackSetPosition, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, audioplayback_klass, "position", rb_playbackGetPosition, MRB_ARGS_NONE());
    mrb_define_method(mrb, audioplayback_klass, "fade_in", rb_playbackFadeIn, MRB_ARGS_ARG(1,3));
    mrb_define_method(mrb, audioplayback_klass, "fade_out", rb_playbackFadeOut, MRB_ARGS_REQ(1));
}
