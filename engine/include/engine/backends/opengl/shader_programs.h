// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H
#define UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H

namespace backend::shaders {
    constexpr const char* ColorVertex = R"(
#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aColor;

uniform mat4 uProjection;
uniform mat4 uView;

out vec4 vColor;

void main() {
    vColor = aColor;
    gl_Position = uProjection * uView * vec4(aPosition, 0.0, 1.0);
}
)";

    constexpr const char* ColorFragment = R"(
#version 330 core

in vec4 vColor;

out vec4 FragColor;

void main() {
    FragColor = vColor;
}
)";

    constexpr const char* TextureVertex = R"(
#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

out vec2 vUV;
out vec4 vColor;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    vUV = aUV;
    vColor = aColor;

    gl_Position = uProjection * uView * vec4(aPosition, 0.0, 1.0);
}
)";

    constexpr const char* TextureFragment = R"(
#version 330 core

in vec2 vUV;
in vec4 vColor;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(uTexture, vUV) * vColor;
}
)";
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H
