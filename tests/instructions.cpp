#include <boost/ut.hpp>

#include "../src/chip8.h"
#include "../src/common.h"

boost::ut::suite instructions = [] {
    using namespace boost::ut;
    Chip8 chip8;
    chip8.set_debug_level(spdlog::level::debug);

    // Execute machine instruction at 0xNNN, which is unimplemented
    "0NNN"_test = [&chip8] {
        chip8.execute(0x0000);
        expect(chip8.bad_opcode());
    };

    // Clear the screen
    "00E0"_test = [&chip8] {
        chip8.execute(0x00E0);
        expect(chip8.screen_equal(
            std::array<std::array<bool, 64>, 32>{std::array<bool, 64>{false}}));
    };

    // Return from subroutine popping the PC from the stack
    "00EE"_test = [&chip8] {
        chip8.execute(0x1BAD);
        chip8.execute(0x2AA0);
        expect(eq(chip8.pc(), 0x0AA0));
        chip8.execute(0x00EE);
        expect(eq(chip8.pc(), 0x0BAD));

        chip8.execute(0x1CCC);
        chip8.execute(0x2024);
        expect(eq(chip8.pc(), 0x0024));
        chip8.execute(0x00EE);
        expect(eq(chip8.pc(), 0x0CCC));
    };

    // Jump to address 0xNNN
    "1NNN"_test = [&chip8] {
        chip8.execute(0x1AAA);
        expect(eq(chip8.pc(), 0x0AAA));

        chip8.execute(0x1123);
        expect(eq(chip8.pc(), 0x0123));

        chip8.execute(0x1050);
        expect(eq(chip8.pc(), 0x0050));

        chip8.execute(0x14CE);
        expect(eq(chip8.pc(), 0x04CE));
    };

    // Execute subroutine at 0xNNN, and push current PC onto stack
    "2NNN"_test = [&chip8] {
        chip8.execute(0x1AAA);
        chip8.execute(0x2001);
        expect(eq(chip8.pc(), 0x0001));
        chip8.execute(0x00EE);
        expect(eq(chip8.pc(), 0x0AAA));

        chip8.execute(0x1123);
        chip8.execute(0x2ABC);
        expect(eq(chip8.pc(), 0x0ABC));
        chip8.execute(0x00EE);
        expect(eq(chip8.pc(), 0x0123));

        chip8.execute(0x1D1E);
        chip8.execute(0x2C4D);
        expect(eq(chip8.pc(), 0x0C4D));
        chip8.execute(0x00EE);
        expect(eq(chip8.pc(), 0x0D1E));
    };


    // Store number 0xNN in register VX
    "6XNN"_test = [&chip8] {
        chip8.execute(0x6015);
        expect(eq(chip8.V(0x0), 0x15));

        chip8.execute(0x6120);
        expect(eq(chip8.V(0x1), 0x20));

        chip8.execute(0x6225);
        expect(eq(chip8.V(0x2), 0x25));

        chip8.execute(0x6335);
        expect(eq(chip8.V(0x3), 0x35));

        chip8.execute(0x6445);
        expect(eq(chip8.V(0x4), 0x45));

        chip8.execute(0x6555);
        expect(eq(chip8.V(0x5), 0x55));

        chip8.execute(0x6665);
        expect(eq(chip8.V(0x6), 0x65));

        chip8.execute(0x6775);
        expect(eq(chip8.V(0x7), 0x75));

        chip8.execute(0x6880);
        expect(eq(chip8.V(0x8), 0x80));

        chip8.execute(0x6905);
        expect(eq(chip8.V(0x9), 0x05));

        chip8.execute(0x6A03);
        expect(eq(chip8.V(0xA), 0x03));

        chip8.execute(0x6B13);
        expect(eq(chip8.V(0xB), 0x13));

        chip8.execute(0x6C23);
        expect(eq(chip8.V(0xC), 0x23));

        chip8.execute(0x6D33);
        expect(eq(chip8.V(0xD), 0x33));

        chip8.execute(0x6E43);
        expect(eq(chip8.V(0xE), 0x43));

        chip8.execute(0x6F53);
        expect(eq(chip8.V(0xF), 0x53));
    };

    "7XNN"_test = [&chip8] {
        chip8.execute(0x6000);
        expect(eq(chip8.V(0x0), 0x00));
        chip8.execute(0x7001);
        expect(eq(chip8.V(0x0), 0x01));

        chip8.execute(0x6100);
        chip8.execute(0x7102);
        expect(eq(chip8.V(0x1), 0x02));

        chip8.execute(0x6200);
        chip8.execute(0x7203);
        expect(eq(chip8.V(0x2), 0x03));

        chip8.execute(0x6300);
        chip8.execute(0x7304);
        expect(eq(chip8.V(0x3), 0x04));

        chip8.execute(0x6400);
        chip8.execute(0x7414);
        expect(eq(chip8.V(0x4), 0x14));

        chip8.execute(0x6500);
        chip8.execute(0x7515);
        expect(eq(chip8.V(0x5), 0x15));

        chip8.execute(0x6600);
        chip8.execute(0x7616);
        expect(eq(chip8.V(0x6), 0x16));

        chip8.execute(0x6700);
        chip8.execute(0x7717);
        expect(eq(chip8.V(0x7), 0x17));

        chip8.execute(0x6800);
        chip8.execute(0x7818);
        expect(eq(chip8.V(0x8), 0x18));

        chip8.execute(0x6900);
        chip8.execute(0x7919);
        expect(eq(chip8.V(0x9), 0x19));

        chip8.execute(0x6A05);
        chip8.execute(0x7A05);
        expect(eq(chip8.V(0xA), 0xA));

        chip8.execute(0x6B0A);
        chip8.execute(0x7B05);
        expect(eq(chip8.V(0xB), 0xF));

        chip8.execute(0x6C01);
        chip8.execute(0x7CAA);
        expect(eq(chip8.V(0xC), 0xAB));

        chip8.execute(0x6D02);
        chip8.execute(0x7D09);
        expect(eq(chip8.V(0xD), 0xB));

        chip8.execute(0x6E03);
        chip8.execute(0x7ECB);
        expect(eq(chip8.V(0xE), 0xCE));

        chip8.execute(0x6F0A);
        chip8.execute(0x7F00);
        expect(eq(chip8.V(0xF), 0xA));
    };

    "8XY0"_test = [&chip8] {
        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8000);
        expect(eq(chip8.V(0x0), 0xA));

        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8100);
        expect(eq(chip8.V(0x1), 0xA));

        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8200);
        expect(eq(chip8.V(0x2), 0xA));

        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8300);
        expect(eq(chip8.V(0x3), 0xA));

        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8400);
        expect(eq(chip8.V(0x4), 0xA));

        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8500);
        expect(eq(chip8.V(0x5), 0xA));

        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8600);
        expect(eq(chip8.V(0x6), 0xA));

        chip8.execute(0x600A);
        expect(eq(chip8.V(0x0), 0xA));
        chip8.execute(0x8700);
        expect(eq(chip8.V(0x7), 0xA));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8850);
        expect(eq(chip8.V(0x8), 0x10));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8950);
        expect(eq(chip8.V(0x9), 0x10));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8A50);
        expect(eq(chip8.V(0xA), 0x10));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8B50);
        expect(eq(chip8.V(0xB), 0x10));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8C50);
        expect(eq(chip8.V(0xC), 0x10));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8D50);
        expect(eq(chip8.V(0xD), 0x10));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8E50);
        expect(eq(chip8.V(0xE), 0x10));

        chip8.execute(0x6510);
        expect(eq(chip8.V(0x5), 0x10));
        chip8.execute(0x8F50);
        expect(eq(chip8.V(0xF), 0x10));
    };

    // 8XY1 sets VX to VX | VY
    for (u8 X = 0; X < 16; ++X) {
        for (u8 Y = X + 1; Y < 16; ++Y) {
            test("8XY1; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x8001 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0xFF));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x8001 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0xFF));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x8001 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0xFE));

                // test 8XX1
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x8002 + (X << 8) + (X << 4));
                expect(eq(chip8.V(X), 0x11));
            };
        }
    }

    // 8XY2 sets VX to VX & VY
    for (u8 X = 0; X < 16; ++X) {
        for (u8 Y = X + 1; Y < 16; ++Y) {
            test("8XY2; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x8002 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0x01));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x8002 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0x0));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x8002 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0x08));

                // test 8XX2
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x8002 + (X << 8) + (X << 4));
                expect(eq(chip8.V(X), 0x11));
            };
        }
    }

    // 8XY3 sets VX to VX & VY
    for (u8 X = 0; X < 16; ++X) {
        for (u8 Y = X + 1; Y < 16; ++Y) {
            test("8XY3; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x8003 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0xFE));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x8003 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0xFF));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x8003 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0xF6));

                // test 8XX3
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x8003 + (X << 8) + (X << 4));
                expect(eq(chip8.V(X), 0x00));
            };
        }
    }

    // Store memory address 0xNNN in register I
    "ANNN"_test = [&chip8] {
        chip8.execute(0xA0AB);
        expect(eq(chip8.I(), 0x00AB));

        chip8.execute(0xA111);
        expect(eq(chip8.I(), 0x0111));

        chip8.execute(0xADED);
        expect(eq(chip8.I(), 0x0DED));

        chip8.execute(0xA456);
        expect(eq(chip8.I(), 0x0456));

        chip8.execute(0xAC4E);
        expect(eq(chip8.I(), 0x0C4E));
    };

    // Draw sprite at position VX, VY with 0xN bytes of sprite data
    // starting at the address stored in I
    // Set VF to 0x01 if any set pixels are changed to unset, and 00 otherwise
    "DXYN"_test = [&chip8] {
        // start of font
        chip8.execute(0xA050);
        // display character 0 at 0,0
        chip8.execute(0xD005);
        std::array<std::array<bool, 64>, 32> expected = {std::array<bool, 64> {false}};
        std::array<u8, 5> font_zero { 0xF0, 0x90, 0x90, 0x90, 0xF0 };
        auto modify = [&expected](u8 byte) {
            auto bitmap = byte_to_bitmap(byte);
            std::copy(std::cbegin(bitmap), std::cend(bitmap), std::begin(expected[0]));
        };
        std::for_each(std::cbegin(font_zero), std::cend(font_zero), modify);
        spdlog::debug("DXYN_test: completed for each");
        expect(chip8.screen_equal(expected));
        spdlog::debug("After expect");
    };
};

int main() {}
