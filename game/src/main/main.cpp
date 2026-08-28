// Copyright 2026 Jannik Laugmand Bülow

#include <engine/backends/opengl/renderer.h>
#include <engine/backends/opengl/graphics_device.h>

#include <engine/util/timer.h>

#include <iostream>

int main() {
    backend::GLFWWindow window(100, 100, "SWINGALING");
    backend::OpenGLGraphicsDevice device;
    backend::OpenGLRenderer renderer(device, window);

    util::Timer<std::chrono::steady_clock> timer;
    timer.setLimit(165);

    backend::Camera2D camera;
    camera.worldHeight = 20.0f;

    math::Vec2 playerPosition = math::Vec2::Zero();

    timer.start();
    while (!window.shouldClose()) {
        float dt = timer.tick();

        window.pollEvents();

        playerPosition.y += 1 * dt;

        renderer.beginFrame();
        renderer.clearScreen(math::Color(0, 0, 0, 255));

        renderer.beginWorld(camera);

        renderer.drawRect({
            playerPosition,
            {1, 1},
            {255, 255, 255, 255}
        });

        renderer.endWorld();

        renderer.beginUI();

        renderer.endUI();
        renderer.endFrame();

        timer.waitForLimit();

        std::cout << timer.getRate() << " fps\n";
    }

    return 0;
}