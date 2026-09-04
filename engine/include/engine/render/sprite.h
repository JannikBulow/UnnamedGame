// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RENDER_SPRITE_H
#define UNNAMEDGAME_ENGINE_RENDER_SPRITE_H

#include "engine/resource/texture.h"

#include <optional>

namespace engine {
    class Sprite {
    public:
        explicit Sprite(Texture texture, std::optional<math::Rect> pixelBounds = std::nullopt);

        const Texture& texture() const { return mTexture; }
        math::Rect uv() const { return mUV; }

    private:
        Texture mTexture;
        math::Rect mUV;
    };
}

#endif //UNNAMEDGAME_ENGINE_RENDER_SPRITE_H
