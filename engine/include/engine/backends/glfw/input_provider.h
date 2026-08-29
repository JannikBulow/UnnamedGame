// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_GLFW_INPUT_PROVIDER_H
#define UNNAMEDGAME_ENGINE_BACKENDS_GLFW_INPUT_PROVIDER_H

#include "engine/backend/input_provider.h"

#include "engine/backends/glfw/window.h"

namespace backend {
    class GLFWInputProvider : public IInputProvider {
    public:
        explicit GLFWInputProvider(GLFWWindow& window);

        bool isKeyDown(Key key) override;
        bool isKeyUp(Key key) override;

        math::Vec2 getMousePosition() override;
        bool isMouseButtonDown(int button) override;
        bool isMouseButtonUp(int button) override;

    private:
        GLFWwindow* mWindow; // unowned!!!
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_GLFW_INPUT_PROVIDER_H
