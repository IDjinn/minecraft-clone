cmake_minimum_required(VERSION 3.10)
project(minecraft)

set(CMAKE_CXX_STANDARD 17)

add_executable(minecraft
        src/main.cpp
        src/glad.c
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
