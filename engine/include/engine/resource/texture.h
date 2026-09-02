// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H
#define UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H

#include "engine/backend/graphics_device.h"

#include "engine/util/resource_location.h"

#include "engine/compile_options.h"

#include <optional>

namespace engine {
    class ResourceManager;

    struct SamplerDescriptor {
        backend::TextureFilter filter;
        backend::TextureWrap wrap;
    };

    struct TextureResource {
        ResourceManager* resourceManager;

        util::ResourceLocation* location;
        SamplerDescriptor* samplerDesc;

        options::Atomic<uint32_t> strongReferences;

        std::optional<backend::Image> image;
        backend::TextureHandle textureHandle;
        backend::SamplerHandle samplerHandle;

        TextureResource(ResourceManager* resourceManager, util::ResourceLocation* location, SamplerDescriptor* samplerDesc);
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

        backend::SamplerHandle sampler() const {
            mResource->ensureGPUResidence();
            return mResource->samplerHandle;
        }

    private:
        TextureResource* mResource;
    };
}

template<>
struct std::hash<engine::SamplerDescriptor> {
    size_t operator()(const engine::SamplerDescriptor& samplerDescriptor) const {
        size_t h1 = std::hash<backend::TextureFilter>()(samplerDescriptor.filter);
        size_t h2 = std::hash<backend::TextureWrap>()(samplerDescriptor.wrap);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

#endif //UNNAMEDGAME_ENGINE_RESOURCE_TEXTURE_H
