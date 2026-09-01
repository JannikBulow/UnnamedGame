// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H

#include "engine/util/math.h"
#include "engine/util/unicode.h"

#include <cstdint>

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
        unicode::codepoint codepoint; // unicode
        float offsetX;
        float offsetY;
        float advanceX;
        math::Rect atlasBounds;
    };

    struct Font {
        int baseSize;
        int glyphCount;
        Image atlas;
        Glyph* glyphs;

        int getGlyphIndex(unicode::codepoint codepoint) const {
            int index = 0;
            int fallbackIndex = 0;

            for (int i = 0; i < glyphCount; i++) {
                if (glyphs[i].codepoint == '?') fallbackIndex = i;

                if (glyphs[i].codepoint == codepoint) {
                    index = i;
                    break;
                }
            }

            if (index == 0 && glyphs[0].codepoint != codepoint) index = fallbackIndex;
            return index;
        }
    };

    class IAssetProvider {
    public:
        virtual ~IAssetProvider() = default;

        virtual Image loadImage(const char* path) = 0;
        virtual void unloadImage(Image image) = 0;

        virtual Font loadFont(const char* path, int size, const unicode::codepoint* codepoints = nullptr, int codepointCount = 0) = 0;
        virtual void unloadFont(Font font) = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H
