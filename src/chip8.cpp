#include "chip8.h"
#include "common.h"
#include <algorithm>

void Chip8::execute(u16 opcode) noexcept {
    // clear bad_opcode if it was previously set
    clear_bad_opcode();

    u16 first_nibble = nibble(nib::first, opcode);
    switch (first_nibble) {
    case 0x0: {
        // 00E0 instruction
        if (opcode == 0x00E0) {
            _00E0(opcode);
        }
        // 00EE instruction
        else if (opcode == 0x00EE) {
            _00EE(opcode);
        }
        // 0NNN instruction
        else {
            _0NNN(opcode);
        }
        break;
    }
    case 0x1:
        _1NNN(opcode);
        break;
    case 0x2:
        _2NNN(opcode);
        break;
    case 0x3:
        break;
    case 0x4:
        break;
    case 0x5:
        break;
    case 0x6:
        _6XNN(opcode);
        break;
    case 0x7:
        _7XNN(opcode);
        break;
    case 0x8: {
        auto fourth_nibble = nibble(nib::fourth, opcode);
        switch (fourth_nibble) {
        case 0x0:
            _8XY0(opcode);
            break;
        case 0x1:
            _8XY1(opcode);
            break;
        case 0x2:
            _8XY2(opcode);
            break;
        case 0x3:
            _8XY3(opcode);
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            break;
        case 0x7:
            break;
        case 0xE:
            break;
        }
        break;
    }
    case 0x9:
        break;
    case 0xA:
        _ANNN(opcode);
        break;
    case 0xB:
        break;
    case 0xC:
        break;
    case 0xD:
        _DXYN(opcode);
        break;
    case 0xE:
        break;
    case 0xF:
        break;
    }

    // TODO: increment pc
}

// instructions
// Execute machine language instruction, UNIMPLEMENTED
void inline Chip8::_0NNN([[maybe_unused]] u16 opcode) noexcept {
    bad_opcode_ = true;
}

// Clear the screen
void inline Chip8::_00E0([[maybe_unused]] u16 opcode) noexcept {
    clear_screen();
}

// Return from subroutine popping from stack and setting PC
void inline Chip8::_00EE([[maybe_unused]] u16 opcode) noexcept {
    assert(stack_.size() > 0);
    pc_ = stack_.top();
    stack_.pop();
}

// Jump to address 0xNNN
void inline Chip8::_1NNN(u16 opcode) noexcept {
    u16 address = 0x0FFF & opcode;
    pc_ = address;
}

// Execute subroutine at address 0xNNN pushing current PC onto stack
void inline Chip8::_2NNN(u16 opcode) noexcept {
    stack_.push(pc_);
    u16 address = 0x0FFF & opcode;
    pc_ = address;
}

// Store 0xNN into register VX
void inline Chip8::_6XNN(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] = opcode & 0x00FF;
}

// Add value 0xNN to register VX
void inline Chip8::_7XNN(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] += opcode & 0x00FF;
}

void inline Chip8::_8XY0(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] = V_[third_nibble];
}

void inline Chip8::_8XY1(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] |= V_[third_nibble];
}

void inline Chip8::_8XY2(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] &= V_[third_nibble];
}

void inline Chip8::_8XY3(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] ^= V_[third_nibble];
}

void inline Chip8::_ANNN(u16 opcode) noexcept { I_ = opcode & 0x0FFF; }

// Draw sprite at position VX, VY with 0xN bytes of sprite data
// starting at the address stored in I
// Set VF to 0x01 if any set pixels are changed to unset, and 00 otherwise
void inline Chip8::_DXYN(u16 opcode) noexcept { 
    // get x and y coords from VX, VY modulo screen size
    auto x_start = V_[nibble(nib::second, opcode)] % SCREEN_WIDTH;
    auto x_end = std::min(static_cast<u16> (x_start + 8u), SCREEN_WIDTH);
    auto y_start = V_[nibble(nib::third, opcode)] % SCREEN_HEIGHT;
    auto pixel_height = nibble(nib::fourth, opcode);
    auto y_end = std::min(static_cast<u16> (y_start + pixel_height), SCREEN_HEIGHT);

    spdlog::debug("In DXYN");
    auto fill_row = [this, x_start, x_end](std::array<bool, SCREEN_WIDTH>& row) {
        auto byte = memory_[I_];
        spdlog::debug("In DXYN: loaded byte from memory");
        auto bitmap = byte_to_bitmap(byte);
        // XOR bitmap with row and set VF if a row bit is unset
        // TODO: More elegant solution for this?
        // std::copy(std::cbegin(bitmap), std::cbegin(bitmap) + x_end, std::begin(row) + x_start);
        V_[0xF] = 0x0;
        for (auto i = 0; i < x_end; ++i) {
            // row bit becomes unset if both bits are true
            if (bitmap[i] && row[x_start + i]) {
                V_[0xF] = 0x1;
            }

            row[x_start +i] ^= bitmap[i];
        }
    };

    std::for_each(std::begin(screen_) + y_start, std::end(screen_) + y_end, fill_row);
    spdlog::debug("Exiting DXYN: loaded byte from memory");
}

    

