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
        struct Slot {
            std::optional<T> value;
            uint32_t generation;
        };

    public:
        template<class C, class It>
        class IteratorBase {
            friend class HandleStorage;
        public:
            C& operator*() { return mIterator->value.value(); }
            C* operator->() { return &mIterator->value.value(); }
            IteratorBase& operator++() {
                while (++mIterator != mEnd && !mIterator->value.has_value()) {}
                return *this;
            }

            bool operator==(const IteratorBase& other) const { return mIterator == other.mIterator; }
            bool operator!=(const IteratorBase& other) const { return !(*this == other); }

        private:
            It mIterator;
            It mEnd;

            IteratorBase(It it, It end) : mIterator(it), mEnd(end) {}
        };

        using Iterator = IteratorBase<T, typename std::vector<Slot>::iterator>;
        using ConstIterator = IteratorBase<const T, typename std::vector<Slot>::const_iterator>;

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

        Iterator begin() { return {mValues.begin(), mValues.end()}; }
        Iterator end() { return {mValues.end(), mValues.end()}; }
        ConstIterator begin() const { return {mValues.begin(), mValues.end()}; }
        ConstIterator end() const { return {mValues.end(), mValues.end()}; }
        ConstIterator cbegin() const { return {mValues.cbegin(), mValues.cbegin()}; }
        ConstIterator cend() const { return {mValues.cend(), mValues.cend()}; }

    private:
        std::vector<Slot> mValues;
        std::vector<uint32_t> mFreeList;
    };
}

#endif //UNNAMEDGAME_ENGINE_UTIL_HANDLE_H
