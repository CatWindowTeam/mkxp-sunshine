#include "audioplayback.h"
#include "meow.h"
#include "sharedstate.h"
#include "audio.h"
#include <cstring>

AudioPlayback::AudioPlayback(MIX_Mixer* mixer) {}

AudioPlayback::AudioPlayback(MIX_Mixer* mixer, AudioSource* source, AudioGroup* group) {
    initialize(mixer, source, group);
}

//AudioPlayback::AudioPlayback(MIX_Mixer* mixer, MIX_Audio* audio, AudioGroup* group) {
//    initialize(mixer, audio, group);
//}

AudioPlayback::~AudioPlayback() {
    deinitilize();
}

bool AudioPlayback::initialize(MIX_Mixer* mixer, AudioSource* source, AudioGroup* group) {
    if (!mixer || !source)
        return false;

    deinitilize();

    p_track = MIX_CreateTrack(mixer);

    if (!p_track)
    {
        ErrorMsg("AudioPlayback: MIX_CreateTrack failed: %s", SDL_GetError());
        return false;
    }

    shState->audio().registerPlayback(this);

    if (group) {
        p_group = group;
        MIX_SetTrackGroup(p_track, group->mixGroup);
        MIX_TagTrack(p_track, group->tag().c_str());
        group->update();
    }

    p_source = source;

    return true;
}

//bool AudioPlayback::initialize(MIX_Mixer* mixer, MIX_Audio* audio, AudioGroup* group) {
//    return initialize(mixer, new AudioSource(mixer, audio), group);
//}

void AudioPlayback::deinitilize() {
    if (p_track)
    {
        shState->audio().unregisterPlayback(this);
        MIX_StopTrack(p_track, 0);
        MIX_DestroyTrack(p_track);
        p_track = nullptr;
    }

    p_source = nullptr;
}

// TODO: Realize panning
bool AudioPlayback::play(int loops, float volume, float pitch, float pan) {/*
    if (!p_track || !p_source)
        return false;
    
    MIX_SetTrackGain(p_track, volume);
    MIX_SetTrackFrequencyRatio(p_track, pitch);
    //MIX_SetTrack3DPosition();
    if (p_group)
        MIX_SetTrackGroup(p_track, p_group->mixGroup);

    return play(loops);*/
    return fadeIn(DEFAULT_FADE_IN, loops, volume, pitch);
}
bool AudioPlayback::play(int loops) {/*
    if (!p_track || !p_source)
        return false;

    MIX_SetTrackLoops(p_track, loops);

    return play();*/
    
    return fadeIn(DEFAULT_FADE_IN, loops);
}

bool AudioPlayback::play() {/*
    if (!p_track || !p_source)
        return false;

    bool result = MIX_SetTrackAudio(p_track, p_source->getAudio());

    if (result)
        result = MIX_PlayTrack(p_track, 0);

    return result;*/

    return fadeIn(DEFAULT_FADE_IN);
}

bool AudioPlayback::stop(){
    if (!p_track)
        return false;
    return MIX_StopTrack(p_track, 0);
}

bool AudioPlayback::isPlaying() const {
    return p_track && MIX_TrackPlaying(p_track);
}

long AudioPlayback::getLength() const {
    if (p_source)
        return MIX_GetAudioDuration(p_source->getAudio());
    return 0.0;
}

double AudioPlayback::getLengthNormalized() const {
    if (p_source)
    {
        MIX_Audio* audio = p_source->getAudio();
        SDL_AudioSpec spec;
        if (MIX_GetAudioFormat(audio, &spec))
            return static_cast<double>(MIX_GetAudioDuration(audio)) / static_cast<double>(spec.freq);
    }
    return 0.0;
}

void AudioPlayback::addTag(const std::string tagName) {
    if (!p_track)
        return;

    MIX_TagTrack(p_track, tagName.data());
}
void AudioPlayback::removeTag(const std::string tagName) {
    if (!p_track)
        return;

    MIX_UntagTrack(p_track, tagName.data());
}

