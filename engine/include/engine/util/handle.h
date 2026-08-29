// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_HANDLE_H
#define UNNAMEDGAME_ENGINE_UTIL_HANDLE_H

#include "engine/util/exceptions.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <strings.h>
#include <vector>

namespace util {
    template<class T>
    class Handle {
    public:
        uint32_t index;
        uint32_t generation;

        Handle(std::nullptr_t = nullptr) : index(0xFFFFFFFF), generation(0xFFFFFFFF) {}
        Handle(uint32_t index, uint32_t generation) : index(index), generation(generation) {}

        bool operator==(std::nullptr_t) const { return index == 0xFFFFFFFF && generation == 0xFFFFFFFF; }
        bool operator==(Handle other) const { return index == other.index && generation == other.generation; }

        explicit operator bool() const { return *this != nullptr; }
    };

    template<class T, class H = T>
    class HandleStorage {
    public:
        template<class... Args>
        Handle<H> create(Args&&... args) {
            uint32_t index;

            if (!mFreeList.empty()) {
                index = mFreeList.back();
                mFreeList.pop_back();
                mValues[index].value.emplace(std::forward<Args>(args)...);
            } else {
                index = mValues.size();
                mValues.emplace_back(Slot(std::optional<T>(std::in_place, std::forward<Args>(args)...), 0));
            }

            return {index, mValues[index].generation};
        }

        void destroy(Handle<H> handle) {
            if (handle.index >= mValues.size()) throw GameException();

            Slot& slot = mValues[handle.index];
            if (slot.generation != handle.generation) throw GameException();

            slot.value.reset();
            slot.generation += 1;
            mFreeList.push_back(handle.index);
        }

        T& get(Handle<H> handle) {
            if (handle.index >= mValues.size()) throw GameException();

            Slot& slot = mValues[handle.index];
            if (slot.generation != handle.generation) throw GameException();
            if (!slot.value.has_value()) throw GameException();

            return slot.value.value();
        }

    private:
        struct Slot {
            std::optional<T> value;
            uint32_t generation;
        };

        std::vector<Slot> mValues;
        std::vector<uint32_t> mFreeList;
    };
}

#endif //UNNAMEDGAME_ENGINE_UTIL_HANDLE_H
