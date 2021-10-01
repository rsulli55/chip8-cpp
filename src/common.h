#pragma once

#include <array>
#include <cstdint>
#include <ranges>
#include <random>

#include "spdlog/spdlog.h"

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

const char* const glsl_version = "#version 440";

enum class nib { first, second, third, fourth };

/// returns the @param which nibble of @param what
constexpr auto nibble(nib which, u16 what) -> u16 {
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

/// converts a byte into an length 8 array of bools
constexpr auto byte_to_bitmap(u8 byte) -> std::array<bool, 8> {
    std::array<bool, 8> to_return = {false};

    auto transformed = std::views::iota(0u, 8u) |
                       std::views::transform([&byte](u8 bit) {
                           return (byte & (1 << bit)) != 0;
                       }) |
                       std::views::reverse;

    // copy it for returning
    std::copy(std::cbegin(transformed), std::cend(transformed),
              std::begin(to_return));
    return to_return;
}

auto random_byte() -> u8; 
