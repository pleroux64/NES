#include "cpu.h"
#include <fstream>
#include <iostream>

void CPU::performAND(uint8_t value)
{
    A = A & value;        // Perform bitwise AND
    setFlag(Z, A == 0);   // Set Zero flag if result is 0
    setFlag(N, A & 0x80); // Set Negative flag if bit 7 is set
}

void CPU::performORA(uint8_t value)
{
    A = A | value;        // Perform bitwise OR
    setFlag(Z, A == 0);   // Update Zero flag
    setFlag(N, A & 0x80); // Update Negative flag
}

void CPU::performEOR(uint8_t value)
{
    A ^= value;                  // Perform XOR operation
    setFlag(Z, A == 0);          // Set Zero flag if result is zero
    setFlag(N, (A & 0x80) != 0); // Set Negative flag if bit 7 is set
    // std::cout << "EOR: A = " << std::hex << (int)A
    //           << ", Z flag: " << getFlag(Z)
    //           << ", N flag: " << getFlag(N) << std::dec << std::endl;
}

void CPU::performBIT(uint8_t value)
{
    uint8_t result = A & value;

    // Debugging output
    // std::cout << "BIT operation: A = " << std::hex << (int)A
    //           << ", memory = " << (int)value
    //           << ", result = " << (int)result << std::dec << std::endl;

    setFlag(Z, result == 0);  // Set Zero flag
    setFlag(V, value & 0x40); // Set Overflow flag (bit 6)
    //std::cout << "Setting V to: " << ((value & 0x40) != 0) << std::endl;

    setFlag(N, value & 0x80); // Set Negative flag (bit 7)
}

void initializeBitwiseOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{

// ===== AND Instructions =====
#pragma region AND Opcodes

    // Immediate mode - #Value
    opcodeTable[0x29] = [](CPU &cpu) {   // AND Immediate
        uint8_t value = cpu.fetchByte(); // Fetch the operand
        cpu.performAND(value);           // Perform AND operation
    };

    // Zero Page mode - 0x00 to 0xFF (first 256 bytes of memory)
    opcodeTable[0x25] = [](CPU &cpu) {        // AND Zero Page
        uint8_t addr = cpu.fetchByte();       // Fetch the address
        uint8_t value = cpu.readMemory(addr); // Fetch the value from memory
        cpu.performAND(value);                // Perform AND operation
    };

    // Zero Page, X mode - (Zero Page + X)
    opcodeTable[0x35] = [](CPU &cpu) {                         // AND Zero Page,X
        uint8_t addr = cpu.fetchByte();                        // Fetch the base address
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFF); // Add X to the address
        cpu.performAND(value);                                 // Perform AND operation
    };

    // Absolute mode - 0x0000 to 0xFFFF (16-bit address)
    opcodeTable[0x2D] = [](CPU &cpu) {        // AND Absolute
        uint16_t addr = cpu.fetchWord();      // Fetch the address
        uint8_t value = cpu.readMemory(addr); // Fetch the value from memory
        cpu.performAND(value);                // Perform AND operation
    };

    // Absolute, X mode - (Absolute + X)
    opcodeTable[0x3D] = [](CPU &cpu) {                           // AND Absolute,X
        uint16_t addr = cpu.fetchWord();                         // Fetch the address
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFFFF); // Add X to the address
        cpu.performAND(value);                                   // Perform AND operation
    };

    // Absolute, Y mode - (Absolute + Y)
    opcodeTable[0x39] = [](CPU &cpu) {                           // AND Absolute,Y
        uint16_t addr = cpu.fetchWord();                         // Fetch the address
        uint8_t value = cpu.readMemory((addr + cpu.Y) & 0xFFFF); // Add Y to the address
        cpu.performAND(value);                                   // Perform AND operation
    };

    // Indirect, X mode - (Indirect,X)
    opcodeTable[0x21] = [](CPU &cpu) {                                 // AND (Indirect,X)
        uint8_t addr = cpu.fetchByte();                                // Fetch the base address
        uint8_t effectiveAddr = cpu.readMemory((addr + cpu.X) & 0xFF); // Add X to the address
        uint8_t value = cpu.readMemory(effectiveAddr);                 // Fetch the value from memory
        cpu.performAND(value);                                         // Perform AND operation
    };

    // Indirect, Y mode - (Indirect),Y
    opcodeTable[0x31] = [](CPU &cpu) {                                    // AND (Indirect),Y
        uint8_t addr = cpu.fetchByte();                                   // Fetch the base address
        uint8_t effectiveAddr = cpu.readMemory(addr);                     // Fetch the value at the indirect address
        uint8_t value = cpu.readMemory((effectiveAddr + cpu.Y) & 0xFFFF); // Add Y to the address
        cpu.performAND(value);                                            // Perform AND operation
    };

#pragma endregion

    // ===== ORA Instructions =====
