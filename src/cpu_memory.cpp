#include "cpu.h"
#include <fstream>
#include <iostream>

void CPU::updateLDAFlags()
{
    setFlag(Z, A == 0);   // Set Zero flag if A == 0
    setFlag(N, A & 0x80); // Set Negative flag if the most significant bit of A is set
}

void initializeMemoryOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== LDA Instructions =====
#pragma region LDA Opcodes

    // LDA - Load A (Accumulator)
    opcodeTable[0xA9] = [](CPU &cpu) { // Immediate
        cpu.A = cpu.fetchByte();       // Load the immediate value into A
        cpu.updateLDAFlags();          // Update flags
    };

    opcodeTable[0xA5] = [](CPU &cpu) {  // Zero Page
        uint8_t addr = cpu.fetchByte(); // Fetch the zero-page address
        cpu.A = cpu.memory[addr];       // Load the value from memory into A
        cpu.updateLDAFlags();           // Update flags
    };

    opcodeTable[0xB5] = [](CPU &cpu) {                   // Zero Page,X
        uint8_t addr = (cpu.fetchByte() + cpu.X) & 0xFF; // Fetch the zero-page address and add X (wrap around at 0xFF)
        cpu.A = cpu.memory[addr];                        // Load the value into A
        cpu.updateLDAFlags();                            // Update flags
    };

    opcodeTable[0xAD] = [](CPU &cpu) {   // Absolute
        uint16_t addr = cpu.fetchWord(); // Fetch the absolute address
        cpu.A = cpu.memory[addr];        // Load the value into A
        cpu.updateLDAFlags();            // Update flags
    };

    opcodeTable[0xBD] = [](CPU &cpu) {           // Absolute,X
        uint16_t addr = cpu.fetchWord() + cpu.X; // Add X to the absolute address
        cpu.A = cpu.memory[addr];                // Load the value into A
        cpu.updateLDAFlags();                    // Update flags
    };

    opcodeTable[0xB9] = [](CPU &cpu) {           // Absolute,Y
        uint16_t addr = cpu.fetchWord() + cpu.Y; // Add Y to the absolute address
        cpu.A = cpu.memory[addr];                // Load the value into A
        cpu.updateLDAFlags();                    // Update flags
    };

    opcodeTable[0xA1] = [](CPU &cpu) {                                          // (Indirect,X)
        uint8_t baseAddr = (cpu.fetchByte() + cpu.X) & 0xFF;                    // Add X to the zero-page address
        uint16_t addr = cpu.memory[baseAddr] | (cpu.memory[baseAddr + 1] << 8); // Fetch the 16-bit address
        cpu.A = cpu.memory[addr];                                               // Load the value into A
        cpu.updateLDAFlags();                                                   // Update flags
    };

    opcodeTable[0xB1] = [](CPU &cpu) {                                          // (Indirect),Y
        uint8_t baseAddr = cpu.fetchByte();                                     // Fetch the base zero-page address
        uint16_t addr = cpu.memory[baseAddr] | (cpu.memory[baseAddr + 1] << 8); // Fetch the 16-bit address
        addr += cpu.Y;                                                          // Add Y offset
        cpu.A = cpu.memory[addr];                                               // Load the value into A
        cpu.updateLDAFlags();                                                   // Update flags
    };
#pragma endregion

