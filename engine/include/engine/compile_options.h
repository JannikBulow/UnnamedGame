// Copyright 2026 Jannik Laugmand Bülow

// Collection of definitions defined by macros

#ifndef UNNAMEDGAME_ENGINE_COMPILE_OPTIONS_H
#define UNNAMEDGAME_ENGINE_COMPILE_OPTIONS_H

#ifdef UNNAMEDGAME_TARGET_MULTITHREADED
#include <atomic>
#endif

namespace options {
#ifdef UNNAMEDGAME_TARGET_MULTITHREADED
    template<class T>
    using Atomic = std::atomic<T>;
#else
    template<class T>
    using Atomic = T;
#endif
}

#endif //UNNAMEDGAME_ENGINE_COMPILE_OPTIONS_H
