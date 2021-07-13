#include "SDL.h"
#include "chip8.h"
#include "emu.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {

    Emu emu{16, Emu::State::Debug};

    const auto rom = emu.load_rom_file("ibm_logo.ch8");

    emu.run();
}
