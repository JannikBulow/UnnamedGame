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
        if (audio) resourceManager->evictSound(*this);
    }

    void SoundResource::addStrongReference() {
        if (++strongReferences == 1) {
            resourceManager->markUsed(*this);
        }
    }

    void SoundResource::removeStrongReference() {
        if (--strongReferences == 0) {
            resourceManager->markUnused(*this);
        }
    }

    void SoundResource::ensureResidency() {
        if (!audio) [[unlikely]] resourceManager->realizeSound(*this);
    }
}
