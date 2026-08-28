// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/opengl/renderer.h"
#include "engine/backends/opengl/shader_programs.h"

#include "engine/util/exceptions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace backend {
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

        if (mShader) {
            mDevice.destroyShader(mShader);
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

        mDevice.bindShader(mShader);
        mDevice.setUniform(mProjectionUniform, camera.projectionMatrix(viewportSize));
        mDevice.setUniform(mViewUniform, camera.viewMatrix());
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

        mDevice.bindShader(mShader);
        mDevice.setUniform(mProjectionUniform, projection);
        mDevice.setUniform(mViewUniform, math::Mat4::Identity());
    }

    void OpenGLRenderer::endUI() {

    }

    void OpenGLRenderer::clearScreen(math::Color color) {
        mDevice.clear(color);
    }

    void OpenGLRenderer::drawRect(const DrawRectCommand& command) {
        math::Mat4 model = math::Mat4::Translation(command.position) * math::Mat4::Scale(command.size);

        mDevice.bindShader(mShader);

        mDevice.setUniform(mModelUniform, model);
        mDevice.setUniform(mColorUniform, command.color);

        mDevice.bindVertexArray(mRectVAO);

        mDevice.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = 6,
            .firstVertex = 0
        });
    }

    void OpenGLRenderer::initGL() {
        createShaders();
        createRectGeometry();
    }

    void OpenGLRenderer::createShaders() {
        mShader = mDevice.createShader(shaders::Vertex, shaders::Fragment);

        mProjectionUniform = mDevice.getUniform(mShader, "uProjection");
        mViewUniform = mDevice.getUniform(mShader, "uView");
        mModelUniform = mDevice.getUniform(mShader, "uModel");
        mColorUniform = mDevice.getUniform(mShader, "uColor");
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
}
