// Copyright 2026 Jannik Laugmand Bülow

#include "engine/render/frame_controller.h"

namespace engine {
    FrameController::FrameController(backend::Backend& backend)
        : mBackend(backend) {}

    void FrameController::setCamera(const Camera& camera) {
        mCamera = &camera;
    }

    void FrameController::beginFrame() {
        mBackend.renderer.beginFrame();
    }

    void FrameController::endFrame() {
        mBackend.renderer.endFrame();
        mTimer.waitForLimit();
    }

    void FrameController::beginWorld() {
        mBackend.renderer.beginWorld(*mCamera);
    }

    void FrameController::endWorld() {
        mBackend.renderer.endWorld();
    }

    void FrameController::beginUI() {
        mBackend.renderer.beginUI();
    }

    void FrameController::endUI() {
        mBackend.renderer.endUI();
    }
}
