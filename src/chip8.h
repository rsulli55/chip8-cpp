#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <ranges>
#include <stack>
#include <vector>

#include "spdlog/spdlog.h"

#include "common.h"

class Chip8 {

  public:
    u8 V(u8 reg) const noexcept { return V_[reg]; }
    u16 pc() const noexcept { return pc_; }
    u16 I() const noexcept { return I_; }
    const std::stack<u16> &stack() const noexcept { return stack_; }
    u8 sound() const noexcept { return sound_; }
    u8 delay() const noexcept { return delay_; }
    bool bad_opcode() const noexcept { return bad_opcode_; }

    void load_rom(std::array<u8, 3584> rom) noexcept {
        std::copy(std::cbegin(rom), std::cend(rom), std::begin(memory_) + ROM_START);
    }

    void execute(u16 opcode) noexcept;

    bool screen_equal(
        const std::array<std::array<bool, 32>, 64> &other) const noexcept {
        return std::ranges::equal(screen_, other);
    }

    void set_debug_level(spdlog::level::level_enum level) {
        spdlog::set_level(level);
    }

    // TODO: figure out a nice way to keep track of where the screen is
    // incorrect
    /* auto screen_difference(const std::array<std::array<bool, 32>, 64>& other)
     * const noexcept { */
    /*     auto to_return = std::vector<decltype(std::views::iota())>{}; */
    /*     auto check_equal = [this, &other](size_t i) { */
    /*         return screen_[i] != other[i]; */
    /*     }; */
    /*     std::ranges::for_each(other, [](std::array<bool, 32>& row) { */
    /*             return std::views::iota(0u, row.size()) |
     * std::views::filter(check_equal); */
    /*             } */
    /* } */

  private:
    // internal operations
    void clear_screen() noexcept {
        std::ranges::fill(screen_, std::array<bool, 32>{false});
    }
    void clear_bad_opcode() noexcept { bad_opcode_ = false; }
    void initialize_font() noexcept {
        std::copy(std::cbegin(FONT), std::cend(FONT), std::begin(memory_) + FONT_START);
    }


    /// instructions
    void inline _0NNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _00E0([[maybe_unused]] u16 opcode) noexcept;
    void inline _00EE([[maybe_unused]] u16 opcode) noexcept;
    void inline _1NNN(u16 opcode) noexcept;
    void inline _2NNN(u16 opcode) noexcept;
    void inline _6XNN(u16 opcode) noexcept;
    void inline _7XNN(u16 opcode) noexcept;
    void inline _8XY0(u16 opcode) noexcept;
    void inline _8XY1(u16 opcode) noexcept;
    void inline _8XY2(u16 opcode) noexcept;
    void inline _8XY3(u16 opcode) noexcept;

    /// special registers
    u16 pc_ = 0x200;
    u16 I_ = 0x0;

    // general purpose registers
    std::array<u8, 16> V_ = {0x0};

    // memory data
    static constexpr u8 ROM_START = 0x200;
    std::array<u8, 4096> memory_ = {0x0}; // 4K memory

    // font data
    static constexpr u8 FONT_START = 0x50;
    static constexpr std::array<u8, 80> FONT = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // screen
    std::array<std::array<bool, 32>, 64> screen_ = {
        std::array<bool, 32>{false}};

    // stack
    // used for storing 16-bit addresses
    std::stack<u16> stack_;

    // timers
    u8 sound_ = 0x0;
    u8 delay_ = 0x0;

    // bad instruction flag
    bool bad_opcode_ = false;
};
