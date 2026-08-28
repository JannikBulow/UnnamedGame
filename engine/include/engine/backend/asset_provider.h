// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H

#include <cstdint>

namespace backend {
    // rgba8
    struct Image {
        int width;
        int height;
        const uint8_t* pixels;
    };

    class IAssetProvider {
    public:
        virtual ~IAssetProvider() = default;

        virtual Image loadImage(const char* path) = 0;
        virtual void unloadImage(Image image) = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H
