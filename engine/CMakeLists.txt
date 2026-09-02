cmake_minimum_required(VERSION 3.26)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.5.1
)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/dav1dde/glad
    GIT_TAG v2.0.8
    SOURCE_SUBDIR cmake
)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG master
)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio
    GIT_TAG 0.11.25
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(
    glfw
    glad
)

glad_add_library(glad_gl_core_33
    STATIC
    REPRODUCIBLE
    API gl:core=3.3
)

FetchContent_GetProperties(stb)

if(NOT stb_POPULATED)
    FetchContent_Populate(stb)

    add_library(stb INTERFACE)

    target_include_directories(stb
        INTERFACE
            ${stb_SOURCE_DIR}
    )
endif()

FetchContent_GetProperties(miniaudio)

if(NOT miniaudio_POPULATED)
    FetchContent_Populate(miniaudio)

    add_library(miniaudio STATIC ${miniaudio_SOURCE_DIR}/miniaudio.c)

    target_include_directories(miniaudio
        PUBLIC
            ${miniaudio_SOURCE_DIR}
    )
endif()

find_package(OpenGL REQUIRED)

option(UNNAMEDGAME_TARGET_MULTITHREADED "Enable/disable certain features such as atomics and internal mutexes to make the engine multithreading safe. Keep off for single threaded usages to improve performance" OFF)

set(SOURCES
    src/backends/glfw/input_provider.cpp
    src/backends/glfw/window.cpp

    src/backends/opengl/graphics_device.cpp
    src/backends/opengl/renderer.cpp

    src/backends/stb/asset_provider.cpp

    src/resource/resource_manager.cpp
    src/resource/texture.cpp

    src/util/resource_location.cpp
    src/util/unicode.cpp
)

set(HEADERS
    include/engine/backends/glfw/input_provider.h
    include/engine/backends/glfw/window.h

    include/engine/backend/asset_provider.h
    include/engine/backend/backend.h
    include/engine/backend/graphics_device.h
    include/engine/backend/input_provider.h
    include/engine/backend/renderer.h
    include/engine/backend/window.h

    include/engine/backends/opengl/graphics_device.h
    include/engine/backends/opengl/renderer.h
    include/engine/backends/opengl/shader_programs.h

    include/engine/backends/stb/asset_provider.h

    include/engine/util/exceptions.h
    include/engine/util/handle.h
    include/engine/util/math.h
    include/engine/util/object_allocator.h
    include/engine/util/resource_location.h
    include/engine/util/timer.h
    include/engine/util/unicode.h
    include/engine/resource/texture.h

    include/engine/compile_options.h
    include/engine/resource/resource_manager.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(unnamedgame-engine STATIC ${SOURCES} ${HEADERS})
add_library(UnnamedGame::Engine ALIAS unnamedgame-engine)

target_compile_definitions(unnamedgame-engine PRIVATE UNNAMEDGAME_RL_DEFAULT_DOMAIN="${PROJECT_SOURCE_DIR}/assets")
target_compile_definitions(unnamedgame-engine PRIVATE UNNAMEDGAME_RL_DEFAULT_DOMAINsv="${PROJECT_SOURCE_DIR}/assets"sv)

if (UNNAMEDGAME_TARGET_MULTITHREADED)
    target_compile_definitions(unnamedgame-engine PUBLIC UNNAMEDGAME_TARGET_MULTITHREADED=1)
endif()

target_include_directories(unnamedgame-engine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(unnamedgame-engine PUBLIC cxx_std_20)

target_link_libraries(unnamedgame-engine
    PUBLIC
        glfw
        OpenGL::GL
        glad_gl_core_33
        stb
        miniaudio
)