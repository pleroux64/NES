#include "cpu.h"
#include <fstream>
#include <iostream>

void initializeMiscOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== NOP Instructions =====
#pragma region NOP Opcodes
    opcodeTable[0xEA] = [](CPU &cpu)
    {
        //std::cout << "Executing NOP: No Operation" << std::endl;
// NOP does nothing, but it consumes one cycle.
#pragma endregion
    };
}