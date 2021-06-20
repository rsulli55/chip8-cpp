#include <boost/ut.hpp>

#include "../src/chip8.h"
#include "../src/common.h"


boost::ut::suite initialization = [] {
    using namespace boost::ut;

    Chip8 chip8;

    "check pc"_test = [&chip8] {
        expect(eq(chip8.pc(), 0x200));
    };

    "check I register"_test = [&chip8] {
        expect(eq(chip8.I(), 0x0));
    };

    "check general purpose registers"_test = [&chip8] {
        expect(eq(chip8.V(0x0), 0x0));
        expect(eq(chip8.V(0x1),  0x0));
        expect(eq(chip8.V(0x2), 0x0));
        expect(eq(chip8.V(0x3), 0x0));
        expect(eq(chip8.V(0x4), 0x0));
        expect(eq(chip8.V(0x5), 0x0));
        expect(eq(chip8.V(0x6), 0x0));
        expect(eq(chip8.V(0x7), 0x0));
        expect(eq(chip8.V(0x8), 0x0));
        expect(eq(chip8.V(0x9), 0x0));
        expect(eq(chip8.V(0xa), 0x0));
        expect(eq(chip8.V(0xb), 0x0));
        expect(eq(chip8.V(0xc), 0x0));
        expect(eq(chip8.V(0xd), 0x0));
        expect(eq(chip8.V(0xe), 0x0));
        expect(eq(chip8.V(0xf), 0x0));
    };

    "check stack"_test = [&chip8] {
        expect(chip8.stack().size() == 0);
    };

    "check timers"_test = [&chip8] {
        expect(eq(chip8.sound(), 0x0));
        expect(eq(chip8.delay(), 0x0));
    };

};

boost::ut::suite helpers = [] {
    using namespace boost::ut;

    "check nibble calculations"_test = [] {
        expect(eq(nibble(nib::first, 0xABCD), 0xA));
        expect(eq(nibble(nib::second, 0xABCD), 0xB));
        expect(eq(nibble(nib::third, 0xABCD), 0xC));
        expect(eq(nibble(nib::fourth, 0xABCD), 0xD));

        expect(eq(nibble(nib::first, 0x1234), 0x1));
        expect(eq(nibble(nib::second, 0x1234), 0x2));
        expect(eq(nibble(nib::third, 0x1234), 0x3));
        expect(eq(nibble(nib::fourth, 0x1234), 0x4));
    };
};

boost::ut::suite instructions = [] {
    using namespace boost::ut;
    Chip8 chip8;

    "store instruction"_test = [&chip8] {
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
};
int main() {}
