// Copyright 2026 Jannik Laugmand Bülow

#include "../../../include/engine/backends/glfw/window.h"

#include "engine/util/exceptions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace backend {
    GLFWWindow::GLFWWindow(int width, int height, const char* title) {
        if (!glfwInit()) {
            throw util::GameException();
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

        mWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!mWindow) {
            glfwTerminate();
            throw util::GameException();
        }

        glfwMakeContextCurrent(mWindow);

        glfwSwapInterval(0); // vsync off
        // TODO: make vsync optional
    }

    GLFWWindow::~GLFWWindow() {
        if (mWindow) {
            glfwDestroyWindow(mWindow);
        }

        glfwTerminate();
    }

    bool GLFWWindow::shouldClose() const {
        return glfwWindowShouldClose(mWindow);
    }

    void GLFWWindow::pollEvents() {
        glfwPollEvents();
    }

    void GLFWWindow::swapBuffers() {
        glfwSwapBuffers(mWindow);
    }

    math::Vec2I GLFWWindow::getFramebufferSize() const {
        math::Vec2I size;
        glfwGetFramebufferSize(mWindow, &size.x, &size.y);
        return size;
    }

    math::Vec2I GLFWWindow::getWindowSize() const {
        math::Vec2I size;
        glfwGetWindowSize(mWindow, &size.x, &size.y);
        return size;
    }
}
