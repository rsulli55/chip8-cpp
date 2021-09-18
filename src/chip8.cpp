#include <algorithm>
#include <fstream>
#include <filesystem>

#include "chip8.h"
#include "common.h"

Chip8::Chip8() : instruction_table_{read_instruction_table(instruction_table_path_)} { 
    spdlog::debug("Chip8 Constructor called\n"); 
    initialize_font();
}

auto Chip8::fetch_with_pc(u16 pc) const noexcept -> u16 {
    // opcodes stored in big endian
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

void Chip8::load_rom(const Rom &rom) {
    const auto &data = rom.data_;
    const auto end =
        std::min(std::cend(data), std::cbegin(data) + MEMORY_SIZE - ROM_START);
    std::copy(std::cbegin(data), end, std::begin(memory_) + ROM_START);
}

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

void inline Chip8::_ANNN(u16 opcode) noexcept {
    I_ = opcode & 0x0FFF;
}

// Draw sprite at position VX, VY with 0xN bytes of sprite data
// starting at the address stored in I
// Set VF to 0x01 if any set pixels are changed to unset, and 00 otherwise
void inline Chip8::_DXYN(u16 opcode) noexcept {
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

    while (getline(table, type, '\t')) {
        getline(table, description);
        const auto inst_type = string_to_instruction_type(type);
        instruction_table.emplace(inst_type, description);
    }

    /* spdlog::debug("Printing instruction table\n"); */
    /* for (const auto& [type, desc] : instruction_table)
     * spdlog::debug("{}\t{}\n", type, desc); */

    return instruction_table;
}
