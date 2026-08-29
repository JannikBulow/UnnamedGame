// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/glfw/input_provider.h"

#include "GLFW/glfw3.h"

namespace backend {
    static constexpr int glfwKeyMap[static_cast<size_t>(Key::_keyCount)] = {
        GLFW_KEY_UNKNOWN,

        GLFW_KEY_1,
        GLFW_KEY_2,
        GLFW_KEY_3,
        GLFW_KEY_4,
        GLFW_KEY_5,
        GLFW_KEY_6,
        GLFW_KEY_7,
        GLFW_KEY_8,
        GLFW_KEY_9,
        GLFW_KEY_0,

        GLFW_KEY_A,
        GLFW_KEY_B,
        GLFW_KEY_C,
        GLFW_KEY_D,
        GLFW_KEY_E,
        GLFW_KEY_F,
        GLFW_KEY_G,
        GLFW_KEY_H,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_M,
        GLFW_KEY_N,
        GLFW_KEY_O,
        GLFW_KEY_P,
        GLFW_KEY_Q,
        GLFW_KEY_R,
        GLFW_KEY_S,
        GLFW_KEY_T,
        GLFW_KEY_U,
        GLFW_KEY_V,
        GLFW_KEY_W,
        GLFW_KEY_X,
        GLFW_KEY_Y,
        GLFW_KEY_Z,
    };


    GLFWInputProvider::GLFWInputProvider(GLFWWindow& window)
        : mWindow(window.mWindow) {}

    bool GLFWInputProvider::isKeyDown(Key key) {
        return glfwGetKey(mWindow, glfwKeyMap[static_cast<size_t>(key)]) == GLFW_PRESS;
    }

    bool GLFWInputProvider::isKeyUp(Key key) {
        return glfwGetKey(mWindow, glfwKeyMap[static_cast<size_t>(key)]) == GLFW_RELEASE;
    }

    math::Vec2 GLFWInputProvider::getMousePosition() {
        double x, y;
        glfwGetCursorPos(mWindow, &x, &y);
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    bool GLFWInputProvider::isMouseButtonDown(int button) {
        return glfwGetMouseButton(mWindow, button) == GLFW_PRESS; // glfw and this engine use the same mouse button numbers
    }

    bool GLFWInputProvider::isMouseButtonUp(int button) {
        return glfwGetMouseButton(mWindow, button) == GLFW_RELEASE; // glfw and this engine use the same mouse button numbers
    }
}
