// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H
#define UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H

#include "engine/backend/graphics_device.h"

#include "engine/util/resource_location.h"

#include "engine/compile_options.h"

#include <optional>

namespace engine {
    class ResourceManager;

    struct TextureResource {
        ResourceManager* resourceManager;
        util::ResourceLocation location;
        options::Atomic<uint32_t> strongReferences;

        std::optional<backend::Image> image;
        backend::TextureHandle textureHandle;

        TextureResource(ResourceManager* resourceManager, util::ResourceLocation location);
        ~TextureResource();

        void addStrongReference();
        void removeStrongReference();

        void ensureCPUResidence(); // ensure the image is valid in case the resourcemanager has evicted it
        void ensureGPUResidence(); // ensure the texture handle is valid in case the resourcemanager has evicted it
    };

    class Texture {
    public:
        explicit Texture(TextureResource* resource)
            : mResource(resource) {
            mResource->addStrongReference();
        }

        Texture(const Texture& other)
            : mResource(other.mResource) {
            mResource->addStrongReference();
        }

        Texture(Texture&& other) noexcept
            : mResource(other.mResource) {
            other.mResource = nullptr;
        }

        ~Texture() {
            if (mResource) mResource->removeStrongReference();
        }

        Texture& operator=(const Texture& other) {
            if (&other != this) {
                if (mResource) mResource->removeStrongReference();
                mResource = other.mResource;
                mResource->addStrongReference();
            }
            return *this;
        }

        Texture& operator=(Texture&& other) noexcept {
            if (&other != this) {
                if (mResource) mResource->removeStrongReference();
                mResource = other.mResource;
                other.mResource = nullptr;
            }
            return *this;
        }

        backend::Image image() const {
            mResource->ensureCPUResidence();
            return mResource->image.value();
        }
        
        backend::TextureHandle handle() const {
            mResource->ensureGPUResidence();
            return mResource->textureHandle;
        }

    private:
        TextureResource* mResource;
    };
}

#endif //UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H
