#include <algorithm>
#include <fstream>
#include <filesystem>

#include "chip8.h"
#include "common.h"
#include "instruction.h"

Chip8::Chip8(std::map<InstructionType, std::string> instruction_table) :
    instruction_table_{std::move(instruction_table)} {
    spdlog::debug("Chip8 Constructor called\n"); 
    initialize_font();
}

Chip8::Chip8() : instruction_table_{} { 
    spdlog::debug("Chip8 Constructor called\n"); 
    initialize_font();
}

auto Chip8::fetch_with_pc(u16 pc) const noexcept -> u16 {
    // opcodes stored in big endian
    const auto unknown_instruction = 0x8AA8;
    if (pc > MEMORY_SIZE) return unknown_instruction;
    u8 upper_byte = memory_[pc++];
    u8 lower_byte = memory_[pc++];
    u16 opcode = (upper_byte << 8) + lower_byte;

    return opcode;
}

auto Chip8::fetch() noexcept -> u16 {
    // opcodes stored in big endian
    u8 upper_byte = memory_[pc_++];
    u8 lower_byte = memory_[pc_++];
    u16 opcode = (upper_byte << 8) + lower_byte;

    return opcode;
}

void Chip8::cycle() noexcept {
    auto opcode = fetch();
    execute(opcode);
}

auto Chip8::decrement_delay() noexcept -> bool {
    if (delay_ == 0) return false;

    --delay_;
    return true;
}

auto Chip8::decrement_sound() noexcept -> bool {
    if (sound_ == 0) return false;

    --sound_;
    return true;
}

void Chip8::load_rom(const Rom &rom) {
    const auto &data = rom.data_;
    const auto end =
        std::min(std::cend(data), std::cbegin(data) + MEMORY_SIZE - ROM_START);
    std::copy(std::cbegin(data), end, std::begin(memory_) + ROM_START);
}

void Chip8::execute(u16 opcode) noexcept {
    // clear bad_opcode if it was previously set
    clear_bad_opcode();

    auto type = opcode_to_instruction_type(opcode);

    switch (type) {
    case InstructionType::_00E0: 
        _00E0(opcode);
        break;
    case InstructionType::_00EE:
        _00EE(opcode);
        break;
    case InstructionType::_0NNN:
        _0NNN(opcode);
        break;
    case InstructionType::_1NNN:
        _1NNN(opcode);
        break;
    case InstructionType::_2NNN:
        _2NNN(opcode);
        break;
    case InstructionType::_3XNN: 
        _3XNN(opcode);
        break;
    case InstructionType::_4XNN:
        _4XNN(opcode);
        break;
    case InstructionType::_5XY0:
        _5XY0(opcode);
        break;
    case InstructionType::_6XNN:
        _6XNN(opcode);
        break;
    case InstructionType::_7XNN:
        _7XNN(opcode);
        break;
    case InstructionType::_8XY0:
        _8XY0(opcode);
        break;
    case InstructionType::_8XY1:
        _8XY1(opcode);
        break;
    case InstructionType::_8XY2:
        _8XY2(opcode);
        break;
    case InstructionType::_8XY3:
        _8XY3(opcode);
        break;
    case InstructionType::_8XY4:
        _8XY4(opcode);
        break;
    case InstructionType::_8XY5:
        _8XY5(opcode);
        break;
    case InstructionType::_8XY6:
        _8XY6(opcode);
        break;
    case InstructionType::_8XY7:
        _8XY7(opcode);
        break;
    case InstructionType::_8XYE:
        _8XYE(opcode);
        break;
    case InstructionType::_9XY0:
        _9XY0(opcode);
        break;
    case InstructionType::ANNN:
        ANNN(opcode);
        break;
    case InstructionType::BNNN:
        BNNN(opcode);
        break;
    case InstructionType::CXNN: 
        CXNN(opcode);
        break;
    case InstructionType::DXYN:
        DXYN(opcode);
        break;
    case InstructionType::EX9E:
        EX9E(opcode);
        break;
    case InstructionType::EXA1:
        EXA1(opcode);
        break;
    case InstructionType::FX07:
        FX07(opcode);
        break;
    case InstructionType::FX0A:
        FX0A(opcode);
        break;
    case InstructionType::FX15:
        FX15(opcode);
        break;
    case InstructionType::FX18:
        FX18(opcode);
        break;
    case InstructionType::FX1E:
        FX1E(opcode);
        break;
    case InstructionType::FX29:
        FX29(opcode);
        break;
    case InstructionType::FX33:
        FX33(opcode);
        break;
    case InstructionType::FX55:
        FX55(opcode);
        break;
    case InstructionType::FX65:
        FX65(opcode);
        break;
    case InstructionType::Unknown:
        bad_opcode_ = true;
        break;
    default:
        spdlog::error("Unexpected InstructionType in Chip8::execute()\n");
        break;
    }

}

