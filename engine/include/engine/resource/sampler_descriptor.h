// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RESOURCE_SAMPLER_DESCRIPTOR_H
#define UNNAMEDGAME_ENGINE_RESOURCE_SAMPLER_DESCRIPTOR_H

#include "engine/backend/graphics_device.h"

namespace engine {
    struct SamplerDescriptor {
        backend::TextureFilter filter;
        backend::TextureWrap wrap;

        bool operator==(const SamplerDescriptor& other) const {
            return filter == other.filter && wrap == other.wrap;
        }
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

#endif //UNNAMEDGAME_ENGINE_RESOURCE_SAMPLER_DESCRIPTOR_H
