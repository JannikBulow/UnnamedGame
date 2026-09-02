// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H
#define UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H

#include "engine/backend/backend.h"

#include "engine/resource/texture.h"

#include "engine/util/object_allocator.h"

#include <unordered_map>

namespace engine {
    class ResourceManager {
        friend class TextureResource;
    public:
        explicit ResourceManager(backend::Backend& backend);
        ~ResourceManager();

        Texture createTexture(util::ResourceLocation location);

    private:
        struct TextureKey {
            util::ResourceLocation location;
            SamplerDescriptor samplerDesc;

            bool operator==(const TextureKey& other) const {
                return samplerDesc == other.samplerDesc && location == other.location;
            }

            struct Hash {
                size_t operator()(const TextureKey& key) const {
                    size_t h1 = std::hash<util::ResourceLocation>{}(key.location);
                    size_t h2 = std::hash<SamplerDescriptor>{}(key.samplerDesc);
                    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
                }
            };
        };

        backend::Backend& mBackend;

        util::ObjectAllocator<TextureResource> mTextureAllocator;

        std::unordered_map<TextureKey, TextureResource*, TextureKey::Hash> mTextures;

        size_t mReclaimableCPUMemory = 0;
        size_t mReclaimableGPUMemory = 0;

        void markPotentiallyReclaimable(TextureResource& resource);

        void realizeTextureCPU(TextureResource& resource);
        void realizeTextureGPU(TextureResource& resource);
        void evictTextureCPU(TextureResource& resource);
        void evictTextureGPU(TextureResource& resource);
    };
}

#endif //UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H
