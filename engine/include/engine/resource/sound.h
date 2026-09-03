// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_SOUND_H
#define UNNAMEDGAME_ENGINE_RESOURCE_SOUND_H

#include "engine/backend/asset_provider.h"

#include "engine/util/resource_location.h"

#include "engine/compile_options.h"

#include <optional>

namespace engine {
    class ResourceManager;

    struct SoundResource {
        ResourceManager* resourceManager;

        const util::ResourceLocation* location;

        options::Atomic<uint32_t> strongReferences;

        std::optional<backend::Audio> audio;

        explicit SoundResource(ResourceManager* resourceManager);
        ~SoundResource();

        void addStrongReference();
        void removeStrongReference();

        void ensureResidency();
    };

    class Sound {
    public:
        explicit Sound(SoundResource* resource)
            : mResource(resource) {}

        Sound(const Sound& other)
            : mResource(other.mResource) {
            mResource->addStrongReference();
        }

        Sound(Sound&& other) noexcept
            : mResource(other.mResource) {
            other.mResource = nullptr;
        }

        ~Sound() {
            if (mResource) mResource->removeStrongReference();
        }

        Sound& operator=(const Sound& other) {
            if (&other != this) {
                if (mResource) mResource->removeStrongReference();
                mResource = other.mResource;
                mResource->addStrongReference();
            }
            return *this;
        }

        Sound& operator=(Sound&& other) noexcept {
            if (&other != this) {
                if (mResource) mResource->removeStrongReference();
                mResource = other.mResource;
                other.mResource = nullptr;
            }
            return *this;
        }

        backend::Audio audio() const {
            mResource->ensureResidency();
            return mResource->audio.value();
        }

    private:
        SoundResource* mResource;
    };
}

#endif //UNNAMEDGAME_ENGINE_RESOURCE_SOUND_H
