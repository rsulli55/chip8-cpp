#pragma once

#include <string_view>

#include "spdlog/spdlog.h"

#include "common.h"

enum class Key : u8 {
    One, Two, Three, C,
    Four, Five, Six, D,
    Seven, Eight, Nine, E,
    A, Zero, B, F
};

constexpr auto key_to_index(Key key) -> u8 {
    return static_cast<u8>(key);
}

constexpr auto index_to_key(u8 index) -> Key {
    if (index >= 16) {
        spdlog::error("index_to_key(): invalid index");
        std::terminate();
    }

    return static_cast<Key>(index);
}

constexpr auto key_to_str(Key key) -> std::string_view {
    switch (key) {
        case Key::One:
            return "1";
        case Key::Two:
            return "2";
        case Key::Three:
            return "3";
        case Key::C:
            return "C";
        case Key::Four:
            return "4";
        case Key::Five:
            return "5";
        case Key::Six:
            return "6";
        case Key::D:
            return "D";
        case Key::Seven:
            return "7";
        case Key::Eight:
            return "8";
        case Key::Nine:
            return "9";
        case Key::E:
            return "E";
        case Key::A:
            return "A";
        case Key::Zero:
            return "0";
        case Key::B:
            return "B";
        case Key::F:
            return "F";
        default:
            return "Invalid";
    }
}


