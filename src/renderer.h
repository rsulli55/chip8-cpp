#pragma once

#include <glbinding/gl/gl.h>

#include "common.h"
#include "chip8.h"
#include "rgb.h"
#include "shader.h"

using namespace gl;

class Renderer {

    public:
        Renderer(Shader& shader);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void render(const std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>& screen);

        bool show_debugger() { return show_debugger_;}
        void toggle_debugger() { show_debugger_ = !show_debugger_;}

    private:

        // vertex array object, vertex buffer object, and elements buffer object
        GLuint texture_;
        GLuint vao_;
        GLuint vbo_;
        GLuint ebo_;
        // TODO: maybe move shader to an argument of render, it is only used to set the sampler
        // for texture
        Shader& shader_;

        bool show_debugger_ = true;

      /* u32 screen_width_; */
      /* u32 screen_height_; */
      /* u32 box_size_; */
      /* u32 frames_per_second_ = 60; */
      /* u32 instructions_per_frame_ = 10; */

        // colors
        RGB bg_color_ {0.067f, 0.137f, 0.173f}; // Gunmetal
        RGB box_color_ {0.867f, 0.867f, 0.867f}; // Gainsboro

        std::array<GLubyte, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT> luminance_ {0u};

        const u8 POS_ATTR = 0;
        const u8 POS_ATTR_SIZE = 2;
        const u8 POS_OFFSET = 0;
        const u8 TEX_ATTR = 1;
        const u8 TEX_ATTR_SIZE = 2;
        const u8 TEX_OFFSET = 2 * sizeof(GLfloat);
        const u32 VERTEX_SIZE = 4 * sizeof(GLfloat);
        const u8 NUM_VERTICES = 16;
        const GLfloat vertices_[16] = {
            // screen coordinates   // texture coordinates
            -1.0f,  -1.0f,          0.0f, 0.0f,      // Lower left corner
             1.0f,  -1.0f,          1.0f, 0.0f,      // Lower right corner
             1.0f,   1.0f,          1.0f, 1.0f,      // Upper right
            -1.0f,   1.0f,          0.0f, 1.0f       // Upper left
        };
        const u8 NUM_ELEMENTS = 6;
        const GLuint elements_[6] = {
            0, 1, 2,
            2, 3, 0
        };




        void init_texture();
        void init_vbo();
        void init_ebo();
        void init_vao();
        void update_texture(const std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>& screen);

};
