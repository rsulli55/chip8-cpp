# Taken from Modern CMake Tutorial https://cliutils.gitlab.io/modern-cmake/chapters/basics.html

add_executable(test_suite tests.cpp)
set_property(TARGET test_suite
    PROPERTY CXX_STANDARD 20)
conan_target_link_libraries(test_suite CONAN_PKG::boost-ext-ut)
add_test(NAME suite COMMAND $<TARGET_FILE:test_suite>)
