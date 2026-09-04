// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_INPUT_INPUT_HANDLER_H
#define UNNAMEDGAME_ENGINE_INPUT_INPUT_HANDLER_H

#include "engine/backend/backend.h"

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace engine {
    template<class T>
    concept InputAction = std::is_enum_v<T>;

    using Key = backend::Key;

    template<InputAction Action>
    class InputHandler {
    public:
        explicit InputHandler(backend::Backend& backend)
            : mProvider(backend.inputProvider) {}

        void setKeybind(Action action, Key key) {
            auto& input = mInputMap[static_cast<size_t>(action)];
            input.type = InputType::Keyboard;
            input.value = static_cast<int>(key);
            input.exists = true;
        }

        void setMouseButton(Action action, int button) {
            auto& input = mInputMap[static_cast<size_t>(action)];
            input.type = InputType::Mouse;
            input.value = button;
            input.exists = true;
        }

        bool isDown(Action action) const {
            return mInputMap[static_cast<size_t>(action)].downThisFrame;
        }

        bool isUp(Action action) const {
            return !mInputMap[static_cast<size_t>(action)].downThisFrame;
        }

        bool isPressed(Action action) const {
            auto& input = mInputMap[static_cast<size_t>(action)];
            return input.downThisFrame && !input.downLastFrame;
        }

        bool isReleased(Action action) const {
            auto& input = mInputMap[static_cast<size_t>(action)];
            return !input.downThisFrame && input.downLastFrame;
        }

        void update() {
            for (auto& input : mInputMap) {
                if (!input.exists) continue;

                input.downLastFrame = input.downThisFrame;
                input.downThisFrame = false;

                switch (input.type) {
                    case InputType::Keyboard:
                        input.downThisFrame = mProvider.isKeyDown(static_cast<Key>(input.value));
                        break;
                    case InputType::Mouse:
                        input.downThisFrame = mProvider.isMouseButtonDown(input.value);
                        break;
                }
            }
        }

    private:
        enum InputType {
            Keyboard,
            Mouse
        };

        struct Input {
            InputType type;
            int value = -1;
            bool exists = false;
            bool downThisFrame = false;
            bool downLastFrame = false;
        };

        backend::IInputProvider& mProvider;

        std::array<Input, static_cast<size_t>(Action::Count)> mInputMap{};
    };
}

#endif //UNNAMEDGAME_ENGINE_INPUT_INPUT_HANDLER_H
