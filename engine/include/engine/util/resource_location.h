// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_RESOURCE_LOCATION_H
#define UNNAMEDGAME_ENGINE_UTIL_RESOURCE_LOCATION_H

#include <filesystem>
#include <memory>

namespace util {
    class ResourceLocation {
    public:
        explicit ResourceLocation(std::string_view resource);
        ResourceLocation(std::string_view domain, std::string_view resource);

        const char* cstr() const;

    private:
        std::unique_ptr<const char[]> mPath;

        static std::unique_ptr<const char[]> CreatePath(std::string_view domain, std::string_view resource);
    };
}

#endif //UNNAMEDGAME_ENGINE_UTIL_RESOURCE_LOCATION_H
