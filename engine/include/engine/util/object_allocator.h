// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_OBJECT_ALLOCATOR_H
#define UNNAMEDGAME_ENGINE_UTIL_OBJECT_ALLOCATOR_H

#include "engine/util/exceptions.h"

#include <cmath>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

namespace util {
    template<class T, size_t ObjectsPerSlab = 1024>
    requires (!std::is_array_v<T>)
    class ObjectAllocator {
    public:
        ObjectAllocator() = default;
        ObjectAllocator(const ObjectAllocator&) = delete;
        ObjectAllocator& operator=(const ObjectAllocator&) = delete;

        ObjectAllocator(ObjectAllocator&& other) noexcept
            : mFree(std::exchange(other.mFree, nullptr))
            , mSlabs(std::exchange(other.mSlabs, nullptr)) {}

        ObjectAllocator& operator=(ObjectAllocator&& other) noexcept {
            if (&other != this) {
                this.~ObjectAllocator();
                mFree = std::exchange(other.mFree, nullptr);
                mSlabs = std::exchange(other.mSlabs, nullptr);
            }
            return *this;
        }

        ~ObjectAllocator() {
            while (mSlabs) {
                Slab* next = mSlabs->next;

                ::operator delete(mSlabs, std::align_val_t(alignof(Slab)));

                mSlabs = next;
            }
        }

        template<class... Args>
        T* create(Args&&... args) {
            if (!mFree) grow();

            Node* node = mFree;
            mFree = node->next;

            T* object = std::construct_at(reinterpret_cast<T*>(node), std::forward<Args>(args)...);

            auto [slab, index] = locate(object);
            slab->occupied[index / 64] |= static_cast<uintptr_t>(1) << (index % 64);

            return object;
        }

        void destroy(T* object) {
            auto [slab, index] = locate(object);

            std::destroy_at(object);

            slab->occupied[index / 64] &= ~(static_cast<uintptr_t>(1) << (index % 64));

            auto* node = reinterpret_cast<Node*>(object);
            node->next = mFree;
            mFree = node;
        }

        template<class F>
        void iterateObjects(F&& func) {
            using Result = std::invoke_result_t<F&, T&>;

            static_assert(std::is_void_v<Result> || std::is_same_v<Result, bool>);

            for (Slab* slab = mSlabs; slab; slab = slab->next) {
                for (size_t word = 0; word < BitmapWords; word++) {
                    uintptr_t bits = slab->occupied[word];

                    while (bits) {
                        unsigned bit = std::countr_zero(bits);
                        uintptr_t mask = static_cast<uintptr_t>(1) << bit;

                        bits &= ~mask;

                        size_t index = word * 64 + bit;

                        T* object = std::launder(reinterpret_cast<T*>(slab->storage + index * Stride));

                        if constexpr (std::is_void_v<Result>) {
                            std::invoke(func, *object);
                        } else {
                            if (std::invoke(func, *object)) {
                                std::destroy_at(object);

                                auto* node = reinterpret_cast<Node*>(object);
                                node->next = mFree;
                                mFree = node;

                                slab->occupied[word] &= ~mask;
                            }
                        }

                        bits &= bits - 1;
                    }
                }
            }
        }

    private:
        struct Node {
            Node* next;
        };

        static constexpr size_t Align = std::max(alignof(T), alignof(Node));
        static constexpr size_t Stride = (std::max(sizeof(T), sizeof(Node)) + Align - 1) / Align * Align;
        static constexpr size_t BitsPerWord = sizeof(uintptr_t) * 8;
        static constexpr size_t BitmapWords = (ObjectsPerSlab + BitsPerWord - 1) / BitsPerWord;

        struct Slab {
            Slab* next;
            uintptr_t occupied[BitmapWords];
            alignas(T) std::byte storage[Stride * ObjectsPerSlab];
        };

        Node* mFree = nullptr;
        Slab* mSlabs = nullptr;

        Node* grow() {
            auto* slab = static_cast<Slab*>(::operator new(sizeof(Slab), std::align_val_t(alignof(Slab))));
            slab->next = mSlabs;
            mSlabs = slab;

            for (size_t i = 0; i < ObjectsPerSlab; i++) {
                auto* node = reinterpret_cast<Node*>(slab->storage + i + Stride);
                node->next = mFree;
                mFree = node;
            }

            return mFree;
        }

        std::pair<Slab*, size_t> locate(T* object) {
            uintptr_t address = reinterpret_cast<uintptr_t>(object);
            for (Slab* slab = mSlabs; slab; slab = slab->next) {
                uintptr_t begin = reinterpret_cast<uintptr_t>(slab->storage);
                uintptr_t end = begin + Stride * ObjectsPerSlab;

                if (address >= begin && address < end) {
                    size_t index = (address - begin) / Stride;
                    return {slab, index};
                }
            }
            throw GameException();
        }
    };
}

#endif //UNNAMEDGAME_ENGINE_UTIL_OBJECT_ALLOCATOR_H
