// Copyright 2026 Jannik Laugmand Bülow

#include "engine/resource/font.h"
#include "engine/resource/resource_manager.h"

namespace engine {
    FontResource::FontResource(ResourceManager* resourceManager)
        : resourceManager(resourceManager)
        , location(nullptr)
        , samplerDesc(nullptr)
        , strongReferences(0)
        , font(std::nullopt)
        , textureHandle(nullptr) {}

    FontResource::~FontResource() {
        if (font) resourceManager->evictFontCPU(*this);
        if (textureHandle) resourceManager->evictFontGPU(*this);
    }

    void FontResource::addStrongReference() {
        if (++strongReferences == 1) {
            resourceManager->markUsed(*this);
        }
    }

    void FontResource::removeStrongReference() {
        if (--strongReferences == 0) {
            resourceManager->markUnused(*this);
        }
    }

    void FontResource::ensureCPUResidence() {
        if (!font) [[unlikely]] resourceManager->realizeFontCPU(*this);
    }

    void FontResource::ensureGPUResidence() {
        if (!textureHandle) [[unlikely]] resourceManager->realizeFontGPU(*this);
    }
}
