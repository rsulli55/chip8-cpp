#pragma once

#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <ranges>
#include <deque>
#include <vector>
#include <map>

#include "key.h"
#include "spdlog/spdlog.h"

#include "common.h"
#include "rom.h"
#include "instruction.h"

class Chip8 {
  public:

    Chip8();
    Chip8(std::map<InstructionType, std::string> instruction_table);
    ~Chip8() = default;

    Chip8(const Chip8&) = default;
    Chip8(Chip8&&) = default;

    auto operator=(const Chip8&) -> Chip8& = default;
    auto operator=(Chip8&&) -> Chip8& = default;


    auto V(u8 reg) const noexcept -> u8 { return V_[reg]; }
    auto pc() const noexcept -> u16 { return pc_; }
    auto I() const noexcept -> u16 { return I_; }
    auto stack() const noexcept -> const std::deque<u16> & { return stack_; }
    auto screen() const noexcept -> const auto & { return screen_; }
    auto memory() const noexcept -> const auto & { return memory_; }
    auto sound() const noexcept -> u8 { return sound_; }
    auto delay() const noexcept -> u8 { return delay_; }
    auto bad_opcode() const noexcept -> bool { return bad_opcode_; }
    auto instruction_table(InstructionType it) const -> const char* {
        return instruction_table_.at(it).c_str();
    }
    void keydown(Key key) {
        spdlog::debug("Key Down: {}", key_to_str(key));
        keydown_[key_to_index(key)] = true;
        current_key_down_ = key;
    }
    void keyup(Key key) {
        spdlog::debug("Key Up: {}", key_to_str(key));
        keydown_[key_to_index(key)] = false;
        current_key_down_ = Key::None;
    }
    
    auto decrement_delay() noexcept -> bool; 
    auto decrement_sound() noexcept -> bool; 

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
    static constexpr u8  NUM_KEYS = 16u;
    static constexpr u16 FONT_START = 0x50u;

    // screen helpers
    auto screen_equal(const std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> &other) const noexcept -> bool; 
    auto screen_difference(
        const std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> &other)
        const noexcept; 

    // testing helpers
    void inline modify_memory(u16 location, u8 value) noexcept {
        assert(location < Chip8::MEMORY_SIZE);
        memory_[location] = value;
    }


  private:
    /// special registers
    u16 pc_ = ROM_START;
    u16 I_ = 0x0;

    // general purpose registers
    std::array<u8, 16> V_ = {0x0};

    // memory data
    std::array<u8, MEMORY_SIZE> memory_ = {0x0}; // 4K memory

    // font data
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

    // keyboard
    std::array<bool, NUM_KEYS> keydown_ = {false};
    Key current_key_down_ {Key::None};

    // stack
    // used for storing 16-bit addresses
    std::deque<u16> stack_{};

    // instruction table
    std::map<InstructionType, std::string> instruction_table_;

    // timers
    u8 sound_ = 0x0;
    u8 delay_ = 0x0;

    // internal operations
    void clear_screen() noexcept;     
    void clear_bad_opcode() noexcept;
    void initialize_font(); 

    /// instructions
    // 0 - 7
    void inline _0NNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _00E0([[maybe_unused]] u16 opcode) noexcept;
    void inline _00EE([[maybe_unused]] u16 opcode) noexcept;
    void inline _1NNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _2NNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _3XNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _4XNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _5XY0([[maybe_unused]] u16 opcode) noexcept;
    void inline _6XNN([[maybe_unused]] u16 opcode) noexcept;
    void inline _7XNN([[maybe_unused]] u16 opcode) noexcept;
    // 8
    void inline _8XY0([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XY1([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XY2([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XY3([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XY4([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XY5([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XY6([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XY7([[maybe_unused]] u16 opcode) noexcept;
    void inline _8XYE([[maybe_unused]] u16 opcode) noexcept;
    // 9
    void inline _9XY0([[maybe_unused]] u16 opcode) noexcept;
    // A - E
    void inline ANNN([[maybe_unused]] u16 opcode) noexcept;
    void inline BNNN([[maybe_unused]] u16 opcode) noexcept;
    void inline CXNN([[maybe_unused]] u16 opcode) noexcept;
    void inline DXYN([[maybe_unused]] u16 opcode) noexcept;
    void inline EX9E([[maybe_unused]] u16 opcode) noexcept;
    void inline EXA1([[maybe_unused]] u16 opcode) noexcept;
    // F
    void inline FX07([[maybe_unused]] u16 opcode) noexcept;
    void inline FX0A([[maybe_unused]] u16 opcode) noexcept;
    void inline FX15([[maybe_unused]] u16 opcode) noexcept;
    void inline FX18([[maybe_unused]] u16 opcode) noexcept;
    void inline FX1E([[maybe_unused]] u16 opcode) noexcept;
    void inline FX29([[maybe_unused]] u16 opcode) noexcept;
    void inline FX33([[maybe_unused]] u16 opcode) noexcept;
    void inline FX55([[maybe_unused]] u16 opcode) noexcept;
    void inline FX65([[maybe_unused]] u16 opcode) noexcept;

    // bad instruction flag
    bool bad_opcode_ = false;
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

    

