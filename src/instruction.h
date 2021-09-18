#pragma once

#include <string>
#include <iostream>
#include <compare>

#include "common.h"

enum class InstructionType {
    _00E0, _00EE, _0NNN,
    _1NNN,
    _2NNN,
    _3XNN,
    _4XNN,
    _5XY0,
    _6XNN,
    _7XNN,
    _8XY0, _8XY1, _8XY2, _8XY3, _8XY4, _8XY5, _8XY6, _8XY7, _8XYE,
    _9XY0,
    ANNN,
    BNNN,
    CXNN,
    DXYN,
    EX9E, EXA1,
    FX07, FX0A, FX15, FX18, FX1E, FX29, FX33, FX55, FX65,
    Unknown
};


struct Instruction {
   InstructionType type;
   u16 opcode;
};

auto operator<=>(InstructionType i1, InstructionType i2) -> std::strong_ordering;
auto operator<<(std::ostream& os, const InstructionType& i) -> std::ostream&;


constexpr auto opcode_to_instruction_type(u16 opcode) -> InstructionType {
    u16 first_nibble = nibble(nib::first, opcode);
    using enum InstructionType;
    switch (first_nibble) {
    case 0x0: {
        if (opcode == 0x00E0) return _00E0;
        else if (opcode == 0x00EE) return _00EE;
        else return _0NNN;
    }
    case 0x1:
        return _1NNN;
    case 0x2:
        return _2NNN;
    case 0x3:
        return _3XNN;
    case 0x4:
        return _4XNN;
    case 0x5: {
        if (nibble(nib::fourth, opcode) == 0x0) return _5XY0;
        else return Unknown;
    }
    case 0x6:
        return _6XNN;
    case 0x7:
        return _7XNN;
    case 0x8: {
        const auto fourth_nibble = nibble(nib::fourth, opcode);
        switch (fourth_nibble) {
        case 0x0:
            return _8XY0;
        case 0x1:
            return _8XY1;
        case 0x2:
            return _8XY2;
        case 0x3:
            return _8XY3;
        case 0x4:
            return _8XY4;
        case 0x5:
            return _8XY5;
        case 0x6:
            return _8XY6;
        case 0x7:
            return _8XY7;
        case 0xE:
            return _8XYE;
        default:
            return Unknown;
        }
    }
    case 0x9:
        return _9XY0;
    case 0xA:
        return ANNN;
    case 0xB:
        return BNNN;
    case 0xC:
        return CXNN;
    case 0xD:
        return DXYN;
    case 0xE: {
        const auto third_nibble = nibble(nib::third, opcode);
        const auto fourth_nibble = nibble(nib::fourth, opcode);
        if (third_nibble == 0x9 && fourth_nibble == 0xE) return EX9E;
        else if (third_nibble == 0XA && fourth_nibble == 0X1) return EXA1;
        else return Unknown;
    }
    case 0xF: {
        const auto third_nibble = nibble(nib::third, opcode);
        const auto fourth_nibble = nibble(nib::fourth, opcode);
        if (third_nibble == 0x0 && fourth_nibble == 0x7) return FX07;
        if (third_nibble == 0x0 && fourth_nibble == 0xA) return FX0A;
        if (third_nibble == 0x1 && fourth_nibble == 0x5) return FX15;
        if (third_nibble == 0x1 && fourth_nibble == 0x8) return FX18;
        if (third_nibble == 0x1 && fourth_nibble == 0xE) return FX1E;
        if (third_nibble == 0x2 && fourth_nibble == 0x9) return FX29;
        if (third_nibble == 0x3 && fourth_nibble == 0x3) return FX33;
        if (third_nibble == 0x5 && fourth_nibble == 0x5) return FX55;
        if (third_nibble == 0x6 && fourth_nibble == 0x5) return FX65;
        else return Unknown;
    }
    default:
        return Unknown;
    }
}

constexpr auto string_to_instruction_type(std::string_view op) -> InstructionType {
    using enum InstructionType;
    // 0
    if (op == "00E0") return _00E0;
    else if (op == "00EE") return _00EE;
    else if (op == "0NNN") return _0NNN;
    // 1-7
    else if (op == "1NNN") return _1NNN;
    else if (op == "2NNN") return _2NNN;
    else if (op == "3XNN") return _3XNN;
    else if (op == "4XNN") return _4XNN;
    else if (op == "5XY0") return _5XY0;
    else if (op == "6XNN") return _6XNN;
    else if (op == "7XNN") return _7XNN;
    // 8
    else if (op == "8XY0") return _8XY0;
    else if (op == "8XY1") return _8XY1;
    else if (op == "8XY2") return _8XY2;
    else if (op == "8XY3") return _8XY3;
    else if (op == "8XY4") return _8XY4;
    else if (op == "8XY5") return _8XY5;
    else if (op == "8XY6") return _8XY6;
    else if (op == "8XY7") return _8XY7;
    else if (op == "8XYE") return _8XYE;
    // A - D
    else if (op == "ANNN") return ANNN;
    else if (op == "BNNN") return BNNN;
    else if (op == "CXNN") return CXNN;
    else if (op == "DXYN") return DXYN;
    // E
    else if (op == "EX9E") return EX9E;
    else if (op == "EXA1") return EXA1;
    // F
    else if (op == "FX07") return FX07;
    else if (op == "FX0A") return FX0A;
    else if (op == "FX15") return FX15;
    else if (op == "FX18") return FX18;
    else if (op == "FX1E") return FX1E;
    else if (op == "FX29") return FX29;
    else if (op == "FX33") return FX33;
    else if (op == "FX55") return FX55;
    else if (op == "FX65") return FX65;
    else return Unknown;
}

constexpr auto instruction_type_to_string(InstructionType it) -> const char* {
    using enum InstructionType;
    // 0
    if (it == _00E0) return "00E0";
    else if (it == _00EE) return "00EE";
    else if (it == _0NNN) return "0NNN";
    // 1-7
    else if (it == _1NNN) return "1NNN";
    else if (it == _2NNN) return "2NNN";
    else if (it == _3XNN) return "3XNN";
    else if (it == _4XNN) return "4XNN";
    else if (it == _5XY0) return "5XY0";
    else if (it == _6XNN) return "6XNN";
    else if (it == _7XNN) return "7XNN";
    // 8
    else if (it == _8XY0) return "8XY0";
    else if (it == _8XY1) return "8XY1";
    else if (it == _8XY2) return "8XY2";
    else if (it == _8XY3) return "8XY3";
    else if (it == _8XY4) return "8XY4";
    else if (it == _8XY5) return "8XY5";
    else if (it == _8XY6) return "8XY6";
    else if (it == _8XY7) return "8XY7";
    else if (it == _8XYE) return "8XYE";
    // A - D
    else if (it == ANNN) return "ANNN";
    else if (it == BNNN) return "BNNN";
    else if (it == CXNN) return "CXNN";
    else if (it == DXYN) return "DXYN";
    // E
    else if (it == EX9E) return "EX9E";
    else if (it == EXA1) return "EXA1";
    // F
    else if (it == FX07) return "FX07";
    else if (it == FX0A) return "FX0A";
    else if (it == FX15) return "FX15";
    else if (it == FX18) return "FX18";
    else if (it == FX1E) return "FX1E";
    else if (it == FX29) return "FX29";
    else if (it == FX33) return "FX33";
    else if (it == FX55) return "FX55";
    else if (it == FX65) return "FX65";
    else return "Unknown";
}
