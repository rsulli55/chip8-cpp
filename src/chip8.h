#pragma once

#include <array>
#include <cstdint>
#include <stack>

#include "common.h"

class Chip8 {

  public:
    u8 V(u8 reg) const noexcept { return V_[reg]; }
    u16 pc() const noexcept { return pc_; }
    u16 I() const noexcept { return I_; }
    const std::stack<u16> &stack() const noexcept { return stack_; }
    u8 sound() const noexcept { return sound_; }
    u8 delay() const noexcept { return delay_; }

    void execute(u16 opcode) noexcept;

  private:
    /// instructions
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
    std::array<std::array<bool, 32>, 64> screen_;

    // stack
    // used for storing 16-bit addresses
    std::stack<u16> stack_;

    // timers
    u8 sound_ = 0x0;
    u8 delay_ = 0x0;
};
