#include "SDL.h"
#include "chip8.h"
#include "emu.h"
#include "rom.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {

    Emu emu{16, Emu::State::Debug};

    const auto rom = read_rom_file("ibm_logo.ch8");
    emu.load_rom(rom);

    emu.run();
}
