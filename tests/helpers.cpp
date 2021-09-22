#include <boost/ut.hpp>

#include "../src/chip8.h"
#include "../src/common.h"
#include "../src/instruction.h"

boost::ut::suite helpers = [] {
    using namespace boost::ut;

    "check nibble()"_test = [] {
        expect(eq(nibble(nib::first, 0xABCD), 0xA));
        expect(eq(nibble(nib::second, 0xABCD), 0xB));
        expect(eq(nibble(nib::third, 0xABCD), 0xC));
        expect(eq(nibble(nib::fourth, 0xABCD), 0xD));

        expect(eq(nibble(nib::first, 0x1234), 0x1));
        expect(eq(nibble(nib::second, 0x1234), 0x2));
        expect(eq(nibble(nib::third, 0x1234), 0x3));
        expect(eq(nibble(nib::fourth, 0x1234), 0x4));
    };

    "check byte_to_bitmap"_test = [] {
        expect(eq(byte_to_bitmap(0b0), std::array{false, false, false, false,
                                                  false, false, false, false}));
        expect(eq(byte_to_bitmap(0b1), std::array{false, false, false, false,
                                                  false, false, false, true}));
        expect(eq(byte_to_bitmap(0b101), std::array{false, false, false, false,
                                                    false, true, false, true}));
        expect(eq(
            byte_to_bitmap(0b00010001),
            std::array{false, false, false, true, false, false, false, true}));
        expect(
            eq(byte_to_bitmap(0b10010011),
               std::array{true, false, false, true, false, false, true, true}));
        expect(eq(byte_to_bitmap(0b11111111),
                  std::array{true, true, true, true, true, true, true, true}));
        expect(
            eq(byte_to_bitmap(0xF0),
               std::array{true, true, true, true, false, false, false, false}));
        expect(
            eq(byte_to_bitmap(0x90), std::array{true, false, false, true, false,
                                                false, false, false}));
    };

    "check string_to_instruction_type"_test = [] {
        using enum InstructionType;
        // 0
        expect(eq(string_to_instruction_type("00E0"), _00E0));
        expect(eq(string_to_instruction_type("00EE"), _00EE));
        expect(eq(string_to_instruction_type("0NNN"), _0NNN));
        // 1 - 7
        expect(eq(string_to_instruction_type("1NNN"), _1NNN));
        expect(eq(string_to_instruction_type("2NNN"), _2NNN));
        expect(eq(string_to_instruction_type("3XNN"), _3XNN));
        expect(eq(string_to_instruction_type("4XNN"), _4XNN));
        expect(eq(string_to_instruction_type("5XY0"), _5XY0));
        expect(eq(string_to_instruction_type("6XNN"), _6XNN));
        expect(eq(string_to_instruction_type("7XNN"), _7XNN));
        // 8
        expect(eq(string_to_instruction_type("8XY0"), _8XY0));
        expect(eq(string_to_instruction_type("8XY1"), _8XY1));
        expect(eq(string_to_instruction_type("8XY2"), _8XY2));
        expect(eq(string_to_instruction_type("8XY3"), _8XY3));
        expect(eq(string_to_instruction_type("8XY4"), _8XY4));
        expect(eq(string_to_instruction_type("8XY5"), _8XY5));
        expect(eq(string_to_instruction_type("8XY6"), _8XY6));
        expect(eq(string_to_instruction_type("8XY7"), _8XY7));
        expect(eq(string_to_instruction_type("8XYE"), _8XYE));
        // 9 
        expect(eq(string_to_instruction_type("9XY0"), _9XY0));
        // A - D
        expect(eq(string_to_instruction_type("ANNN"), ANNN));
        expect(eq(string_to_instruction_type("BNNN"), BNNN));
        expect(eq(string_to_instruction_type("CXNN"), CXNN));
        expect(eq(string_to_instruction_type("DXYN"), DXYN));
        // E
        expect(eq(string_to_instruction_type("EX9E"), EX9E));
        expect(eq(string_to_instruction_type("EXA1"), EXA1));
        // F
        expect(eq(string_to_instruction_type("FX07"), FX07));
        expect(eq(string_to_instruction_type("FX0A"), FX0A));
        expect(eq(string_to_instruction_type("FX15"), FX15));
        expect(eq(string_to_instruction_type("FX18"), FX18));
        expect(eq(string_to_instruction_type("FX1E"), FX1E));
        expect(eq(string_to_instruction_type("FX29"), FX29));
        expect(eq(string_to_instruction_type("FX33"), FX33));
        expect(eq(string_to_instruction_type("FX55"), FX55));
        expect(eq(string_to_instruction_type("FX65"), FX65));
        // Unknown
        expect(eq(string_to_instruction_type("FX85"), Unknown));
        expect(eq(string_to_instruction_type("5AB1"), Unknown));
        expect(eq(string_to_instruction_type("EACE"), Unknown));
        expect(eq(string_to_instruction_type("8AB8"), Unknown));
    };

    "check opcode_to_instruction_type"_test = [] {
        using enum InstructionType;
        // 0
        expect(eq(opcode_to_instruction_type(0x00E0), _00E0));
        expect(eq(opcode_to_instruction_type(0x00EE), _00EE));
        expect(eq(opcode_to_instruction_type(0x0AAA), _0NNN));
        expect(eq(opcode_to_instruction_type(0x0AAA), _0NNN));
        expect(eq(opcode_to_instruction_type(0x0123), _0NNN));
        expect(eq(opcode_to_instruction_type(0x0ABE), _0NNN));
        // 1 - 7
        expect(eq(opcode_to_instruction_type(0x1AAA), _1NNN));
        expect(eq(opcode_to_instruction_type(0x1DED), _1NNN));
        expect(eq(opcode_to_instruction_type(0x1A12), _1NNN));

        expect(eq(opcode_to_instruction_type(0x2AAA), _2NNN));
        expect(eq(opcode_to_instruction_type(0x2BAC), _2NNN));
        expect(eq(opcode_to_instruction_type(0x2C1C), _2NNN));

        expect(eq(opcode_to_instruction_type(0x3FAA), _3XNN));
        expect(eq(opcode_to_instruction_type(0x3FBA), _3XNN));
        expect(eq(opcode_to_instruction_type(0x31AA), _3XNN));
        expect(eq(opcode_to_instruction_type(0x3121), _3XNN));

        expect(eq(opcode_to_instruction_type(0x4F01), _4XNN));
        expect(eq(opcode_to_instruction_type(0x4FBA), _4XNN));
        expect(eq(opcode_to_instruction_type(0x4C12), _4XNN));
        expect(eq(opcode_to_instruction_type(0x4C11), _4XNN));

        expect(eq(opcode_to_instruction_type(0x5010), _5XY0));
        expect(eq(opcode_to_instruction_type(0x5AB0), _5XY0));
        expect(eq(opcode_to_instruction_type(0x5F10), _5XY0));
        expect(eq(opcode_to_instruction_type(0x5CE0), _5XY0));

        expect(eq(opcode_to_instruction_type(0x61CD), _6XNN));
        expect(eq(opcode_to_instruction_type(0x61AB), _6XNN));
        expect(eq(opcode_to_instruction_type(0x6C01), _6XNN));
        expect(eq(opcode_to_instruction_type(0x6C56), _6XNN));

        expect(eq(opcode_to_instruction_type(0x726C), _7XNN));
        expect(eq(opcode_to_instruction_type(0x7316), _7XNN));
        expect(eq(opcode_to_instruction_type(0x7432), _7XNN));
        expect(eq(opcode_to_instruction_type(0x77AB), _7XNN));
        // 8
        expect(eq(opcode_to_instruction_type(0x8EF0), _8XY0));
        expect(eq(opcode_to_instruction_type(0x8A20), _8XY0));
        expect(eq(opcode_to_instruction_type(0x8C40), _8XY0));

        expect(eq(opcode_to_instruction_type(0x8EF1), _8XY1));
        expect(eq(opcode_to_instruction_type(0x8A21), _8XY1));
        expect(eq(opcode_to_instruction_type(0x8C41), _8XY1));

        expect(eq(opcode_to_instruction_type(0x8EF2), _8XY2));
        expect(eq(opcode_to_instruction_type(0x8A22), _8XY2));
        expect(eq(opcode_to_instruction_type(0x8C42), _8XY2));

        expect(eq(opcode_to_instruction_type(0x8EF3), _8XY3));
        expect(eq(opcode_to_instruction_type(0x8A23), _8XY3));
        expect(eq(opcode_to_instruction_type(0x8C43), _8XY3));

        expect(eq(opcode_to_instruction_type(0x8EF4), _8XY4));
        expect(eq(opcode_to_instruction_type(0x8A24), _8XY4));
        expect(eq(opcode_to_instruction_type(0x8C44), _8XY4));

        expect(eq(opcode_to_instruction_type(0x8EF5), _8XY5));
        expect(eq(opcode_to_instruction_type(0x8A25), _8XY5));
        expect(eq(opcode_to_instruction_type(0x8C45), _8XY5));

        expect(eq(opcode_to_instruction_type(0x8DB6), _8XY6));
        expect(eq(opcode_to_instruction_type(0x8276), _8XY6));
        expect(eq(opcode_to_instruction_type(0x8DB6), _8XY6));

        expect(eq(opcode_to_instruction_type(0x8277), _8XY7));
        expect(eq(opcode_to_instruction_type(0x8DB7), _8XY7));
        expect(eq(opcode_to_instruction_type(0x8A47), _8XY7));

        expect(eq(opcode_to_instruction_type(0x8DBE), _8XYE));
        expect(eq(opcode_to_instruction_type(0x827E), _8XYE));
        expect(eq(opcode_to_instruction_type(0x8A4E), _8XYE));
        // 9
        expect(eq(opcode_to_instruction_type(0x9EF0), _9XY0));
        expect(eq(opcode_to_instruction_type(0x9A20), _9XY0));
        expect(eq(opcode_to_instruction_type(0x9C40), _9XY0));
        // A - D
        expect(eq(opcode_to_instruction_type(0xAAAA), ANNN));
        expect(eq(opcode_to_instruction_type(0xABDB), ANNN));
        expect(eq(opcode_to_instruction_type(0xA123), ANNN));

        expect(eq(opcode_to_instruction_type(0xBAAA), BNNN));
        expect(eq(opcode_to_instruction_type(0xB678), BNNN));
        expect(eq(opcode_to_instruction_type(0xBFED), BNNN));

        expect(eq(opcode_to_instruction_type(0xC1AA), CXNN));
        expect(eq(opcode_to_instruction_type(0xC2ED), CXNN));
        expect(eq(opcode_to_instruction_type(0xCCB2), CXNN));
        expect(eq(opcode_to_instruction_type(0xCDA1), CXNN));

        expect(eq(opcode_to_instruction_type(0xD12A), DXYN));
        expect(eq(opcode_to_instruction_type(0xDABA), DXYN));
        expect(eq(opcode_to_instruction_type(0xDC3A), DXYN));
        expect(eq(opcode_to_instruction_type(0xDF4A), DXYN));
        // E
        expect(eq(opcode_to_instruction_type(0xEA9E), EX9E));
        expect(eq(opcode_to_instruction_type(0xE49E), EX9E));
        expect(eq(opcode_to_instruction_type(0xEF9E), EX9E));

        expect(eq(opcode_to_instruction_type(0xE0A1), EXA1));
        expect(eq(opcode_to_instruction_type(0xE8A1), EXA1));
        expect(eq(opcode_to_instruction_type(0xE9A1), EXA1));
        // F
        expect(eq(opcode_to_instruction_type(0xFA07), FX07));
        expect(eq(opcode_to_instruction_type(0xF707), FX07));
        expect(eq(opcode_to_instruction_type(0xF807), FX07));

        expect(eq(opcode_to_instruction_type(0xFA0A), FX0A));
        expect(eq(opcode_to_instruction_type(0xF70A), FX0A));
        expect(eq(opcode_to_instruction_type(0xF80A), FX0A));

        expect(eq(opcode_to_instruction_type(0xFA15), FX15));
        expect(eq(opcode_to_instruction_type(0xF715), FX15));
        expect(eq(opcode_to_instruction_type(0xF815), FX15));

        expect(eq(opcode_to_instruction_type(0xFA18), FX18));
        expect(eq(opcode_to_instruction_type(0xF718), FX18));
        expect(eq(opcode_to_instruction_type(0xF818), FX18));

        expect(eq(opcode_to_instruction_type(0xFA1E), FX1E));
        expect(eq(opcode_to_instruction_type(0xF71E), FX1E));
        expect(eq(opcode_to_instruction_type(0xF81E), FX1E));

        expect(eq(opcode_to_instruction_type(0xF829), FX29));
        expect(eq(opcode_to_instruction_type(0xF529), FX29));
        expect(eq(opcode_to_instruction_type(0xFD29), FX29));

        expect(eq(opcode_to_instruction_type(0xF833), FX33));
        expect(eq(opcode_to_instruction_type(0xF533), FX33));
        expect(eq(opcode_to_instruction_type(0xFD33), FX33));

        expect(eq(opcode_to_instruction_type(0xF855), FX55));
        expect(eq(opcode_to_instruction_type(0xF555), FX55));
        expect(eq(opcode_to_instruction_type(0xFD55), FX55));

        expect(eq(opcode_to_instruction_type(0xF865), FX65));
        expect(eq(opcode_to_instruction_type(0xF565), FX65));
        expect(eq(opcode_to_instruction_type(0xFD65), FX65));
        // Unknown
        expect(eq(opcode_to_instruction_type(0xFA85), Unknown));
        expect(eq(opcode_to_instruction_type(0xFA16), Unknown));
        expect(eq(opcode_to_instruction_type(0x5AB1), Unknown));
        expect(eq(opcode_to_instruction_type(0xEACE), Unknown));
        expect(eq(opcode_to_instruction_type(0x8AB8), Unknown));
        expect(eq(opcode_to_instruction_type(0x8129), Unknown));
        expect(eq(opcode_to_instruction_type(0x9129), Unknown));
        expect(eq(opcode_to_instruction_type(0xFA27), Unknown));
    };
};

int main() {}
