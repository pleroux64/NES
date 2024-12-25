#include "cpu.h"
#include <fstream>
#include <iostream>

void CPU::performCMP(uint8_t value)
{
    uint8_t result = A - value; // Perform subtraction (comparison)
    setFlag(C, A >= value);     // Set Carry if A >= memory
    setFlag(Z, A == value);     // Set Zero if A == memory
    setFlag(N, result & 0x80);  // Set Negative if result bit 7 is set
}
void CPU::performCPX(uint8_t value)
{
    uint8_t result = X - value; // Perform subtraction (comparison)
    setFlag(C, X >= value);     // Set Carry if X >= memory
    setFlag(Z, X == value);     // Set Zero if X == memory
    setFlag(N, result & 0x80);  // Set Negative if result bit 7 is set
}

void CPU::performCPY(uint8_t value)
{
    uint8_t result = Y - value; // Perform subtraction (comparison)
    setFlag(C, Y >= value);     // Set Carry if Y >= memory
    setFlag(Z, Y == value);     // Set Zero if Y == memory
    setFlag(N, result & 0x80);  // Set Negative if result bit 7 is set
}

void initializeComparisonOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== CMP Instructions =====
#pragma region CMP Opcodes

    // CMP Immediate
    opcodeTable[0xC9] = [](CPU &cpu)
    {
        uint8_t value = cpu.fetchByte(); // Fetch immediate value
        cpu.performCMP(value);
    };

    // CMP Zero Page
    opcodeTable[0xC5] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte();       // Fetch zero page address
        uint8_t value = cpu.readMemory(addr); // Get value from memory
        cpu.performCMP(value);
    };

    // CMP Zero Page,X
    opcodeTable[0xD5] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte();                        // Fetch zero page address
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFF); // Add X to address (zero page wrapping)
        cpu.performCMP(value);
    };

    // CMP Absolute
    opcodeTable[0xCD] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();      // Fetch 16-bit absolute address
        uint8_t value = cpu.readMemory(addr); // Get value from memory
        cpu.performCMP(value);
    };

    // CMP Absolute,X
    opcodeTable[0xDD] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();                         // Fetch 16-bit absolute address
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFFFF); // Add X to address
        cpu.performCMP(value);
    };

    // CMP Absolute,Y
    opcodeTable[0xD9] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();                         // Fetch 16-bit absolute address
        uint8_t value = cpu.readMemory((addr + cpu.Y) & 0xFFFF); // Add Y to address
        cpu.performCMP(value);
    };

    // CMP (Indirect,X)
    opcodeTable[0xC1] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte(); // Fetch zero page base address
        uint16_t indAddr = cpu.readMemory((addr + cpu.X) & 0xFF) | (cpu.readMemory((addr + cpu.X + 1) & 0xFF)
                                                                    << 8); // Fetch 16-bit effective address
        uint8_t value = cpu.readMemory(indAddr);                           // Get value from memory
        cpu.performCMP(value);
    };

    // CMP (Indirect),Y
    opcodeTable[0xD1] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte(); // Fetch zero page base address
        uint16_t indAddr = cpu.readMemory(addr) | (cpu.readMemory(addr + 1)
                                                   << 8);           // Fetch 16-bit effective address
        uint8_t value = cpu.readMemory((indAddr + cpu.Y) & 0xFFFF); // Add Y to effective address
        cpu.performCMP(value);
    };

#pragma endregion

// ===== CPX Instructions =====
#pragma region CPX Opcodes

    // CPX Immediate
    opcodeTable[0xE0] = [](CPU &cpu)
    {
        uint8_t value = cpu.fetchByte(); // Fetch immediate value
        cpu.performCPX(value);
    };

    // CPX Zero Page
    opcodeTable[0xE4] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte();       // Fetch zero page address
        uint8_t value = cpu.readMemory(addr); // Get value from memory
        cpu.performCPX(value);
    };

    // CPX Absolute
    opcodeTable[0xEC] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();      // Fetch 16-bit absolute address
        uint8_t value = cpu.readMemory(addr); // Get value from memory
        cpu.performCPX(value);
    };

#pragma endregion

// ===== CPY Instructions =====
#pragma region CPY Opcodes

    // CPY Immediate
    opcodeTable[0xC0] = [](CPU &cpu)
    {
        uint8_t value = cpu.fetchByte(); // Fetch immediate value
        cpu.performCPY(value);
    };

    // CPY Zero Page
    opcodeTable[0xC4] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte();       // Fetch zero page address
        uint8_t value = cpu.readMemory(addr); // Get value from memory
        cpu.performCPY(value);
    };

    // CPY Absolute
    opcodeTable[0xCC] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();      // Fetch 16-bit absolute address
        uint8_t value = cpu.readMemory(addr); // Get value from memory
        cpu.performCPY(value);
    };

#pragma endregion
}