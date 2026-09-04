// Copyright 2026 Jannik Laugmand Bülow

#include <engine/backend/backend.h>

#include <engine/backends/glfw/input_provider.h>
#include <engine/backends/glfw/window.h>

#include <engine/backends/miniaudio/audio_device.h>

#include <engine/backends/opengl/renderer.h>
#include <engine/backends/opengl/graphics_device.h>

#include <engine/backends/stb/asset_provider.h>

#include <engine/resource/resource_manager.h>

#include <engine/util/timer.h>

#include <iostream>

int main() {
    backend::GLFWWindow window(100, 100, "SWINGALING");
    backend::GLFWInputProvider inputProvider(window);
    backend::OpenGLGraphicsDevice device;
    backend::OpenGLRenderer renderer(device, window);
    backend::StbAssetProvider assetProvider;
    backend::MiniaudioAudioDevice audioDevice;

    backend::Backend backend = {
        .assetProvider = assetProvider,
        .audio = audioDevice,
        .gpu = device,
        .inputProvider = inputProvider,
        .renderer = renderer,
        .window = window,
    };

    engine::ResourceManager resourceManager(backend);

    engine::Sound intro = resourceManager.createSound({"/home/jannik/Downloads", "intro.wav"});
    engine::Font font = resourceManager.createFont({"/usr/share/fonts/liberation", "LiberationSans-Regular.ttf"}, 24);
    engine::Texture rat = resourceManager.createTexture({"/home/jannik/Downloads", "rat.png"});

    backend::AudioVoiceHandle voice = audioDevice.createVoice();
    audioDevice.setAudio(voice, intro.audio());
    audioDevice.play(voice);

    util::Timer<std::chrono::steady_clock> timer;
    timer.setLimit(165);

    backend::Camera2D camera;

    math::Vec2 playerPosition = math::Vec2::Zero();

    timer.start();
    while (!window.shouldClose()) {
        float dt = timer.tick();

        window.pollEvents();

        if (inputProvider.isKeyDown(backend::Key::W)) playerPosition.y += 10 * dt;
        if (inputProvider.isKeyDown(backend::Key::S)) playerPosition.y -= 10 * dt;
        if (inputProvider.isKeyDown(backend::Key::A)) playerPosition.x -= 10 * dt;
        if (inputProvider.isKeyDown(backend::Key::D)) playerPosition.x += 10 * dt;

        camera.position = playerPosition;

        renderer.beginFrame();
        renderer.clearScreen(math::Color::White);

        renderer.beginWorld(camera);

        renderer.drawRect({
            .position = math::Vec2::Zero(),
            .size = math::Vec2::One(),
            .color = math::Color::Blue
        });

        renderer.drawTexture({
            .texture = rat.handle(),
            .position = playerPosition,
            .size = {1, 1.5},
            .color = math::Color::White
        });

        renderer.drawText({
            .texture = font.texture(),
            .font = font.font(),
            .text = "playa",
            .fontSize = 24.0f,
            .position = {playerPosition.x, playerPosition.y + 1.0f},
            .color = math::Color::Black,
            .centerOrigin = true
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