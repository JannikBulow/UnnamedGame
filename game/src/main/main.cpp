// Copyright 2026 Jannik Laugmand Bülow

#include <engine/backend/backend.h>

#include <engine/backends/glfw/input_provider.h>
#include <engine/backends/glfw/window.h>

#include <engine/backends/miniaudio/audio_device.h>

#include <engine/backends/opengl/renderer.h>
#include <engine/backends/opengl/graphics_device.h>

#include <engine/backends/stb/asset_provider.h>

#include <engine/render/frame_controller.h>
#include <engine/render/renderer.h>

#include <engine/resource/resource_manager.h>

#include <engine/sound/audio_device.h>

#include <engine/util/timer.h>

#include <iostream>

int main() {
    backend::GLFWWindow window(100, 100, "SWINGALING");
    backend::GLFWInputProvider inputProvider(window);
    backend::OpenGLGraphicsDevice device;
    backend::OpenGLRenderer lowLevelRenderer(device, window);
    backend::StbAssetProvider assetProvider;
    backend::MiniaudioAudioDevice audioDevice;

    backend::Backend backend = {
        .assetProvider = assetProvider,
        .audio = audioDevice,
        .gpu = device,
        .inputProvider = inputProvider,
        .renderer = lowLevelRenderer,
        .window = window,
    };

    engine::FrameController frameController(backend);
    engine::Renderer renderer(backend);
    engine::ResourceManager resourceManager(backend);
    engine::AudioDevice audio(backend);

    backend::Camera2D camera;

    frameController.setCamera(camera);
    frameController.timer().setLimit(165);

    engine::Sound intro = resourceManager.createSound({"/home/jannik/Downloads", "intro.wav"});
    engine::Font font = resourceManager.createFont({"/usr/share/fonts/liberation", "LiberationSans-Regular.ttf"}, 24);

    engine::Sprite rat(resourceManager.createTexture({"/home/jannik/Downloads", "rat.png"}));

    audio.play(intro);

    math::Vec2 playerPosition = math::Vec2::Zero();

    frameController.timer().start();
    while (!window.shouldClose()) {
        frameController.execute(
            [&](float dt) {
                if (inputProvider.isKeyDown(backend::Key::W)) playerPosition.y += 10 * dt;
                if (inputProvider.isKeyDown(backend::Key::S)) playerPosition.y -= 10 * dt;
                if (inputProvider.isKeyDown(backend::Key::A)) playerPosition.x -= 10 * dt;
                if (inputProvider.isKeyDown(backend::Key::D)) playerPosition.x += 10 * dt;

                camera.position = playerPosition;

                audio.update(dt);
            },
            [&](float dt) {
                renderer.clear(math::Color::White);
            },
            [&](float dt) {
                renderer.drawRect(math::Vec2::Zero(), math::Vec2::One(), math::Color::Blue);
                renderer.drawSprite(rat, playerPosition, {1.0f, 1.5f});
                renderer.drawText(font, "playa", {playerPosition.x, playerPosition.y + 1.0f}, 24, math::Color::Black, true);
            },
            [&](float dt) {

            }
        );
    }

    return 0;
}