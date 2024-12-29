#include "cpu.h"
#include <fstream>
#include <iostream>

void initializeTransferOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== TAX Instructions =====
#pragma region TAX Opcodes
    opcodeTable[0xAA] = [](CPU &cpu) {     // TAX Implied
        cpu.X = cpu.A;                     // Transfer value from A to X
        cpu.setFlag(CPU::Z, cpu.X == 0);   // Set Zero flag if result is zero
        cpu.setFlag(CPU::N, cpu.X & 0x80); // Set Negative flag if bit 7 is set
        // std::cout << "TAX: A = " << std::hex << (int)cpu.A
        //           << ", X = " << (int)cpu.X
        //           << ", Z = " << cpu.getFlag(CPU::Z)
        //           << ", N = " << cpu.getFlag(CPU::N)
        //           << std::endl;
    };

#pragma endregion

// ===== TXA Instructions =====
#pragma region TXA Opcodes
    opcodeTable[0x8A] = [](CPU &cpu) {     // TXA Implied
        cpu.A = cpu.X;                     // Transfer value from X to A
        cpu.setFlag(CPU::Z, cpu.A == 0);   // Set Zero flag if result is zero
        cpu.setFlag(CPU::N, cpu.A & 0x80); // Set Negative flag if bit 7 is set
        // std::cout << "TXA: X = " << std::hex << (int)cpu.X
        //           << ", A = " << (int)cpu.A
        //           << ", Z = " << cpu.getFlag(CPU::Z)
        //           << ", N = " << cpu.getFlag(CPU::N)
        //           << std::endl;
    };

#pragma endregion

// ===== TAY Instructions =====
#pragma region TAY Opcodes
    opcodeTable[0xA8] = [](CPU &cpu) {     // TAY Implied
        cpu.Y = cpu.A;                     // Transfer value from A to Y
        cpu.setFlag(CPU::Z, cpu.Y == 0);   // Set Zero flag if result is zero
        cpu.setFlag(CPU::N, cpu.Y & 0x80); // Set Negative flag if bit 7 is set
        // std::cout << "TAY: A = " << std::hex << (int)cpu.A
        //           << ", Y = " << (int)cpu.Y
        //           << ", Z = " << cpu.getFlag(CPU::Z)
        //           << ", N = " << cpu.getFlag(CPU::N)
        //           << std::endl;
    };
#pragma endregion

// ===== TYA Instructions =====
#pragma region TYA Opcodes
    opcodeTable[0x98] = [](CPU &cpu) {     // TYA Implied
        cpu.A = cpu.Y;                     // Transfer value from Y to A
        cpu.setFlag(CPU::Z, cpu.A == 0);   // Set Zero flag if A is zero
        cpu.setFlag(CPU::N, cpu.A & 0x80); // Set Negative flag if the most significant bit of A is set

        // std::cout << "TYA: Y = " << std::hex << (int)cpu.Y
        //           << ", A = " << (int)cpu.A
        //           << std::endl;
    };

#pragma endregion
}