// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_WINDOW_H
#define UNNAMEDGAME_ENGINE_BACKEND_WINDOW_H

#include "engine/util/math.h"

namespace backend {
    class IWindow {
    public:
        virtual ~IWindow() = default;

        virtual bool shouldClose() const = 0;
        virtual void pollEvents() = 0;

        virtual math::Vec2I getFramebufferSize() const = 0;
        virtual math::Vec2I getWindowSize() const = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_WINDOW_H
