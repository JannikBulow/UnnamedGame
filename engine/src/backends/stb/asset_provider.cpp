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

        stbi_uc* data = stbi_load(path, &width, &height, &channels, 4);
        if (!data) {
            throw util::GameException();
        }
    }

    void StbAssetProvider::unloadImage(Image image) {}
}
