#include "SDL.h"
#include "chip8.h"
#include "emu.h"
#include "rom.h"
#include "settings.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {

    spdlog::set_level(spdlog::level::debug);
    static constexpr std::string_view instr_path = "instruction_table.csv";

    Settings settings {
        .instructions_per_frame = 10, 
        .delay_decrement_per_frame = 1,
        .screen_scale = 24
    };

    Emu emu{Chip8{read_instruction_table(instr_path)}, settings};

    const auto rom = read_rom_file("ibm_logo.ch8");
    emu.load_rom(rom);


    emu.run();
}
