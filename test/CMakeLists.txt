# Taken from Modern CMake Tutorial https://cliutils.gitlab.io/modern-cmake/chapters/basics.html

add_executable(test_memory memory.cpp)
set_property(TARGET test_memory
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(test_memory CONAN_PKG::boost-ext-ut)
