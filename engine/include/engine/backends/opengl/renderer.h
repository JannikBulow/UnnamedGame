// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H
#define UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H

#include "engine/backend/graphics_device.h"
#include "engine/backend/renderer.h"
#include "engine/backend/window.h"

namespace backend {
    class OpenGLRenderer : public IRenderer {
    public:
        OpenGLRenderer(IGraphicsDevice& device, IWindow& window);
        ~OpenGLRenderer() override;

        void beginFrame() override;
        void endFrame() override;

        void beginWorld(const Camera2D& camera) override;
        void endWorld() override;

        void beginUI() override;
        void endUI() override;

        void clearScreen(math::Color color) override;

        void drawRect(const DrawRectCommand& command) override;

        void drawTexture(const DrawTextureCommand& command) override;

    private:
        struct ColorVertex {
            math::Vec2 position;
            math::Color4F color;
        };

        struct TextureVertex {
            math::Vec2 position;
            math::Vec2 uv;
            math::Color4F color;
        };

        using uint = unsigned int;

        static constexpr size_t MAX_BATCH_QUADS = 4096;

        IGraphicsDevice& mDevice;
        IWindow& mWindow;

        ShaderHandle mColorShader;

        UniformHandle mColorProjectionUniform;
        UniformHandle mColorViewUniform;

        ShaderHandle mTextureShader;

        UniformHandle mTextureProjectionUniform;
        UniformHandle mTextureViewUniform;
        UniformHandle mTextureSamplerUniform;


        VertexArrayHandle mRectVAO;
        BufferHandle mRectVBO;
        std::vector<ColorVertex> mRectBatch;

        VertexArrayHandle mTextureVAO;
        BufferHandle mTextureVBO;
        std::vector<TextureVertex> mTextureBatch;
        TextureHandle mBatchTexture;

        void initGL();
        void createShaders();
        void createRectGeometry();
        void createTextureGeometry();

        void warmupShaders();

        void flushRectBatch();
        void flushTextureBatch();
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H
