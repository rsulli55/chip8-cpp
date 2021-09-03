#pragma once

#include <glbinding/gl/gl.h>

#include "common.h"
#include "chip8.h"
#include "rgb.h"

using namespace gl;

class Renderer {

    public:
        Renderer() {};
        // free resources
        ~Renderer() {};
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void render(const std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>& screen);

        bool show_debugger() { return show_debugger_;}
        void toggle_debugger() { show_debugger_ = !show_debugger_;}

    private:

      u32 screen_width_;
      u32 screen_height_;
      u32 box_size_;


      u32 frames_per_second_ = 60;
      u32 instructions_per_frame_ = 10;

      // colors
      RGB bg_color_;
      RGB box_color_;

      // shaders
      GLuint vert_shader_;
      GLuint frag_shader_;
      GLuint shader_prog_;

      // vertex array object, vertex buffer object, and elements buffer object
      GLuint vao_;
      GLuint vbo_;
      GLuint ebo_;

      bool show_debugger_ = true;

      void init_shaders();

};
