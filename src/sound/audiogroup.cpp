#include "audiogroup.h"
#include "sharedstate.h"
#include "audio.h"

AudioGroup::AudioGroup(MIX_Mixer* mixer, int id) :
    mixGroup(MIX_CreateGroup(mixer)),
    id(id)
{}

AudioGroup::AudioGroup(AudioGroup&& other) noexcept :
    mixGroup(other.mixGroup),
    id(other.id) {

    other.mixGroup = nullptr;
}

AudioGroup::~AudioGroup() {
    if (mixGroup)
        MIX_DestroyGroup(mixGroup);
}

AudioGroup& AudioGroup::operator=(AudioGroup&& other) noexcept{
    if (this == &other)
        return *this;

    if (mixGroup)
        MIX_DestroyGroup(mixGroup);

    mixGroup = other.mixGroup;
    id = other.id;

    other.mixGroup = nullptr;

    return *this;
}

std::string const AudioGroup::tag() {
	return "__group_" + std::to_string(id);
}

float AudioGroup::getVolume() const {
    return p_volume;
}

void AudioGroup::setVolume(float volume) {
    p_volume = volume;
    update();
}

float AudioGroup::getPitch() const {
    return p_pitch;
}

void AudioGroup::setPitch(float pitch) {
    p_pitch = pitch;
    update();
}

void AudioGroup::update(){
    if (mixGroup){
        int count;
        MIX_Track** tracks = MIX_GetTaggedTracks(MIX_GetGroupMixer(mixGroup), tag().c_str(), &count);
        for (int i = 0; i < count; ++i){
            MIX_Track* track = tracks[i];
            shState->audio().getPlayback(track)->update();
        }
    }
}