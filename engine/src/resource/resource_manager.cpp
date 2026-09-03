// Copyright 2026 Jannik Laugmand Bülow

#include "engine/resource/resource_manager.h"

namespace engine {
    ResourceManager::ResourceManager(backend::Backend& backend)
        : mBackend(backend) {}

    ResourceManager::~ResourceManager() {
        for (auto& [desc, sampler] : mSamplers) {
            mBackend.gpu.destroySampler(sampler);
        }
    }

    Sound ResourceManager::createSound(util::ResourceLocation location) {
        auto it = mSounds.find(location);
        if (it != mSounds.end()) return Sound(&it->second);

        auto [it2, success] = mSounds.emplace(std::move(location), this);
        if (!success) throw util::GameException();

        it2->second.location = &it2->first;

        return Sound(&it2->second);
    }

    Texture ResourceManager::createTexture(util::ResourceLocation location, std::optional<SamplerDescriptor> sampler) {
        TextureKey key(std::move(location), std::move(sampler));
        auto it = mTextures.find(key);
        if (it != mTextures.end()) return Texture(&it->second);

        auto [it2, success] = mTextures.emplace(std::move(key), this);
        if (!success) throw util::GameException();

        it2->second.location = &it2->first.location;
        if (it2->first.samplerDesc) it2->second.samplerDesc = &*it2->first.samplerDesc;

        return Texture(&it2->second);
    }

    backend::SamplerHandle ResourceManager::getSampler(SamplerDescriptor desc) {
        auto it = mSamplers.find(desc);
        if (it != mSamplers.end()) return it->second;

        backend::SamplerHandle sampler = mBackend.gpu.createSampler(desc.filter, desc.wrap);
        auto [it2, success] = mSamplers.emplace(desc, sampler);
        if (!success) throw util::GameException();

        return sampler;
    }

    void ResourceManager::markUsed(const SoundResource& resource) {
        if (resource.audio) mCPUMemoryProfile.potentialReclaimable -= resource.audio->getSizeBytes();
    }

    void ResourceManager::markUnused(const SoundResource& resource) {
        if (resource.audio) mCPUMemoryProfile.potentialReclaimable += resource.audio->getSizeBytes();
    }

    void ResourceManager::markUsed(const TextureResource& resource) {
        if (resource.image) mCPUMemoryProfile.potentialReclaimable -= resource.image->getSizeBytes();
        if (resource.textureHandle) mGPUMemoryProfile.potentialReclaimable -= resource.estimatedTextureSize;
    }

    void ResourceManager::markUnused(const TextureResource& resource) {
        if (resource.image) mCPUMemoryProfile.potentialReclaimable += resource.image->getSizeBytes();
        if (resource.textureHandle) mGPUMemoryProfile.potentialReclaimable += resource.estimatedTextureSize;
    }

    void ResourceManager::realizeSound(SoundResource& resource) {
        resource.audio = mBackend.assetProvider.loadAudio(resource.location->cstr());
        mCPUMemoryProfile.used += resource.audio->getSizeBytes();
    }

    void ResourceManager::evictSound(SoundResource& resource) {
        mBackend.assetProvider.unloadAudio(*resource.audio);
        resource.audio = std::nullopt;
    }

    void ResourceManager::realizeTextureCPU(TextureResource& resource) {
        resource.image = mBackend.assetProvider.loadImage(resource.location->cstr());
        mCPUMemoryProfile.used += resource.image->getSizeBytes();
    }

    void ResourceManager::realizeTextureGPU(TextureResource& resource) {
        backend::Image image;
        bool cachedImage;
        if (!resource.image) {
            image = mBackend.assetProvider.loadImage(resource.location->cstr());
            cachedImage = false;
        } else {
            image = *resource.image;
            cachedImage = true;
        }

        size_t imageSizeBytes = image.getSizeBytes();

        if (!cachedImage && mCPUMemoryProfile.memoryPressureAfterAllocation(imageSizeBytes) < 0.7) {
            resource.image = image;
            mCPUMemoryProfile.used += imageSizeBytes;
            cachedImage = true;
        }

        resource.textureHandle = mBackend.gpu.createTexture(image);
        resource.estimatedTextureSize = imageSizeBytes;

        if (!cachedImage) {
            mBackend.assetProvider.unloadImage(image);
        }

        mGPUMemoryProfile.used += resource.estimatedTextureSize;
    }

    void ResourceManager::evictTextureCPU(TextureResource& resource) {
        mBackend.assetProvider.unloadImage(*resource.image);
        resource.image = std::nullopt;
    }

    void ResourceManager::evictTextureGPU(TextureResource& resource) {
        mBackend.gpu.destroyTexture(resource.textureHandle);
        resource.textureHandle = nullptr;
        resource.estimatedTextureSize = 0; // safety to catch bugs from using texture size after evict
    }
}
