#pragma once

#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <ranges>
#include <stack>
#include <vector>

#include "spdlog/spdlog.h"

#include "common.h"

class Chip8 {
  private:
    /// special registers
    u16 pc_ = 0x200;
    u16 I_ = 0x0;

    // general purpose registers
    std::array<u8, 16> V_ = {0x0};

    // memory data
    static constexpr u16 ROM_START = 0x200u;
    std::array<u8, 4096> memory_ = {0x0}; // 4K memory

    // font data
    static constexpr u16 FONT_START = 0x50u;
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
    static constexpr u16 SCREEN_WIDTH = 64;
    static constexpr u16 SCREEN_HEIGHT = 32;
    std::array<std::array<bool, SCREEN_WIDTH>, SCREEN_HEIGHT> screen_ = {
        std::array<bool, SCREEN_WIDTH>{false}};

    // stack
    // used for storing 16-bit addresses
    std::stack<u16> stack_;

    // timers
    u8 sound_ = 0x0;
    u8 delay_ = 0x0;

  public:
    Chip8() { initialize_font(); }

    u8 V(u8 reg) const noexcept { return V_[reg]; }
    u16 pc() const noexcept { return pc_; }
    u16 I() const noexcept { return I_; }
    const std::stack<u16> &stack() const noexcept { return stack_; }
    const auto &screen() const noexcept { return screen_; }
    u8 sound() const noexcept { return sound_; }
    u8 delay() const noexcept { return delay_; }
    bool bad_opcode() const noexcept { return bad_opcode_; }

    void load_rom(std::array<u8, 3584> rom) noexcept {
        std::copy(std::cbegin(rom), std::cend(rom),
                  std::begin(memory_) + ROM_START);
    }

    void execute(u16 opcode) noexcept;

    bool screen_equal(const std::array<std::array<bool, SCREEN_WIDTH>,
                                       SCREEN_HEIGHT> &other) const noexcept {
        spdlog::debug("In screen_equal");
        auto intermediate = std::views::iota(0u, SCREEN_HEIGHT) |
                            std::views::transform([this, &other](u16 i) {
                                return std::ranges::equal(screen_[i], other[i]);
                            });

        spdlog::debug("In screen_equal: finished intermediate");
        return std::ranges::all_of(std::cbegin(intermediate),
                                   std::cend(intermediate),
                                   [](bool b) { return b; });
    }

    void set_debug_level(spdlog::level::level_enum level) {
        spdlog::set_level(level);
    }

    auto screen_difference(
        const std::array<std::array<bool, SCREEN_WIDTH>, SCREEN_HEIGHT> &other)
        const noexcept {
        auto differences =
            std::array<std::array<bool, SCREEN_WIDTH>, SCREEN_HEIGHT>{};
        auto compute_xor = [](const std::array<bool, SCREEN_WIDTH> &row1,
                              const std::array<bool, SCREEN_WIDTH> &row2) {
            auto xored = std::array<bool, SCREEN_WIDTH>{};
            std::ranges::transform(row1, row2, std::begin(xored),
                                   [](bool b1, bool b2) { return b1 != b2; });

            return xored;
        };

        std::ranges::transform(screen_, other, std::begin(differences),
                               compute_xor);

        return differences;
    }

  private:
    // internal operations
    void clear_screen() noexcept {
        std::ranges::fill(screen_, std::array<bool, SCREEN_WIDTH>{false});
    }
    void clear_bad_opcode() noexcept { bad_opcode_ = false; }
    void initialize_font() noexcept {
        std::copy(std::cbegin(FONT), std::cend(FONT),
                  std::begin(memory_) + FONT_START);
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

    void inline _ANNN(u16 opcode) noexcept;
    void inline _DXYN(u16 opcode) noexcept;

    // bad instruction flag
    bool bad_opcode_ = false;
};
