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
        void resize() noexcept;
        void play_audio() noexcept;
        void pause_audio() noexcept;



    private: 
    SDL_Window *window_ = nullptr;
    SDL_GLContext gl_context_ = nullptr;
    Renderer& renderer_;

    // SDL sound stuff
    SDL_AudioSpec beep_spec_{.freq = 44100, .format = AUDIO_S16SYS, .channels = 1, .samples = 1024};
    std::vector<int16_t> beep_samples_;
    SDL_AudioDeviceID audio_device_;
    static constexpr u8 beep_sec_ = 3;
    static constexpr u8 beep_freq_ = 400;
    static constexpr u8 beep_gain_ = 2000;

    u8 screen_scale_;
    u32 window_width_;
    u32 window_height_;

    void init_video();
    void init_audio();
    void queue_audio();
    void debug_window_widget(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show);
    void registers_box(const Chip8& chip8);
    void instructions_box(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show);
    void start_ImGui_frame();
    void render_ImGui_frame(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show);
    std::vector<int16_t> build_samples(u8 seconds, u32 frequency, u32 gain);

    // constants
    const char *WINDOW_NAME = "Chip8-cpp";
};

static void gl_dbg_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                 GLsizei length, const GLchar* message, const void* userParam) {
    spdlog::debug("GL CALLBACK: {}, severity = {:x}, message = {}",
            (type == GL_DEBUG_TYPE_ERROR ? "GL Error" : ""),
            severity, message);
}


