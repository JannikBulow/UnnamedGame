// Copyright 2026 Jannik Laugmand Bülow

#include "engine/util/resource_location.h"

#include <cstring>
#include <filesystem>

namespace util {
    using namespace std::string_view_literals;

    ResourceLocation::ResourceLocation(std::string_view resource)
        : mPath(CreatePath(UNNAMEDGAME_RL_DEFAULT_DOMAINsv, resource)) {}

    ResourceLocation::ResourceLocation(std::string_view domain, std::string_view resource)
        : mPath(CreatePath(domain, resource)) {}

    bool ResourceLocation::operator==(const ResourceLocation& other) const {
        return strcmp(mPath.get(), other.mPath.get()) == 0;
    }

    const char* ResourceLocation::cstr() const {
        return mPath.get();
    }

    std::unique_ptr<const char[]> ResourceLocation::CreatePath(std::string_view domain, std::string_view resource) {
        int needSep = domain.ends_with(std::filesystem::path::preferred_separator) ? 1 : 0;
        size_t size = domain.size() + needSep + resource.size();
        auto path = std::make_unique<char[]>(size);

        memcpy(path.get(), domain.data(), size);
        if (needSep) path[domain.size()] = std::filesystem::path::preferred_separator;
        memcpy(path.get() + domain.size() + needSep, resource.data(), resource.size());
        path[size] = '\0';

        return path;
    }
}
