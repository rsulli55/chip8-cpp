#include <filesystem> 
#include <fstream>

#include "spdlog/spdlog.h"


#include "shader.h"

Shader::Shader(std::string_view vertex_src_path, std::string_view fragment_src_path) {
    // read shader source
    auto vertex_src = load_glsl_src(vertex_src_path);
    auto fragment_src = load_glsl_src(fragment_src_path);
    // build shaders 
    auto [v_status, vertex] = build_shader(GLenum::GL_VERTEX_SHADER, vertex_src);
    auto [f_status, fragment] = build_shader(GLenum::GL_FRAGMENT_SHADER, fragment_src);
    // build program and delete shaders
    auto [p_status, program] = build_program(vertex, fragment);
    glDetachShader(program, vertex);
    glDeleteShader(vertex);
    glDetachShader(program, fragment);
    glDeleteShader(fragment);
    // store program id
    id_ = program;
    glUseProgram(id_);
}

Shader::~Shader() {
    spdlog::debug("Deleting Shader");
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
        GLint max_length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
        auto buffer = std::string(max_length, '\0');
        glGetShaderInfoLog(shader, 1024, nullptr, buffer.data());
        spdlog::error("Program linking failed: {}", buffer);
        spdlog::error("Shader compilation failed: {}", buffer);
        return false;
    }

    return true;
}

bool check_program_linking(GLuint program) {
    int status = 0;
    glGetProgramiv(program, GLenum::GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLint max_length = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
        auto buffer = std::string(max_length, '\0');
        glGetProgramInfoLog(program, 1024, nullptr, buffer.data());
        spdlog::error("Program linking failed: {}", buffer);
        return false;
    }

    return true;
}

std::string load_glsl_src(std::string_view path) {
    namespace fs = std::filesystem;
    fs::path file_path{path};

    if (!fs::is_regular_file(file_path)) {
        spdlog::error("Shader file: {} is not a regular file", file_path.string());
        return "";
    }

    std::ifstream file{file_path, std::ios::in};
    if (!file.is_open()) { 
        spdlog::error("Shader file: could not open {}", file_path.string());
        return "";
    }

    auto file_size = fs::file_size(file_path);
    auto to_return = std::string(file_size, '\0');

    file.read(to_return.data(), file_size);

    spdlog::debug("Loaded the following shader:\n{}\n", to_return);

    return to_return;
}