auto Chip8::screen_equal(
    const std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> &other) const noexcept
    -> bool {
    return std::ranges::equal(screen_, other);
}

auto Chip8::screen_difference(
    const std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> &other)
    const noexcept {
    auto differences = std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT>{false};
    auto compute_xor = [](const bool b1, const bool b2) { return b1 != b2; };
    std::ranges::transform(screen_, other, std::begin(differences),
                           compute_xor);

    return differences;
}

// internal operations
void Chip8::clear_screen() noexcept { std::ranges::fill(screen_, false); }
void Chip8::clear_bad_opcode() noexcept { bad_opcode_ = false; }
void Chip8::initialize_font() {
    std::copy(std::cbegin(FONT), std::cend(FONT),
              std::begin(memory_) + FONT_START);
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
    pc_ = stack_.front();
    stack_.pop_front();
}

// Jump to address 0xNNN
void inline Chip8::_1NNN([[maybe_unused]] u16 opcode) noexcept {
    u16 address = 0x0FFF & opcode;
    pc_ = address;
}

// Execute subroutine at address 0xNNN pushing current PC onto stack
void inline Chip8::_2NNN([[maybe_unused]] u16 opcode) noexcept {
    stack_.push_front(pc_);
    u16 address = 0x0FFF & opcode;
    pc_ = address;
}

