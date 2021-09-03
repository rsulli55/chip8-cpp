#include "SDL.h"
#include "spdlog/spdlog.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string_view>

#include "chip8.h"
#include "rom.h"
#include "common.h"
#include "renderer.h"

class Emu {

  public:
    enum class State { Debug, Pause, Run };

  private:
    SDL_Window *window_;
    SDL_GLContext gl_context_;
    SDL_Event event_;

    Renderer renderer_;

    Chip8 chip8_;
    u8 screen_scale_;
    State state_;
    u32 window_width_;
    u32 window_height_;

    u32 frames_per_second_ = 60;
    u32 instructions_per_frame_ = 10;
    bool running_ = true;
    bool chip8_paused_ = true;
    // colors
    const u8 background_red = 0x0F;
    const u8 background_green = 0x0F;
    const u8 background_blue = 0xFF;

    const u8 pixel_red = 0xFF;
    const u8 pixel_green = 0x00;
    const u8 pixel_blue = 0x0F;

    // constants
    const char *WINDOW_NAME = "Chip8-cpp";

    u32 init_video();

  public:
    Emu(u8 screen_scale, State state);
    Emu(const Emu&) = delete;
    Emu(Emu&&) = delete;
    Emu& operator=(const Emu&) = delete;
    Emu& operator=(Emu&&) = delete;
    ~Emu();

    void load_rom(const Rom& rom);
    void run();
    void cycle_forward(u8 cycles_remaining);
    void render();
    void step();
    u8 handle_event(const SDL_Event &event);
};

Rom read_rom_file(const std::string_view &path);
