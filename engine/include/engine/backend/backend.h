// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_BACKEND_H
#define UNNAMEDGAME_ENGINE_BACKEND_BACKEND_H

#include "engine/backend/asset_provider.h"
#include "engine/backend/graphics_device.h"
#include "engine/backend/input_provider.h"
#include "engine/backend/renderer.h"
#include "engine/backend/window.h"

namespace backend {
    struct Backend {
        IAssetProvider& assetProvider;
        IGraphicsDevice& gpu;
        IInputProvider& inputProvider;
        IRenderer& renderer;
        IWindow& window;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_BACKEND_H
