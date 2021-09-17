# Taken from Modern CMake Tutorial https://cliutils.gitlab.io/modern-cmake/chapters/basics.html

add_executable(initialization_tests initialization.cpp ../src/chip8.cpp)
add_executable(instruction_tests instructions.cpp ../src/chip8.cpp)
add_executable(helper_tests helpers.cpp ../src/chip8.cpp ../src/instruction.cpp)
add_executable(functionality_tests functionality.cpp ../src/chip8.cpp)

set_property(TARGET initialization_tests
    PROPERTY CXX_STANDARD 20)
set_property(TARGET instruction_tests
    PROPERTY CXX_STANDARD 20)
set_property(TARGET helper_tests
    PROPERTY CXX_STANDARD 20)
set_property(TARGET functionality_tests
    PROPERTY CXX_STANDARD 20)

conan_target_link_libraries(initialization_tests CONAN_PKG::boost-ext-ut CONAN_PKG::spdlog)
target_link_libraries(initialization_tests -fsanitize=address)
conan_target_link_libraries(instruction_tests CONAN_PKG::boost-ext-ut CONAN_PKG::spdlog)
target_link_libraries(instruction_tests -fsanitize=address)
conan_target_link_libraries(helper_tests CONAN_PKG::boost-ext-ut CONAN_PKG::spdlog)
target_link_libraries(helper_tests -fsanitize=address)
conan_target_link_libraries(functionality_tests CONAN_PKG::boost-ext-ut CONAN_PKG::spdlog)
target_link_libraries(functionality_tests -fsanitize=address)

target_compile_options(initialization_tests PRIVATE -Werror -Wall -Wextra -pedantic-errors)
target_compile_options(instruction_tests PRIVATE -Werror -Wall -Wextra -pedantic-errors)
target_compile_options(helper_tests PRIVATE -Werror -Wall -Wextra -pedantic-errors)
target_compile_options(functionality_tests PRIVATE -Werror -Wall -Wextra -pedantic-errors)

add_test(NAME initialization COMMAND $<TARGET_FILE:initialization_tests>)
add_test(NAME helpers COMMAND $<TARGET_FILE:helper_tests>)
add_test(NAME instructions COMMAND $<TARGET_FILE:instruction_tests>)
add_test(NAME functionality COMMAND $<TARGET_FILE:functionality_tests>)
