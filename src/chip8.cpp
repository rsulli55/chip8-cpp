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
        const auto fourth_nibble = nibble(nib::fourth, opcode);
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
    const auto second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] = opcode & 0x00FF;
}

// Add value 0xNN to register VX
void inline Chip8::_7XNN(u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] += opcode & 0x00FF;
}

void inline Chip8::_8XY0(u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] = V_[third_nibble];
}

void inline Chip8::_8XY1(u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] |= V_[third_nibble];
}

void inline Chip8::_8XY2(u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] &= V_[third_nibble];
}

void inline Chip8::_8XY3(u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] ^= V_[third_nibble];
}

void inline Chip8::_ANNN(u16 opcode) noexcept { I_ = opcode & 0x0FFF; }

// Draw sprite at position VX, VY with 0xN bytes of sprite data
// starting at the address stored in I
// Set VF to 0x01 if any set pixels are changed to unset, and 00 otherwise
void inline Chip8::_DXYN(u16 opcode) noexcept { 
    // get x and y coords from VX, VY modulo screen size
    const auto x_start = V_[nibble(nib::second, opcode)] % SCREEN_WIDTH;
    const auto x_end = std::min(static_cast<u16> (x_start + 8u), SCREEN_WIDTH);
    const auto y_start = V_[nibble(nib::third, opcode)] % SCREEN_HEIGHT;
    const auto pixel_height = nibble(nib::fourth, opcode);
    const auto y_end = std::min(static_cast<u16> (y_start + pixel_height), SCREEN_HEIGHT);

    spdlog::debug("In DXYN x_start = {}, x_end = {}, y_start = {}, y_end = {}", 
            x_start, x_end, y_start, y_end);
    // TODO: More elegant solution for this?
    // std::copy(std::cbegin(bitmap), std::cbegin(bitmap) + x_end, std::begin(row) + x_start);
    /* std::for_each(std::begin(screen_) + y_start, std::begin(screen_) + y_end, fill_row); */
    auto transform_row = [this, x_start, x_end, y_start](std::array<bool, SCREEN_WIDTH>& pixel_row, u16 row) {
        const auto byte = memory_[I_ + row - y_start];
        spdlog::debug("In DXYN: loaded byte {:X} from memory", byte);
        spdlog::debug("In DXYN: row = {}", row);
        const auto bitmap = byte_to_bitmap(byte);
        // XOR bitmap with pixel_row and set VF if a pixel_row bit is unset
        for (auto col = x_start; col < x_end; ++col) {
            // pixel_row bit becomes unset if both bits are true
            if (bitmap[col-x_start] && pixel_row[col]) {
                V_[0xF] = 0x1;
            }



            pixel_row[col] = (pixel_row[col] != bitmap[col - x_start]);
            spdlog::debug("In DXYN: setting pixel_row[{}] to  {}", col, pixel_row[col]);
        }
    };

    // set VF to 0 before drawing sprite
    V_[0xF] = 0x0;
    const auto rows = std::views::iota(static_cast<u16>(y_start), static_cast<u16>(y_end));
    for (const auto row : rows) {
        transform_row(screen_[row], row);
    }

    spdlog::debug("Exiting DXYN");
}

    

