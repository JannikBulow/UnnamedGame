// Copyright 2026 Jannik Laugmand Bülow

#include "engine/backends/opengl/renderer.h"
#include "engine/backends/opengl/shader_programs.h"

#include "engine/util/exceptions.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <cstring>

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
        , mWindow(window)
        , mRectPipeline(*this)
        , mTexturePipeline(*this)
        , mFontPipeline(*this)
        , mRectBatch(*this)
        , mTextureBatch(*this)
        , mRenderQueue({&mRectPipeline, &mTexturePipeline, &mFontPipeline}, {&mRectBatch, &mTextureBatch}) {
        if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
            throw util::GameException();
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugCallback, nullptr);

        mRectPipeline = {*this, mDevice.createShader(shaders::ColorVertex, shaders::ColorFragment)};
        mTexturePipeline = {*this, mDevice.createShader(shaders::TextureVertex, shaders::TextureFragment)};
        mFontPipeline = {*this, mDevice.createShader(shaders::FontVertex, shaders::FontFragment)};
        mRectBatch = {*this, CreateRectAttributes(), sizeof(ColorVertex), MAX_BATCH_QUADS * 6};
        mTextureBatch = {*this, CreateTextureAttributes(), sizeof(TextureVertex), MAX_BATCH_QUADS * 6};

        warmupShaders();
    }

    OpenGLRenderer::~OpenGLRenderer() {

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

        mRenderQueue.setProjections(projection);
        mRenderQueue.setViews(view);

        mDevice.setBlendMode(BlendMode::Alpha);

        mFontPixelScale = 1.0f / camera.pixelsPerWorldUnit(viewportSize);
    }

    void OpenGLRenderer::endWorld() {
        mRenderQueue.flush();
    }

    void OpenGLRenderer::beginUI() {
        math::Vec2I frameBufferSize = mWindow.getFramebufferSize();
        math::Mat4 projection = math::Mat4::Orthographic(
            0.0f,
            static_cast<float>(frameBufferSize.x),
            static_cast<float>(frameBufferSize.y),
            0.0f
        );

        mRenderQueue.setProjections(projection);
        mRenderQueue.setViews(math::Mat4::Identity());

        mFontPixelScale = 1.0f;
    }

    void OpenGLRenderer::endUI() {
        mRenderQueue.flush();
    }

    void OpenGLRenderer::clearScreen(math::Color color) {
        mRenderQueue.flush(); // could we just clear it without drawing?

        mDevice.clear(color);
    }

    void OpenGLRenderer::drawRect(const DrawRectCommand& command) {
        math::Mat4 model = math::Mat4::Translation(command.position) * math::Mat4::RotationZ(-command.rotation) * math::Mat4::Scale(command.size);
        std::array<ColorVertex, 6> vertices{};
        for (int i = 0; i < QUAD_CORNERS.size(); i++) {
            vertices[i] = {TransformPoint(model, QUAD_CORNERS[i]), math::Color4F(command.color)};
        }
        mRenderQueue.submit(mRectPipeline, mRectBatch, nullptr, nullptr, 6, vertices.data());
    }

    void OpenGLRenderer::drawTexture(const DrawTextureCommand& command) {
        drawTextureTo(command, mTexturePipeline, mTextureBatch);
    }

    void OpenGLRenderer::drawCodepoint(const DrawCodepointCommand& command) {
        const Glyph& glyph = command.font.glyphs[command.font.getGlyphIndex(command.codepoint)];

        float scale = (command.fontSize * mFontPixelScale) / static_cast<float>(command.font.baseSize);
        float glyphWidth = (glyph.atlasBounds.right - glyph.atlasBounds.left) * scale;
        float glyphHeight = (glyph.atlasBounds.bottom - glyph.atlasBounds.top) * scale;

        drawTextureTo({
            .texture = command.texture,
            .position = command.centerOrigin ? command.position : math::Vec2{
                command.position.x + glyph.offsetX * scale + glyphWidth * 0.5f,
                command.position.y + glyph.offsetY * scale - glyphHeight * 0.5f
            },
            .size = {glyphWidth, glyphHeight},
            .uv = {
                glyph.atlasBounds.left / static_cast<float>(command.font.atlas.width),
                glyph.atlasBounds.right / static_cast<float>(command.font.atlas.width),
                glyph.atlasBounds.top / static_cast<float>(command.font.atlas.height),
                glyph.atlasBounds.bottom / static_cast<float>(command.font.atlas.height)
            },
            .color = command.color
        }, mFontPipeline, mTextureBatch);
    }

    void OpenGLRenderer::drawCodepoints(const DrawCodepointsCommand& command) {
        math::Vec2 pen = command.position;

        if (command.centerOrigin) {
            math::Vec2 sizePixels = command.font.measureCodepoints(command.codepoints, command.fontSize, command.spacing, command.textLineSpacing);
            math::Vec2 sizeWorld = sizePixels * mFontPixelScale;
            pen -= sizeWorld * 0.5f;
        }

        float textOffsetX = 0.0f;
        float textOffsetY = 0.0f;

        float scale = (command.fontSize * mFontPixelScale) / static_cast<float>(command.font.baseSize);

        for (unicode::codepoint codepoint : command.codepoints) {
            const Glyph& glyph = command.font.glyphs[command.font.getGlyphIndex(codepoint)];

            if (codepoint == '\n') {
                textOffsetX = 0.0f;
                textOffsetY -= (command.fontSize + command.textLineSpacing) * mFontPixelScale;
            } else {
                if (codepoint != ' ' && codepoint != '\t') {
                    drawCodepoint({
                        .texture = command.texture,
                        .font = command.font,
                        .codepoint = codepoint,
                        .fontSize = command.fontSize,
                        .position = {pen.x + textOffsetX, pen.y + textOffsetY},
                        .color = command.color
                    });
                }

                if (glyph.advanceX == 0) textOffsetX += glyph.atlasBounds.width() * scale + command.spacing * mFontPixelScale;
                else textOffsetX += glyph.advanceX * scale + command.spacing * mFontPixelScale;
            }
        }
    }

    void OpenGLRenderer::drawText(const DrawTextCommand& command) {
        size_t textLength = command.textLength ? command.textLength : strlen(command.text);

        math::Vec2 pen = command.position;

        if (command.centerOrigin) {
            math::Vec2 sizePixels = command.font.measureText(command.text, command.fontSize, command.spacing, command.textLineSpacing);
            math::Vec2 sizeWorld = sizePixels * mFontPixelScale;
            pen -= sizeWorld * 0.5f;
        }

        float textOffsetX = 0.0f;
        float textOffsetY = 0.0f;

        float scale = (command.fontSize * mFontPixelScale) / static_cast<float>(command.font.baseSize);

        for (size_t i = 0; i < textLength;) {
            int codepointSize = 0;
            unicode::codepoint codepoint = unicode::GetNextCodepoint(command.text + i, &codepointSize);
            const Glyph& glyph = command.font.glyphs[command.font.getGlyphIndex(codepoint)];

            if (codepoint == '\n') {
                textOffsetX = 0.0f;
                textOffsetY -= (command.fontSize + command.textLineSpacing) * mFontPixelScale;
            } else {
                if (codepoint != ' ' && codepoint != '\t') {
                    drawCodepoint({
                        .texture = command.texture,
                        .font = command.font,
                        .codepoint = codepoint,
                        .fontSize = command.fontSize,
                        .position = {pen.x + textOffsetX, pen.y + textOffsetY},
                        .color = command.color
                    });
                }

                if (glyph.advanceX == 0) textOffsetX += glyph.atlasBounds.width() * scale + command.spacing * mFontPixelScale;
                else textOffsetX += glyph.advanceX * scale + command.spacing * mFontPixelScale;
            }

            i += codepointSize;
        }
    }

    OpenGLRenderer::Pipeline::Pipeline(OpenGLRenderer& renderer)
        : mRenderer(renderer) {}

    OpenGLRenderer::Pipeline::Pipeline(OpenGLRenderer& renderer, ShaderHandle shader)
        : mRenderer(renderer)
        , mShader(shader) {
        auto& device = mRenderer.mDevice;

        device.bindShader(mShader);
        mProjectionUniform = device.getUniform(mShader, "uProjection");
        mViewUniform = device.getUniform(mShader, "uView");
        mTextureUniform = device.getUniform(mShader, "uTexture");

        if (mTextureUniform) device.setUniform(mTextureUniform, 0); // texture always in 0 anyway
    }

    OpenGLRenderer::Pipeline::~Pipeline() {
        if (mShader) mRenderer.mDevice.destroyShader(mShader);
    }

    OpenGLRenderer::Pipeline& OpenGLRenderer::Pipeline::operator=(Pipeline&& other) noexcept {
        if (&mRenderer != &other.mRenderer) throw util::GameException("moving pipelines between renderers");

        mShader = other.mShader;
        mProjectionUniform = other.mProjectionUniform;
        mViewUniform = other.mViewUniform;
        mTextureUniform = other.mTextureUniform;
        mProjection = other.mProjection;
        mView = other.mView;

        other.mShader = nullptr;
        other.mProjectionUniform = nullptr;
        other.mViewUniform = nullptr;
        other.mTextureUniform = nullptr;

        return *this;
    }

    math::Mat4& OpenGLRenderer::Pipeline::projection() {
        return mProjection;
    }

    math::Mat4& OpenGLRenderer::Pipeline::view() {
        return mView;
    }

    void OpenGLRenderer::Pipeline::uploadUniforms() {
        auto& device = mRenderer.mDevice;
        device.bindShader(mShader);
        device.setUniform(mProjectionUniform, mProjection);
        device.setUniform(mViewUniform, mView);
    }

    void OpenGLRenderer::Pipeline::bind() {
        mRenderer.mDevice.bindShader(mShader);
    }

    void OpenGLRenderer::Pipeline::setTexture(TextureHandle texture) {
        mRenderer.mDevice.bindTexture(0, texture);
    }

    void OpenGLRenderer::Pipeline::setSampler(SamplerHandle sampler) {
        mRenderer.mDevice.bindSampler(0, sampler);
    }

    OpenGLRenderer::Batch::Batch(OpenGLRenderer& renderer)
        : mRenderer(renderer) {}

    OpenGLRenderer::Batch::Batch(OpenGLRenderer& renderer, std::span<const VertexAttribute> attributes, uint32_t vertexSize, uint32_t maxVertices)
        : mRenderer(renderer)
        , mVertexBytes(std::make_unique<uint8_t[]>(vertexSize * maxVertices))
        , mVertexCount(0)
        , mVertexSize(vertexSize)
        , mMaxVertices(maxVertices) {
        mVBO = mRenderer.mDevice.createBuffer(BufferUsage::Dynamic, maxVertices * vertexSize, nullptr);
        mVAO = mRenderer.mDevice.createVertexArray();

        mRenderer.mDevice.setVertexLayout(
            mVAO,
            mVBO,
            static_cast<int>(mVertexSize),
            attributes
        );
    }

    OpenGLRenderer::Batch::~Batch() {
        if (mVBO) mRenderer.mDevice.destroyBuffer(mVBO);
        if (mVAO) mRenderer.mDevice.destroyVertexArray(mVAO);
    }

    OpenGLRenderer::Batch& OpenGLRenderer::Batch::operator=(Batch&& other) noexcept {
        if (&mRenderer != &other.mRenderer) throw util::GameException("moving batches between renderers");

        mVAO = other.mVAO;
        mVBO = other.mVBO;
        mVertexBytes = std::move(other.mVertexBytes);
        mVertexCount = other.mVertexCount;
        mVertexSize = other.mVertexSize;
        mMaxVertices = other.mMaxVertices;

        other.mVAO = nullptr;
        other.mVBO = nullptr;

        return *this;
    }

    std::optional<uint32_t> OpenGLRenderer::Batch::submit(uint32_t vertexCount, const void* vertices) {
        if (mVertexCount + vertexCount > mMaxVertices) return std::nullopt;

        uint32_t start = mVertexCount;

        size_t byteOffset = mVertexCount * mVertexSize;
        memcpy(mVertexBytes.get() + byteOffset, vertices, vertexCount * mVertexSize);
        mVertexCount += vertexCount;

        return start;
    }

    void OpenGLRenderer::Batch::upload() {
        mRenderer.mDevice.updateBuffer(mVBO, 0, mVertexCount * mVertexSize, mVertexBytes.get());
        mVertexCount = 0;
    }

    void OpenGLRenderer::Batch::draw(uint32_t offset, uint32_t count) {
        auto& device = mRenderer.mDevice;

        device.bindVertexArray(mVAO);
        device.draw({
            .primitive = PrimitiveType::Triangles,
            .vertexCount = count,
            .firstVertex = offset,
        });
    }

    OpenGLRenderer::RenderQueue::RenderQueue(std::vector<Pipeline*> pipelines, std::vector<Batch*> batches)
        : mPipelines(std::move(pipelines))
        , mBatches(std::move(batches)) {
        mCommands.reserve(512);
    }

    void OpenGLRenderer::RenderQueue::setProjections(const math::Mat4& projection) {
        for (Pipeline* pipeline : mPipelines) {
            pipeline->projection() = projection;
        }
    }

    void OpenGLRenderer::RenderQueue::setViews(const math::Mat4& view) {
        for (Pipeline* pipeline : mPipelines) {
            pipeline->view() = view;
        }
    }

    void OpenGLRenderer::RenderQueue::submit(Pipeline& pipeline, Batch& batch, TextureHandle texture, SamplerHandle sampler, uint32_t vertexCount, const void* vertices) {
        std::optional<uint32_t> offset = batch.submit(vertexCount, vertices);
        if (!offset) { // batch full, try again
            flush();
            offset = batch.submit(vertexCount, vertices);
            if (!offset) [[unlikely]] throw util::GameException("huge fucking model detected");
        }

        if (!mCommands.empty()) [[likely]] {
            auto& last = mCommands.back();

            if (last.pipeline == &pipeline && last.batch == &batch && last.texture == texture && last.vertexOffset + last.vertexCount == *offset) {
                last.vertexCount += vertexCount;
                return;
            }
        }

        mCommands.emplace_back(&pipeline, &batch, texture, sampler, *offset, vertexCount);
    }

    void OpenGLRenderer::RenderQueue::flush() {
        for (Pipeline* pipeline : mPipelines) pipeline->uploadUniforms();
        for (Batch* batch : mBatches) batch->upload();

        Pipeline* currentPipeline = nullptr;
        TextureHandle currentTexture = nullptr;
        SamplerHandle currentSampler = nullptr;
        for (auto& command : mCommands) {
            if (currentPipeline != command.pipeline) {
                command.pipeline->bind();
                currentPipeline = command.pipeline;
                currentTexture = nullptr;
            }

            if (command.texture && currentTexture != command.texture) {
                command.pipeline->setTexture(command.texture);
                currentTexture = command.texture;
            }

            if (command.sampler && currentSampler != command.sampler) {
                command.pipeline->setSampler(command.sampler);
                currentSampler = command.sampler;
            }

            command.batch->draw(command.vertexOffset, command.vertexCount);
        }

        mCommands.clear();
    }

    std::span<const VertexAttribute> OpenGLRenderer::CreateRectAttributes() {
        static constexpr VertexAttribute attributes[] = {
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
        return attributes;
    }

    std::span<const VertexAttribute> OpenGLRenderer::CreateTextureAttributes() {
        static constexpr VertexAttribute attributes[] = {
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
        return attributes;
    }

    void OpenGLRenderer::drawTextureTo(const DrawTextureCommand& command, Pipeline& pipeline, Batch& batch) {
        math::Mat4 model = math::Mat4::Translation(command.position) * math::Mat4::RotationZ(-command.rotation) * math::Mat4::Scale(command.size);
        float uMin = command.uv.left;
        float uRange = command.uv.right - command.uv.left;
        float vMin = command.uv.bottom;
        float vRange = command.uv.top - command.uv.bottom;

        std::array<TextureVertex, 6> vertices{};

        for (size_t i = 0; i < QUAD_CORNERS.size(); i++) {
            math::Vec2 uv = {
                uMin + QUAD_UVS[i].x * uRange,
                vMin + QUAD_UVS[i].y * vRange
            };

            vertices[i] = {TransformPoint(model, QUAD_CORNERS[i]), uv, math::Color4F(command.color)};
        }

        mRenderQueue.submit(pipeline, batch, command.texture, command.sampler, 6, vertices.data());
    }

    void OpenGLRenderer::warmupShaders() {
        mRectPipeline.bind();
        mRectBatch.draw(0, 0);

        mTexturePipeline.bind();
        mTextureBatch.draw(0, 0);

        mFontPipeline.bind();
        mTextureBatch.draw(0, 0);
    }
}
