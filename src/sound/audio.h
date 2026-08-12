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

#pragma once

#include "util.h"
#include "audioplayback.h"

struct AudioPrivate;
struct RGSSThreadData;

class Audio{
public:
	// Audio created from path will be cached
	AudioSource* createAudioSource(const std::string& path, bool predecode = false);
	AudioSource* createAudioSource(MIX_Audio* audio);
	AudioPlayback* createAudioPlayback(const std::string& path, int group = 0, bool predecode = false);
	AudioPlayback* createAudioPlayback(AudioSource* source, int group = 0);
	AudioPlayback* createAudioPlayback(MIX_Audio* audio, int group = 0);
	AudioPlayback* getPlayback(MIX_Track* track);

	// returns id of group, returns -1 if group not created
	int createGroup();
	AudioGroup* getGroup(int group) const;
	void destroyGroup(int group);

	float getMasterVolume() const;
	void setMasterVolume(float volume);

	float getGroupVolume(int group) const;
	void setGroupVolume(int group, float volume);
	void stopSoundsInGroup(int group, float fadeoutTime = 0.0f);

	// MIX_GetTagGain does not exist :/
	//float getTagVolume(const std::string& tag) const;
	void setTagVolume(const std::string& tag, float volume);
	void playSoundsInTag(const std::string& tag);
	void stopSoundsInTag(const std::string& tag, float fadeoutTime = 0.0f);

	void load(const std::string& path, AudioSource* audio);
	void unload(const std::string& path);
	void clearCache();

	void reset();
private:
	Audio(RGSSThreadData &threadData);
	~Audio();

	void registerPlayback(AudioPlayback* pb);
	void unregisterPlayback(AudioPlayback* pb);

	friend struct SharedStatePrivate;
	friend class AudioPlayback;

	AudioPrivate* p;

	MIX_Mixer* mixer;
	
};