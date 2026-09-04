// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_AUDIO_DEVICE_H
#define UNNAMEDGAME_ENGINE_BACKEND_AUDIO_DEVICE_H

#include "engine/backend/asset_provider.h"

#include "engine/util/handle.h"

namespace backend {
    using AudioVoiceHandle = util::Handle<struct AudioVoiceHandleTag>;

    class IAudioDevice {
    public:
        virtual ~IAudioDevice() = default;

        virtual float getMasterVolume() = 0;
        virtual void setMasterVolume(float volume) = 0;

        virtual AudioVoiceHandle createVoice() = 0;
        virtual void destroyVoice(AudioVoiceHandle voice) = 0;

        virtual void setAudio(AudioVoiceHandle voice, Audio audio) = 0;

        virtual bool isPlaying(AudioVoiceHandle voice) = 0;
        virtual bool isPaused(AudioVoiceHandle voice) = 0;
        virtual bool isFinished(AudioVoiceHandle voice) = 0;

        virtual void play(AudioVoiceHandle voice) = 0;
        virtual void stop(AudioVoiceHandle voice) = 0;
        virtual void pause(AudioVoiceHandle voice) = 0;
        virtual void resume(AudioVoiceHandle voice) = 0;
        virtual void setVolume(AudioVoiceHandle voice, float volume) = 0;
        virtual void setPitch(AudioVoiceHandle voice, float pitch) = 0;
        virtual void setLooping(AudioVoiceHandle voice, bool looping) = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_AUDIO_DEVICE_H
