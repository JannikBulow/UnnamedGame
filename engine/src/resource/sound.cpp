// Copyright 2026 Jannik Laugmand Bülow

#include "engine/resource/resource_manager.h"
#include "engine/resource/sound.h"

namespace engine {
    SoundResource::SoundResource(ResourceManager* resourceManager)
        : resourceManager(resourceManager)
        , location(nullptr)
        , strongReferences(0)
        , audio(std::nullopt) {}

    SoundResource::~SoundResource() {
        if (audio)
    }

    void SoundResource::addStrongReference() {}

    void SoundResource::removeStrongReference() {}

    void SoundResource::ensureResidency() {}
}
