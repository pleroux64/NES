#include "cpu.h"
#include <fstream>
#include <iostream>

void initializeFlagsOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== CLC Instructions =====
#pragma region CLC Opcodes

    opcodeTable[0x18] = [](CPU &cpu)
    {
        std::cout << "Executing CLC: Clearing Carry Flag (C)" << std::endl;
        cpu.P &= ~(1 << 0); // Clear the Carry flag (bit 0 of P)
    };

#pragma endregion

// ===== SEC Instructions =====
#pragma region SEC Opcodes
    opcodeTable[0x38] = [](CPU &cpu) { // SEC - Set Carry
        std::cout << "Executing SEC: Setting Carry Flag (C)" << std::endl;
        cpu.setFlag(CPU::C, true); // Set the Carry flag (C) to 1
    };
#pragma endregion

// ===== CLI Instructions =====
#pragma region SEC Opcodes

    opcodeTable[0x58] = [](CPU &cpu)
    {
        std::cout << "Executing CLI: Clearing Interrupt Disable Flag (I)" << std::endl;
        cpu.P &= ~(1 << 2); // Clear the Interrupt Disable flag (bit 2 of P)
    };
#pragma endregion

// ===== SEI Instructions =====
#pragma region SEI Opcodes

    opcodeTable[0x78] = [](CPU &cpu) { // SEI - Set Interrupt Disable
        std::cout << "Executing SEI: Setting Interrupt Disable Flag (I)" << std::endl;
        cpu.setFlag(CPU::I, true); // Set the Interrupt Disable flag (I) to 1
    };

#pragma endregion

// ===== CLD Instructions =====
#pragma region CLD Opcodes

    opcodeTable[0xD8] = [](CPU &cpu)
    {
        std::cout << "Executing CLD: Clearing Decimal Flag (D)" << std::endl;
        cpu.P &= ~(1 << 3); // Clear the Decimal flag (bit 3 of P)
    };
#pragma endregion

// ===== CED Instructions =====
#pragma region CED Opcodes

    opcodeTable[0xF8] = [](CPU &cpu) { // SED - Set Decimal
        std::cout << "Executing SED: Setting Decimal Flag (D)" << std::endl;
        cpu.setFlag(CPU::D, true); // Set the Decimal flag (D) to 1
    };

#pragma endregion

// ===== CLV Instructions =====
#pragma region SLV Opcodes

    opcodeTable[0xB8] = [](CPU &cpu)
    {
        std::cout << "Executing CLV: Clearing Overflow Flag (V)" << std::endl;
        cpu.P &= ~(1 << 6); // Clear the Overflow flag (bit 6 of P)
    };
#pragma endregion
}