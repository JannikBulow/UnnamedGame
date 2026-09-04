// Copyright 2026 Jannik Laugmand Bülow

#include "engine/render/sprite.h"

namespace engine {
    Sprite::Sprite(Texture texture, std::optional<math::Rect> pixelBounds)
        : mTexture(std::move(texture)) {
        math::Vec2 textureBounds = math::Vec2(math::Vec2I{mTexture.image().width, mTexture.image().height});
        if (pixelBounds) {
            mUV = {
                pixelBounds->left / textureBounds.x,
                pixelBounds->right / textureBounds.x,
                1.0f - pixelBounds->top / textureBounds.y,
                1.0f - pixelBounds->bottom / textureBounds.y
            };
        } else {
            mUV = {0, 1, 1, 0};
        }
    }
}
