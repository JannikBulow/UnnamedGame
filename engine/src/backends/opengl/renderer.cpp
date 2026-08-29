// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/opengl/renderer.h"
#include "engine/backends/opengl/shader_programs.h"

#include "engine/util/exceptions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <cmath>

namespace backend {
    struct TextureVertex {
        math::Vec2 position;
        math::Vec2 uv;
    };

    void APIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    }

    OpenGLRenderer::OpenGLRenderer(IGraphicsDevice& device, GLFWWindow& window)
        : mDevice(device)
        , mWindow(window) {
        if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
            throw util::GameException();
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugCallback, nullptr);

        initGL();
    }

    OpenGLRenderer::~OpenGLRenderer() {
        if (mRectVBO) {
            mDevice.destroyBuffer(mRectVBO);
        }

        if (mRectVAO) {
            mDevice.destroyVertexArray(mRectVAO);
        }

        if (mColorShader) {
            mDevice.destroyShader(mColorShader);
        }
    }

    void OpenGLRenderer::beginFrame() {
        mDevice.beginFrame();

        math::Vec2I frameBufferSize = mWindow.getFramebufferSize();

        mDevice.setViewport(0, 0, frameBufferSize.x, frameBufferSize.y);
    }

    void OpenGLRenderer::endFrame() {
        mDevice.endFrame();
        glfwSwapBuffers(mWindow.getHandle());
    }

    void OpenGLRenderer::beginWorld(const Camera2D& camera) {
        math::Vec2I frameBufferSize = mWindow.getFramebufferSize();
        math::Vec2 viewportSize = {
            static_cast<float>(frameBufferSize.x),
            static_cast<float>(frameBufferSize.y)
        };

        math::Mat4 projection = camera.projectionMatrix(viewportSize);
        math::Mat4 view = camera.viewMatrix();

        mDevice.bindShader(mColorShader);
        mDevice.setUniform(mColorProjectionUniform, projection);
        mDevice.setUniform(mColorViewUniform, view);

        mDevice.bindShader(mTextureShader);
        mDevice.setUniform(mTextureProjectionUniform, projection);
        mDevice.setUniform(mTextureViewUniform, view);

        mDevice.setBlendMode(BlendMode::Alpha);
    }

    void OpenGLRenderer::endWorld() {

    }

    void OpenGLRenderer::beginUI() {
        math::Vec2I frameBufferSize = mWindow.getFramebufferSize();
        math::Mat4 projection = math::Mat4::Orthographic(
            0.0f,
            static_cast<float>(frameBufferSize.x),
            0.0f,
            static_cast<float>(frameBufferSize.y)
        );

        mDevice.bindShader(mColorShader);
        mDevice.setUniform(mColorProjectionUniform, projection);
        mDevice.setUniform(mColorViewUniform, math::Mat4::Identity());
    }

    void OpenGLRenderer::endUI() {

    }

    void OpenGLRenderer::clearScreen(math::Color color) {
        mDevice.clear(color);
    }

    void OpenGLRenderer::drawRect(const DrawRectCommand& command) {
        math::Mat4 model = math::Mat4::Translation(command.position) * math::Mat4::Scale(command.size);

        mDevice.bindShader(mColorShader);

        mDevice.setUniform(mColorModelUniform, model);
        mDevice.setUniform(mColorColorUniform, command.color);

        mDevice.bindVertexArray(mRectVAO);

        mDevice.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = 6,
            .firstVertex = 0
        });
    }

    void OpenGLRenderer::drawTexture(const DrawTextureCommand& command) {
        math::Mat4 model = math::Mat4::Translation(command.position) * math::Mat4::Scale(command.size);
        math::Mat3 transform;

        transform(0, 0) = command.uv.right - command.uv.left;
        transform(0, 1) = 0.0f;
        transform(0, 2) = command.uv.left;

        transform(1, 0) = 0.0f;
        transform(1, 1) = command.uv.top - command.uv.bottom;
        transform(1, 2) = command.uv.bottom;

        transform(2, 0) = 0.0f;
        transform(2, 1) = 0.0f;
        transform(2, 2) = 1.0f;

        mDevice.bindShader(mTextureShader);

        mDevice.setUniform(mTextureModelUniform, model);
        mDevice.setUniform(mTextureTransformUniform, transform);
        mDevice.setUniform(mTextureColorUniform, command.color);

        mDevice.bindVertexArray(mTextureVAO);

        mDevice.bindTexture(0, command.texture);

        mDevice.setUniform(mTextureSamplerUniform, 0);

        mDevice.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = 6,
            .firstVertex = 0
        });
    }

    void OpenGLRenderer::initGL() {
        createShaders();
        createRectGeometry();
        createTextureGeometry();
    }

    void OpenGLRenderer::createShaders() {
        mColorShader = mDevice.createShader(shaders::ColorVertex, shaders::ColorFragment);

        mColorProjectionUniform = mDevice.getUniform(mColorShader, "uProjection");
        mColorViewUniform = mDevice.getUniform(mColorShader, "uView");
        mColorModelUniform = mDevice.getUniform(mColorShader, "uModel");
        mColorColorUniform = mDevice.getUniform(mColorShader, "uColor");


        mTextureShader = mDevice.createShader(shaders::TextureVertex, shaders::TextureFragment);

        mTextureProjectionUniform = mDevice.getUniform(mTextureShader, "uProjection");
        mTextureViewUniform = mDevice.getUniform(mTextureShader, "uView");
        mTextureModelUniform = mDevice.getUniform(mTextureShader, "uModel");
        mTextureTransformUniform = mDevice.getUniform(mTextureShader, "uTextureTransform");
        mTextureColorUniform = mDevice.getUniform(mTextureShader, "uColor");
        mTextureSamplerUniform = mDevice.getUniform(mTextureShader, "uTexture");
    }

    void OpenGLRenderer::createRectGeometry() {
        struct RectVertex {
            math::Vec2 position;
        };

        static std::array<RectVertex, 6> VERTICES = {{
            {{-0.5f, -0.5f}},
            {{0.5f, -0.5f}},
            {{0.5f, 0.5f}},
            {{-0.5f, -0.5f}},
            {{0.5f, 0.5f}},
            {{-0.5f, 0.5f}}
        }};

        static constexpr VertexAttribute POSITION_ATTRIBUTE = {
            .location = 0,
            .components = 2,
            .type = VertexAttributeType::Float,
            .normalized = false,
            .offset = 0
        };

        mRectVBO = mDevice.createBuffer(BufferUsage::Static, sizeof(VERTICES), VERTICES.data());
        mRectVAO = mDevice.createVertexArray();

        mDevice.setVertexLayout(
            mRectVAO,
            mRectVBO,
            sizeof(RectVertex),
            std::span(&POSITION_ATTRIBUTE, 1)
        );
    }

    void OpenGLRenderer::createTextureGeometry() {
        static std::array<TextureVertex, 6> VERTICES = {{
            {{-0.5f, -0.5f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f}, {1.0f, 1.0f}},

            {{-0.5f, -0.5f}, {0.0f, 0.0f}},
            {{ 0.5f,  0.5f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f}, {0.0f, 1.0f}}
        }};

        VertexAttribute attributes[] = {
            {
                .location = 0,
                .components = 2,
                .type = VertexAttributeType::Float,
                .normalized = false,
                .offset = offsetof(TextureVertex, position)
            },
            {
                .location = 1,
                .components = 2,
                .type = VertexAttributeType::Float,
                .normalized = false,
                .offset = offsetof(TextureVertex, uv)
            }
        };

        mTextureVBO = mDevice.createBuffer(BufferUsage::Static, sizeof(VERTICES), VERTICES.data());
        mTextureVAO = mDevice.createVertexArray();

        mDevice.setVertexLayout(
            mTextureVAO,
            mTextureVBO,
            sizeof(TextureVertex),
            attributes
        );
    }
}
