// Copyright 2026 Jannik Laugmand Bülow

#include "engine/sound/audio_device.h"

namespace engine {
    AudioDevice::AudioDevice(backend::Backend& backend)
        : mDevice(backend.audio) {}

    void AudioDevice::setMasterVolume(float volume) {
        mDevice.setMasterVolume(volume);
    }

    void AudioDevice::play(Sound sound) {
        backend::AudioVoiceHandle voice = getVoice();

        mDevice.setAudio(voice, sound.audio());
        mDevice.setLooping(voice, false);
        mDevice.setVolume(voice, 1.0f);
        mDevice.setPitch(voice, 1.0f);
        mDevice.play(voice);

        mActiveVoices.emplace_back(std::move(sound), voice);
    }

    void AudioDevice::stop(const Sound& sound) {
        backend::AudioVoiceHandle voice = findVoice(sound);
        mDevice.stop(voice);
    }

    void AudioDevice::pause(const Sound& sound) {
        backend::AudioVoiceHandle voice = findVoice(sound);
        mDevice.pause(voice);
    }

    void AudioDevice::resume(const Sound& sound) {
        backend::AudioVoiceHandle voice = findVoice(sound);
        mDevice.resume(voice);
    }

    void AudioDevice::update(float dt) {
        for (auto it = mActiveVoices.begin(); it != mActiveVoices.end();) {
            if (!mDevice.isPlaying(it->voice) && !mDevice.isPaused(it->voice)) {
                mFreeVoices.push_back(it->voice);
                it = mActiveVoices.erase(it);
            } else {
                ++it;
            }
        }
    }

    backend::AudioVoiceHandle AudioDevice::getVoice() {
        if (mFreeVoices.empty()) return mDevice.createVoice();
        auto voice = mFreeVoices.back();
        mFreeVoices.pop_back();
        return voice;
    }

    backend::AudioVoiceHandle AudioDevice::findVoice(const Sound& sound) {
        for (const auto& activeVoice : mActiveVoices) {
            if (activeVoice.sound == sound) return activeVoice.voice;
        }
        return nullptr;
    }
}
