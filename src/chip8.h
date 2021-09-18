#pragma once

#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <ranges>
#include <stack>
#include <vector>
#include <map>

#include "spdlog/spdlog.h"

#include "common.h"
#include "rom.h"
#include "instruction.h"

class Chip8 {
  public:

    Chip8();
    ~Chip8() = default;

    Chip8(const Chip8&) = default;
    Chip8(Chip8&&) = default;

    auto operator=(const Chip8&) -> Chip8& = default;
    auto operator=(Chip8&&) -> Chip8& = default;


    auto V(u8 reg) const noexcept -> u8 { return V_[reg]; }
    auto pc() const noexcept -> u16 { return pc_; }
    auto I() const noexcept -> u16 { return I_; }
    auto stack() const noexcept -> const std::stack<u16> & { return stack_; }
    auto screen() const noexcept -> const auto & { return screen_; }
    auto memory() const noexcept -> const auto & { return memory_; }
    auto sound() const noexcept -> u8 { return sound_; }
    auto delay() const noexcept -> u8 { return delay_; }
    auto bad_opcode() const noexcept -> bool { return bad_opcode_; }
    auto instruction_table(InstructionType it) const noexcept -> const char* {
        return instruction_table_.at(it).c_str();
    }

    void load_rom(const Rom& rom);

    auto fetch_with_pc(u16 pc) const noexcept -> u16;


    auto fetch() noexcept -> u16;     
    void execute(u16 opcode) noexcept;
    void cycle() noexcept; 

    // constants
    static constexpr u32 SCREEN_WIDTH = 64u;
    static constexpr u32 SCREEN_HEIGHT = 32u;
    static constexpr u16 ROM_START = 0x200u;
    static constexpr u16 MEMORY_SIZE = 4096u;

    // screen helpers
    auto screen_equal(const std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> &other) const noexcept -> bool; 
    auto screen_difference(
        const std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> &other)
        const noexcept; 

  private:
    /// special registers
    u16 pc_ = ROM_START;
    u16 I_ = 0x0;

    // general purpose registers
    std::array<u8, 16> V_ = {0x0};

    // memory data
    std::array<u8, MEMORY_SIZE> memory_ = {0x0}; // 4K memory

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
    std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> screen_ = {false};

    // stack
    // used for storing 16-bit addresses
    std::stack<u16> stack_;

    // timers
    u8 sound_ = 0x0;
    u8 delay_ = 0x0;

    // internal operations
    void clear_screen() noexcept;     
    void clear_bad_opcode() noexcept;
    void initialize_font(); 

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

    // instruction table
    static constexpr std::string_view instruction_table_path_ =
        "instruction_table.csv";
    std::map<InstructionType, std::string> instruction_table_;
};

constexpr u32 row_col_to_screen_index(u32 row, u32 col) {
    return row * Chip8::SCREEN_WIDTH + col;
}

constexpr std::pair<u32, u32> screen_index_to_row_col(u32 index) {
    const auto row = index / Chip8::SCREEN_WIDTH;
    const auto col = index - row * Chip8::SCREEN_WIDTH;
    return {row, col};
}

auto read_instruction_table(std::string_view path)
    -> std::map<InstructionType, std::string>;

    

