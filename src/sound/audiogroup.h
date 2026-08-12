#pragma once

#include <string>
#include <SDL3_mixer/SDL_mixer.h>

struct AudioGroup
{
    AudioGroup(MIX_Mixer* mixer, int id);
    AudioGroup(AudioGroup&& other) noexcept;
    ~AudioGroup();

    AudioGroup& operator=(AudioGroup&& other) noexcept;

    std::string const tag();

    float getVolume() const;
    void setVolume(float volume);
    float getPitch() const;
    void setPitch(float pitch);

    void update();

    MIX_Group* mixGroup;
    int id = 0;
private:
    float p_volume = 1.0f;
    float p_pitch = 1.0f;
};
