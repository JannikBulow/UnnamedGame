// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H
#define UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H

#include "engine/backend/graphics_device.h"

#include "engine/util/resource_location.h"

namespace engine {
    class ResourceManager;

    class Texture {
        friend class ResourceManager;
    public:
        Texture(ResourceManager* resourceManager, util::ResourceLocation location);
        ~Texture();

    private:
        ResourceManager* mResourceManager;
        util::ResourceLocation mLocation;
        uint16_t* mRefCount;
    };
}

#endif //UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H