#pragma region ORA Opcodes

    opcodeTable[0x09] = [](CPU &cpu) { // Immediate Mode
        uint8_t value = cpu.fetchByte();
        cpu.performORA(value);
    };

    opcodeTable[0x05] = [](CPU &cpu) { // Zero Page
        uint8_t address = cpu.fetchByte();
        uint8_t value = cpu.readMemory(address);
        cpu.performORA(value);
    };

    opcodeTable[0x15] = [](CPU &cpu) { // Zero Page, X
        uint8_t address = (cpu.fetchByte() + cpu.X) & 0xFF;
        uint8_t value = cpu.readMemory(address);
        cpu.performORA(value);
    };

    opcodeTable[0x0D] = [](CPU &cpu) { // Absolute
        uint16_t address = cpu.fetchWord();
        uint8_t value = cpu.readMemory(address);
        cpu.performORA(value);
    };

    opcodeTable[0x1D] = [](CPU &cpu) { // Absolute, X
        uint16_t address = cpu.fetchWord() + cpu.X;
        uint8_t value = cpu.readMemory(address & 0xFFFF);
        cpu.performORA(value);
    };

    opcodeTable[0x19] = [](CPU &cpu) { // Absolute, Y
        uint16_t address = cpu.fetchWord() + cpu.Y;
        uint8_t value = cpu.readMemory(address & 0xFFFF);
        cpu.performORA(value);
    };

    opcodeTable[0x01] = [](CPU &cpu) { // Indirect, X
        uint8_t address = (cpu.fetchByte() + cpu.X) & 0xFF;
        uint16_t effectiveAddress = cpu.readMemory(address) | (cpu.readMemory((address + 1) & 0xFF)
                                                               << 8);
        uint8_t value = cpu.readMemory(effectiveAddress);
        cpu.performORA(value);
    };

    opcodeTable[0x11] = [](CPU &cpu) { // Indirect, Y
        uint8_t address = cpu.fetchByte();
        uint16_t effectiveAddress = (cpu.readMemory(address) | (cpu.readMemory((address + 1) & 0xFF)
                                                                << 8)) +
                                    cpu.Y;
        uint8_t value = cpu.readMemory(effectiveAddress & 0xFFFF);
        cpu.performORA(value);
    };

#pragma endregion

    // ===== EOR Instructions =====
#pragma region EOR Opcodes

    // Immediate mode
    opcodeTable[0x49] = [](CPU &cpu)
    {
        uint8_t value = cpu.fetchByte(); // Fetch immediate value
        cpu.performEOR(value);           // Perform EOR operation
    };

    // Zero Page
    opcodeTable[0x45] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte();       // Fetch address from zero page
        uint8_t value = cpu.readMemory(addr); // Fetch value from memory
        cpu.performEOR(value);                // Perform EOR operation
    };

    // Zero Page, X
    opcodeTable[0x55] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte();                        // Fetch base address
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFF); // Add X and wrap around zero page
        cpu.performEOR(value);                                 // Perform EOR operation
    };

    // Absolute
    opcodeTable[0x4D] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();      // Fetch absolute address
        uint8_t value = cpu.readMemory(addr); // Fetch value from memory
        cpu.performEOR(value);                // Perform EOR operation
    };

    // Absolute, X
    opcodeTable[0x5D] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();                         // Fetch base address
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFFFF); // Add X to address
        cpu.performEOR(value);                                   // Perform EOR operation
    };

    // Absolute, Y
    opcodeTable[0x59] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();                         // Fetch base address
        uint8_t value = cpu.readMemory((addr + cpu.Y) & 0xFFFF); // Add Y to address
        cpu.performEOR(value);                                   // Perform EOR operation
    };

    // (Indirect, X)
    opcodeTable[0x41] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte(); // Fetch zero page base address
        uint16_t effectiveAddr = cpu.readMemory((addr + cpu.X) & 0xFF) |
                                 (cpu.readMemory((addr + cpu.X + 1) & 0xFF)
                                  << 8);               // Fetch indirect address
        uint8_t value = cpu.readMemory(effectiveAddr); // Fetch value from memory
        cpu.performEOR(value);                         // Perform EOR operation
    };

    // (Indirect), Y
    opcodeTable[0x51] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte(); // Fetch zero page base address
        uint16_t effectiveAddr = cpu.readMemory(addr) |
                                 (cpu.readMemory((addr + 1) & 0xFF)
                                  << 8);                                  // Fetch indirect address
        uint8_t value = cpu.readMemory((effectiveAddr + cpu.Y) & 0xFFFF); // Add Y to address
        cpu.performEOR(value);                                            // Perform EOR operation
    };

#pragma endregion

    // ===== BIT Instructions =====
#pragma region BIT Opcodes

    // BIT Zero Page (opcode 0x24)
    opcodeTable[0x24] = [](CPU &cpu)
    {
        uint8_t address = cpu.fetchByte();       // Fetch the zero page address
        uint8_t value = cpu.readMemory(address); // Get value from memory
        cpu.performBIT(value);                   // Perform BIT operation
    };

    // BIT Absolute (opcode 0x2C)
    opcodeTable[0x2C] = [](CPU &cpu)
    {
        uint16_t address = cpu.fetchWord();      // Fetch the 16-bit absolute address
        uint8_t value = cpu.readMemory(address); // Get value from memory
        cpu.performBIT(value);                   // Perform BIT operation
    };

#pragma endregion
}
