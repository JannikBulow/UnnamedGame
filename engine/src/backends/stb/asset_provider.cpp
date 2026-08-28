// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/stb/asset_provider.h"

#include "engine/util/exceptions.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstring>
#include <limits>

namespace backend {
    Image StbAssetProvider::loadImage(const char* path) {
        int width = 0;
        int height = 0;
        int channels = 0;

        constexpr int DesiredChannels = 4;

        stbi_uc* data = stbi_load(path, &width, &height, &channels, DesiredChannels);
        if (!data) {
            throw util::GameException();
        }

        size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
        size_t byteCount = pixelCount * DesiredChannels;

        unsigned char* pixels = new unsigned char[byteCount];

        Image image = {
            .width = width,
            .height = height,
            .pixels = pixels,
        };
        std::memcpy(pixels, data, byteCount);

        stbi_image_free(data);

        return image;
    }

    void StbAssetProvider::unloadImage(Image image) {
        delete[] image.pixels;
    }
}
