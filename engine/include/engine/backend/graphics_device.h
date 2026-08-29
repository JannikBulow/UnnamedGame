// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKEND_GRAPHICS_DEVICE_H
#define UNNAMEDGAME_ENGINE_BACKEND_GRAPHICS_DEVICE_H

#include "engine/backend/asset_provider.h"

#include "engine/util/handle.h"
#include "engine/util/math.h"

#include <span>

namespace backend {
    using BufferHandle = util::Handle<struct BufferHandleTag>;
    using ShaderHandle = util::Handle<struct ShaderHandleTag>;
    using TextureHandle = util::Handle<struct TextureHandleTag>;
    using UniformHandle = util::Handle<struct UniformHandleTag>;
    using VertexArrayHandle = util::Handle<struct VertexArrayHandleTag>;

    enum class PrimitiveType {
        Triangles,
        TriangleStrip,
        Lines,
        LineStrip
    };

    enum class BlendMode {
        None,
        Alpha,
        Additive
    };

    enum class BufferUsage {
        Static,
        Dynamic,
        Stream
    };

    enum class VertexAttributeType {
        Float,
        UnsignedByte
    };

    struct VertexAttribute {
        int location;
        int components;
        VertexAttributeType type;
        bool normalized;
        int offset;
    };

    enum class TextureFilter {
        Nearest,
        Linear,
    };

    enum class TextureWrap {
        Clamp,
        Repeat,
    };

    struct DrawCommand {
        PrimitiveType primitive = PrimitiveType::Triangles;

        uint32_t vertexCount = 0;
        uint32_t firstVertex = 0;
    };

    class IGraphicsDevice {
    public:
        virtual ~IGraphicsDevice() = default;

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual void setViewport(int x, int y, int width, int height) = 0;

        virtual void clear(math::Color color) = 0;

        virtual BufferHandle createBuffer(BufferUsage usage, size_t size, const void* initialData) = 0;
        virtual void destroyBuffer(BufferHandle buffer) = 0;
        virtual void updateBuffer(BufferHandle buffer, size_t offset, size_t size, const void* data) = 0;

        virtual VertexArrayHandle createVertexArray() = 0;
        virtual void destroyVertexArray(VertexArrayHandle vertexArray) = 0;
        virtual void bindVertexArray(VertexArrayHandle vertexArray) = 0;
        virtual void setVertexAttribute(VertexArrayHandle vertexArray, BufferHandle buffer, int stride, int location, int componentCount, int offset) = 0;
        virtual void setVertexLayout(VertexArrayHandle vertexArray, BufferHandle buffer, int stride, std::span<const VertexAttribute> attributes) = 0;

        virtual ShaderHandle createShader(const char* vertexSource, const char* fragmentSource) = 0;
        virtual void destroyShader(ShaderHandle shader) = 0;
        virtual void bindShader(ShaderHandle shader) = 0;

        virtual UniformHandle getUniform(ShaderHandle shader, const char* name) = 0;
        virtual void setUniform(UniformHandle uniform, int value) = 0;
        virtual void setUniform(UniformHandle uniform, float value) = 0;
        virtual void setUniform(UniformHandle uniform, math::Vec2F value) = 0;
        virtual void setUniform(UniformHandle uniform, math::Mat3F value) = 0;
        virtual void setUniform(UniformHandle uniform, math::Mat4F value) = 0;
        virtual void setUniform(UniformHandle uniform, math::Color4B value) = 0;

        virtual TextureHandle createTexture(Image image) = 0;
        virtual void destroyTexture(TextureHandle texture) = 0;
        virtual void bindTexture(unsigned slot, TextureHandle texture) = 0;
        virtual void setTextureFilter(TextureHandle texture, TextureFilter filter) = 0;
        virtual void setTextureWrap(TextureHandle texture, TextureWrap wrap) = 0;

        virtual void setBlendMode(BlendMode mode) = 0;

        virtual void draw(DrawCommand command) = 0;
    };
}

#endif //UNNAMEDGAME_ENGINE_BACKEND_GRAPHICS_DEVICE_H
