// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RENDER_FRAME_CONTROLLER_H
#define UNNAMEDGAME_ENGINE_RENDER_FRAME_CONTROLLER_H

#include "engine/backend/backend.h"

#include "engine/render/camera.h"

#include "engine/util/timer.h"

namespace engine {
    class FrameController {
    public:
        using Timer = util::Timer<std::chrono::steady_clock>;

        explicit FrameController(backend::Backend& backend);

        void setCamera(const Camera& camera);

        Timer& timer() { return mTimer; }
        const Timer& timer() const { return mTimer; }

        template<class F0, class F1, class F2, class F3>
        void execute(F0&& preFrame, F1&& frameBegin, F2&& world, F3&& ui) {
            float dt = timer().tick();
            mBackend.window.pollEvents();
            preFrame(dt);
            beginFrame();
            frameBegin(dt);
            beginWorld();
            world(dt);
            endWorld();
            beginUI();
            ui(dt);
            endUI();
            endFrame();
        }

    private:
        backend::Backend& mBackend;
        const Camera* mCamera;
        Timer mTimer;

        void beginFrame();
        void endFrame();

        void beginWorld();
        void endWorld();

        void beginUI();
        void endUI();
    };
}

#endif //UNNAMEDGAME_ENGINE_RENDER_FRAME_CONTROLLER_H
