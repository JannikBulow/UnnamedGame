// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H

#include "engine/util/math.h"

#include <cstdint>
#include <cstddef>

namespace backend {
    enum class ImageFormat {
        R8,
        RGB8,
        RGBA8,
    };

    struct Image {
        int width;
        int height;
        ImageFormat format;
        uint8_t* pixels;
    };

    struct Glyph {
        uint32_t codepoint; // unicode
        int offsetX;
        int offsetY;
        int advanceX;
        math::Rect atlasBounds;
    };

    struct Font {
        int baseSize;
        int glyphCount;
        Image atlas;
        Glyph* glyphs;
    };

    class IAssetProvider {
    public:
        virtual ~IAssetProvider() = default;

        virtual Image loadImage(const char* path) = 0;
        virtual void unloadImage(Image image) = 0;

        virtual Font loadFont(const char* path, int size) = 0;
        virtual void unloadFont(Font font) = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H
