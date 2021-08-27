
add_executable(imgui_gl opengl_imgui.cpp imgui_impl_sdl.cpp imgui_impl_opengl3.cpp)
set_property(TARGET imgui_gl
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(imgui_gl CONAN_PKG::spdlog CONAN_PKG::sdl2 CONAN_PKG::glbinding CONAN_PKG::imgui)
target_link_libraries(imgui_gl -fsanitize=address)

add_executable(gl_testing gl_testing.cpp )
# add_executable(gl_testing gl_testing.cpp imgui_impl_sdl.cpp imgui_impl_opengl3.cpp)
set_property(TARGET gl_testing
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(gl_testing CONAN_PKG::spdlog CONAN_PKG::sdl2 CONAN_PKG::glbinding CONAN_PKG::imgui)
target_link_libraries(gl_testing -fsanitize=address)

add_executable(vk_testing vk_testing.cpp imgui_impl_sdl.cpp imgui_impl_vulkan.cpp)
set_property(TARGET vk_testing
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(vk_testing CONAN_PKG::spdlog CONAN_PKG::sdl2 CONAN_PKG::vulkan-headers CONAN_PKG::imgui)
target_link_libraries(vk_testing -fsanitize=address)

add_executable(chip8_cpp main.cpp chip8.cpp emu.cpp)
set_property(TARGET chip8_cpp
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(chip8_cpp CONAN_PKG::spdlog CONAN_PKG::sdl2 CONAN_PKG::pulseaudio)
target_link_libraries(chip8_cpp -fsanitize=address)
