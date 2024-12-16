#include "opcode_cycles.h"
#include <unordered_map>
#include <cstdint>

const std::unordered_map<uint8_t, int> opcodeCycles = {
    // ADC - Add with Carry
    {0x69, 2}, // Immediate
    {0x65, 3}, // Zero Page
    {0x75, 4}, // Zero Page,X
    {0x6D, 4}, // Absolute
    {0x7D, 4}, // Absolute,X
    {0x79, 4}, // Absolute,Y
    {0x61, 6}, // (Indirect,X)
    {0x71, 5}, // (Indirect),Y

    // AND - Bitwise AND
    {0x29, 2}, // Immediate
    {0x25, 3}, // Zero Page
    {0x35, 4}, // Zero Page,X
    {0x2D, 4}, // Absolute
    {0x3D, 4}, // Absolute,X
    {0x39, 4}, // Absolute,Y
    {0x21, 6}, // (Indirect,X)
    {0x31, 5}, // (Indirect),Y

    // ASL - Arithmetic Shift Left
    {0x0A, 2}, // Accumulator
    {0x06, 5}, // Zero Page
    {0x16, 6}, // Zero Page,X
    {0x0E, 6}, // Absolute
    {0x1E, 7}, // Absolute,X

    // Branch Instructions
    {0x90, 2}, // BCC - Branch if Carry Clear
    {0xB0, 2}, // BCS - Branch if Carry Set
    {0xF0, 2}, // BEQ - Branch if Equal
    {0x30, 2}, // BMI - Branch if Minus
    {0xD0, 2}, // BNE - Branch if Not Equal
    {0x10, 2}, // BPL - Branch if Plus
    {0x50, 2}, // BVC - Branch if Overflow Clear
    {0x70, 2}, // BVS - Branch if Overflow Set

    // BIT - Bit Test
    {0x24, 3}, // Zero Page
    {0x2C, 4}, // Absolute

    // BRK - Break
    {0x00, 7}, // Implied

    // CMP - Compare Accumulator
    {0xC9, 2}, // Immediate
    {0xC5, 3}, // Zero Page
    {0xD5, 4}, // Zero Page,X
    {0xCD, 4}, // Absolute
    {0xDD, 4}, // Absolute,X
    {0xD9, 4}, // Absolute,Y
    {0xC1, 6}, // (Indirect,X)
    {0xD1, 5}, // (Indirect),Y

    // CPX - Compare X
    {0xE0, 2}, // Immediate
    {0xE4, 3}, // Zero Page
    {0xEC, 4}, // Absolute

    // CPY - Compare Y
    {0xC0, 2}, // Immediate
    {0xC4, 3}, // Zero Page
    {0xCC, 4}, // Absolute

    // DEC - Decrement Memory
    {0xC6, 5}, // Zero Page
    {0xD6, 6}, // Zero Page,X
    {0xCE, 6}, // Absolute
    {0xDE, 7}, // Absolute,X

    // DEX - Decrement X
    {0xCA, 2}, // Implied

    // DEY - Decrement Y
    {0x88, 2}, // Implied

    // EOR - Exclusive OR
    {0x49, 2}, // Immediate
    {0x45, 3}, // Zero Page
    {0x55, 4}, // Zero Page,X
    {0x4D, 4}, // Absolute
    {0x5D, 4}, // Absolute,X
    {0x59, 4}, // Absolute,Y
    {0x41, 6}, // (Indirect,X)
    {0x51, 5}, // (Indirect),Y

    // INC - Increment Memory
    {0xE6, 5}, // Zero Page
    {0xF6, 6}, // Zero Page,X
    {0xEE, 6}, // Absolute
    {0xFE, 7}, // Absolute,X

    // INX - Increment X
    {0xE8, 2}, // Implied

    // INY - Increment Y
    {0xC8, 2}, // Implied

    // JMP - Jump
    {0x4C, 3}, // Absolute
    {0x6C, 5}, // Indirect

    // JSR - Jump to Subroutine
    {0x20, 6}, // Absolute

    // LDA - Load Accumulator
    {0xA9, 2}, // Immediate
    {0xA5, 3}, // Zero Page
    {0xB5, 4}, // Zero Page,X
    {0xAD, 4}, // Absolute
    {0xBD, 4}, // Absolute,X
    {0xB9, 4}, // Absolute,Y
    {0xA1, 6}, // (Indirect,X)
    {0xB1, 5}, // (Indirect),Y

    // LDX - Load X
    {0xA2, 2}, // Immediate
    {0xA6, 3}, // Zero Page
    {0xB6, 4}, // Zero Page,Y
    {0xAE, 4}, // Absolute
    {0xBE, 4}, // Absolute,Y

    // LDY - Load Y
    {0xA0, 2}, // Immediate
    {0xA4, 3}, // Zero Page
    {0xB4, 4}, // Zero Page,X
    {0xAC, 4}, // Absolute
    {0xBC, 4}, // Absolute,X

    // LSR - Logical Shift Right
    {0x4A, 2}, // Accumulator
    {0x46, 5}, // Zero Page
    {0x56, 6}, // Zero Page,X
    {0x4E, 6}, // Absolute
    {0x5E, 7}, // Absolute,X

    // NOP - No Operation
    {0xEA, 2}, // Implied

    // ORA - Logical Inclusive OR
    {0x09, 2}, // Immediate
    {0x05, 3}, // Zero Page
    {0x15, 4}, // Zero Page,X
    {0x0D, 4}, // Absolute
    {0x1D, 4}, // Absolute,X
    {0x19, 4}, // Absolute,Y
    {0x01, 6}, // (Indirect,X)
    {0x11, 5}, // (Indirect),Y

    // PHA - Push Accumulator
    {0x48, 3}, // Implied

    // PHP - Push Processor Status
    {0x08, 3}, // Implied

    // PLA - Pull Accumulator
    {0x68, 4}, // Implied

    // PLP - Pull Processor Status
    {0x28, 4}, // Implied

    // ROL - Rotate Left
    {0x2A, 2}, // Accumulator
    {0x26, 5}, // Zero Page
    {0x36, 6}, // Zero Page,X
    {0x2E, 6}, // Absolute
    {0x3E, 7}, // Absolute,X

    // ROR - Rotate Right
    {0x6A, 2}, // Accumulator
    {0x66, 5}, // Zero Page
    {0x76, 6}, // Zero Page,X
    {0x6E, 6}, // Absolute
    {0x7E, 7}, // Absolute,X

    // RTI - Return from Interrupt
    {0x40, 6}, // Implied

    // RTS - Return from Subroutine
    {0x60, 6}, // Implied

    // SBC - Subtract with Carry
    {0xE9, 2}, // Immediate
    {0xE5, 3}, // Zero Page
    {0xF5, 4}, // Zero Page,X
    {0xED, 4}, // Absolute
    {0xFD, 4}, // Absolute,X
    {0xF9, 4}, // Absolute,Y
    {0xE1, 6}, // (Indirect,X)
    {0xF1, 5}, // (Indirect),Y

    // SEC - Set Carry
    {0x38, 2}, // Implied

    // SED - Set Decimal
    {0xF8, 2}, // Implied

    // SEI - Set Interrupt Disable
    {0x78, 2}, // Implied

    // STA - Store Accumulator
    {0x85, 3}, // Zero Page
    {0x95, 4}, // Zero Page,X
    {0x8D, 4}, // Absolute
    {0x9D, 5}, // Absolute,X
    {0x99, 5}, // Absolute,Y
    {0x81, 6}, // (Indirect,X)
    {0x91, 6}, // (Indirect),Y

    // STX - Store X
    {0x86, 3}, // Zero Page
    {0x96, 4}, // Zero Page,Y
    {0x8E, 4}, // Absolute

    // STY - Store Y
    {0x84, 3}, // Zero Page
    {0x94, 4}, // Zero Page,X
    {0x8C, 4}, // Absolute

    // TAX - Transfer Accumulator to X
    {0xAA, 2}, // Implied

    // TAY - Transfer Accumulator to Y
    {0xA8, 2}, // Implied

    // TSX - Transfer Stack Pointer to X
    {0xBA, 2}, // Implied

    // TXA - Transfer X to Accumulator
    {0x8A, 2}, // Implied

    // TXS - Transfer X to Stack Pointer
    {0x9A, 2}, // Implied

    // TYA - Transfer Y to Accumulator
    {0x98, 2}, // Implied
};
