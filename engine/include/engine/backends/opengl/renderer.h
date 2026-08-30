// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H
#define UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H

#include "engine/backend/graphics_device.h"
#include "engine/backend/renderer.h"
#include "engine/backend/window.h"

#include <memory>

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

        void drawCodepoint(const DrawCodepointCommand& command) override;
        void drawCodepoints(const DrawCodepointsCommand& command) override;
        void drawText(const DrawTextCommand& command) override;

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

        class Pipeline {
        public:
            explicit Pipeline(OpenGLRenderer& renderer);
            Pipeline(OpenGLRenderer& renderer, ShaderHandle shader);
            ~Pipeline();

            Pipeline& operator=(Pipeline&& other) noexcept;

            math::Mat4& projection();
            math::Mat4& view();

            void uploadUniforms();

            void bind();

            // ONLY CALL AFTER bind()!!!!!
            // doesn't store its texture cpu side to avoid wasting uploads so caller must manually do that
            void setTexture(TextureHandle texture);

        private:
            OpenGLRenderer& mRenderer;
            ShaderHandle mShader;
            UniformHandle mProjectionUniform;
            UniformHandle mViewUniform;
            UniformHandle mTextureUniform;

            math::Mat4 mProjection;
            math::Mat4 mView;
        };

        class Batch {
        public:
            explicit Batch(OpenGLRenderer& renderer);
            Batch(OpenGLRenderer& renderer, std::span<const VertexAttribute> attributes, uint32_t vertexSize, uint32_t maxVertices);
            ~Batch();

            Batch& operator=(Batch&& other) noexcept;

            std::optional<uint32_t> submit(uint32_t vertexCount, const void* vertices);
            void upload();
            void draw(uint32_t offset, uint32_t count);

        private:
            OpenGLRenderer& mRenderer;
            VertexArrayHandle mVAO;
            BufferHandle mVBO;
            std::unique_ptr<uint8_t[]> mVertexBytes;
            uint32_t mVertexCount;
            uint32_t mVertexSize;
            uint32_t mMaxVertices;
        };

        struct RenderCommand {
            Pipeline* pipeline;
            Batch* batch;
            TextureHandle texture;
            uint32_t vertexOffset;
            uint32_t vertexCount;
        };

        class RenderQueue {
        public:
            RenderQueue(std::vector<Pipeline*> pipelines, std::vector<Batch*> batches);

            void setProjections(const math::Mat4& projection);
            void setViews(const math::Mat4& view);

            void submit(Pipeline& pipeline, Batch& batch, TextureHandle texture, uint32_t vertexCount, const void* vertices);
            void flush();

        private:
            std::vector<Pipeline*> mPipelines;
            std::vector<Batch*> mBatches; // all unique batches
            std::vector<RenderCommand> mCommands;
        };

        using uint = unsigned int;

        static constexpr size_t MAX_BATCH_QUADS = 4096;

        IGraphicsDevice& mDevice;
        IWindow& mWindow;

        Pipeline mRectPipeline;
        Pipeline mTexturePipeline;

        Batch mRectBatch;
        Batch mTextureBatch;

        RenderQueue mRenderQueue;

        /*
        ShaderHandle mColorShader;

        UniformHandle mColorProjectionUniform;
        UniformHandle mColorViewUniform;

        ShaderHandle mTextureShader;

        UniformHandle mTextureProjectionUniform;
        UniformHandle mTextureViewUniform;
        UniformHandle mTextureSamplerUniform;

        ShaderHandle mFontShader;

        UniformHandle mFontProjectionUniform;
        UniformHandle mFontViewUniform;
        UniformHandle mFontAtlasUniform;


        VertexArrayHandle mRectVAO;
        BufferHandle mRectVBO;
        std::vector<ColorVertex> mRectBatch;

        VertexArrayHandle mTextureVAO;
        BufferHandle mTextureVBO;
        std::vector<TextureVertex> mTextureBatch;
        TextureHandle mBatchTexture;
        */

        std::span<const VertexAttribute> CreateRectAttributes();
        std::span<const VertexAttribute> CreateTextureAttributes();

        void warmupShaders();
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_RENDERER_H
