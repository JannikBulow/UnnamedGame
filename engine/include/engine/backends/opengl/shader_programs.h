// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H
#define UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H

namespace backend::shaders {
    constexpr const char* Vertex = R"(
#version 330 core

layout(location = 0) in vec2 aPosition;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 0.0, 1.0);
}
)";

    constexpr const char* Fragment = R"(
#version 330 core

uniform vec4 uColor;

out vec4 FragColor;

void main() {
    FragColor = uColor;
}
)";
}

#endif //UNNAMEDGAME_ENGINE_BACKENDS_OPENGL_SHADER_PROGRAMS_H
