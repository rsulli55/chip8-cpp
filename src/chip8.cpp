#include "chip8.h"
#include "common.h"
#include "spdlog/spdlog.h"

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
        break;
    case 0x2:
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
        break;
    case 0xB:
        break;
    case 0xC:
        break;
    case 0xD:
        break;
    case 0xE:
        break;
    case 0xF:
        break;
    }

    // TODO: increment pc
}

// instructions
void inline Chip8::_0NNN([[maybe_unused]] u16 opcode) noexcept {
    bad_opcode_ = true;
}

void inline Chip8::_00E0([[maybe_unused]] u16 opcode) noexcept {
    clear_screen();
}

void inline Chip8::_00EE([[maybe_unused]] u16 opcode) noexcept {
    bad_opcode_ = true;
}

void inline Chip8::_6XNN(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] = opcode & 0x00FF;
}

void inline Chip8::_7XNN(u16 opcode) noexcept {
    auto second_nibble = nibble(nib::second, opcode);
    /* spdlog::set_level(spdlog::level::debug); */
    V_[second_nibble] += opcode & 0x00FF;
    /* spdlog::debug("After add current val = {}", V_[second_nibble]); */
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
