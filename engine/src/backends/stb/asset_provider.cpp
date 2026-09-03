// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/stb/asset_provider.h"

#include "engine/util/exceptions.h"

#include <miniaudio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <cstring>
#include <memory>

namespace backend {
    StbAssetProvider::StbAssetProvider() {
        stbi_set_flip_vertically_on_load(true);
    }

    //TODO: figure out this. we're using miniaudio in the stb provider..
    Audio StbAssetProvider::loadAudio(const char* path) {
        ma_decoder decoder;
        ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
        if (ma_decoder_init_file(path, &config, &decoder) != MA_SUCCESS) {
            throw util::GameException("failed to load audio file");
        }

        ma_uint64 frameCount;
        float* decoded = nullptr;

        ma_result result = ma_decode_file(path, &config, &frameCount, reinterpret_cast<void**>(&decoded));
        if (result != MA_SUCCESS) {
            ma_decoder_uninit(&decoder);
            throw util::GameException("failed to decode audio");
        }

        Audio audio{};
        audio.channels = decoder.outputChannels;
        audio.sampleRate = decoder.outputSampleRate;
        audio.frameCount = frameCount;
        audio.samples = decoded;

        ma_decoder_uninit(&decoder);

        return audio;
    }

    void StbAssetProvider::unloadAudio(Audio audio) {
        ma_free(audio.samples, nullptr);
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
        };
        std::memcpy(pixels, data, byteCount);

        stbi_image_free(data);

        return image;
    }

    void StbAssetProvider::unloadImage(Image image) {
        delete[] image.pixels;
    }

    Font StbAssetProvider::loadFont(const char* path, int size, const unicode::codepoint* codepoints, int codepointCount) {
        static constexpr unicode::codepoint defaultCodepoints[] = {
            // Printable ASCII
            32, 33, 34, 35, 36, 37, 38, 39,
            40, 41, 42, 43, 44, 45, 46, 47,
            48, 49, 50, 51, 52, 53, 54, 55,
            56, 57, 58, 59, 60, 61, 62, 63,
            64, 65, 66, 67, 68, 69, 70, 71,
            72, 73, 74, 75, 76, 77, 78, 79,
            80, 81, 82, 83, 84, 85, 86, 87,
            88, 89, 90, 91, 92, 93, 94, 95,
            96, 97, 98, 99, 100, 101, 102, 103,
            104, 105, 106, 107, 108, 109, 110, 111,
            112, 113, 114, 115, 116, 117, 118, 119,
            120, 121, 122, 123, 124, 125, 126
        };

        if (!codepoints || codepointCount == 0) {
            codepoints = defaultCodepoints;
            codepointCount = sizeof(defaultCodepoints) / sizeof(defaultCodepoints[0]);
        }

        FILE* file = fopen(path, "rb");
        if (!file) {
            throw util::GameException("failed to open font file");
        }

        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        uint8_t* fontData = new uint8_t[fileSize];

        if (fread(fontData, 1, fileSize, file) != fileSize) {
            delete[] fontData;
            fclose(file);
            throw util::GameException("failed to read font file");
        }

        fclose(file);

        stbtt_fontinfo fontInfo;
        if (!stbtt_InitFont(&fontInfo, fontData, 0)) {
            delete[] fontData;
            throw util::GameException("failed to init font");
        }

        int atlasWidth = 256;
        int atlasHeight = 256;

        uint8_t* pixels = nullptr;
        stbtt_packedchar* packedChars = nullptr;

        bool packed = false;

        while (!packed) {
            delete[] pixels;
            delete[] packedChars;

            pixels = new uint8_t[atlasWidth * atlasHeight];
            std::memset(pixels, 0, atlasWidth * atlasHeight);

            packedChars = new stbtt_packedchar[codepointCount];

            stbtt_pack_context context;
            if (!stbtt_PackBegin(&context, pixels, atlasWidth, atlasHeight, 0, 1, nullptr)) {
                delete[] fontData;
                delete[] pixels;
                delete[] packedChars;
                throw util::GameException("failed to init font packer");
            }

            stbtt_PackSetOversampling(&context, 1, 1);

            // fuck stbtt
            auto mutableCodepoints = std::make_unique<int[]>(codepointCount);
            std::memcpy(mutableCodepoints.get(), codepoints, codepointCount * sizeof(int));

            stbtt_pack_range range = {
                .font_size = static_cast<float>(size),
                .first_unicode_codepoint_in_range = 0,
                .array_of_unicode_codepoints = mutableCodepoints.get(),
                .num_chars = codepointCount,
                .chardata_for_range = packedChars
            };

            packed = stbtt_PackFontRanges(&context, fontData, 0, &range, 1);
            stbtt_PackEnd(&context);

            if (!packed) {
                atlasWidth *= 2;
                atlasHeight *= 2;

                if (atlasWidth > 4096) {
                    delete[] fontData;
                    delete[] pixels;
                    delete[] packedChars;
                    throw util::GameException("huge font atlas");
                }
            }
        }

        delete[] fontData;

        Glyph* glyphs = new Glyph[codepointCount];
        for (int i = 0; i < codepointCount; i++) {
            const stbtt_packedchar& packedChar = packedChars[i];

            glyphs[i] = {
                .codepoint = codepoints[i],
                .offsetX = packedChar.xoff,
                .offsetY = -packedChar.yoff,
                .advanceX = packedChar.xadvance,
                .atlasBounds = {
                    static_cast<float>(packedChar.x0),
                    static_cast<float>(packedChar.x1),
                    static_cast<float>(packedChar.y0),
                    static_cast<float>(packedChar.y1)
                }
            };
        }

        delete[] packedChars;

        return {
            .baseSize = size,
            .glyphCount = codepointCount,
            .atlas = {
                .width = atlasWidth,
                .height = atlasHeight,
                .format = ImageFormat::R8,
                .pixels = pixels
            },
            .glyphs = glyphs
        };
    }

    void StbAssetProvider::unloadFont(Font font) {
        unloadImage(font.atlas);
        delete[] font.glyphs;
    }
}
