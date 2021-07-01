#pragma once

#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

enum class nib { first, second, third, fourth };

/// nibble returns the @param which nibble of @param what
inline u16 nibble(nib which, u16 what) {
    switch (which) {
    case nib::first:
        return (0xF000 & what) >> (4 * 3); // shift down by 3 half-bytes
    case nib::second:
        return (0x0F00 & what) >> (4 * 2); // shift down by 2 half-bytes
    case nib::third:
        return (0x00F0 & what) >> (4 * 1); // shift down by 1 half-bytes
    case nib::fourth:
        return 0x000F & what;
    }
    return 0x0;
}
