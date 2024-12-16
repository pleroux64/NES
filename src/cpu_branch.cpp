#include "cpu.h"
#include <fstream>
#include <iostream>

void CPU::performBMI()
{
    int8_t offset = static_cast<int8_t>(fetchByte()); // Fetch signed offset
    std::cout << "Offset: " << (int)offset << ", Negative flag (N): " << getFlag(N) << std::endl;

    if (getFlag(N))
    {                 // Branch if Negative flag is set
        PC += offset; // Apply signed offset
        std::cout << "Branching! New PC: " << std::hex << PC << std::dec << std::endl;
    }
    else
    {
        PC += 0; // No branch; PC remains at the next instruction
        std::cout << "No Branch. PC unchanged: " << std::hex << PC << std::dec << std::endl;
    }
}

void CPU::performBVC()
{
    uint16_t prePC = PC;                              // Save the current PC for debugging
    int8_t offset = static_cast<int8_t>(fetchByte()); // Fetch the signed branch offset
    std::cout << "Fetching BVC instruction. Offset: " << (int)offset
              << ", Overflow flag (V): " << getFlag(V)
              << ", Current PC: " << std::hex << prePC << std::dec << std::endl;

    if (!getFlag(V)) // Branch if Overflow flag is clear
    {
        uint16_t oldPC = PC; // Save the current PC for logging
        PC += offset;        // Add the signed offset to PC
        std::cout << "Branch taken. Old PC: " << std::hex << oldPC
                  << ", New PC: " << PC << std::dec << std::endl;
    }
    else
    {
        std::cout << "No branch. PC remains at: " << std::hex << PC << std::dec << std::endl;
    }
}

void CPU::performBVS()
{
    int8_t offset = static_cast<int8_t>(fetchByte()); // Fetch the signed branch offset
    std::cout << "Offset: " << (int)offset << ", Overflow flag (V): " << getFlag(V) << std::endl;

    if (getFlag(V)) // Branch if Overflow flag is set
    {
        uint16_t oldPC = PC; // Save the old PC for debugging
        PC += offset;        // Apply signed offset to the PC
        std::cout << "Branching from PC: " << std::hex << oldPC
                  << " to New PC: " << PC << std::dec << std::endl;
    }
    else
    {
        std::cout << "No branch. PC remains at: " << std::hex << PC << std::dec << std::endl;
    }
}

void initializeBranchOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== BCC Instructions =====
#pragma region BCC Opcodes

    opcodeTable[0x90] = [](CPU &cpu)
    {
        int8_t offset = static_cast<int8_t>(cpu.fetchByte());
        if (!cpu.getFlag(CPU::C))
        {
            cpu.PC += offset;
        }
        else
        {
            cpu.PC += 2;
        }
    };
#pragma endregion

    // ===== BCS Instructions =====
#pragma region BCS Opcodes

    opcodeTable[0xB0] = [](CPU &cpu)
    {
        int8_t offset = static_cast<int8_t>(cpu.fetchByte());
        if (cpu.getFlag(CPU::C))
        {
            cpu.PC += offset;
        }
        else
        {
            cpu.PC += 2;
        }
    };
#pragma endregion

    // ===== BEQ Instructions =====
#pragma region BEQ Opcodes

    opcodeTable[0xF0] = [](CPU &cpu)
    {
        int8_t offset = static_cast<int8_t>(cpu.fetchByte()); // Fetch signed offset
        std::cout << "Offset: " << (int)offset << ", Zero flag (Z): " << cpu.getFlag(CPU::Z) << std::endl;

        if (cpu.getFlag(CPU::Z))
        {                     // Branch if Zero flag is set
            cpu.PC += offset; // Adjust PC by the signed offset
            std::cout << "Branching! New PC: " << std::hex << cpu.PC << std::dec << std::endl;
        }
        else
        {
            std::cout << "No Branch. PC unchanged: " << std::hex << cpu.PC << std::dec << std::endl;
        }
    };
#pragma endregion

// ===== BNE Instructions =====
#pragma region BNE Opcodes

    opcodeTable[0xD0] = [](CPU &cpu)
    {
        int8_t offset = static_cast<int8_t>(cpu.fetchByte()); // Fetch signed offset
        std::cout << "Offset: " << (int)offset << ", Zero flag (Z): " << cpu.getFlag(CPU::Z) << std::endl;

        if (!cpu.getFlag(CPU::Z))
        {                     // Branch if Zero flag is clear
            cpu.PC += offset; // Adjust PC by the signed offset
            std::cout << "Branching! New PC: " << std::hex << cpu.PC << std::dec << std::endl;
        }
        else
        {
            // No branch: Only fetchByte() has already incremented PC by 1.
            std::cout << "No Branch. PC unchanged: " << std::hex << cpu.PC << std::dec << std::endl;
        }
    };
#pragma endregion

// ===== BPL Instructions =====
#pragma region BPL Opcodes

    // BPL - Branch if Plus (Opcode: 0x10)
    opcodeTable[0x10] = [](CPU &cpu)
    {
        int8_t offset = static_cast<int8_t>(cpu.fetchByte()); // Fetch signed offset
        std::cout << "Offset: " << (int)offset << ", Negative flag (N): " << cpu.getFlag(CPU::N) << std::endl;

        if (!cpu.getFlag(CPU::N))
        {                     // Branch if Negative flag is clear
            cpu.PC += offset; // Adjust PC by the signed offset
            std::cout << "Branching! New PC: " << std::hex << cpu.PC << std::dec << std::endl;
        }
        else
        {
            // No need to increment PC here, fetchByte already incremented past the operand
            std::cout << "No Branch. PC unchanged: " << std::hex << cpu.PC << std::dec << std::endl;
        }
    };
#pragma endregion

// ===== BMI Instructions =====
#pragma region BMI Opcodes

    opcodeTable[0x30] = [](CPU &cpu)
    {
        cpu.performBMI();
    };

#pragma endregion

// ===== BVC Instructions =====
#pragma region BVC Opcodes

    opcodeTable[0x50] = [](CPU &cpu)
    {
        cpu.performBVC();
    };

#pragma endregion

// ===== BVS Instructions =====
#pragma region BVS Opcodes

    opcodeTable[0x70] = [](CPU &cpu)
    {
        cpu.performBVS();
    };

#pragma endregion
}