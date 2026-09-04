// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RENDER_SPRITE_H
#define UNNAMEDGAME_ENGINE_RENDER_SPRITE_H

#include "engine/resource/texture.h"

namespace engine {
    class Sprite {
    public:
        Sprite(Texture texture, math::Rect pixelBounds);

        const Texture& texture() const { return mTexture; }
        math::Rect uv() const { return mUV; }

    private:
        Texture mTexture;
        math::Rect mUV;
    };
}

#endif //UNNAMEDGAME_ENGINE_RENDER_SPRITE_H
