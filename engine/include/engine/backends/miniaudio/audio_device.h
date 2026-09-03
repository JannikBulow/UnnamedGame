// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_MINIAUDIO_AUDIO_DEVICE_H
#define UNNAMEDGAME_ENGINE_BACKENDS_MINIAUDIO_AUDIO_DEVICE_H

#include "engine/backend/audio_device.h"

#include <miniaudio.h>

namespace backend {
    class MiniaudioAudioDevice : public IAudioDevice {
    public:
        MiniaudioAudioDevice();
        ~MiniaudioAudioDevice() override;

        AudioVoiceHandle createVoice() override;
        void destroyVoice(AudioVoiceHandle voice) override;

        void setAudio(AudioVoiceHandle voice, Audio audio) override;

        void play(AudioVoiceHandle voice) override;
        void stop(AudioVoiceHandle voice) override;
        void pause(AudioVoiceHandle voice) override;
        void resume(AudioVoiceHandle voice) override;
        void setVolume(AudioVoiceHandle voice, float volume) override;
        void setPitch(AudioVoiceHandle voice, float pitch) override;
        void setLooping(AudioVoiceHandle voice, bool looping) override;

    private:
        struct Voice {
            ma_sound sound;
            ma_audio_buffer buffer;
            Audio audio;
            bool paused = false;
            bool soundInitialized = false;
            bool bufferInitialized = false;
        };

        ma_engine mEngine;
        util::HandleStorage<Voice, AudioVoiceHandleTag> mVoices;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_MINIAUDIO_AUDIO_DEVICE_H
