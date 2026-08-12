#include "audiosource.h"
#include "filesystem.h"
#include "meow.h"
#include "sharedstate.h"

class AudioOpenHandler : public FileSystem::OpenHandler
{
public:
    AudioOpenHandler(MIX_Mixer* mixer, MIX_Audio** result, bool predecode) :
        mixer(mixer),
        result(result),
        predecode(predecode) {}

    bool tryRead(SDL_IOStream*& ops, const char* ext) override {
        *result = MIX_LoadAudio_IO(mixer, ops, predecode, true);

        if (*result) {
            ops = nullptr;
            return true;
        }

        return false;
    }

private:
    MIX_Mixer* mixer;
    MIX_Audio** result;

    bool predecode;
};

AudioSource::AudioSource(MIX_Mixer* mixer) {}

AudioSource::AudioSource(MIX_Mixer* mixer, const std::string& path, bool predecode) {
    load(mixer, path, predecode);
}

AudioSource::AudioSource(MIX_Mixer* mixer, MIX_Audio* audio) {
    p_audio = audio;
}

AudioSource::~AudioSource() {
    unload();
}

bool AudioSource::load(MIX_Mixer* mixer, const std::string& path, bool predecode) {
    if (!mixer)
        return false;

    unload();

    AudioOpenHandler handler(mixer, &p_audio, predecode);

    shState->fileSystem().openRead(handler, path.c_str());

    if (!p_audio) {
        ErrorMsg("Sound: failed to load %s: %s", path.c_str(), SDL_GetError());
        return false;
    }

    p_path = path;

    return true;
}

void AudioSource::unload() {
    if (p_audio)
    {
        MIX_DestroyAudio(p_audio);
        p_audio = nullptr;
    }

    p_path.clear();
}

bool AudioSource::isLoaded() const {
    return p_audio != nullptr;
}

MIX_Audio* AudioSource::getAudio() const {
    return p_audio;
}

const std::string& AudioSource::getPath() const {
    return p_path;
}