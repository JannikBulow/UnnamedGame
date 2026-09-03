// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_UNICODE_H
#define UNNAMEDGAME_ENGINE_UTIL_UNICODE_H

#include <cstddef>
#include <cstdint>

namespace unicode {
    using codepoint = uint32_t;

    codepoint GetNextCodepoint(const char* text, int* codepointSize);

    size_t Hash(const codepoint* codepoints, size_t codepointCount);
}

#endif //UNNAMEDGAME_ENGINE_UTIL_UNICODE_H
