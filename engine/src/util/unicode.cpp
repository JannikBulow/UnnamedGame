// Copyright 2026 Jannik Laugmand Bülow

#include "engine/util/unicode.h"

namespace unicode {
    codepoint GetNextCodepoint(const char* text, int* codepointSize) {
        *codepointSize = 1;
        if (!text) return '?';

        if ((text[0] & 0xF8) == 0xF0) {
            if (((text[1] & 0xC0) ^ 0x80) || ((text[2] & 0xC0) ^ 0x80) || ((text[3] & 0xC0) ^ 0x80)) return '?';
            *codepointSize = 4;
            return ((text[0] & 0x07) << 18) | ((text[1] & 0x3F) << 12) | ((text[2] & 0x3F) << 6) | (text[3] & 0x3F);
        }
        if ((text[0] & 0xF0) == 0xE0) {
            if (((text[1] & 0xC0) ^ 0x80) || ((text[2] & 0xC0) ^ 0x80)) return '?';
            *codepointSize = 3;
            return ((text[0] & 0x0F) << 12) | ((text[1] & 0x3F) << 6) | (text[2] & 0x3F);
        }
        if ((text[0] & 0xE0) ==  0xC0) {
            if (((text[1] & 0xC0) ^ 0x80)) return '?';
            *codepointSize = 2;
            return ((text[0] & 0X1F) << 6) | (text[1] & 0x3F);
        }
        if ((text[0] & 0x80) == 0x00) {
            *codepointSize = 1;
            return text[0];
        }

        return '?';
    }

    size_t Hash(const codepoint* codepoints, size_t codepointCount) {
        constexpr size_t basis = sizeof(size_t) == 8 ? 14695981039346656037ULL : 2166136261u;
        constexpr size_t prime = sizeof(size_t) == 8 ? 1099511628211ULL : 16777216u;

        size_t h = basis;

        for (size_t i = 0; i < codepointCount; i++) {
            for (int j = 0; i < sizeof(codepoint); j++) {
                unsigned char byte = static_cast<unsigned char>(codepoints[i] >> (i * 8));
                h ^= byte;
                h *= prime;
            }
        }

        return h;
    }
}
