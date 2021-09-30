#include <boost/ut.hpp>

#include "../src/chip8.h"
#include "../src/common.h"

// helper function to copy a byte size bitmap to a screen sized array
void copy_bitmap(std::array<bool, 8> bitmap, u32 row_start, u32 col_start,
        std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>& screen) {
    auto screen_ind = row_col_to_screen_index(row_start, col_start);
    std::copy(std::cbegin(bitmap), std::cend(bitmap), 
            std::begin(screen) + screen_ind);
}


boost::ut::suite instructions = [] {
    using namespace boost::ut;
    Chip8 chip8;
    spdlog::set_level(spdlog::level::debug);

    // Execute machine instruction at 0xNNN, which is unimplemented
    "0NNN"_test = [&chip8] {
        chip8.execute(0x0000);
        expect(chip8.bad_opcode());
    };

    // Clear the screen
    "00E0"_test = [&chip8] {
        chip8.execute(0x00E0);
        expect(chip8.screen_equal(
            std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>{false}));
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

    // Skip the following instruction if the value of register VX equals NN
    for (u8 reg = 0x0; reg < 0xF; ++reg) {
        for (u8 value =0x0005; value < 0x00AC; value += 0x0009) {
            test("3XNN; X = " + std::to_string(reg) +
                 ", NN = " + std::to_string(value)) = [&chip8, reg, value] {
                u16 base_store = 0x6000;
                u16 base_check = 0x3000;
                u16 op = base_store + (reg << 8) + value;
                chip8.execute(op);
                auto pc = chip8.pc();
                // do not increment pc if VX does not equal NN
                op = base_check + (reg << 8) + value + 1;
                chip8.execute(op);
                expect(eq(chip8.pc(), pc));
                // increment pc if VX does equal NN
                op = base_check + (reg << 8) + value;
                chip8.execute(op);
                expect(eq(chip8.pc(), pc+2));
            };
        }
    }

    // Skip the following instruction if the value of register VX does not equal NN
    for (u8 reg = 0x0; reg < 0xF; ++reg) {
        for (u8 value =0x0005; value < 0x00AC; value += 0x0009) {
            test("4XNN; X = " + std::to_string(reg) +
                 ", NN = " + std::to_string(value)) = [&chip8, reg, value] {
                u16 base_store = 0x6000;
                u16 base_check = 0x4000;
                u16 op = base_store + (reg << 8) + value;
                chip8.execute(op);
                auto pc = chip8.pc();
                // increment pc if VX does not equal NN
                op = base_check + (reg << 8) + value + 1;
                chip8.execute(op);
                expect(eq(chip8.pc(), pc+2));
                pc += 2;
                // do not increment pc if VX does equal NN
                op = base_check + (reg << 8) + value;
                chip8.execute(op);
                expect(eq(chip8.pc(), pc));
            };
        }
    }

    for (u8 X = 0x0; X < 0xF; ++X) {
        for (u8 Y = X; Y < 0xF; ++Y) {
            test("5XY0; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                u16 base_store = 0x6000;
                u16 base_check = 0x5000;
                for (auto value : {3, 5, 7, 8}) {
                    u16 op = base_store + (X << 8) + value;
                    chip8.execute(op);
                    op = base_store + (Y << 8) + value;
                    chip8.execute(op);
                    // VX and VY are equal, so skip next instruction
                    auto pc = chip8.pc();
                    op = base_check + (X << 8) + (Y << 4);
                    chip8.execute(op);
                    expect(eq(chip8.pc(), pc + 2));
                }

                // don't check that are unequal if they are the same register
                if (X == Y) return;

                for (auto value : {3, 5, 7, 8}) {
                    u16 op = base_store + (X << 8) + value + 1;
                    chip8.execute(op);
                    op = base_store + (Y << 8) + value;
                    chip8.execute(op);
                    // VX and VY are not equal equal, so don't skip next instruction
                    auto pc = chip8.pc();
                    op = base_check + (X << 8) + (Y << 4);
                    chip8.execute(op);
                    expect(eq(chip8.pc(), pc));
                }
            };
        }
    }


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

    // 8XY4 adds the value of register VY to register VX
    // Sets VF to 1 if a carry occurs and 0 otherwise
    // we do not want to include VF in the testing
    for (u8 X = 0; X < 0xF; ++X) {
        for (u8 Y = X + 1; Y < 0xF; ++Y) {
            test("8XY4; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x8004 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0x00));
                expect(eq(chip8.V(0xF), 0x01));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x8004 + (X << 8) + (Y << 4));
                expect(eq(chip8.V(X), 0xFF));
                expect(eq(chip8.V(0xF), 0x00));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x8004 + (X << 8) + (Y << 4));
                // 0xBA + 0x4C = 0x106
                expect(eq(chip8.V(X), 0x06));
                expect(eq(chip8.V(0xF), 0x01));

                // test 8XX4
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x8004 + (X << 8) + (X << 4));
                expect(eq(chip8.V(X), 0x22));
                expect(eq(chip8.V(0xF), 0x00));
            };
        }
    }

    // 8XY5 subtract the value of register VY from register VX
    // Sets VF to 0 if a borrow occurs and 1 if a borrow does occur
    // we do not want to include VF in the testing
    for (u8 X = 0; X < 0xF; ++X) {
        for (u8 Y = X + 1; Y < 0xF; ++Y) {
            test("8XY5; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x8005 + (X << 8) + (Y << 4));
                // 0xFF - 0x01 = 0xFE
                expect(eq(chip8.V(X), 0xFE));
                expect(eq(chip8.V(0xF), 0x01));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x8005 + (X << 8) + (Y << 4));
                // 0x0F - 0xF0 = 0x10F - 0xF0 (mod 0x100) = 0x1F (mod 0x100)
                expect(eq(chip8.V(X), 0x1F));
                expect(eq(chip8.V(0xF), 0x00));

                chip8.execute(0x6013 + (Y << 8));
                expect(eq(chip8.V(Y), 0x13));
                chip8.execute(0x60A2 + (X << 8));
                expect(eq(chip8.V(X), 0xA2));
                chip8.execute(0x8005 + (X << 8) + (Y << 4));
                // 0xA2 - 0x13 = 0x8F
                expect(eq(chip8.V(X), 0x8F));
                expect(eq(chip8.V(0xF), 0x01));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x8005 + (X << 8) + (Y << 4));
                // 0x4C - 0xBA = 0x14C - 0xBA (mod 0x100) = 0x92 (mod 0x100) 
                expect(eq(chip8.V(X), 0x92));
                expect(eq(chip8.V(0xF), 0x00));

                // test 8XX5
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x8005 + (X << 8) + (X << 4));
                expect(eq(chip8.V(X), 0x00));
                expect(eq(chip8.V(0xF), 0x01));
            };
        }
    }

    // 8XY6 store the value of register VY shifted right one bit in register VX
    // Set register VF to the least significant bit prior to the shift
    // we do not want to include VF in the testing
    for (u8 X = 0; X < 0xF; ++X) {
        for (u8 Y = X + 1; Y < 0xF; ++Y) {
            test("8XY6; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x8006 + (X << 8) + (Y << 4));
                // 0xFF - 0x01 = 0xFE
                expect(eq(chip8.V(X), 0x00));
                expect(eq(chip8.V(0xF), 0x01));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x8006 + (X << 8) + (Y << 4));
                // 0xF0 >> 1 = 0x7F
                expect(eq(chip8.V(X), 0x78));
                expect(eq(chip8.V(0xF), 0x00));

                chip8.execute(0x6013 + (Y << 8));
                expect(eq(chip8.V(Y), 0x13));
                chip8.execute(0x60A2 + (X << 8));
                expect(eq(chip8.V(X), 0xA2));
                chip8.execute(0x8006 + (X << 8) + (Y << 4));
                // 0x13 >> 1 = 0x09
                expect(eq(chip8.V(X), 0x09));
                expect(eq(chip8.V(0xF), 0x01));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x8006 + (X << 8) + (Y << 4));
                // 0xBA >> 1 = 0x5D
                expect(eq(chip8.V(X), 0x5D));
                expect(eq(chip8.V(0xF), 0x00));

                // test 8XX6
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x8006 + (X << 8) + (X << 4));
                // 0x11 >> 1 = 0x0F
                expect(eq(chip8.V(X), 0x08));
                expect(eq(chip8.V(0xF), 0x01));
            };
        }
    }

    // 8XY7 set the register VX to the value of VY minus VX
    // Set register VF to 0 if borrow occurs, otherwise 1
    // we do not want to include VF in the testing
    for (u8 X = 0; X < 0xF; ++X) {
        for (u8 Y = X + 1; Y < 0xF; ++Y) {
            test("8XY7; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x8007 + (X << 8) + (Y << 4));
                // 0x01 - 0xFF = 0x101 - 0xFF (mod 0x100) = 0x2 (mod 0x100)
                expect(eq(chip8.V(X), 0x02));
                expect(eq(chip8.V(0xF), 0x00));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x8007 + (X << 8) + (Y << 4));
                // 0xF0 - 0x0F = 0xE1
                expect(eq(chip8.V(X), 0xE1));
                expect(eq(chip8.V(0xF), 0x01));

                chip8.execute(0x6013 + (Y << 8));
                expect(eq(chip8.V(Y), 0x13));
                chip8.execute(0x60A2 + (X << 8));
                expect(eq(chip8.V(X), 0xA2));
                chip8.execute(0x8007 + (X << 8) + (Y << 4));
                // 0x13 - 0xA2 = 0x113 - 0xA2 (mod 0x100) = 0x71 (mod 0x100)
                expect(eq(chip8.V(X), 0x71));
                expect(eq(chip8.V(0xF), 0x00));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x8007 + (X << 8) + (Y << 4));
                // 0xBA - 0x4C = 0x6E
                expect(eq(chip8.V(X), 0x6E));
                expect(eq(chip8.V(0xF), 0x01));

                // test 8XX7
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x8007 + (X << 8) + (X << 4));
                // 0x11 - 0x11 = 0
                expect(eq(chip8.V(X), 0x00));
                expect(eq(chip8.V(0xF), 0x01));
            };
        }
    }


    // 8XYE stores the value of register VY shifted left one bit in register VX
    // Set register VF to the most significant bit prior to shift
    // we do not want to include VF in the testing
    for (u8 X = 0; X < 0xF; ++X) {
        for (u8 Y = X + 1; Y < 0xF; ++Y) {
            test("8XYE; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                chip8.execute(0x6001 + (Y << 8));
                expect(eq(chip8.V(Y), 0x01));
                chip8.execute(0x60FF + (X << 8));
                expect(eq(chip8.V(X), 0xFF));
                chip8.execute(0x800E + (X << 8) + (Y << 4));
                // 0x01 << 1 = 0x02
                expect(eq(chip8.V(X), 0x02));
                expect(eq(chip8.V(0xF), 0x00));

                chip8.execute(0x60F0 + (Y << 8));
                expect(eq(chip8.V(Y), 0xF0));
                chip8.execute(0x600F + (X << 8));
                expect(eq(chip8.V(X), 0x0F));
                chip8.execute(0x800E + (X << 8) + (Y << 4));
                // 0xF0 << 1 = 0xE0
                expect(eq(chip8.V(X), 0xE0));
                expect(eq(chip8.V(0xF), 0x01));

                chip8.execute(0x6013 + (Y << 8));
                expect(eq(chip8.V(Y), 0x13));
                chip8.execute(0x60A2 + (X << 8));
                expect(eq(chip8.V(X), 0xA2));
                chip8.execute(0x800E + (X << 8) + (Y << 4));
                // 0x13 << 1 = 0x26
                expect(eq(chip8.V(X), 0x26));
                expect(eq(chip8.V(0xF), 0x00));

                chip8.execute(0x60BA + (Y << 8));
                expect(eq(chip8.V(Y), 0xBA));
                chip8.execute(0x604C + (X << 8));
                expect(eq(chip8.V(X), 0x4C));
                chip8.execute(0x800E + (X << 8) + (Y << 4));
                // 0xBA << 1 = 0x74
                expect(eq(chip8.V(X), 0x74));
                expect(eq(chip8.V(0xF), 0x01));

                // test 8XXE
                chip8.execute(0x6011 + (X << 8));
                expect(eq(chip8.V(X), 0x11));
                chip8.execute(0x800E + (X << 8) + (X << 4));
                // 0x11 << 1 = 
                expect(eq(chip8.V(X), 0x22));
                expect(eq(chip8.V(0xF), 0x00));
            };
        }
    }


    // 9XY0
    // Skip the following instruction if the value of register VX is not equal to the value of register VY
    for (u8 X = 0x0; X < 0xF; ++X) {
        for (u8 Y = X; Y < 0xF; ++Y) {
            test("9XY0; X = " + std::to_string(X) +
                 ", Y = " + std::to_string(Y)) = [&chip8, X, Y] {
                u16 base_store = 0x6000;
                u16 base_check = 0x9000;
                for (auto value : {3, 5, 7, 8}) {
                    u16 op = base_store + (X << 8) + value;
                    chip8.execute(op);
                    op = base_store + (Y << 8) + value;
                    chip8.execute(op);
                    // VX and VY are equal, so don't skip next instruction
                    auto pc = chip8.pc();
                    op = base_check + (X << 8) + (Y << 4);
                    chip8.execute(op);
                    expect(eq(chip8.pc(), pc));
                }

                // don't check if they are unequal if they are the same register
                if (X == Y) return;

                for (auto value : {3, 5, 7, 8}) {
                    u16 op = base_store + (X << 8) + value + 1;
                    chip8.execute(op);
                    op = base_store + (Y << 8) + value;
                    chip8.execute(op);
                    // VX and VY are not equal, so skip next instruction
                    auto pc = chip8.pc();
                    op = base_check + (X << 8) + (Y << 4);
                    chip8.execute(op);
                    expect(eq(chip8.pc(), pc + 2));
                }
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
        // clear screen
        chip8.execute(0x00E0);
        // display character 0 at 0,0
        chip8.execute(0x6000);
        chip8.execute(0xD005);
        std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT> expected { false};
        std::array<u8, 5> font_zero{0xF0, 0x90, 0x90, 0x90, 0xF0};
        auto modify = [&expected](u8 byte, int row) {
            spdlog::debug("In modify: byte is {:x}", byte);
            auto bitmap = byte_to_bitmap(byte);
            copy_bitmap(bitmap, row, 0, expected);
        };
        const auto rows = std::views::iota(0ul, font_zero.size());
        for (const auto row : rows) {
            modify(font_zero[row], row);
        }
        expect(chip8.screen_equal(expected));
        expect(eq(chip8.V(0xF), 0x0));

        // calling it again should clear the screen (XOR) and set VF
        chip8.execute(0xA050);
        chip8.execute(0x6000);
        chip8.execute(0xD005);
        // reset expected
        std::fill(std::begin(expected), std::end(expected), false);
        expect(chip8.screen_equal(expected));
        expect(eq(chip8.V(0xF), 0x1));

        // check wrapping of x and y
        chip8.execute(0xA050);
        // 0x48 = 72 = 8 (mod 64)
        chip8.execute(0x6048);
        // 0x24 = 36 = 4 (mod 32)
        chip8.execute(0x6124);
        // reset screen
        chip8.execute(0x00E0);
        // letter is shifted 8 pixels right and 4 pixels down
        chip8.execute(0xD015);
        // reset expected
        std::fill(std::begin(expected), std::end(expected), false);

        auto modify2 = [&expected](u8 byte, int row, int col_start) {
            spdlog::debug("In modify2: byte is {:x}, row = {}, col_start = {}",
                          byte, row, col_start);
            auto bitmap = byte_to_bitmap(byte);
            copy_bitmap(bitmap, row, col_start, expected);
        };
        for (const auto row : rows) {
            modify2(font_zero[row], row + 4, 8);
        }
        expect(chip8.screen_equal(expected));
        expect(eq(chip8.V(0xF), 0x0));

        // check when a sprite gets cut off
        chip8.execute(0xA050);
        // 0x3B = 59 = -5 (mod 64)
        chip8.execute(0x603B);
        // 0x1D = 29 = -3 (mod 32)
        chip8.execute(0x611D);
        // reset screen
        chip8.execute(0x00E0);
        // letter is shifted 8 pixels right and 4 pixels down
        chip8.execute(0xD015);

        // reset expected
        std::fill(std::begin(expected), std::end(expected), false);
        // just modify expected by hand
        // Sprite 0 looks like the following
        // F0: 1 1 1 1 0 0 0 0
        // 90: 1 0 0 1 0 0 0 0
        // 90: 1 0 0 1 0 0 0 0
        // 90: 1 0 0 1 0 0 0 0
        // F0: 1 1 1 1 0 0 0 0
        // If we draw it at x = 59, y = 29 we should expect to see
        // F0: 1 1 1 1 0 |
        // 90: 1 0 0 1 0 |
        // 90: 1 0 0 1 0 |
        // ---------------
        // F0
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 3, Chip8::SCREEN_WIDTH - 2)] = true;
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 3, Chip8::SCREEN_WIDTH - 3)] = true;
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 3, Chip8::SCREEN_WIDTH - 4)] = true;
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 3, Chip8::SCREEN_WIDTH - 5)] = true;
        // 90
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 2, Chip8::SCREEN_WIDTH - 2)] = true;
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 2, Chip8::SCREEN_WIDTH - 5)] = true;
        // 90
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 1, Chip8::SCREEN_WIDTH - 2)] = true;
        expected[row_col_to_screen_index(Chip8::SCREEN_HEIGHT - 1, Chip8::SCREEN_WIDTH - 5)] = true;
        expect(chip8.screen_equal(expected));
        expect(eq(chip8.V(0xF), 0x0));


        // used to display screen differences if a check fails
        /* const auto differences = chip8.screen_difference(expected); */
        /* const auto screen = chip8.screen(); */
        /* for (auto row = 0; row < 32; ++row) { */
        /*     for (auto col = 0; col < 64; ++col) { */
        /*         if (differences[row][col]) { */
        /*             spdlog::debug("Difference at row {} and col {}", row, col); */
        /*             spdlog::debug("screen[{}][{}] = {}", row, col, */
        /*                           screen[row][col]); */
        /*             spdlog::debug("expected[{}][{}] = {}", row, col, */
        /*                           expected[row][col]); */
        /*         } */
        /*     } */
        /* } */
    };
};


int main() {}
