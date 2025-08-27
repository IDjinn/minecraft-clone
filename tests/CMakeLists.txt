cmake_minimum_required(VERSION 3.10)

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(minecraft_tests
        test_main.cpp
        test_coordinates.cpp
)

target_link_libraries(minecraft_tests
        PRIVATE
        minecraft_lib
        Catch2::Catch2WithMain
)

target_include_directories(minecraft_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)

include(CTest)
include(Catch)
catch_discover_tests(minecraft_tests)

target_compile_options(minecraft_tests PRIVATE -Wall -Wextra -pedantic)

set_property(TARGET minecraft_tests PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")