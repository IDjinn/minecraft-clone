cmake_minimum_required(VERSION 3.10)
project(minecraft)

set(CMAKE_CXX_STANDARD 17)

add_executable(minecraft
        src/main.cpp
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
        src/game/world/generation/WorldGenerator.cpp
        src/game/world/generation/WorldGenerator.h
        src/utils/Assert.h
)

target_include_directories(minecraft PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/glfw/include
)

target_link_libraries(minecraft
        ${CMAKE_SOURCE_DIR}/glfw/lib-mingw-w64/libglfw3.a
        opengl32
        gdi32
)


target_include_directories(minecraft PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_options(minecraft PRIVATE -Wall -Wextra -pedantic)

set_property(TARGET minecraft PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
