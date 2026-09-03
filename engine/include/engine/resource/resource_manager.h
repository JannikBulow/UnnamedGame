// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H
#define UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H

#include "engine/backend/backend.h"

#include "engine/resource/font.h"
#include "engine/resource/sound.h"
#include "engine/resource/texture.h"

#include "engine/util/object_allocator.h"

#include <unordered_map>

namespace engine {
    class ResourceManager {
        friend struct SoundResource;
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

        Font createFont(util::ResourceLocation location, int fontSize, const unicode::codepoint* codepoints = nullptr, int codePointCount = 0, std::optional<SamplerDescriptor> sampler = std::nullopt);
        Sound createSound(util::ResourceLocation location);
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

        // vile creation from hell
        struct FontKey {
            util::ResourceLocation location;
            std::optional<SamplerDescriptor> sampler;
            int size;
            const unicode::codepoint* codepoints;
            int codepointCount;

            bool operator==(const FontKey& other) const {
                return location == other.location && sampler == other.sampler && size == other.size && codepointCount == other.codepointCount && memcmp(codepoints, other.codepoints, codepointCount * sizeof(unicode::codepoint)) == 0;
            }

            struct Hash {
                size_t operator()(const FontKey& key) const {
                    auto combine = [](size_t& seed, size_t hash) {
                        seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                    };

                    size_t seed = 0;

                    combine(seed, std::hash<util::ResourceLocation>{}(key.location));
                    if (key.sampler) combine(seed, std::hash<SamplerDescriptor>{}(*key.sampler));
                    combine(seed, std::hash<int>{}(key.size));
                    if (key.codepoints) {
                        combine(seed, unicode::Hash(key.codepoints, key.codepointCount));
                        combine(seed, std::hash<int>{}(key.codepointCount));
                    }

                    return seed;
                }
            };
        };

        backend::Backend& mBackend;

        MemoryProfile mCPUMemoryProfile{};
        MemoryProfile mGPUMemoryProfile{};

        std::unordered_map<SamplerDescriptor, backend::SamplerHandle> mSamplers;

        std::unordered_map<FontKey, FontResource, FontKey::Hash> mFonts;
        std::unordered_map<util::ResourceLocation, SoundResource> mSounds;
        std::unordered_map<TextureKey, TextureResource, TextureKey::Hash> mTextures;

        backend::SamplerHandle getSampler(SamplerDescriptor desc);

        void markUsed(const FontResource& resource);
        void markUnused(const FontResource& resource);

        void markUsed(const SoundResource& resource);
        void markUnused(const SoundResource& resource);

        void markUsed(const TextureResource& resource);
        void markUnused(const TextureResource& resource);

        // these don't check for existing values or auto destroy and will cause memory leaks if used incorrectly

        void realizeFontCPU(FontResource& resource);
        void realizeFontGPU(FontResource& resource); // this specific function will check and realize the cpu font
        void evictFontCPU(FontResource& resource);
        void evictFontGPU(FontResource& resource);

        void realizeSound(SoundResource& resource);
        void evictSound(SoundResource& resource);

        void realizeTextureCPU(TextureResource& resource);
        void realizeTextureGPU(TextureResource& resource); // this will take care of the CPU texture on its own. it may or may not cache it as well
        void evictTextureCPU(TextureResource& resource);
        void evictTextureGPU(TextureResource& resource);
    };
}

#endif //UNNAMEDGAME_ENGINE_RESOURCE_RESOURCE_MANAGER_H
