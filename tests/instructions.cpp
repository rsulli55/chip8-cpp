#include <boost/ut.hpp>

#include "../src/chip8.h"
#include "../src/common.h"

boost::ut::suite instructions = [] {
    using namespace boost::ut;
    Chip8 chip8;

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

    "7XNN"_test = [&chip8] {
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
};

int main () {}
