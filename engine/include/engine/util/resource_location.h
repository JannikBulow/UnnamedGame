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

        bool operator==(const ResourceLocation& other) const;

        const char* cstr() const;

    private:
        std::unique_ptr<const char[]> mPath;

        static std::unique_ptr<const char[]> CreatePath(std::string_view domain, std::string_view resource);
    };
}

template<>
struct std::hash<util::ResourceLocation> {
    size_t operator()(const util::ResourceLocation& location) const {
        constexpr size_t basis = sizeof(size_t) == 8 ? 14695981039346656037ULL : 2166136261u;
        constexpr size_t prime = sizeof(size_t) == 8 ? 1099511628211ULL : 16777216u;

        const unsigned char* s = reinterpret_cast<const unsigned char*>(location.cstr());
        size_t h = basis;

        while (*s) {
            h ^= static_cast<unsigned char>(*s++);
            h *= prime;
        }

        return h;
    }
};

#endif //UNNAMEDGAME_ENGINE_UTIL_RESOURCE_LOCATION_H
