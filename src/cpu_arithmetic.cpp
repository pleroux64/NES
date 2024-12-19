#include "cpu.h"
#include <fstream>
#include <iostream>

void CPU::performADC(uint8_t value)
{
    uint16_t result = A + value + getFlag(C);             // A + memory + carry flag
    A = result & 0xFF;                                    // Update A with the lower byte of result
    setFlag(C, result > 0xFF);                            // Set carry if overflow occurred
    setFlag(Z, A == 0);                                   // Set zero flag if result is zero
    setFlag(N, A & 0x80);                                 // Set negative flag if bit 7 is set
    setFlag(V, ((A ^ value) & (A ^ result) & 0x80) != 0); // Set overflow flag for signed overflow
}

void CPU::performSBC(uint8_t value)
{
    uint16_t temp = A - value - (1 - getFlag(CPU::C)); // Subtract with inverted carry
    setFlag(CPU::C, temp < 0x100);                    // Set Carry flag if no unsigned underflow
    setFlag(CPU::Z, (temp & 0xFF) == 0);              // Set Zero flag if result is zero
    setFlag(CPU::N, temp & 0x80);                     // Set Negative flag if bit 7 is set
    setFlag(CPU::V, ((A ^ temp) & 0x80) && ((A ^ value) & 0x80)); // Set Overflow flag
    A = temp & 0xFF;                                  // Store lower 8 bits of the result
}







void CPU::performINC(uint16_t addr)
{
    uint8_t value = readMemory(addr); // Read the value at the address
    value++;                      // Increment the value
    writeMemory(addr, value);       // Write the new value back to memory

    setFlag(Z, value == 0);     // Set Zero flag if result is 0
    setFlag(N, (value & 0x80)); // Set Negative flag if bit 7 is set

    std::cout << "INC: Address: " << std::hex << addr
              << ", Value: " << (int)value
              << ", Z flag: " << getFlag(Z)
              << ", N flag: " << getFlag(N) << std::dec << std::endl;
}

void CPU::performDEC(uint16_t addr)
{
    uint8_t value = readMemory(addr); // Read the value from memory
    std::cout << "Initial value: " << std::hex << (int)value << std::dec << std::endl;

    value--;              // Decrement the value
    writeMemory(addr, value); // Write the decremented value back to memory

    setFlag(Z, value == 0);          // Set Zero flag if the result is 0
    setFlag(N, (value & 0x80) != 0); // Set Negative flag if bit 7 is set

    std::cout << "DEC: Address: " << std::hex << addr
              << ", Value: " << (int)value
              << ", Z flag: " << getFlag(Z)
              << ", N flag: " << getFlag(N) << std::dec << std::endl;
}

void CPU::performINX()
{
    X++;                         // Increment X register
    setFlag(Z, X == 0);          // Set Zero flag if X is 0
    setFlag(N, (X & 0x80) != 0); // Set Negative flag if bit 7 of X is set

    std::cout << "INX: X = " << std::hex << (int)X
              << ", Z flag: " << getFlag(Z)
              << ", N flag: " << getFlag(N) << std::dec << std::endl;
}

void CPU::performINY()
{
    Y++;                         // Increment Y register
    setFlag(Z, Y == 0);          // Set Zero flag if Y is 0
    setFlag(N, (Y & 0x80) != 0); // Set Negative flag if bit 7 of Y is set

    std::cout << "INY: Y = " << std::hex << (int)Y
              << ", Z flag: " << getFlag(Z)
              << ", N flag: " << getFlag(N) << std::dec << std::endl;
}

void initializeArithmeticOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{

// ===== ADC (Add with Carry) Instructions =====
#pragma region ADC Opcodes

    opcodeTable[0x69] = [](CPU &cpu)
    {
        uint8_t value = cpu.fetchByte(); // Fetch the operand
        cpu.performADC(value);
    };

    opcodeTable[0x65] = [](CPU &cpu) {       // ADC Zero Page
        uint8_t address = cpu.fetchByte();   // Fetch the zero page address
        uint8_t value = cpu.readMemory(address); // Get value from memory
        cpu.performADC(value);               // Use the helper to perform ADC operation and update flags
    };

    opcodeTable[0x75] = [](CPU &cpu) {                     // ADC Zero Page, X
        uint8_t addr = cpu.fetchByte();                    // Fetch the zero page address byte
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFF); // Get the value at addr + X (zero page wrapping)
        cpu.performADC(value);                             // Call the helper function to perform ADC and set flags
    };

    opcodeTable[0x6D] = [](CPU &cpu) {    // ADC Absolute
        uint16_t addr = cpu.fetchWord();  // Fetch the 16-bit address
        uint8_t value = cpu.readMemory(addr); // Get value from memory
        cpu.performADC(value);            // Use the helper to perform ADC operation and update flags
    };

    opcodeTable[0x7D] = [](CPU &cpu) {                       // ADC Absolute,X
        uint16_t addr = cpu.fetchWord();                     // Fetch the 16-bit address
        uint8_t value = cpu.readMemory((addr + cpu.X) & 0xFFFF); // Fetch the value at addr + X (wrap if needed)

        cpu.performADC(value); // Perform ADC using the helper
    };

    opcodeTable[0x79] = [](CPU &cpu) {                       // ADC Absolute,Y
        uint16_t addr = cpu.fetchWord();                     // Fetch the 16-bit address
        uint8_t value = cpu.readMemory((addr + cpu.Y) & 0xFFFF); // Fetch the value at addr + Y (wrap if needed)

        cpu.performADC(value); // Perform ADC using the helper
    };

    opcodeTable[0x61] = [](CPU &cpu) {                                                                       // ADC (Indirect,X)
        uint8_t addr = cpu.fetchByte();                                                                      // Fetch the base address (zero page)
        uint16_t indAddr = cpu.readMemory((addr + cpu.X) & 0xFF) | (cpu.readMemory((addr + cpu.X + 1) & 0xFF) << 8); // Fetch the 16-bit address
        uint8_t value = cpu.readMemory(indAddr);                                                                 // Fetch the value at the indirect address

        cpu.performADC(value); // Perform ADC using the helper
    };

    opcodeTable[0x71] = [](CPU &cpu) {                                     // ADC (Indirect),Y
        uint8_t addr = cpu.fetchByte();                                    // Fetch the base address (zero page)
        uint16_t indAddr = cpu.readMemory(addr) | (cpu.readMemory(addr + 1) << 8); // Fetch the 16-bit address
        uint8_t value = cpu.readMemory((indAddr + cpu.Y) & 0xFFFF);            // Fetch the value at (indAddr + Y)

        cpu.performADC(value); // Perform ADC using the helper
    };
#pragma endregion

// ===== SBC (Subtract with Carry) Instructions =====
#pragma region SBC Opcodes

    // Immediate Mode
    opcodeTable[0xE9] = [](CPU &cpu) {
        uint8_t value = cpu.fetchByte();
        cpu.performSBC(value);
    };

    // Zero Page
    opcodeTable[0xE5] = [](CPU &cpu) {
        uint8_t address = cpu.fetchByte();
        uint8_t value = cpu.readMemory(address);
        cpu.performSBC(value);
    };

    // Zero Page, X
    opcodeTable[0xF5] = [](CPU &cpu) {
        uint8_t address = (cpu.fetchByte() + cpu.X) & 0xFF;
        uint8_t value = cpu.readMemory(address);
        cpu.performSBC(value);
    };

    // Absolute
    opcodeTable[0xED] = [](CPU &cpu) {
        uint16_t address = cpu.fetchWord();
        uint8_t value = cpu.readMemory(address);
        cpu.performSBC(value);
    };

    // Absolute, X
    opcodeTable[0xFD] = [](CPU &cpu) {
        uint16_t address = cpu.fetchWord() + cpu.X;
        uint8_t value = cpu.readMemory(address & 0xFFFF);
        cpu.performSBC(value);
    };

    // Absolute, Y
    opcodeTable[0xF9] = [](CPU &cpu) {
        uint16_t address = cpu.fetchWord() + cpu.Y;
        uint8_t value = cpu.readMemory(address & 0xFFFF);
        cpu.performSBC(value);
    };

    // Indirect, X
    opcodeTable[0xE1] = [](CPU &cpu) {
        uint8_t address = (cpu.fetchByte() + cpu.X) & 0xFF;
        uint16_t effectiveAddress = cpu.readMemory(address) | (cpu.readMemory((address + 1) & 0xFF) << 8);
        uint8_t value = cpu.readMemory(effectiveAddress);
        cpu.performSBC(value);
    };

    // Indirect, Y
    opcodeTable[0xF1] = [](CPU &cpu) {
        uint8_t address = cpu.fetchByte();
        uint16_t effectiveAddress = (cpu.readMemory(address) | (cpu.readMemory((address + 1) & 0xFF) << 8)) + cpu.Y;
        uint8_t value = cpu.readMemory(effectiveAddress & 0xFFFF);
        cpu.performSBC(value);
    };
