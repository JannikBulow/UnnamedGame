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
}