// ===== STA Instructions =====
#pragma region STA Opcodes
    opcodeTable[0x85] = [](CPU &cpu) {     // STA Zero Page
        uint8_t address = cpu.fetchByte(); // Fetch the zero-page address
        cpu.memory[address] = cpu.A;       // Store A into memory
        std::cout << "STA Zero Page: A = " << std::hex << (int)cpu.A << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x95] = [](CPU &cpu) {                      // STA Zero Page,X
        uint8_t address = (cpu.fetchByte() + cpu.X) & 0xFF; // Add X and wrap around at 0xFF
        cpu.memory[address] = cpu.A;                        // Store A into memory
        std::cout << "STA Zero Page,X: A = " << std::hex << (int)cpu.A << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x8D] = [](CPU &cpu) {      // STA Absolute
        uint16_t address = cpu.fetchWord(); // Fetch the 16-bit absolute address
        cpu.memory[address] = cpu.A;        // Store A into memory
        std::cout << "STA Absolute: A = " << std::hex << (int)cpu.A << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x9D] = [](CPU &cpu) {              // STA Absolute,X
        uint16_t address = cpu.fetchWord() + cpu.X; // Add X to the absolute address
        cpu.memory[address] = cpu.A;                // Store A into memory
        std::cout << "STA Absolute,X: A = " << std::hex << (int)cpu.A << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x99] = [](CPU &cpu) {              // STA Absolute,Y
        uint16_t address = cpu.fetchWord() + cpu.Y; // Add Y to the absolute address
        cpu.memory[address] = cpu.A;                // Store A into memory
        std::cout << "STA Absolute,Y: A = " << std::hex << (int)cpu.A << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x81] = [](CPU &cpu) {                                                            // STA (Indirect,X)
        uint8_t baseAddress = (cpu.fetchByte() + cpu.X) & 0xFF;                                   // Base address in zero-page
        uint16_t address = cpu.memory[baseAddress] | (cpu.memory[(baseAddress + 1) & 0xFF] << 8); // Fetch 16-bit address
        cpu.memory[address] = cpu.A;                                                              // Store A into memory
        std::cout << "STA (Indirect,X): A = " << std::hex << (int)cpu.A << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x91] = [](CPU &cpu) {                                                                      // STA (Indirect),Y
        uint8_t baseAddress = cpu.fetchByte();                                                              // Fetch base address in zero-page
        uint16_t address = (cpu.memory[baseAddress] | (cpu.memory[(baseAddress + 1) & 0xFF] << 8)) + cpu.Y; // Fetch address and add Y
        cpu.memory[address] = cpu.A;                                                                        // Store A into memory
        std::cout << "STA (Indirect),Y: A = " << std::hex << (int)cpu.A << ", Address = " << (int)address << std::endl;
    };

#pragma endregion

// ===== LDX Instructions =====
#pragma region LDX Opcodes

    opcodeTable[0xA2] = [](CPU &cpu) { // LDX Immediate
        uint8_t value = cpu.fetchByte();
        cpu.X = value;
        cpu.setFlag(CPU::Z, cpu.X == 0);
        cpu.setFlag(CPU::N, cpu.X & 0x80);
    };

    opcodeTable[0xA6] = [](CPU &cpu) { // LDX Zero Page
        uint8_t address = cpu.fetchByte();
        uint8_t value = cpu.memory[address];
        cpu.X = value;
        cpu.setFlag(CPU::Z, cpu.X == 0);
        cpu.setFlag(CPU::N, cpu.X & 0x80);
    };

    opcodeTable[0xB6] = [](CPU &cpu) { // LDX Zero Page,Y
        uint8_t baseAddress = cpu.fetchByte();
        uint8_t address = (baseAddress + cpu.Y) & 0xFF; // Wrap around in zero page
        uint8_t value = cpu.memory[address];
        cpu.X = value;
        cpu.setFlag(CPU::Z, cpu.X == 0);
        cpu.setFlag(CPU::N, cpu.X & 0x80);
    };

    opcodeTable[0xAE] = [](CPU &cpu) { // LDX Absolute
        uint16_t address = cpu.fetchWord();
        uint8_t value = cpu.memory[address];
        cpu.X = value;
        cpu.setFlag(CPU::Z, cpu.X == 0);
        cpu.setFlag(CPU::N, cpu.X & 0x80);
    };

    opcodeTable[0xBE] = [](CPU &cpu) { // LDX Absolute,Y
        uint16_t baseAddress = cpu.fetchWord();
        uint16_t address = baseAddress + cpu.Y;
        uint8_t value = cpu.memory[address & 0xFFFF]; // Ensure wrap-around for 16-bit addresses
        cpu.X = value;
        cpu.setFlag(CPU::Z, cpu.X == 0);
        cpu.setFlag(CPU::N, cpu.X & 0x80);
    };
#pragma endregion

