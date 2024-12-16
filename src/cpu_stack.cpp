#include "cpu.h"
#include <fstream>
#include <iostream>

void initializeStackOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{

    // ===== PHA Instructions =====
#pragma region PHA Opcodes

    // Opcode implementation in the opcode table
    opcodeTable[0x48] = [](CPU &cpu) {       // PHA - Push A
        cpu.memory[0x0100 + cpu.SP] = cpu.A; // Store A at the stack location
        cpu.SP--;                            // Decrement the stack pointer
        std::cout << "PHA: Pushed value " << std::hex << (int)cpu.A
                  << " to stack at address " << (0x0100 + cpu.SP + 1)
                  << ", SP = " << (int)cpu.SP << std::dec << std::endl;
    };
#pragma endregion

    // ===== PLA Instructions =====
#pragma region PLA Opcodes

    // Opcode implementation in the opcode table
    opcodeTable[0x68] = [](CPU &cpu) {               // PLA - Pull A
        cpu.SP++;                                    // Increment the stack pointer
        uint8_t value = cpu.memory[0x0100 + cpu.SP]; // Fetch the value from the stack
        cpu.A = value;                               // Load the value into the accumulator
        cpu.setFlag(CPU::Z, cpu.A == 0);             // Set Zero flag if A is 0
        cpu.setFlag(CPU::N, cpu.A & 0x80);           // Set Negative flag if bit 7 of A is set
        std::cout << "PLA: Pulled value " << std::hex << (int)value
                  << " into A, SP = " << (int)cpu.SP
                  << ", A = " << (int)cpu.A << std::dec << std::endl;
    };
#pragma endregion

    // ===== PHP Instructions =====
#pragma region PHP Opcodes

    // Opcode implementation in the opcode table
    opcodeTable[0x08] = [](CPU &cpu) {        // PHP - Push Processor Status
        uint8_t status = cpu.P | 0x30;        // Ensure B flag and unused flag are set (NV1BDIZC with B and bit 4 set)
        cpu.memory[0x0100 + cpu.SP] = status; // Store status flags at the stack location
        cpu.SP--;                             // Decrement the stack pointer
        std::cout << "PHP: Pushed status " << std::hex << (int)status
                  << " to stack at address " << (0x0100 + cpu.SP + 1)
                  << ", SP = " << (int)cpu.SP << std::dec << std::endl;
    };
#pragma endregion

    // ===== PLP Instructions =====
#pragma region PLP Opcodes
opcodeTable[0x28] = [](CPU &cpu) {
    // Increment the stack pointer
    cpu.SP++;

    // Pull the value from the stack
    uint8_t flags = cpu.memory[0x0100 + cpu.SP];

    // Load the pulled value into the status register (excluding B flag)
    cpu.P = flags & 0xEF; // Mask out the B flag (bit 4)

    // Debug output
    std::cerr << "[PLP Debug] Pulled Flags: " << std::bitset<8>(flags) << "\n";
    std::cerr << "[PLP Debug] Status Register After PLP: " << std::bitset<8>(cpu.P) << "\n";
};



#pragma endregion

// ===== TXS Instructions =====
#pragma region TXS Opcodes
opcodeTable[0x9A] = [](CPU &cpu) { // TXS Implied
    cpu.SP = cpu.X; // Transfer value from X to SP
    std::cout << "TXS: X = " << std::hex << (int)cpu.X
              << ", SP = " << (int)cpu.SP
              << std::endl;
};


#pragma endregion

// ===== TSX Instructions =====
#pragma region TXS Opcodes
#pragma region TXS Opcodes
opcodeTable[0xBA] = [](CPU &cpu) { // TSX Implied
    cpu.X = cpu.SP;                     // Transfer value from SP to X
    cpu.setFlag(CPU::Z, cpu.X == 0);    // Set Zero flag if result is zero
    cpu.setFlag(CPU::N, cpu.X & 0x80);  // Set Negative flag if bit 7 is set
    std::cout << "TSX: SP = " << std::hex << (int)cpu.SP 
              << ", X = " << (int)cpu.X 
              << ", Z = " << cpu.getFlag(CPU::Z)
              << ", N = " << cpu.getFlag(CPU::N)
              << std::endl;
};
#pragma endregion
}