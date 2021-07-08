#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <ranges>
#include <stack>
#include <vector>

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

    void execute(u16 opcode) noexcept;

    bool screen_equal(
        const std::array<std::array<bool, 32>, 64> &other) const noexcept {
        return std::ranges::equal(screen_, other);
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

    /// instructions
    void inline _0NNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _00E0([[maybe_unused]] u16 opcode) noexcept;
    void inline _00EE([[maybe_unused]] u16 opcode) noexcept;
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

    // memory
    std::array<u8, 4096> memory_ = {0x0}; // 4K memory

    // font location
    static constexpr u8 FONT_START = 0x50;

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
