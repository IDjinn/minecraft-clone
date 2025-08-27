cmake_minimum_required(VERSION 3.10)
project(minecraft)

set(CMAKE_CXX_STANDARD 17)

add_library(minecraft_lib
        src/glad.c
        include/stb_image/stb_image.h
        src/game/world/World.cpp
        src/game/world/World.h
        src/game/world/chunks/Chunk.cpp
        src/game/world/chunks/Chunk.h
        src/game/world/blocks/Block.h
        src/game/world/blocks/BlockType.h
        src/game/players/Player.cpp
        src/game/players/Player.h
        src/game/world/blocks/Block.cpp
        src/game/world/blocks/Block.h
        src/game/world/WorldConstants.h
        src/utils/Assert.h
        src/utils/DebugGui.cpp
        src/utils/DebugGui.h
        src/render/Render.cpp
        src/render/Render.h
        src/game/GameConstants.h
        src/render/TextureManager.cpp
        src/render/TextureManager.h
        src/render/ShaderManager.cpp
        src/render/ShaderManager.h
        src/game/Game.cpp
        src/game/Game.h
        src/game/world/generation/WorldGeneration.cpp
        src/game/world/generation/WorldGeneration.h
)

add_executable(minecraft src/main.cpp)

add_subdirectory(include/FastNoise2)
target_include_directories(minecraft_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/imgui
        ${CMAKE_SOURCE_DIR}/include/imgui/backends
        ${CMAKE_SOURCE_DIR}/glfw/include
        ${CMAKE_SOURCE_DIR}/include/FastNoise2/include
)

target_sources(minecraft_lib PRIVATE
        ${CMAKE_SOURCE_DIR}/include/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/include/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/include/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/include/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/include/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/include/imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(minecraft_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/glfw/lib-mingw-w64/libglfw3.a
        opengl32
        gdi32
        FastNoise
)

target_compile_options(minecraft_lib PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(minecraft minecraft_lib)

set_property(TARGET minecraft PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(tests)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-O0 -g")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "-O3")
endif ()