void AudioPlayback::clearTags() {
    if (!p_track)
        return;

    int count = 0;
    char** tags = MIX_GetTrackTags(p_track, &count);

    for (int i = 0; i < count; ++i) {
        if (std::strcmp(tags[i], p_group->tag().c_str()) != 0)
            MIX_UntagTrack(p_track, tags[i]);
    }
}

void AudioPlayback::setGroup(int group) {
    AudioGroup* a_group = shState->audio().getGroup(group);
    if (p_track)
        MIX_SetTrackGroup(p_track, a_group->mixGroup);
    p_group = a_group;
    MIX_TagTrack(p_track, a_group->tag().c_str());
    a_group->update();
}

int AudioPlayback::getGroup() const {
    return p_group->id;
}

const std::string& AudioPlayback::getPath() const{
    if (p_source)
        return p_source->getPath();
    return "";
}

MIX_Track* AudioPlayback::getTrack() const {
    return p_track;
}

void AudioPlayback::setLoops(int loops) {
    if (p_track)
        MIX_SetTrackLoops(p_track, loops);
}

int AudioPlayback::getLoops() const {
    if (p_track)
        return MIX_GetTrackLoops(p_track);
    return 0;
}

void AudioPlayback::setVolume(float volume){
    p_volume = volume;
    update();
}

float AudioPlayback::getVolume() const {
    return p_volume;
}

void AudioPlayback::setPitch(float pitch) {
    p_pitch = pitch;
    update();
}

float AudioPlayback::getPitch() const {
    return p_pitch;
}

// TODO: Realize panning
void AudioPlayback::setPanning(float pan) {
    //MIX_SetTrack3DPosition();
}

float AudioPlayback::getPanning() const {
    //if (p_track)
        //return MIX_GetTrack3DPosition();
    return 0;
}

void AudioPlayback::setPlaybackPosition(double time) {
    if (p_track)
        MIX_SetTrackPlaybackPosition(p_track, MIX_TrackMSToFrames(p_track, time * 1000.0));
}

double AudioPlayback::getPlaybackPosition() const {
    if (!p_track)
        return 0;
    return MIX_TrackFramesToMS(p_track, MIX_GetTrackPlaybackPosition(p_track)) / 1000.0;
}

bool AudioPlayback::fadeIn(double time, int loops, float volume, float pitch, float pan) {
    if (!p_track || !p_source)
        return false;
    
    p_volume = volume;
    p_pitch = pitch;
    update();
    //MIX_SetTrack3DPosition();
    if (p_group)
        MIX_SetTrackGroup(p_track, p_group->mixGroup);

    return fadeIn(time, loops);
}

bool AudioPlayback::fadeIn(double time, int loops) {
    if (!p_track || !p_source)
        return false;

    bool result = MIX_SetTrackAudio(p_track, p_source->getAudio());

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, time * 1000.0);
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    if (result)
    {
        result = MIX_PlayTrack(p_track, props);
    }
    SDL_DestroyProperties(props);

    return result;
}

bool AudioPlayback::fadeIn(double time) {
    if (!p_track || !p_source)
        return false;

    bool result = MIX_SetTrackAudio(p_track, p_source->getAudio());

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, time * 1000.0);
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_START_FRAME_NUMBER, startSample);
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_MAX_FRAME_NUMBER, maxSample);
    if (result)
    {
        result = MIX_PlayTrack(p_track, props);
    }
    SDL_DestroyProperties(props);

    return result;
}

void AudioPlayback::fadeOut(double time) {
    if (isPlaying())
        MIX_StopTrack(p_track, MIX_TrackMSToFrames(p_track, time * 1000.0));
}

void AudioPlayback::update() {
    if (p_track) {
        if (p_group) {
            MIX_SetTrackGain(p_track, p_volume * p_group->getVolume());
            MIX_SetTrackFrequencyRatio(p_track, p_pitch * p_group->getPitch());
        }
        else {
            MIX_SetTrackGain(p_track, p_volume);
            MIX_SetTrackFrequencyRatio(p_track, p_pitch);
        }
    }
}