// ===== STX Instructions =====
#pragma region STX Opcodes
    opcodeTable[0x86] = [](CPU &cpu) {     // STX Zero Page
        uint8_t address = cpu.fetchByte(); // Fetch the zero-page address
        cpu.memory[address] = cpu.X;       // Store X into memory
        std::cout << "STX Zero Page: X = " << std::hex << (int)cpu.X << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x96] = [](CPU &cpu) {                      // STX Zero Page,Y
        uint8_t address = (cpu.fetchByte() + cpu.Y) & 0xFF; // Add Y and wrap around at 0xFF
        cpu.memory[address] = cpu.X;                        // Store X into memory
        std::cout << "STX Zero Page,Y: X = " << std::hex << (int)cpu.X << ", Address = " << (int)address << std::endl;
    };

    opcodeTable[0x8E] = [](CPU &cpu) {      // STX Absolute
        uint16_t address = cpu.fetchWord(); // Fetch the 16-bit absolute address
        cpu.memory[address] = cpu.X;        // Store X into memory
        std::cout << "STX Absolute: X = " << std::hex << (int)cpu.X << ", Address = " << (int)address << std::endl;
    };

#pragma endregion

// ===== LDY Instructions =====
#pragma region LDY Opcodes

    opcodeTable[0xA0] = [](CPU &cpu) { // LDY Immediate
        uint8_t value = cpu.fetchByte();
        cpu.Y = value;
        cpu.setFlag(CPU::Z, cpu.Y == 0);
        cpu.setFlag(CPU::N, cpu.Y & 0x80);
    };

    opcodeTable[0xA4] = [](CPU &cpu) { // LDY Zero Page
        uint8_t address = cpu.fetchByte();
        uint8_t value = cpu.memory[address];
        cpu.Y = value;
        cpu.setFlag(CPU::Z, cpu.Y == 0);
        cpu.setFlag(CPU::N, cpu.Y & 0x80);
    };

    opcodeTable[0xB4] = [](CPU &cpu) { // LDY Zero Page,X
        uint8_t baseAddress = cpu.fetchByte();
        uint8_t address = (baseAddress + cpu.X) & 0xFF; // Wrap around in zero page
        uint8_t value = cpu.memory[address];
        cpu.Y = value;
        cpu.setFlag(CPU::Z, cpu.Y == 0);
        cpu.setFlag(CPU::N, cpu.Y & 0x80);
    };

    opcodeTable[0xAC] = [](CPU &cpu) { // LDY Absolute
        uint16_t address = cpu.fetchWord();
        uint8_t value = cpu.memory[address];
        cpu.Y = value;
        cpu.setFlag(CPU::Z, cpu.Y == 0);
        cpu.setFlag(CPU::N, cpu.Y & 0x80);
    };

    opcodeTable[0xBC] = [](CPU &cpu) { // LDY Absolute,X
        uint16_t baseAddress = cpu.fetchWord();
        uint16_t address = baseAddress + cpu.X;
        uint8_t value = cpu.memory[address & 0xFFFF]; // Ensure wrap-around for 16-bit addresses
        cpu.Y = value;
        cpu.setFlag(CPU::Z, cpu.Y == 0);
        cpu.setFlag(CPU::N, cpu.Y & 0x80);
    };
#pragma endregion

// ===== STY Instructions =====
#pragma region STY Opcodes
opcodeTable[0x84] = [](CPU &cpu) { // STY Zero Page
    uint8_t address = cpu.fetchByte();   // Fetch the zero-page address
    cpu.memory[address] = cpu.Y;         // Store Y into memory
    std::cout << "STY Zero Page: Y = " << std::hex << (int)cpu.Y << ", Address = " << (int)address << std::endl;
};

opcodeTable[0x94] = [](CPU &cpu) { // STY Zero Page,X
    uint8_t address = (cpu.fetchByte() + cpu.X) & 0xFF; // Add X and wrap around at 0xFF
    cpu.memory[address] = cpu.Y;                        // Store Y into memory
    std::cout << "STY Zero Page,X: Y = " << std::hex << (int)cpu.Y << ", Address = " << (int)address << std::endl;
};

opcodeTable[0x8C] = [](CPU &cpu) { // STY Absolute
    uint16_t address = cpu.fetchWord();  // Fetch the 16-bit absolute address
    cpu.memory[address] = cpu.Y;         // Store Y into memory
    std::cout << "STY Absolute: Y = " << std::hex << (int)cpu.Y << ", Address = " << (int)address << std::endl;
};
#pragma endregion
}