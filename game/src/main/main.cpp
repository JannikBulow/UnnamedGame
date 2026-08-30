// Copyright 2026 Jannik Laugmand Bülow

#include <engine/backends/glfw/input_provider.h>
#include <engine/backends/glfw/window.h>

#include <engine/backends/opengl/renderer.h>
#include <engine/backends/opengl/graphics_device.h>

#include <engine/backends/stb/asset_provider.h>

#include <engine/util/timer.h>

#include <iostream>

int main() {
    backend::GLFWWindow window(100, 100, "SWINGALING");
    backend::GLFWInputProvider inputProvider(window);
    backend::OpenGLGraphicsDevice device;
    backend::OpenGLRenderer renderer(device, window);
    backend::StbAssetProvider assetProvider;

    backend::Font font = static_cast<backend::IAssetProvider&>(assetProvider).loadFont("/usr/share/fonts/TTF/OpenSans-Regular.ttf", 12);
    backend::TextureHandle fontTexture = device.createTexture(font.atlas);

    backend::Image ratImage = assetProvider.loadImage("/home/jannik/Downloads/rat.png");
    backend::TextureHandle rat = device.createTexture(ratImage);
    assetProvider.unloadImage(ratImage);

    util::Timer<std::chrono::steady_clock> timer;
    timer.setLimit(165);

    backend::Camera2D camera;
    camera.worldHeight = 20.0f;

    math::Vec2 playerPosition = math::Vec2::Zero();

    timer.start();
    while (!window.shouldClose()) {
        float dt = timer.tick();

        window.pollEvents();

        if (inputProvider.isKeyDown(backend::Key::W)) playerPosition.y += 10 * dt;
        if (inputProvider.isKeyDown(backend::Key::S)) playerPosition.y -= 10 * dt;
        if (inputProvider.isKeyDown(backend::Key::A)) playerPosition.x -= 10 * dt;
        if (inputProvider.isKeyDown(backend::Key::D)) playerPosition.x += 10 * dt;

        renderer.beginFrame();
        renderer.clearScreen(math::Color::LightGray);

        renderer.beginWorld(camera);

        renderer.drawTexture({
            .texture = rat,
            .position = playerPosition,
            .size = {1, 1},
            .color = {255, 255, 255, 255}
        });

        renderer.endWorld();

        renderer.beginUI();

        renderer.drawCodepoint({
            .texture = fontTexture,
            .font = font,
            .codepoint = 'h',
            .fontSize = 12.0f,
            .position = {100, 100},
            .color = math::Color::Black,
        });

        renderer.endUI();
        renderer.endFrame();

        timer.waitForLimit();

        std::cout << timer.getRate() << " fps\n";
    }

    device.destroyTexture(fontTexture);
    device.destroyTexture(rat);

    assetProvider.unloadFont(font);

    return 0;
}