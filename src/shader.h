#pragma once

#include <glbinding/gl/gl.h>

#include "common.h"

using namespace gl;
class Shader {
    GLuint id_;

    public: 
    Shader(std::string_view vertex_src_path, std::string_view fragment_src_path);
    ~Shader();
    
    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

    GLuint id() const noexcept { return id_; }

    void set_bool(const std::string& name, bool value) const noexcept {
        glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
    }

    void set_int(const std::string& name, int value) const noexcept {
        glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
    }

    void set_float(const std::string& name, float value) const noexcept {
        glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
    }

    void set_float3(const std::string& name, float v1, float v2, float v3) const noexcept {
        glUniform3f(glGetUniformLocation(id_, name.c_str()), v1, v2, v3);
    }

};

bool check_shader_compilation(GLuint shader);
bool check_program_linking(GLuint program);

std::string load_glsl_src(std::string_view path);

// returns status and id for shader
// @param type is one of GLenum::GL_VERTEX_SHADER or GLenum::GL_FRAGMENT_SHADER
std::pair<bool, GLuint> build_shader(GLenum type, std::string_view src);

// returns status and id for shader program
std::pair<bool, GLuint> build_program(GLuint vert_shader, GLuint frag_shader);






