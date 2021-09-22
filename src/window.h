#pragma once
#include "SDL.h"
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include "chip8.h"
#include "renderer.h"

using namespace gl;
class Window {
    public:
        Window(u8 screen_scale, Renderer& renderer);
        ~Window();
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        void render(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show);
        void resize();



    private: 
    SDL_Window *window_ = nullptr;
    SDL_GLContext gl_context_ = nullptr;
    Renderer& renderer_;

    u8 screen_scale_;
    u32 window_width_;
    u32 window_height_;

    auto init_video() -> u32;
    void debug_window_widget(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show);
    void registers_box(const Chip8& chip8);
    void instructions_box(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show);
    void start_ImGui_frame();
    void render_ImGui_frame(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show);

    // constants
    const char *WINDOW_NAME = "Chip8-cpp";
};

static void gl_dbg_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                 GLsizei length, const GLchar* message, const void* userParam) {
    spdlog::debug("GL CALLBACK: {}, severity = {:x}, message = {}",
            (type == GL_DEBUG_TYPE_ERROR ? "GL Error" : ""),
            severity, message);
}

