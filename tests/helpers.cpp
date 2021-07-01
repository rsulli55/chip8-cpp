#include <boost/ut.hpp>

#include "../src/chip8.h"
#include "../src/common.h"

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
};

int main() {}
