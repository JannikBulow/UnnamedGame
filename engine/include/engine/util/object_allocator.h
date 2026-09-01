// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_OBJECT_ALLOCATOR_H
#define UNNAMEDGAME_ENGINE_UTIL_OBJECT_ALLOCATOR_H

#include <cmath>
#include <cstddef>
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

            return std::construct_at(reinterpret_cast<T*>(node), std::forward<Args>(args)...);
        }

        void destroy(T* object) {
            std::destroy_at(object);

            auto* node = reinterpret_cast<Node*>(object);
            node->next = mFree;
            mFree = node;
        }

    private:
        struct Node {
            Node* next;
        };

        static constexpr size_t Align = std::max(alignof(T), alignof(Node));
        static constexpr size_t Stride = (std::max(sizeof(T), sizeof(Node)) + Align - 1) / Align * Align;

        struct Slab {
            Slab* next;
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
    };
}

#endif //UNNAMEDGAME_ENGINE_UTIL_OBJECT_ALLOCATOR_H
