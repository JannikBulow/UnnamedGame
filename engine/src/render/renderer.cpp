// Copyright 2026 Jannik Laugmand Bülow

#include "engine/render/renderer.h"

namespace engine {
    Renderer::Renderer(backend::Backend& backend)
        : mBackend(backend) {}

    void Renderer::clear(math::Color color) {
        mBackend.renderer.clearScreen(color);
    }

    void Renderer::drawRect(math::Vec2 position, math::Vec2 size, math::Color color, float rotation) {
        mBackend.renderer.drawRect({
            .position = position,
            .size = size,
            .color = color,
            .rotation = rotation
        });
    }

    void Renderer::drawTexture(const Texture& texture, math::Vec2 position, math::Vec2 size, math::Color color, float rotation, math::Rect uv) {
        mBackend.renderer.drawTexture({
            .texture = texture.handle(),
            .sampler = texture.sampler(),
            .position = position,
            .size = size,
            .uv = uv,
            .color = color,
            .rotation = rotation
        });
    }

    void Renderer::drawSprite(const Sprite& sprite, math::Vec2 position, math::Vec2 size, math::Color color, float rotation) {
        drawTexture(sprite.texture(), position, size, color, rotation, sprite.uv());
    }

    void Renderer::drawText(const Font& font, const std::string& text, math::Vec2 position, float fontSize, math::Color color, bool centerOrigin) {
        float spacing = fontSize / 12;
        drawText(font, text, position, fontSize, spacing, 2.0f, color, centerOrigin);
    }

    void Renderer::drawText(const Font& font, const std::string& text, math::Vec2 position, float fontSize, float spacing, float textLineSpacing, math::Color color, bool centerOrigin) {
        mBackend.renderer.drawText({
            .texture = font.texture(),
            .sampler = font.sampler(),
            .font = font.font(),
            .text = text.c_str(),
            .textLength = text.length(),
            .fontSize = fontSize,
            .spacing = spacing,
            .textLineSpacing = textLineSpacing,
            .position = position,
            .color = color,
            .centerOrigin = centerOrigin
        });
    }
}
