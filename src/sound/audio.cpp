/*
** audio.cpp
**
** This file is part of mkxp-sunshine.
**
** Copyright (C) 2026 CatWindowTeam
**
** mkxp-sunshine is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp-sunshine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp-sunshine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "audio.h"

#include "sharedstate.h"
#include "eventthread.h"

#include <vector>
#include <memory>

struct AudioPrivate{
	std::unordered_map<std::string, std::shared_ptr<AudioSource>> cache;
	std::unordered_map<MIX_Track*, AudioPlayback*> playbacks;
	std::vector<AudioGroup*> groups;

	AudioPrivate() {}

	~AudioPrivate(){
		cache.clear();
		playbacks.clear();
		groups.clear();
	}
};

Audio::Audio(RGSSThreadData &threadData)
{
	mixer = threadData.mixer;
	p = new AudioPrivate();
}

Audio::~Audio() { delete p; }

AudioSource* Audio::createAudioSource(const std::string& path, bool predecode) {
	AudioSource* source;
	if (p->cache.contains(path))
		source = p->cache[path].get();
	else {
		source = new AudioSource(mixer, path, predecode);
		load(path, source);
	}
	return source;
}

AudioSource* Audio::createAudioSource(MIX_Audio* audio) {
	return new AudioSource(mixer, audio);
}

AudioPlayback* Audio::createAudioPlayback(const std::string& path, int group, bool predecode) {
	AudioSource* source = createAudioSource(path, predecode);
	AudioPlayback* playback = new AudioPlayback(mixer, source, getGroup(group));

	return playback;
}

AudioPlayback* Audio::createAudioPlayback(AudioSource* source, int group) {
	return new AudioPlayback(mixer, source, getGroup(group));
}

AudioPlayback* Audio::createAudioPlayback(MIX_Audio* audio, int group) {
	return new AudioPlayback(mixer, new AudioSource(mixer, audio), getGroup(group));
}

AudioPlayback* Audio::getPlayback(MIX_Track* track){
	return p->playbacks[track];
}

int Audio::createGroup(){

    for (size_t i = 0; i < p->groups.size(); ++i)
        if (!p->groups[i]){
			AudioGroup* group = new AudioGroup(mixer, i);
			if (!group->mixGroup)
				return -1;
            p->groups[i] = group;
            return static_cast<int>(i);
        }

	int id = static_cast<int>(p->groups.size());
	AudioGroup* group = new AudioGroup(mixer, id);
	if (!group->mixGroup)
		return -1;
    p->groups.push_back(group);

    return id;
} 

AudioGroup* Audio::getGroup(int group) const {
    if (group < 0 || group >= p->groups.size())
        return nullptr;
	return p->groups[group];
}

void Audio::destroyGroup(int group) {
    if (group < 0 || group >= p->groups.size())
        return;

    AudioGroup* ptr = p->groups[group];

    if (!ptr)
        return;

	delete ptr;

    p->groups[group] = nullptr;
}

float Audio::getMasterVolume() const {
	return MIX_GetMixerGain(mixer);
}

void Audio::setMasterVolume(float volume) {
	MIX_SetMixerGain(mixer, volume);
}

float Audio::getGroupVolume(int group) const {
	AudioGroup* mixGroup = getGroup(group);
	if (!mixGroup)
		return 1.0f;

	return mixGroup->getVolume();
}

void Audio::setGroupVolume(int group, float volume) {
	AudioGroup* a_group = getGroup(group);
	if (a_group)
		a_group->setVolume(volume);
}

void Audio::stopSoundsInGroup(int group, float fadeoutTime) {
	AudioGroup* a_group = getGroup(group);
	if (a_group)
		MIX_StopTag(mixer, a_group->tag().c_str(), fadeoutTime * 1000.0f);
}

// MIX_GetTagGain does not exist :/
//float Audio::getTagVolume(const std::string& tag) const {}
void Audio::setTagVolume(const std::string& tag, float volume) {
	MIX_SetTagGain(mixer, tag.c_str(), volume);
}
void Audio::playSoundsInTag(const std::string& tag) {
	MIX_PlayTag(mixer, tag.c_str(), 0);
}
void Audio::stopSoundsInTag(const std::string& tag, float fadeoutTime) {
	MIX_StopTag(mixer, tag.c_str(), fadeoutTime * 1000.0);
}

void Audio::load(const std::string& path, AudioSource* audio) {
	p->cache.emplace(path, audio);
}

// TODO: Fix potential memory leak, need to destroy AudioSource,
// and kill all AudioPlaybacks with that source

// SOLUTION: Add disposed state to AudioPlayback
// and for all AudioPlaybacks in p->playbacks with
// that AudioSource call dispose(), which destroys MIX_AudioTrack
// but im lazy to fix it
void Audio::unload(const std::string& path){
	p->cache.erase(path);
}

void Audio::clearCache() {
	p->cache.clear();
}

void Audio::reset() {
	MIX_StopAllTracks(mixer, 0);
}

void Audio::registerPlayback(AudioPlayback* pb){
	p->playbacks.emplace(pb->getTrack(), pb);
}

void Audio::unregisterPlayback(AudioPlayback* pb){
	p->playbacks.erase(pb->getTrack());
}