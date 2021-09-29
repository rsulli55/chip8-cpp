#pragma once

#include "SDL.h"
#include "spdlog/spdlog.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
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
#include "settings.h"

class Emu {
   public:
    Emu(Chip8 chip8, Settings settings);

    void load_rom(const Rom& rom);
    void run();
    void cycle_forward(u8 cycles_remaining);
    void render();
    void step();
    auto handle_event(const SDL_Event& event) -> u8;




   private:
    SDL_Event event_;
    std::deque<Chip8> chip8s_{};
    Chip8 chip8_;

    Settings settings_;

    bool running_ = true;
    bool chip8_paused_ = true;
    bool new_state_ = true;

    Window window_;
    Renderer renderer_;


    void inline push_chip8();
    void inline pop_chip8();
};

auto read_rom_file(std::string_view path) -> Rom;
auto build_past_opcodes(const std::deque<Chip8>& chip8s, u32 max_ops)
    -> std::vector<u16>;
