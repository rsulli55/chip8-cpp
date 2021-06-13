#include <boost/ut.hpp>

#include "../src/chip8.h"

namespace ut = boost::ut;

ut::suite initialization = [] {
    using namespace ut;

    Chip8 chip8;

    "check pc"_test = [&chip8] {
        expect(chip8.pc() == 0x200);
    };

    "check registers"_test = [&chip8] {
        expect(chip8.V0() == 0x0);
        expect(chip8.V1() == 0x0);
        expect(chip8.V2() == 0x0);
        expect(chip8.V3() == 0x0);
        expect(chip8.V4() == 0x0);
        expect(chip8.V5() == 0x0);
        expect(chip8.V6() == 0x0);
        expect(chip8.V7() == 0x0);
        expect(chip8.V8() == 0x0);
        expect(chip8.V9() == 0x0);
        expect(chip8.VA() == 0x0);
        expect(chip8.VB() == 0x0);
        expect(chip8.VC() == 0x0);
        expect(chip8.VD() == 0x0);
        expect(chip8.VE() == 0x0);
        expect(chip8.VF() == 0x0);
    };
};

int main() {}
