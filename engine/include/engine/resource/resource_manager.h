// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H
#define UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H

#include "engine/backend/backend.h"

#include "engine/resource/texture.h"

#include "engine/util/object_allocator.h"

#include <unordered_map>

namespace engine {
    class ResourceManager {
        friend struct TextureResource;
    public:
        struct MemoryProfile {
            size_t used = 0;
            size_t limit = 0;
            size_t potentialReclaimable = 0;

            double memoryPressure() const {
                return static_cast<double>(used) / static_cast<double>(limit);
            }

            double memoryPressureAfterAllocation(size_t allocationSize) const {
                return static_cast<double>(used + allocationSize) / static_cast<double>(limit);
            }
        };

        explicit ResourceManager(backend::Backend& backend);
        ~ResourceManager();

        const MemoryProfile& cpuMemoryProfile() const { return mCPUMemoryProfile; }
        const MemoryProfile& gpuMemoryProfile() const { return mGPUMemoryProfile; }

        Texture createTexture(util::ResourceLocation location, std::optional<SamplerDescriptor> sampler = std::nullopt);

    private:
        struct TextureKey {
            util::ResourceLocation location;
            std::optional<SamplerDescriptor> samplerDesc;

            bool operator==(const TextureKey& other) const {
                return samplerDesc == other.samplerDesc && location == other.location;
            }

            struct Hash {
                size_t operator()(const TextureKey& key) const {
                    size_t h1 = std::hash<util::ResourceLocation>{}(key.location);
                    if (!key.samplerDesc) return h1;
                    size_t h2 = std::hash<SamplerDescriptor>{}(*key.samplerDesc);
                    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
                }
            };
        };

        backend::Backend& mBackend;

        MemoryProfile mCPUMemoryProfile{};
        MemoryProfile mGPUMemoryProfile{};

        std::unordered_map<SamplerDescriptor, backend::SamplerHandle> mSamplers;

        std::unordered_map<TextureKey, TextureResource, TextureKey::Hash> mTextures;

        backend::SamplerHandle getSampler(SamplerDescriptor desc);

        void markUsed(const TextureResource& resource);
        void markUnused(const TextureResource& resource);

        // these don't check for existing values or auto destroy and will cause memory leaks if used incorrectly

        void realizeTextureCPU(TextureResource& resource);
        void realizeTextureGPU(TextureResource& resource); // this will take care of the CPU texture on its own. it may or may not cache it as well
        void evictTextureCPU(TextureResource& resource);
        void evictTextureGPU(TextureResource& resource);
    };
}

#endif //UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H
