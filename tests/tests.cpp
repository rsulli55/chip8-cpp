#include <boost/ut.hpp>

#include "../src/chip8.h"

namespace ut = boost::ut;

ut::suite initialization = [] {
    using namespace ut;

    Chip8 chip8;

    "check pc"_test = [&chip8] {
        expect(chip8.pc() == 0x200);
    };

    "check I register"_test = [&chip8] {
        expect(chip8.I() == 0x0);
    };

    "check general purpose registers"_test = [&chip8] {
        expect(chip8.V(0x0) == 0x0);
        expect(chip8.V(0x1) == 0x0);
        expect(chip8.V(0x2) == 0x0);
        expect(chip8.V(0x3) == 0x0);
        expect(chip8.V(0x4) == 0x0);
        expect(chip8.V(0x5) == 0x0);
        expect(chip8.V(0x6) == 0x0);
        expect(chip8.V(0x7) == 0x0);
        expect(chip8.V(0x8) == 0x0);
        expect(chip8.V(0x9) == 0x0);
        expect(chip8.V(0xa) == 0x0);
        expect(chip8.V(0xb) == 0x0);
        expect(chip8.V(0xc) == 0x0);
        expect(chip8.V(0xd) == 0x0);
        expect(chip8.V(0xe) == 0x0);
        expect(chip8.V(0xf) == 0x0);
    };

    "check stack"_test = [&chip8] {
        expect(chip8.stack().size() == 0);
    };

    "check timers"_test = [&chip8] {
        expect(chip8.sound() == 0x0);
        expect(chip8.delay() == 0x0);
    };

};

int main() {}
