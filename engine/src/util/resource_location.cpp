// Copyright 2026 Jannik Laugmand Bülow

#include "engine/util/resource_location.h"

#include <filesystem>

namespace util {
    using namespace std::string_view_literals;

    ResourceLocation::ResourceLocation(std::string_view resource)
        : mPath(std::filesystem::path(UNNAMEDGAME_RL_DEFAULT_DOMAINsv) / resource) {}

    ResourceLocation::ResourceLocation(std::string_view domain, std::string_view resource)
        : mPath(std::filesystem::path(domain) / resource) {}

    const char* ResourceLocation::cstr() const {
        return mPath.c_str();
    }
}
