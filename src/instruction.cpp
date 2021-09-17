#include "instruction.h"

auto operator==(InstructionType i1, InstructionType i2) -> bool {
    return static_cast<int>(i1) == static_cast<int>(i2);
}

auto operator<<(std::ostream& os, const InstructionType& i) -> std::ostream& {
    return os << static_cast<int>(i);
}
