#include <filesystem> 
#include <fstream>

#include "spdlog/spdlog.h"


#include "shader.h"

Shader::Shader(std::string_view vertex_src, std::string_view fragment_src) {
    // build shaders 
    auto [v_status, vertex] = build_shader(GLenum::GL_VERTEX_SHADER, vertex_src);
    auto [f_status, fragment] = build_shader(GLenum::GL_FRAGMENT_SHADER, fragment_src);
    // build program and delete shaders
    auto [p_status, program] = build_program(vertex, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    // store program id
    id_ = program;
}

Shader::~Shader() {
    glDeleteProgram(id_);
}

std::pair<bool, GLuint> build_shader(GLenum type, std::string_view src) {
    GLuint shader = glCreateShader(type);
    GLchar const* srcs[] = { src.data() };
    GLint lengths[] = { static_cast<GLint>(src.size()) };
    glShaderSource(shader, 1, srcs, lengths);
    glCompileShader(shader);
    bool success = check_shader_compilation(shader);

    return {success, shader};
}

std::pair<bool, GLuint> build_program(GLuint vert_shader, GLuint frag_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);
    
    bool success = check_program_linking(program);

    return {success, program};
}

bool check_shader_compilation(GLuint shader) {
    int status = 0;
    glGetShaderiv(shader, GLenum::GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, buffer);
        spdlog::error("Shader compilation failed: {}", buffer);
        return false;
    }

    return true;
}

bool check_program_linking(GLuint program) {
    int status = 0;
    glGetShaderiv(program, GLenum::GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[1024];
        glGetProgramInfoLog(program, 1024, nullptr, buffer);
        spdlog::error("Program linking failed: {}", buffer);
        return false;
    }

    return true;
}

std::string load_glsl_src(std::string_view path) {
    namespace fs = std::filesystem;
    fs::path file_path{path};

    if (!fs::is_regular_file(file_path)) {
        spdlog::error("Shader file: {} is not a regular file", file_path);
        return "";
    }

    std::ifstream file{path, std::ios::in};
    if (!file.is_open()) { 
        spdlog::error("Shader file: could not open {}", file_path);
        return "";
    }

    auto file_size = fs::file_size(file_path);
    auto to_return = std::string(file_size, '\0');

    file.read(to_return.data(), file_size);

    return to_return;
}