#pragma endregion


// ===== INC (Increment) Instructions =====
#pragma region INC Opcodes

    // Zero Page
    opcodeTable[0xE6] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte(); // Fetch the zero page address
        cpu.performINC(addr);           // Perform INC operation
    };

    // Zero Page, X
    opcodeTable[0xF6] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte();        // Fetch base address
        cpu.performINC((addr + cpu.X) & 0xFF); // Add X and wrap around zero page
    };

    // Absolute
    opcodeTable[0xEE] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord(); // Fetch the absolute address
        cpu.performINC(addr);            // Perform INC operation
    };

    // Absolute, X
    opcodeTable[0xFE] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord();         // Fetch the absolute base address
        cpu.performINC((addr + cpu.X) & 0xFFFF); // Add X and wrap around memory
    };

#pragma endregion

// ===== DEC Instructions =====
#pragma region DEC Opcodes

    // DEC Zero Page
    opcodeTable[0xC6] = [](CPU &cpu)
    {
        uint8_t addr = cpu.fetchByte(); // Fetch zero page address
        cpu.performDEC(addr);           // Perform DEC
    };

    // DEC Zero Page,X
    opcodeTable[0xD6] = [](CPU &cpu)
    {
        uint8_t addr = (cpu.fetchByte() + cpu.X) & 0xFF; // Fetch zero page address and add X
        cpu.performDEC(addr);                            // Perform DEC
    };

    // DEC Absolute
    opcodeTable[0xCE] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord(); // Fetch 16-bit absolute address
        cpu.performDEC(addr);            // Perform DEC
    };

    // DEC Absolute,X
    opcodeTable[0xDE] = [](CPU &cpu)
    {
        uint16_t addr = cpu.fetchWord() + cpu.X; // Fetch absolute address and add X
        cpu.performDEC(addr);                    // Perform DEC
    };
#pragma endregion

// ===== INX Instructions =====
#pragma region INX Opcodes

    opcodeTable[0xE8] = [](CPU &cpu)
    {
        cpu.performINX();
    };

#pragma endregion

// ===== DEX Instructions =====
#pragma region DEX Opcodes

    opcodeTable[0xCA] = [](CPU &cpu) { // DEX - Decrement X
        std::cout << "Executing DEX: Decrement X Register (X)" << std::endl;

        cpu.X--;                             // Decrement the X register
        cpu.setFlag(CPU::Z, cpu.X == 0);     // Set Zero flag if X == 0
        cpu.setFlag(CPU::N, (cpu.X & 0x80)); // Set Negative flag if bit 7 of X is set

        std::cout << "DEX: New X: " << std::hex << (int)cpu.X
                  << ", Z flag: " << cpu.getFlag(CPU::Z)
                  << ", N flag: " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

#pragma endregion

    // ===== INY Instructions =====
#pragma region INY Opcodes

    opcodeTable[0xC8] = [](CPU &cpu)
    {
        cpu.performINY();
    };

#pragma endregion

    // ===== DEY Instructions =====
#pragma region DEY Opcodes

    opcodeTable[0x88] = [](CPU &cpu) { // DEY - Decrement Y
        std::cout << "Executing DEY: Decrement Y Register (Y)" << std::endl;

        cpu.Y--;                             // Decrement the Y register
        cpu.setFlag(CPU::Z, cpu.Y == 0);     // Set Zero flag if Y == 0
        cpu.setFlag(CPU::N, (cpu.Y & 0x80)); // Set Negative flag if bit 7 of Y is set

        std::cout << "DEY: New Y: " << std::hex << (int)cpu.Y
                  << ", Z flag: " << cpu.getFlag(CPU::Z)
                  << ", N flag: " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };
#pragma endregion
}