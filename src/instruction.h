#pragma once

#include <string>

#include "common.h"

struct Instruction {
    Instruction() :
        opcode{0}, type{}, description{} {}
    Instruction(u16 op, std::string_view ty, std::string_view desc) :
        opcode{op}, type{ty}, description{desc} {}
    const u16 opcode;
    const std::string type;
    const std::string description;
};
