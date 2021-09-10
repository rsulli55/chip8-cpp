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

        void render(const std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>& screen);
        void resize();



    private: 
    SDL_Window *window_;
    SDL_GLContext gl_context_;
    Renderer& renderer_;

    u8 screen_scale_;
    u32 window_width_;
    u32 window_height_;

    u32 init_video();
    void debug_window_widget();
    void start_ImGui_frame();
    void render_ImGui_frame();

    // constants
    const char *WINDOW_NAME = "Chip8-cpp";
};

static void gl_dbg_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                 GLsizei length, const GLchar* message, const void* userParam) {
    spdlog::debug("GL CALLBACK: {}, severity = {:x}, message = {}",
            (type == GL_DEBUG_TYPE_ERROR ? "GL Error" : ""),
            severity, message);
}

