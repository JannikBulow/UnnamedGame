// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKEND_ASSET_PROVIDER_H

#include "engine/util/math.h"
#include "engine/util/unicode.h"

#include <cstdint>
#include <cstring>
#include <span>

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

        math::Vec2 measureText(const char* text, float fontSize, float spacing = 1.0f, float textLineSpacing = 2.0f) const {
            if (!text || !text[0]) return math::Vec2::Zero();

            size_t textLength = strlen(text);
            size_t tempByteCounter = 0;
            size_t byteCounter = 0;

            float textWidth = 0.0f;
            float tempTextWidth = 0.0f;
            float textHeight = fontSize;

            float scale = fontSize / static_cast<float>(baseSize);

            unicode::codepoint codepoint = 0;
            int index = 0;

            for (size_t i = 0; i < textLength;) {
                byteCounter++;

                int codepointSize = 0;
                codepoint = unicode::GetNextCodepoint(text + i, &codepointSize);
                index = getGlyphIndex(codepoint);

                const Glyph& glyph = glyphs[index];

                i += codepointSize;

                if (codepoint != '\n') {
                    if (glyph.advanceX > 0) textWidth += glyph.advanceX;
                    else textWidth += glyph.atlasBounds.width() + glyph.offsetX;
                } else {
                    if (textWidth > tempTextWidth) tempTextWidth = textWidth;
                    byteCounter = 0;
                    textWidth = 0.0f;

                    textHeight += fontSize + textLineSpacing;
                }

                if (byteCounter > tempByteCounter) tempByteCounter = byteCounter;
            }

            if (textWidth > tempTextWidth) tempTextWidth = textWidth;

            return {
                tempTextWidth * scale + static_cast<float>(tempByteCounter - 1) * scale,
                textHeight
            };
        }

        math::Vec2 measureCodepoints(std::span<const unicode::codepoint> codepoints, float fontSize, float spacing = 1.0f, float textLineSpacing = 2.0f) const {
            if (codepoints.empty()) return math::Vec2::Zero();

            float textWidth = 0.0f;
            float tempTextWidth = 0.0f;

            int tempGlyphCounter = 0;
            int glyphCounter = 0;

            float textHeight = fontSize;
            float scale = fontSize / static_cast<float>(baseSize);

            for (unicode::codepoint codepoint : codepoints) {
                const Glyph& glyph = glyphs[getGlyphIndex(codepoint)];

                if (codepoint != '\n') {
                    glyphCounter++;

                    if (glyph.advanceX > 0) textWidth += glyph.advanceX;
                    else textWidth += glyph.atlasBounds.width() + glyph.offsetX;
                } else {
                    if (textWidth > tempTextWidth) tempTextWidth = textWidth;

                    textWidth = 0.0f;
                    glyphCounter = 0;

                    textHeight += fontSize + textLineSpacing;
                }

                if (glyphCounter > tempGlyphCounter) tempGlyphCounter = glyphCounter;
            }

            if (textWidth > tempTextWidth) tempTextWidth = textWidth;

            return {
                tempTextWidth * scale + static_cast<float>(tempGlyphCounter - 1) * spacing,
                textHeight
            };
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
