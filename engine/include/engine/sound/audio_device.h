// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_SOUND_AUDIO_DEVICE_H
#define UNNAMEDGAME_ENGINE_SOUND_AUDIO_DEVICE_H

#include "engine/backend/backend.h"

#include "engine/resource/sound.h"

namespace engine {
    class AudioDevice {
    public:
        explicit AudioDevice(backend::Backend& backend);

        void setMasterVolume(float volume);

        void play(Sound sound);
        void stop(const Sound& sound);
        void pause(const Sound& sound);
        void resume(const Sound& sound);

        void update(float dt);

    private:
        struct ActiveVoice {
            Sound sound;
            backend::AudioVoiceHandle voice;
        };

        backend::IAudioDevice& mDevice;
        std::vector<ActiveVoice> mActiveVoices;
        std::vector<backend::AudioVoiceHandle> mFreeVoices;

        backend::AudioVoiceHandle getVoice();
        backend::AudioVoiceHandle findVoice(const Sound& sound);
    };
}

#endif //UNNAMEDGAME_ENGINE_SOUND_AUDIO_DEVICE_H
