// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H
#define UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H

#include "engine/backend/graphics_device.h"
#include "engine/backend/renderer.h"

#include "engine/backends/opengl/window.h"

namespace backend {
    class OpenGLRenderer : public IRenderer {
    public:
        OpenGLRenderer(IGraphicsDevice& device, GLFWWindow& window);
        ~OpenGLRenderer() override;

        void beginFrame() override;
        void endFrame() override;

        void beginWorld(const Camera2D& camera) override;
        void endWorld() override;

        void beginUI() override;
        void endUI() override;

        void clearScreen(math::Color color) override;

        void drawRect(const DrawRectCommand& command) override;

    private:
        using uint = unsigned int;

        IGraphicsDevice& mDevice;
        GLFWWindow& mWindow;

        ShaderHandle mShader;
        VertexArrayHandle mRectVAO;
        BufferHandle mRectVBO;

        UniformHandle mProjectionUniform;
        UniformHandle mViewUniform;
        UniformHandle mModelUniform;
        UniformHandle mColorUniform;

        void initGL();
        void createShaders();
        void createRectGeometry();

    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H
