// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_RENDER_RENDERER_H
#define UNNAMEDGAME_ENGINE_RENDER_RENDERER_H

#include "engine/backend/backend.h"

#include "engine/render/sprite.h"

#include "engine/resource/font.h"
#include "engine/resource/texture.h"

namespace engine {
    class Renderer {
    public:
        explicit Renderer(backend::Backend& backend);

        void clear(math::Color color);

        void drawRect(math::Vec2 position, math::Vec2 size, math::Color color, float rotation = 0.0f);

        void drawTexture(const Texture& texture, math::Vec2 position, math::Vec2 size, math::Color color = math::Color::White, float rotation = 0.0f, math::Rect uv = {0, 1, 1, 0});

        void drawSprite(const Sprite& sprite, math::Vec2 position, math::Vec2 size, math::Color color = math::Color::White, float rotation = 0.0f);

        void drawText(const Font& font, const std::string& text, math::Vec2 position, float fontSize, math::Color color, bool centerOrigin = false);
        void drawText(const Font& font, const std::string& text, math::Vec2 position, float fontSize, float spacing, float textLineSpacing, math::Color color, bool centerOrigin = false);

    private:
        backend::Backend& mBackend;
    };
}

#endif //UNNAMEDGAME_ENGINE_RENDER_RENDERER_H
