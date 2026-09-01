// Copyright 2026 Jannik Laugmand Bülow

#include "engine/resource/texture.h"

namespace engine {
    TextureResource::TextureResource(ResourceManager* resourceManager, util::ResourceLocation location)
        : resourceManager(resourceManager)
        , location(std::move(location))
        , strongReferences(0)
        , image(std::nullopt)
        , textureHandle(nullptr) {}

    TextureResource::~TextureResource() {

    }

    void TextureResource::addStrongReference() {
        strongReferences++;
    }

    void TextureResource::removeStrongReference() {
        strongReferences--;
        if (strongReferences == 0) {
            //TODO: delete stuff with resource manager
        }
    }

    void TextureResource::ensureCPUResidence() {

    }

    void TextureResource::ensureGPUResidence() {

    }
}
