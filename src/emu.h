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
#include "key.h"
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



    void handle_chip8_keydown(const Uint8* const keys);
    void handle_chip8_keyup(const Uint8* const keys);

    void inline push_chip8();
    void inline pop_chip8();

    static const std::map<SDL_Scancode, Key> keymap_;  
    /* { */
    /*     {SDL_SCANCODE_1, Key::One},     {SDL_SCANCODE_2, Key::Two},     {SDL_SCANCODE_3, Key::Three},   {SDL_SCANCODE_C, Key::C}, */
    /*     {SDL_SCANCODE_4, Key::Four},    {SDL_SCANCODE_5, Key::Five},    {SDL_SCANCODE_6, Key::Six},     {SDL_SCANCODE_D, Key::D}, */
    /*     {SDL_SCANCODE_7, Key::Seven},   {SDL_SCANCODE_8, Key::Eight},   {SDL_SCANCODE_9, Key::Nine},    {SDL_SCANCODE_E, Key::E}, */
    /*     {SDL_SCANCODE_A, Key::A},       {SDL_SCANCODE_0, Key::Zero},    {SDL_SCANCODE_B, Key::B},       {SDL_SCANCODE_F, Key::F} */
    /* }; */

};

auto read_rom_file(std::string_view path) -> Rom;
auto build_past_opcodes(const std::deque<Chip8>& chip8s, u32 max_ops)
    -> std::vector<u16>;
