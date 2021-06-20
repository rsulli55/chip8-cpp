#include "chip8.h"
#include "common.h"

void Chip8::execute(u16 opcode) noexcept {
    u16 first_nibble = nibble(nib::first, opcode);
    switch(first_nibble) {
        case 0x0:
            break;
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
            break;
        case 0x8:
            break;
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
}
        
// instructions
void inline Chip8::_6XNN(u16 opcode) noexcept {
    u16 second_nibble = nibble(nib::second, opcode);
    V_[second_nibble] = opcode & 0x00FF;
}
