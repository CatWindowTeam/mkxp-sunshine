#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include <string>

#include "filesystem.h"

class AudioSource
{
public:
    AudioSource(MIX_Mixer* mixer);
    AudioSource(MIX_Mixer* mixer, const std::string& path, bool predecode = false);
    AudioSource(MIX_Mixer* mixer, MIX_Audio* audio);
    ~AudioSource();

    bool load(MIX_Mixer* mixer, const std::string& path, bool predecode = false);
    void unload();
    bool isLoaded() const;

    MIX_Audio* getAudio() const;
    const std::string& getPath() const;
private:
    MIX_Audio* p_audio = nullptr;
    std::string p_path;
};