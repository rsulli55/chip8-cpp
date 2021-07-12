#include "SDL.h"
#include "spdlog/spdlog.h"
#include "emu.h"
#include "chip8.h"

int main(int argc, char *argv[]) {
    
    Chip8 chip8;
    chip8.set_debug_level(spdlog::level::debug);
    Emu emu {chip8, 16};

    const auto rom = emu.load_rom_file("ibm_logo.ch8");
    chip8.load_rom(rom);
    for (auto i = 0; i < 100'000; ++i) 
        emu.step();
}
