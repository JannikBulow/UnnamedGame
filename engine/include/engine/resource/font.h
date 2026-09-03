// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_FONT_H
#define UNNAMEDGAME_ENGINE_RESOURCE_FONT_H

#include "engine/backend/graphics_device.h"

#include "engine/resource/sampler_descriptor.h"

#include "engine/util/resource_location.h"

#include "engine/compile_options.h"

#include <optional>

namespace engine {
    class ResourceManager;

    struct FontResource {
        ResourceManager* resourceManager;

        const util::ResourceLocation* location;
        const SamplerDescriptor* samplerDesc;
        int fontSize;
        const unicode::codepoint* codepoints;
        int codePointCount;

        options::Atomic<uint32_t> strongReferences;

        std::optional<backend::Font> font;
        backend::TextureHandle textureHandle;
        backend::SamplerHandle samplerHandle; // not owned!!!

        size_t estimatedTextureSize = 0; // has a value when textureHandle does. is garbage value otherwise

        explicit FontResource(ResourceManager* resourceManager);
        ~FontResource();

        void addStrongReference();
        void removeStrongReference();

        void ensureCPUResidence(); // ensure the image is valid in case the resourcemanager has evicted it
        void ensureGPUResidence(); // ensure the texture handle is valid in case the resourcemanager has evicted it
    };

    class Font {
    public:
        explicit Font(FontResource* resource)
            : mResource(resource) {
            mResource->addStrongReference();
        }

        Font(const Font& other)
            : mResource(other.mResource) {
            mResource->addStrongReference();
        }

        Font(Font&& other) noexcept
            : mResource(other.mResource) {
            other.mResource = nullptr;
        }

        ~Font() {
            if (mResource) mResource->removeStrongReference();
        }

        Font& operator=(const Font& other) {
            if (&other != this) {
                if (mResource) mResource->removeStrongReference();
                mResource = other.mResource;
                mResource->addStrongReference();
            }
            return *this;
        }

        Font& operator=(Font&& other) noexcept {
            if (&other != this) {
                if (mResource) mResource->removeStrongReference();
                mResource = other.mResource;
                other.mResource = nullptr;
            }
            return *this;
        }

        backend::Font font() const {
            mResource->ensureCPUResidence();
            return mResource->font.value();
        }

        backend::Image fontAtlas() const {
            mResource->ensureCPUResidence();
            return mResource->font->atlas;
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
        FontResource* mResource;
    };
}

#endif //UNNAMEDGAME_ENGINE_RESOURCE_FONT_H
