# Taken from Modern CMake Tutorial https://cliutils.gitlab.io/modern-cmake/chapters/basics.html
cmake_minimum_required(VERSION 3.10...3.20)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()


project(chip8-cpp VERSION 0.1
        DESCRIPTION "CHIP-8 emulator in C++"
        LANGUAGES CXX)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

if(CMAKE_PROJECT_NAME STREQUAL chip8-cpp)
    include(CTest)
endif()


conan_basic_setup(TARGETS)

add_subdirectory(tests)

add_executable(chip8_cpp src/main.cpp src/chip8.cpp src/emu.cpp)
set_property(TARGET chip8_cpp
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(chip8_cpp CONAN_PKG::spdlog CONAN_PKG::sdl2 CONAN_PKG::pulseaudio)
target_link_libraries(chip8_cpp -fsanitize=address)
