#include <algorithm>
#include <boost/ut.hpp>
#include <ranges>

#include "../src/chip8.h"
#include "../src/common.h"

boost::ut::suite functionality = [] {
    using namespace boost::ut;

    Chip8 chip8;
    chip8.set_debug_level(spdlog::level::debug);

    "check load_rom"_test = [&chip8] {
        std::vector<u8> data{0, 1, 2, 3, 4};
        chip8.load_rom(data);
        const auto &memory = chip8.memory();

        auto counter = std::views::iota(0u, data.size());
        for (const auto i : counter) {
            expect(eq(memory[i + chip8.ROM_START], data[i]));
        }

        data.resize(10);
        std::ranges::fill(data, 0xAB);
        chip8.load_rom(data);

        counter = std::views::iota(0u, data.size());
        for (const auto i : counter) {
            expect(eq(memory[i + chip8.ROM_START], data[i]));
        }
    };

    "check fetch"_test = [&chip8] {
        std::vector<u8> data{0x00, 0x0F, 0xA0, 0x00, 0x10,
                             0xDE, 0xBA, 0x1A, 0xAA, 0xAA};
        chip8.load_rom(data);
        auto pc = chip8.ROM_START;

        expect(eq(chip8.pc(), pc));
        auto opcode = chip8.fetch();
        expect(eq(opcode, 0x0000F));
        pc += 2;

        expect(eq(chip8.pc(), pc));
        opcode = chip8.fetch();
        expect(eq(opcode, 0xA000));
        pc += 2;

        expect(eq(chip8.pc(), pc));
        opcode = chip8.fetch();
        expect(eq(opcode, 0x10DE));
        pc += 2;

        expect(eq(chip8.pc(), pc));
        opcode = chip8.fetch();
        expect(eq(opcode, 0xBA1A));
        pc += 2;

        expect(eq(chip8.pc(), pc));
        opcode = chip8.fetch();
        expect(eq(opcode, 0xAAAA));
        pc += 2;
    };
};

// TODO: Add some Emu tests

int main() {}
