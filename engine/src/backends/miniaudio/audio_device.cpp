// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/miniaudio/audio_device.h"

namespace backend {
    MiniaudioAudioDevice::MiniaudioAudioDevice() {
        ma_engine_init(nullptr, &mEngine);
    }

    MiniaudioAudioDevice::~MiniaudioAudioDevice() {
        for (Voice& voice : mVoices) {
            if (voice.soundInitialized) ma_sound_uninit(&voice.sound);
            if (voice.bufferInitialized) ma_audio_buffer_uninit(&voice.buffer);
        }

        ma_engine_uninit(&mEngine);
    }

    AudioVoiceHandle MiniaudioAudioDevice::createVoice() {
        Voice voice;
        memset(&voice.sound, 0, sizeof(voice.sound));
        memset(&voice.buffer, 0, sizeof(voice.buffer));
        memset(&voice.audio, 0, sizeof(voice.audio));
        return mVoices.create(voice);
    }

    void MiniaudioAudioDevice::destroyVoice(AudioVoiceHandle _voice) {
        Voice& voice = mVoices.get(_voice);

        if (voice.soundInitialized) {
            ma_sound_uninit(&voice.sound);
            voice.soundInitialized = false;
        }

        if (voice.bufferInitialized) {
            ma_audio_buffer_uninit(&voice.buffer);
            voice.bufferInitialized = false;
        }

        mVoices.destroy(_voice);
    }

    void MiniaudioAudioDevice::setAudio(AudioVoiceHandle _voice, Audio audio) {
        Voice& voice = mVoices.get(_voice);

        if (voice.soundInitialized) {
            ma_sound_uninit(&voice.sound);
            voice.soundInitialized = false;
        }

        if (voice.bufferInitialized) {
            ma_audio_buffer_uninit(&voice.buffer);
            voice.bufferInitialized = false;
        }

        ma_audio_buffer_config config = ma_audio_buffer_config_init(ma_format_f32, audio.channels, audio.frameCount, audio.samples, nullptr);

        if (ma_audio_buffer_init(&config, &voice.buffer) != MA_SUCCESS) {
            return;
        }

        voice.bufferInitialized = true;

        if (ma_sound_init_from_data_source(&mEngine, &voice.buffer, 0, nullptr,&voice.sound) != MA_SUCCESS) {
            ma_audio_buffer_uninit(&voice.buffer);
            voice.bufferInitialized = false;
            return;
        }

        voice.soundInitialized = true;
        voice.audio = audio;
    }

    void MiniaudioAudioDevice::play(AudioVoiceHandle _voice) {
        Voice& voice = mVoices.get(_voice);
        if (!voice.soundInitialized) return;
        ma_sound_start(&voice.sound);
    }

    void MiniaudioAudioDevice::stop(AudioVoiceHandle _voice) {
        Voice& voice = mVoices.get(_voice);
        if (!voice.soundInitialized) return;
        ma_sound_stop(&voice.sound);
    }

    void MiniaudioAudioDevice::pause(AudioVoiceHandle _voice) {
        Voice& voice = mVoices.get(_voice);
        if (!voice.soundInitialized) return;
        if (!ma_sound_is_playing(&voice.sound)) return;

        ma_sound_stop(&voice.sound);
        voice.paused = true;
    }

    void MiniaudioAudioDevice::resume(AudioVoiceHandle _voice) {
        Voice& voice = mVoices.get(_voice);
        if (!voice.soundInitialized) return;
        if (!voice.paused) return;

        ma_sound_start(&voice.sound);
        voice.paused = false;
    }

    void MiniaudioAudioDevice::setVolume(AudioVoiceHandle _voice, float volume) {
        Voice& voice = mVoices.get(_voice);
        if (!voice.soundInitialized) return;
        ma_sound_set_volume(&voice.sound, volume);
    }

    void MiniaudioAudioDevice::setPitch(AudioVoiceHandle _voice, float pitch) {
        Voice& voice = mVoices.get(_voice);
        if (!voice.soundInitialized) return;
        ma_sound_set_pitch(&voice.sound, pitch);
    }

    void MiniaudioAudioDevice::setLooping(AudioVoiceHandle _voice, bool looping) {
        Voice& voice = mVoices.get(_voice);
        if (!voice.soundInitialized) return;
        ma_sound_set_looping(&voice.sound, looping);
    }
}
