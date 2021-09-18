#include "SDL.h"
#include "spdlog/spdlog.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
#include <cstdlib>
#include <deque>
#include <exception>
#include <filesystem>
#include <fstream>
#include <map>
#include <string_view>

#include "chip8.h"
#include "common.h"
#include "instruction.h"
#include "renderer.h"
#include "rom.h"
#include "window.h"

class Emu {
   public:
    Emu(u8 screen_scale);

    void load_rom(const Rom& rom);
    void run();
    void cycle_forward(u8 cycles_remaining);
    void render();
    void step();
    auto handle_event(const SDL_Event& event) -> u8;


    static constexpr u32 CHIP8_HISTORY_AMOUNT = 200;
    static constexpr u32 MAX_PAST_OPS = 35;
    static constexpr u32 FUTURE_OPS = 15;

   private:
    SDL_Event event_;
    std::deque<Chip8> chip8s_;
    Chip8 chip8_;

    // TODO: move some of these to a Settings struct
    /* u8 screen_scale_; */
    /* u32 window_width_; */
    /* u32 window_height_; */
    u32 frames_per_second_ = 60;
    u32 instructions_per_frame_ = 10;

    bool running_ = true;
    bool chip8_paused_ = true;
    bool new_state_ = false;

    Window window_;
    Renderer renderer_;
};

auto read_rom_file(std::string_view path) -> Rom;
auto build_past_opcodes(const std::deque<Chip8>& chip8s, u32 max_ops)
    -> std::vector<u16>;
