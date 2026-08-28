// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_RENDERER_H
#define UNNAMEDGAME_ENGINE_BACKEND_RENDERER_H

#include "engine/util/math.h"

namespace backend {
    struct Camera2D {
        math::Vec2 position = math::Vec2::Zero();
        float zoom = 1.0f;
        float worldHeight = 10.0f;

        math::Rect worldBounds(math::Vec2 viewportSize) const {
            float height = worldHeight / zoom;
            float width = height * (viewportSize.x / viewportSize.y);

            return {
                position.x - width * 0.5f,
                position.x + width * 0.5f,
                position.y + height * 0.5f,
                position.y - height * 0.5f
            };
        }

        math::Mat4 viewMatrix() const {
            return math::Mat4::Translation(-position);
        }

        math::Mat4 projectionMatrix(math::Vec2 viewportSize) const {
            return math::Mat4::Orthographic(worldBounds(viewportSize));
        }

        math::Vec2 worldToScreen(math::Vec2 world, math::Vec2 viewportSize) const {
            float aspect = viewportSize.x / viewportSize.y;

            float viewHeight = worldHeight / zoom;
            float viewWidth = viewHeight * aspect;

            math::Vec2 relative = world - position;

            float pixelsPerWorldX = viewportSize.x / viewWidth;
            float pixelsPerWorldY = viewportSize.y / viewHeight;

            return {
                viewportSize.x * 0.5f + relative.x * pixelsPerWorldX,
                viewportSize.y * 0.5f - relative.y * pixelsPerWorldY
            };
        }

        math::Vec2 screenToWorld(math::Vec2 screen, math::Vec2 viewportSize) const {
            float aspect = viewportSize.x / viewportSize.y;

            float viewHeight = worldHeight / zoom;
            float viewWidth = viewHeight * aspect;

            float pixelsPerWorldX = viewWidth / viewportSize.x;
            float pixelsPerWorldY = viewHeight / viewportSize.y;

            math::Vec2 relative = {
                screen.x - viewportSize.x * 0.5f,
                screen.y - viewportSize.y * 0.5f
            };

            return {
                position.x + relative.x * pixelsPerWorldX,
                position.y + relative.y * pixelsPerWorldY
            };
        }
    };

    struct DrawRectCommand {
        math::Vec2 position;
        math::Vec2 size;
        math::Color color;
    };

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual void beginWorld(const Camera2D& camera) = 0;
        virtual void endWorld() = 0;

        virtual void beginUI() = 0;
        virtual void endUI() = 0;

        virtual void clearScreen(math::Color color) = 0;

        virtual void drawRect(const DrawRectCommand& command) = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_RENDERER_H
