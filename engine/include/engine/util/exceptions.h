// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_EXCEPTIONS_H
#define UNNAMEDGAME_ENGINE_UTIL_EXCEPTIONS_H

#include <format>
#include <stdexcept>

namespace util {
    class GameException : public std::runtime_error {
    public:
        explicit GameException(const char* message = "no message") : runtime_error(message) {}
        explicit GameException(const std::string& message) : runtime_error(message) {}
    };

    class AbstractShaderException : public GameException {
    public:
        AbstractShaderException(std::string_view rendererID, std::string_view stage, std::string_view log)
            : GameException(std::format("{} {}: {}", rendererID, stage, log)) {}
    };

    class ShaderCompileException : public AbstractShaderException {
    public:
        ShaderCompileException(std::string_view rendererID, std::string_view log)
            : AbstractShaderException(rendererID, "shader compilaton", log) {}
    };

    class ShaderLinkException : public AbstractShaderException {
    public:
        ShaderLinkException(std::string_view rendererID, std::string_view log)
            : AbstractShaderException(rendererID, "shader linking", log) {}
    };
}

#endif //UNNAMEDGAME_ENGINE_UTIL_EXCEPTIONS_H