// Skip the following instruction if the value of register VX equals NN
void inline Chip8::_3XNN([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const u8 value = (nibble(nib::third, opcode) << 4) + nibble(nib::fourth, opcode);
    if (V_[second_nibble] == value) pc_ += 2;
}

// Skip the following instruction if the value of register VX is not equal to NN
void inline Chip8::_4XNN([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const u8 value = (nibble(nib::third, opcode) << 4) + nibble(nib::fourth, opcode);
    if (V_[second_nibble] != value) pc_ += 2;
}

// Skip the following instruction if the value of register VX is equal to the value of register VY
void inline Chip8::_5XY0([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);

    if (V_[second_nibble] == V_[third_nibble]) pc_ += 2;
}

// Store 0xNN into register VX
void inline Chip8::_6XNN([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] = opcode & 0x00FF;
}

// Add value 0xNN to register VX
void inline Chip8::_7XNN([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] += opcode & 0x00FF;
}

void inline Chip8::_8XY0([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] = V_[third_nibble];
}

void inline Chip8::_8XY1([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] |= V_[third_nibble];
}

void inline Chip8::_8XY2([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] &= V_[third_nibble];
}

void inline Chip8::_8XY3([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[second_nibble] ^= V_[third_nibble];
}

// Add the value of register VY to register VX
// Set VF to 01 if a carry occurs
// Set VF to 00 if a carry does not occur
void inline Chip8::_8XY4([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[0xF] = 0;
    u16 sum = V_[second_nibble] + V_[third_nibble];
    V_[second_nibble] = sum & (0xFF);
    if ((sum & 0xFF00) != 0) V_[0xF] = 1; 
}

// Subtract the value of register VY from register VX
// Set VF to 00 if a borrow occurs
// Set VF to 01 if a borrow does not occur
void inline Chip8::_8XY5([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    // if VY > VX then VX - VY = 0x100 + VX - VY (mod 0x100)
    if (V_[third_nibble] > V_[second_nibble]) {
        V_[0xF] = 0;
        V_[second_nibble] = (V_[second_nibble] + (1 << 8)) - V_[third_nibble];
    }
    else {
        V_[0xF] = 1;
        V_[second_nibble] -= V_[third_nibble];
    }
}
// Store the value of register VY shifted right one bit in register VX
// Set register VF to the least significant bit prior to the shift
// VY is unchanged
void inline Chip8::_8XY6([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[0xF] = V_[third_nibble] & 0x01;
    V_[second_nibble] = V_[third_nibble] >> 1;
}

// Set register VX to the value of VY minus VX
// Set VF to 00 if a borrow occurs
// Set VF to 01 if a borrow does not occur
void inline Chip8::_8XY7([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    // if VX > VY then VY - VX = 0x100 + VX - VY (mod 0x100)
    if (V_[second_nibble] > V_[third_nibble]) {
        V_[0xF] = 0;
        V_[second_nibble] = (V_[third_nibble] + (1 << 8)) - V_[second_nibble];
    }
    else {
        V_[0xF] = 1;
        V_[second_nibble] = V_[third_nibble] - V_[second_nibble];
    }
}

// Store the value of register VY shifted left one bit in register VX
// Set register VF to the most significant bit prior to the shift
// VY is unchanged
void inline Chip8::_8XYE([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);
    V_[0xF] = (V_[third_nibble] & 0x80) >> 7;
    V_[second_nibble] = (V_[third_nibble] << 1) & 0xFF;
}

// 9
// Skip the following instruction if the value of register VX is not equal to the value of register VY
void inline Chip8::_9XY0([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    const auto third_nibble = nibble(nib::third, opcode);

    if (V_[second_nibble] != V_[third_nibble]) pc_ += 2;
}

// A - E
void inline Chip8::ANNN(u16 opcode) noexcept {
    I_ = opcode & 0x0FFF;
}

void inline Chip8::BNNN([[maybe_unused]] u16 opcode) noexcept {
    return;
}

void inline Chip8::CXNN([[maybe_unused]] u16 opcode) noexcept {
    return;
}

// Draw sprite at position VX, VY with 0xN bytes of sprite data
// starting at the address stored in I
// Set VF to 0x01 if any set pixels are changed to unset, and 00 otherwise
void inline Chip8::DXYN(u16 opcode) noexcept {
    // get x and y coords from VX, VY modulo screen size
    const auto x_start = V_[nibble(nib::second, opcode)] % SCREEN_WIDTH;
    const auto x_end = std::min(static_cast<u32>(x_start + 8u), SCREEN_WIDTH);
    const auto y_start = V_[nibble(nib::third, opcode)] % SCREEN_HEIGHT;
    const auto pixel_height = nibble(nib::fourth, opcode);
    const auto y_end =
        std::min(static_cast<u32>(y_start + pixel_height), SCREEN_HEIGHT);


    // set VF to 0 before drawing sprite
    V_[0xF] = 0x0;

    // TODO: More elegant solution for this?
    // The tricky part is keeping track of the state to set VF if needed
    for (auto row = y_start; row < y_end; ++row) {
        const auto byte = memory_[I_ + row - y_start];
        const auto bitmap = byte_to_bitmap(byte);
        // XOR bitmap with pixel_row and set VF if a pixel_row bit is unset
        for (auto col = x_start; col < x_end; ++col) {
            // pixel_row bit becomes unset if both bits are true
            auto screen_ind = row_col_to_screen_index(row, col);
            if (bitmap[col - x_start] && screen_[screen_ind]) {
                V_[0xF] = 0x1;
            }

            screen_[screen_ind] =
                (screen_[screen_ind] != bitmap[col - x_start]);
        }
    }

}

// Skip the following instruction if the key corresponding to the hex value 
// stored in register VX is pressed
void inline Chip8::EX9E([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    u8 hex_value = V_[second_nibble] & 0xF;
    if (keydown_[hex_value]) pc_ += 2;
}

// Skip the following instruction if the key corresponding to the hex value stored 
// in register VX is not pressed
void inline Chip8::EXA1([[maybe_unused]] u16 opcode) noexcept {
    const auto second_nibble = nibble(nib::second, opcode);
    u8 hex_value = V_[second_nibble] & 0xF;
    if (!keydown_[hex_value]) pc_ += 2;
}

// F
void inline Chip8::FX07([[maybe_unused]] u16 opcode) noexcept {
    return;
}

// Wait for a keypress and store the result in register VX
void inline Chip8::FX0A([[maybe_unused]] u16 opcode) noexcept {
    // decrement pc by 2 to stay on this instruction if a key has not been pressed
    if (current_key_down_ == Key::None)  pc_ -= 2;
    else {
        const auto second_nibble = nibble(nib::second, opcode);
        V_[second_nibble] = key_to_index(current_key_down_);
    }
}

void inline Chip8::FX15([[maybe_unused]] u16 opcode) noexcept {
    return;
}

void inline Chip8::FX18([[maybe_unused]] u16 opcode) noexcept {
    return;
}

void inline Chip8::FX1E([[maybe_unused]] u16 opcode) noexcept {
    return;
}

void inline Chip8::FX29([[maybe_unused]] u16 opcode) noexcept {
    return;
}

void inline Chip8::FX33([[maybe_unused]] u16 opcode) noexcept {
    return;
}

void inline Chip8::FX55([[maybe_unused]] u16 opcode) noexcept {
    return;
}

void inline Chip8::FX65([[maybe_unused]] u16 opcode) noexcept {
    return;
}





auto read_instruction_table(std::string_view path)
    -> std::map<InstructionType, std::string> {
    namespace fs = std::filesystem;
    fs::path file_path{path};

    if (!fs::is_regular_file(file_path)) {
        spdlog::error("Instruction Table: {} is not a regular file",
                      file_path.string());
        std::terminate();
    }

    std::ifstream table{file_path};
    auto instruction_table = std::map<InstructionType, std::string>{};
    std::string type;
    std::string description;

    while (getline(table, type, ',')) {
        getline(table, description);
         /* spdlog::debug("{}\t{}\n", type, description);  */
        const auto inst_type = string_to_instruction_type(type);
        instruction_table.emplace(inst_type, description);
    }

    return instruction_table;
}
