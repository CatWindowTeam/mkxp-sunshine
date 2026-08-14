#pragma once

#include <SDL3_mixer/SDL_mixer.h>

#include "audiogroup.h"
#include "audiosource.h"

constexpr float DEFAULT_FADE_IN = 0.0f;

class AudioPlayback
{
public:
    AudioPlayback(MIX_Mixer* mixer);
    AudioPlayback(MIX_Mixer* mixer, AudioSource* source, AudioGroup* group);
    AudioPlayback(MIX_Mixer* mixer, MIX_Audio* audio, AudioGroup* group);
    ~AudioPlayback();

    long startSample = 0;
    long maxSample = -1; 

    long getLength() const;
    double getLengthNormalized() const;

    bool initialize(MIX_Mixer* mixer, AudioSource* source, AudioGroup* group);
    bool initialize(MIX_Mixer* mixer, MIX_Audio* audio, AudioGroup* group);
    void deinitilize();

    bool play(int loops, float volume, float pitch = 1.0f, float pan = 0.0f);
    bool play(int loops);
    bool play();
    bool stop();

    bool isPlaying() const;

    void addTag(std::string tagName);
    void removeTag(std::string tagName);
    void clearTags();

    void setGroup(int group);
    int getGroup() const;
    MIX_Track* getTrack() const;
    const std::string& getPath() const;

    void setLoops(int loops);
    int getLoops() const;
    void setVolume(float volume);
    float getVolume() const;
    void setPitch(float pitch);
    float getPitch() const;
    void setPanning(float pan);
    float getPanning() const;

    void setPlaybackPosition(double time);
    double getPlaybackPosition() const;

    bool fadeIn(double time, int loops, float volume, float pitch = 1.0f, float pan = 0.0f);
    bool fadeIn(double time, int loops);
    bool fadeIn(double time);
    void fadeOut(double time);

    void update();
private:
    MIX_Track* p_track = nullptr;
    AudioSource* p_source = nullptr;
    AudioGroup* p_group = nullptr;

    float p_volume = 1.0f;
    float p_pitch = 1.0f;
};