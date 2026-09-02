// Copyright 2026 Jannik Laugmand Bülow

#include "engine/resource/texture.h"

#include "engine/resource/resource_manager.h"

namespace engine {
    TextureResource::TextureResource(ResourceManager* resourceManager)
        : resourceManager(resourceManager)
        , location(nullptr)
        , samplerDesc(nullptr)
        , strongReferences(0)
        , image(std::nullopt)
        , textureHandle(nullptr) {}

    TextureResource::~TextureResource() {
        if (image) resourceManager->evictTextureCPU(*this);
        if (textureHandle || samplerHandle) resourceManager->evictTextureGPU(*this); // this handles both at the same time
    }

    void TextureResource::addStrongReference() {
        if (strongReferences++ == 1) {
            resourceManager->markUsed(*this);
        }
    }

    void TextureResource::removeStrongReference() {
        if (--strongReferences == 0) {
            resourceManager->markUnused(*this);
        }
    }

    void TextureResource::ensureCPUResidence() {
        if (!image) [[unlikely]] resourceManager->realizeTextureCPU(*this);
    }

    void TextureResource::ensureGPUResidence() {
        if (!textureHandle) [[unlikely]] resourceManager->realizeTextureGPU(*this);
    }
}
