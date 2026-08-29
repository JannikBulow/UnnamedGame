// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/stb/asset_provider.h"

#include "engine/util/exceptions.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstring>
#include <limits>

namespace backend {
    StbAssetProvider::StbAssetProvider() {
        stbi_set_flip_vertically_on_load(true);
    }

    Image StbAssetProvider::loadImage(const char* path) {
        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_uc* data = stbi_load(path, &width, &height, &channels, 0);
        if (!data) {
            throw util::GameException(stbi_failure_reason());
        }

        ImageFormat format;
        switch (channels) {
            case 1:
                format = ImageFormat::R8;
                break;
            case 3:
                format = ImageFormat::RGB8;
                break;
            case 4:
                format = ImageFormat::RGBA8;
                break;

            default:
                stbi_image_free(data);
                throw util::GameException("unsupported image format");
        }

        size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
        size_t byteCount = pixelCount * channels;

        uint8_t* pixels = new uint8_t[byteCount];

        Image image = {
            .width = width,
            .height = height,
            .format = format,
            .pixels = pixels,
            .byteCount = byteCount
        };
        std::memcpy(pixels, data, byteCount);

        stbi_image_free(data);

        return image;
    }

    void StbAssetProvider::unloadImage(Image image) {
        delete[] image.pixels;
    }
}
