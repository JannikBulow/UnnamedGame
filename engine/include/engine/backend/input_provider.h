// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_INPUT_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKEND_INPUT_PROVIDER_H

#include "engine/util/math.h"

namespace backend {
    enum class Key {
        Unknown = 0,

        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Zero,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,


        _keyCount
    };

    class IInputProvider {
    public:
        virtual ~IInputProvider() = default;

        virtual bool isKeyDown(Key key) = 0;
        virtual bool isKeyUp(Key key) = 0;

        virtual math::Vec2 getMousePosition() = 0;
        virtual bool isMouseButtonDown(int button) = 0;
        virtual bool isMouseButtonUp(int button) = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_INPUT_PROVIDER_H
