#include <algorithm>
#include <boost/ut.hpp>
#include <ranges>

#include "../src/chip8.h"
#include "../src/common.h"

boost::ut::suite functionality = [] {
    using namespace boost::ut;

    Chip8 chip8;
    spdlog::set_level(spdlog::level::debug);

    "check row_col_to_screen_index"_test = [] {
        expect(eq(0u, row_col_to_screen_index(0, 0)));
        expect(eq(1u, row_col_to_screen_index(0, 1)));
        expect(eq(Chip8::SCREEN_WIDTH - 1, row_col_to_screen_index(0, Chip8::SCREEN_WIDTH - 1)));

        expect(eq(Chip8::SCREEN_WIDTH, row_col_to_screen_index(1, 0)));
        expect(eq(2 * Chip8::SCREEN_WIDTH - 1, row_col_to_screen_index(1, Chip8::SCREEN_WIDTH - 1)));
        expect(eq(2 * Chip8::SCREEN_WIDTH, row_col_to_screen_index(2, 0)));

        expect(eq((Chip8::SCREEN_HEIGHT - 1) * Chip8::SCREEN_WIDTH, row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 1, 0)));
        expect(eq(Chip8::SCREEN_HEIGHT * Chip8::SCREEN_WIDTH -1, row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 1, Chip8::SCREEN_WIDTH - 1)));
    };

    "check screen_index_to_row_col"_test = [] {
        expect(eq(0u,  screen_index_to_row_col(0u).first));
        expect(eq(0u,  screen_index_to_row_col(0u).second));
        auto val = screen_index_to_row_col(Chip8::SCREEN_WIDTH-1);
        expect(eq(0u, val.first));
        expect(eq(Chip8::SCREEN_WIDTH-1, val.second));

        val = screen_index_to_row_col(Chip8::SCREEN_WIDTH);
        expect(eq(1u,  val.first));
        expect(eq(0u,  val.second));
        val = screen_index_to_row_col(2 * Chip8::SCREEN_WIDTH -1);
        expect(eq(1u,  val.first));
        expect(eq(Chip8::SCREEN_WIDTH-1,  val.second));

        val = screen_index_to_row_col((Chip8::SCREEN_HEIGHT - 1) *Chip8::SCREEN_WIDTH);
        expect(eq(Chip8::SCREEN_HEIGHT -1,  val.first));
        expect(eq(0u,  val.second));
        val = screen_index_to_row_col(Chip8::SCREEN_HEIGHT * Chip8::SCREEN_WIDTH -1);
        expect(eq(Chip8::SCREEN_HEIGHT - 1, val.first));
        expect(eq(Chip8::SCREEN_WIDTH - 1, val.second));
    };

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
