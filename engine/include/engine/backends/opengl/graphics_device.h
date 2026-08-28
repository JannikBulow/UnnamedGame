// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_GRAPHICS_DEVICE_H
#define UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_GRAPHICS_DEVICE_H

#include "engine/backend/graphics_device.h"

namespace backend {
    class OpenGLGraphicsDevice : public IGraphicsDevice {
    public:
        void beginFrame() override;
        void endFrame() override;

        void setViewport(int x, int y, int width, int height) override;

        void clear(math::Color color) override;

        BufferHandle createBuffer(BufferUsage usage, size_t size, const void* initialData) override;
        void destroyBuffer(BufferHandle buffer) override;
        void updateBuffer(BufferHandle buffer, size_t offset, size_t size, const void* data) override;

        VertexArrayHandle createVertexArray() override;
        void destroyVertexArray(VertexArrayHandle vertexArray) override;
        void bindVertexArray(VertexArrayHandle vertexArray) override;
        void setVertexBuffer(VertexArrayHandle vertexArray, BufferHandle buffer, int stride) override;
        void setVertexAttribute(VertexArrayHandle vertexArray, int location, int componentCount, int offset) override;
        void setVertexLayout(VertexArrayHandle vertexArray, BufferHandle buffer, int stride, std::span<const VertexAttribute> attributes) override;

        ShaderHandle createShader(const char* vertexSource, const char* fragmentSource) override;
        void destroyShader(ShaderHandle shader) override;
        void bindShader(ShaderHandle shader) override;

        UniformHandle getUniform(ShaderHandle shader, const char* name) override;
        void setUniform(UniformHandle uniform, float value) override;
        void setUniform(UniformHandle uniform, math::Vec2F value) override;
        void setUniform(UniformHandle uniform, math::Mat4F value) override;
        void setUniform(UniformHandle uniform, math::Color4B value) override;

        TextureHandle createTexture(Image image) override;
        void destroyTexture(TextureHandle texture) override;
        void bindTexture(unsigned slot, TextureHandle texture) override;

        void setBlendMode(BlendMode mode) override;

        void draw(DrawCommand command) override;
        void drawIndexed(DrawIndexedCommand command) override;

    private:
        using uint = unsigned int;

        util::HandleStorage<uint, BufferHandleTag> mBuffers;
        util::HandleStorage<uint, ShaderHandleTag> mShaders;
        util::HandleStorage<uint, TextureHandleTag> mTextures;
        util::HandleStorage<int, UniformHandleTag> mUniforms;
        util::HandleStorage<uint, VertexArrayHandleTag> mVertexArrays;

        static uint ToGLPrimitive(PrimitiveType primitive);
        static uint ToGLUsage(BufferUsage usage);
        static uint ToGLVertexAttributeType(VertexAttributeType type);

        static uint CompileShader(uint type, const char* source);
        static uint CreateShaderProgram(uint vertexShader, uint fragmentShader);
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_GRAPHICS_DEVICE_H
