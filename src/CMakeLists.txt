# add_executable(imgui_gl opengl_imgui.cpp imgui_impl_sdl.cpp imgui_impl_opengl3.cpp)
# set_property(TARGET imgui_gl
#     PROPERTY CXX_STANDARD 20)
# conan_target_link_libraries(imgui_gl CONAN_PKG::spdlog CONAN_PKG::sdl CONAN_PKG::glbinding CONAN_PKG::imgui)
# target_link_libraries(imgui_gl -fsanitize=address)

# add_executable(gl_testing3 gl_testing3.cpp )
# add_executable(gl_testing2 gl_testing2.cpp )
# add_executable(gl_testing gl_testing.cpp imgui_impl_sdl.cpp imgui_impl_opengl3.cpp)
# set_property(TARGET gl_testing3
#     PROPERTY CXX_STANDARD 20)
# conan_target_link_libraries(gl_testing3 CONAN_PKG::spdlog CONAN_PKG::sdl CONAN_PKG::glbinding CONAN_PKG::imgui)
# target_link_libraries(gl_testing3 -fsanitize=address)

# set_property(TARGET gl_testing2
#     PROPERTY CXX_STANDARD 20)
# conan_target_link_libraries(gl_testing2 CONAN_PKG::spdlog CONAN_PKG::sdl CONAN_PKG::glbinding CONAN_PKG::imgui)
# target_link_libraries(gl_testing2 -fsanitize=address)

add_executable(chip8_cpp main.cpp chip8.cpp instruction.cpp emu.cpp window.cpp shader.cpp renderer.cpp imgui_impl_opengl3.cpp imgui_impl_sdl.cpp)
set_property(TARGET chip8_cpp
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(chip8_cpp CONAN_PKG::spdlog CONAN_PKG::sdl2 CONAN_PKG::pulseaudio CONAN_PKG::glbinding CONAN_PKG::imgui)
# target_link_libraries(chip8_cpp -fsanitize=address)
# Don't link with sanitizer to get valgrind to work
