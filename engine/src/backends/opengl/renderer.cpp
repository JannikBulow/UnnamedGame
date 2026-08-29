// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/opengl/renderer.h"
#include "engine/backends/opengl/shader_programs.h"

#include "engine/util/exceptions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <cmath>

namespace backend {
    constexpr std::array<math::Vec2, 6> QUAD_CORNERS = {{
        {-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f},
        {-0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}
    }};

    constexpr std::array<math::Vec2, 6> QUAD_UVS = {{
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
        {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
    }};

    math::Vec2 TransformPoint(const math::Mat4& m, math::Vec2 p) {
        float x = m(0, 0) * p.x + m(0, 1) * p.y + m(0, 3);
        float y = m(1, 0) * p.x + m(1, 1) * p.y + m(1, 3);
        return {x, y};
    }


    void APIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
        int a = 67;
    }

    OpenGLRenderer::OpenGLRenderer(IGraphicsDevice& device, IWindow& window)
        : mDevice(device)
        , mWindow(window) {
        if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
            throw util::GameException();
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugCallback, nullptr);

        mRectBatch.reserve(MAX_BATCH_QUADS * 6);
        mTextureBatch.reserve(MAX_BATCH_QUADS * 6);

        initGL();
    }

    OpenGLRenderer::~OpenGLRenderer() {
        if (mColorShader) {
            mDevice.destroyShader(mColorShader);
        }

        if (mTextureShader) {
            mDevice.destroyShader(mTextureShader);
        }

        if (mRectVBO) {
            mDevice.destroyBuffer(mRectVBO);
        }

        if (mRectVAO) {
            mDevice.destroyVertexArray(mRectVAO);
        }

        if (mTextureVBO) {
            mDevice.destroyBuffer(mTextureVBO);
        }

        if (mTextureVAO) {
            mDevice.destroyVertexArray(mTextureVAO);
        }
    }

    void OpenGLRenderer::beginFrame() {
        mDevice.beginFrame();

        math::Vec2I frameBufferSize = mWindow.getFramebufferSize();

        mDevice.setViewport(0, 0, frameBufferSize.x, frameBufferSize.y);
    }

    void OpenGLRenderer::endFrame() {
        mDevice.endFrame();
        mWindow.swapBuffers();
    }

    void OpenGLRenderer::beginWorld(const Camera2D& camera) {
        math::Vec2I frameBufferSize = mWindow.getFramebufferSize();
        math::Vec2 viewportSize{frameBufferSize};

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
        flushRectBatch();
        flushTextureBatch();
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

        mDevice.bindShader(mTextureShader);
        mDevice.setUniform(mTextureProjectionUniform, projection);
        mDevice.setUniform(mTextureViewUniform, math::Mat4::Identity());
    }

    void OpenGLRenderer::endUI() {
        flushRectBatch();
        flushTextureBatch();
    }

    void OpenGLRenderer::clearScreen(math::Color color) {
        flushRectBatch();
        flushTextureBatch();

        mDevice.clear(color);
    }

    void OpenGLRenderer::drawRect(const DrawRectCommand& command) {
        if (!mTextureBatch.empty()) flushTextureBatch();
        if (mRectBatch.size() + 6 > MAX_BATCH_QUADS * 6) flushRectBatch();

        math::Mat4 model = math::Mat4::Translation(command.position) * math::Mat4::RotationZ(-command.rotation) * math::Mat4::Scale(command.size);
        for (const auto& corner : QUAD_CORNERS) {
            mRectBatch.push_back({TransformPoint(model, corner), math::Color4F(command.color)});
        }
    }

    void OpenGLRenderer::drawTexture(const DrawTextureCommand& command) {
        if (!mRectBatch.empty()) flushRectBatch();
        if (mBatchTexture != command.texture || mTextureBatch.size() + 6 > MAX_BATCH_QUADS * 6) flushTextureBatch();

        mBatchTexture = command.texture;

        math::Mat4 model = math::Mat4::Translation(command.position) * math::Mat4::RotationZ(-command.rotation) * math::Mat4::Scale(command.size);
        float uMin = command.uv.left;
        float uRange = command.uv.right - command.uv.left;
        float vMin = command.uv.bottom;
        float vRange = command.uv.top - command.uv.bottom;

        for (size_t i = 0; i < QUAD_CORNERS.size(); i++) {
            math::Vec2 uv = {
                uMin + QUAD_UVS[i].x * uRange,
                vMin + QUAD_UVS[i].y * vRange
            };

            mTextureBatch.push_back({TransformPoint(model, QUAD_CORNERS[i]), uv, math::Color4F(command.color)});
        }
    }

    void OpenGLRenderer::initGL() {
        createShaders();
        createRectGeometry();
        createTextureGeometry();

        warmupShaders(); // without this, opengl complains about performance
    }

    void OpenGLRenderer::createShaders() {
        mColorShader = mDevice.createShader(shaders::ColorVertex, shaders::ColorFragment);

        mColorProjectionUniform = mDevice.getUniform(mColorShader, "uProjection");
        mColorViewUniform = mDevice.getUniform(mColorShader, "uView");


        mTextureShader = mDevice.createShader(shaders::TextureVertex, shaders::TextureFragment);

        mTextureProjectionUniform = mDevice.getUniform(mTextureShader, "uProjection");
        mTextureViewUniform = mDevice.getUniform(mTextureShader, "uView");
        mTextureSamplerUniform = mDevice.getUniform(mTextureShader, "uTexture");

        mDevice.bindShader(mTextureShader);
        mDevice.setUniform(mTextureSamplerUniform, 0); // since textures are only bound to 0 currently we can just set it here once
    }

    void OpenGLRenderer::createRectGeometry() {
        VertexAttribute attributes[] = {
            {
                .location = 0,
                .components = 2,
                .type = VertexAttributeType::Float,
                .normalized = false,
                .offset = offsetof(ColorVertex, position)
            },
            {
                .location = 1,
                .components = 4,
                .type = VertexAttributeType::Float,
                .normalized = false,
                .offset = offsetof(ColorVertex, color)
            }
        };

        mRectVBO = mDevice.createBuffer(BufferUsage::Dynamic, MAX_BATCH_QUADS * 6 * sizeof(ColorVertex), nullptr);
        mRectVAO = mDevice.createVertexArray();

        mDevice.setVertexLayout(
            mRectVAO,
            mRectVBO,
            sizeof(ColorVertex),
            attributes
        );
    }

    void OpenGLRenderer::createTextureGeometry() {
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
            },
            {
                .location = 2,
                .components = 4,
                .type = VertexAttributeType::Float,
                .normalized = false,
                .offset = offsetof(TextureVertex, color)
            }

        };

        mTextureVBO = mDevice.createBuffer(BufferUsage::Dynamic, MAX_BATCH_QUADS * 6 * sizeof(TextureVertex), nullptr);
        mTextureVAO = mDevice.createVertexArray();

        mDevice.setVertexLayout(
            mTextureVAO,
            mTextureVBO,
            sizeof(TextureVertex),
            attributes
        );
    }

    void OpenGLRenderer::warmupShaders() {
        mDevice.bindShader(mColorShader);
        mDevice.bindVertexArray(mRectVAO);
        mDevice.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = 0,
            .firstVertex = 0
        });

        mDevice.bindShader(mTextureShader);
        mDevice.bindVertexArray(mTextureVAO);
        mDevice.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = 0,
            .firstVertex = 0
        });
    }

    void OpenGLRenderer::flushRectBatch() {
        if (mRectBatch.empty()) return;

        mDevice.bindShader(mColorShader);
        mDevice.updateBuffer(mRectVBO, 0, mRectBatch.size() * sizeof(ColorVertex), mRectBatch.data());
        mDevice.bindVertexArray(mRectVAO);

        mDevice.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = static_cast<uint32_t>(mRectBatch.size()),
            .firstVertex = 0
        });

        mRectBatch.clear();
    }

    void OpenGLRenderer::flushTextureBatch() {
        if (mTextureBatch.empty()) return;

        mDevice.bindShader(mTextureShader);
        mDevice.updateBuffer(mTextureVBO, 0, mTextureBatch.size() * sizeof(TextureVertex), mTextureBatch.data());
        mDevice.bindVertexArray(mTextureVAO);
        mDevice.bindTexture(0, mBatchTexture);

        mDevice.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = static_cast<uint32_t>(mTextureBatch.size()),
            .firstVertex = 0
        });

        mTextureBatch.clear();
    }
}
