// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/opengl/graphics_device.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace backend {
    void OpenGLGraphicsDevice::beginFrame() {

    }

    void OpenGLGraphicsDevice::endFrame() {

    }

    void OpenGLGraphicsDevice::setViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    void OpenGLGraphicsDevice::clear(math::Color color) {
        math::Color4F fcolor(color);
        glClearColor(fcolor.r, fcolor.g, fcolor.b, fcolor.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    BufferHandle OpenGLGraphicsDevice::createBuffer(BufferUsage usage, size_t size, const void* initialData) {
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), initialData, ToGLUsage(usage));

        return mBuffers.create(vbo);
    }

    void OpenGLGraphicsDevice::destroyBuffer(BufferHandle buffer) {
        GLuint vbo = mBuffers.get(buffer);
        glDeleteBuffers(1, &vbo);
        mBuffers.destroy(buffer);
    }

    void OpenGLGraphicsDevice::updateBuffer(BufferHandle buffer, size_t offset, size_t size, const void* data) {
        GLuint vbo = mBuffers.get(buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
    }

    VertexArrayHandle OpenGLGraphicsDevice::createVertexArray() {
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        return mVertexArrays.create(vao);
    }

    void OpenGLGraphicsDevice::destroyVertexArray(VertexArrayHandle vertexArray) {
        GLuint vao = mVertexArrays.get(vertexArray);
        glDeleteVertexArrays(1, &vao);
        mVertexArrays.destroy(vertexArray);
        if (mBoundVertexArray == vertexArray) mBoundVertexArray = nullptr;
    }

    void OpenGLGraphicsDevice::bindVertexArray(VertexArrayHandle vertexArray) {
        if (mBoundVertexArray == vertexArray) return;

        GLuint vao = mVertexArrays.get(vertexArray);
        glBindVertexArray(vao);
    }

    void OpenGLGraphicsDevice::setVertexAttribute(VertexArrayHandle vertexArray, BufferHandle buffer, int stride, int location, int componentCount, int offset) {
        GLuint vao = mVertexArrays.get(vertexArray);
        GLuint vbo = mBuffers.get(buffer);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, componentCount, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(static_cast<uintptr_t>(offset)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mBoundVertexArray = nullptr;
    }

    void OpenGLGraphicsDevice::setVertexLayout(VertexArrayHandle vertexArray, BufferHandle buffer, int stride, std::span<const VertexAttribute> attributes) {
        GLuint vao = mVertexArrays.get(vertexArray);
        GLuint vbo = mBuffers.get(buffer);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        for (const auto& attribute : attributes) {
            glEnableVertexAttribArray(attribute.location);
            glVertexAttribPointer(
                attribute.location,
                attribute.components,
                ToGLVertexAttributeType(attribute.type),
                attribute.normalized ? GL_TRUE : GL_FALSE,
                stride,
                reinterpret_cast<const void*>(static_cast<uintptr_t>(attribute.offset))
            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mBoundVertexArray = nullptr;
    }

    ShaderHandle OpenGLGraphicsDevice::createShader(const char* vertexSource, const char* fragmentSource) {
        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
        GLuint program = CreateShaderProgram(vertexShader, fragmentShader);

        return mShaders.create(program);
    }

    void OpenGLGraphicsDevice::destroyShader(ShaderHandle shader) {
        GLuint program = mShaders.get(shader);
        glDeleteProgram(program);
        mShaders.destroy(shader);

        if (mBoundShader == shader) mBoundShader = nullptr;
    }

    void OpenGLGraphicsDevice::bindShader(ShaderHandle shader) {
        if (mBoundShader == shader) return;

        GLuint program = mShaders.get(shader);
        glUseProgram(program);
    }

    UniformHandle OpenGLGraphicsDevice::getUniform(ShaderHandle shader, const char* name) {
        GLuint program = mShaders.get(shader);
        GLint uniform = glGetUniformLocation(program, name);

        if (uniform < 0) return nullptr;
        return mUniforms.create(uniform);
    }

    void OpenGLGraphicsDevice::setUniform(UniformHandle uniform, int value) {
        GLint id = mUniforms.get(uniform);
        glUniform1i(id, value);
    }

    void OpenGLGraphicsDevice::setUniform(UniformHandle uniform, float value) {
        GLint id = mUniforms.get(uniform);
        glUniform1f(id, value);
    }

    void OpenGLGraphicsDevice::setUniform(UniformHandle uniform, math::Vec2F value) {
        GLint id = mUniforms.get(uniform);
        glUniform2f(id, value.x, value.y);
    }

    void OpenGLGraphicsDevice::setUniform(UniformHandle uniform, math::Mat3F value) {
        GLint id = mUniforms.get(uniform);
        glUniformMatrix3fv(id, 1, GL_FALSE, value.data());
    }

    void OpenGLGraphicsDevice::setUniform(UniformHandle uniform, math::Mat4F value) {
        GLint id = mUniforms.get(uniform);
        glUniformMatrix4fv(id, 1, GL_FALSE, value.data());
    }

    void OpenGLGraphicsDevice::setUniform(UniformHandle uniform, math::Color4B value) {
        math::Color4F fcolor(value);
        GLint id = mUniforms.get(uniform);
        glUniform4f(id, fcolor.r, fcolor.g, fcolor.b, fcolor.a);
    }

    TextureHandle OpenGLGraphicsDevice::createTexture(Image image) {
        GLuint texture = 0;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLint internalFormat;
        GLenum format;

        switch (image.format) {
            case ImageFormat::R8:
                internalFormat = GL_R8;
                format = GL_RED;
                break;
            case ImageFormat::RGB8:
                internalFormat = GL_RGB8;
                format = GL_RGB;
                break;
            case ImageFormat::RGBA8:
                internalFormat = GL_RGBA8;
                format = GL_RGBA;
                break;
            default:
                throw util::GameException("unreachable");
        }

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return mTextures.create(texture);
    }

    void OpenGLGraphicsDevice::destroyTexture(TextureHandle texture) {
        GLuint id = mTextures.get(texture);
        glDeleteTextures(1, &id);
        mTextures.destroy(texture);

        for (auto& bound : mBoundTextures) {
            if (bound == texture) bound = nullptr;
        }
    }

    void OpenGLGraphicsDevice::bindTexture(unsigned slot, TextureHandle texture) {
        if (slot < MAX_TEXTURE_SLOTS && mBoundTextures[slot] == texture) return;

        GLuint id = mTextures.get(texture);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);

        if (slot < MAX_TEXTURE_SLOTS) mBoundTextures[slot] = texture;
    }

    void OpenGLGraphicsDevice::setTextureFilter(TextureHandle texture, TextureFilter filter) {
        GLuint id = mTextures.get(texture);
        glBindTexture(GL_TEXTURE_2D, id);

        GLint glFilter = ToGLFilter(filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
    }

    void OpenGLGraphicsDevice::setTextureWrap(TextureHandle texture, TextureWrap wrap) {
        GLuint id = mTextures.get(texture);
        glBindTexture(GL_TEXTURE_2D, id);

        GLint glWrap = ToGLWrap(wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
    }

    void OpenGLGraphicsDevice::setBlendMode(BlendMode mode) {
        if (mBlendModeInitialized && mCurrentBlendMode == mode) return;

        switch (mode) {
            case BlendMode::None:
                glDisable(GL_BLEND);
                break;
            case BlendMode::Alpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case BlendMode::Additive:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
        }

        mCurrentBlendMode = mode;
        mBlendModeInitialized = true;
    }

    void OpenGLGraphicsDevice::draw(DrawCommand command) {
        glDrawArrays(ToGLPrimitive(command.primitive), static_cast<GLint>(command.firstVertex), static_cast<GLsizei>(command.vertexCount));
    }

    uint OpenGLGraphicsDevice::ToGLPrimitive(PrimitiveType primitive) {
        switch (primitive) {
            case PrimitiveType::Triangles:
                return GL_TRIANGLES;
            case PrimitiveType::TriangleStrip:
                return GL_TRIANGLE_STRIP;
            case PrimitiveType::Lines:
                return GL_LINES;
            case PrimitiveType::LineStrip:
                return GL_LINE_STRIP;
        }
        return GL_TRIANGLES;
    }

    uint OpenGLGraphicsDevice::ToGLUsage(BufferUsage usage) {
        switch (usage) {
            case BufferUsage::Static:
                return GL_STATIC_DRAW;
            case BufferUsage::Dynamic:
                return GL_DYNAMIC_DRAW;
            case BufferUsage::Stream:
                return GL_STREAM_DRAW;
        }
        return GL_STATIC_DRAW;
    }

    uint OpenGLGraphicsDevice::ToGLVertexAttributeType(VertexAttributeType type) {
        switch (type) {
            case VertexAttributeType::Float:
                return GL_FLOAT;
            case VertexAttributeType::UnsignedByte:
                return GL_UNSIGNED_BYTE;
        }
        return GL_FLOAT;
    }

    int OpenGLGraphicsDevice::ToGLFilter(TextureFilter filter) {
        switch (filter) {
            case TextureFilter::Nearest:
                return GL_NEAREST;
            case TextureFilter::Linear:
                return GL_LINEAR;
        }
        return GL_LINEAR;
    }

    int OpenGLGraphicsDevice::ToGLWrap(TextureWrap wrap) {
        switch (wrap) {
            case TextureWrap::Repeat:
                return GL_REPEAT;
            case TextureWrap::Clamp:
                return GL_CLAMP_TO_EDGE;
        }
        return GL_REPEAT;
    }

    uint OpenGLGraphicsDevice::CompileShader(uint type, const char* source) {
        GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            char log[2048];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            glDeleteShader(shader);
            throw util::ShaderCompileException("OpenGL", log);
        }

        return shader;
    }

    uint OpenGLGraphicsDevice::CreateShaderProgram(uint vertexShader, uint fragmentShader) {
        GLuint program = glCreateProgram();

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);

        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success) {
            char log[2048];
            glGetProgramInfoLog(program, sizeof(log), nullptr, log);

            glDeleteProgram(program);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            throw util::ShaderLinkException("OpenGL", log);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }
}
