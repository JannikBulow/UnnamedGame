// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H
#define UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H

namespace backend::shaders {
    constexpr const char* ColorVertex = R"(
#version 330 core

layout(location = 0) in vec2 aPosition;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 0.0, 1.0);
}
)";

    constexpr const char* ColorFragment = R"(
#version 330 core

uniform vec4 uColor;

out vec4 FragColor;

void main() {
    FragColor = uColor;
}
)";

    constexpr const char* TextureVertex = R"(
#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aUV;

out vec2 vUV;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

uniform mat3 uTextureTransform;

void main() {
    vUV = (uTextureTransform * vec3(aUV, 1.0)).xy;

    gl_Position = uProjection * uView * uModel * vec4(aPosition, 0.0, 1.0);
}
)";

    constexpr const char* TextureFragment = R"(
#version 330 core

in vec2 vUV;

uniform sampler2D uTexture;
uniform vec4 uColor;

out vec4 FragColor;

void main() {
    FragColor = texture(uTexture, vUV) * uColor;
}
)";
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H
