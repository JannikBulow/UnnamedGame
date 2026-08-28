cmake_minimum_required(VERSION 3.26)

set(SOURCES
    src/main/main.cpp
)

set(HEADERS
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(unnamedgame ${SOURCES} ${HEADERS})
add_executable(UnnamedGame ALIAS unnamedgame)

target_include_directories(unnamedgame
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(unnamedgame PUBLIC cxx_std_20)

target_link_libraries(unnamedgame PRIVATE UnnamedGame::Engine)