#include "cpu.h"
#include <fstream>
#include <iostream>

void CPU::performASL(uint8_t &value)
{
    bool oldCarry = (value & 0x80) != 0;
    value <<= 1;
    setFlag(C, oldCarry);
    setFlag(Z, value == 0);
    setFlag(N, value & 0x80);
}

void CPU::performLSR(uint8_t &value)
{
    //std::cout << "LSR: Initial value = " << std::hex << (int)value << std::dec << std::endl;
    setFlag(CPU::C, value & 0x01); // Set carry to bit 0
    value >>= 1;                   // Shift right by 1
    setFlag(CPU::Z, value == 0);   // Set zero flag if result is zero
    setFlag(CPU::N, false);        // Clear negative flag (always 0 after LSR)
    // std::cout << "LSR: Result = " << std::hex << (int)value
    //           << ", Carry = " << getFlag(CPU::C)
    //           << ", Zero = " << getFlag(CPU::Z)
    //           << ", Negative = " << getFlag(CPU::N) << std::dec << std::endl;
}

void CPU::performLSRMemory(uint16_t address)
{
    uint8_t value = memory[address];
    performLSR(value);
    memory[address] = value; // Write the modified value back to memory
}

void initializeShiftOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== ASL Instructions =====
#pragma region ASL Opcodes

    opcodeTable[0x0A] = [](CPU &cpu)
    {
        cpu.performASL(cpu.A);
    };

    // ASL Zero Page (opcode 0x06)
    opcodeTable[0x06] = [](CPU &cpu)
    {
        uint8_t address = cpu.fetchByte();
        uint8_t value = cpu.readMemory(address);
        cpu.performASL(value);
        cpu.writeMemory(address, value);
    };

    // ASL Zero Page, X (opcode 0x16)
    opcodeTable[0x16] = [](CPU &cpu)
    {
        uint8_t address = cpu.fetchByte();
        uint8_t value = cpu.readMemory((address + cpu.X) & 0xFF);
        cpu.performASL(value);
        cpu.writeMemory((address + cpu.X) & 0xFF, value);
    };

    // ASL Absolute (opcode 0x0E)
    opcodeTable[0x0E] = [](CPU &cpu)
    {
        uint16_t address = cpu.fetchWord();
        uint8_t value = cpu.readMemory(address);
        cpu.performASL(value);
        cpu.writeMemory(address, value);
    };

    // ASL Absolute, X (opcode 0x1E)
    opcodeTable[0x1E] = [](CPU &cpu)
    {
        uint16_t address = cpu.fetchWord();
        uint8_t value = cpu.readMemory((address + cpu.X) & 0xFFFF);
        cpu.performASL(value);
        cpu.writeMemory((address + cpu.X) & 0xFFFF, value);
    };
#pragma endregion

    // ===== LSR Instructions =====
#pragma region LSR Opcodes

    opcodeTable[0x4A] = [](CPU &cpu) { // LSR Accumulator
        cpu.performLSR(cpu.A);
    };

    opcodeTable[0x46] = [](CPU &cpu) { // LSR Zero Page
        uint8_t address = cpu.fetchByte();
        cpu.performLSRMemory(address);
    };

    opcodeTable[0x56] = [](CPU &cpu) { // LSR Zero Page,X
        uint8_t baseAddress = cpu.fetchByte();
        uint8_t address = (baseAddress + cpu.X) & 0xFF; // Wrap around in zero page
        cpu.performLSRMemory(address);
    };

    opcodeTable[0x4E] = [](CPU &cpu) { // LSR Absolute
        uint16_t address = cpu.fetchWord();
        cpu.performLSRMemory(address);
    };

    opcodeTable[0x5E] = [](CPU &cpu) { // LSR Absolute,X
        uint16_t baseAddress = cpu.fetchWord();
        uint16_t address = baseAddress + cpu.X;
        cpu.performLSRMemory(address & 0xFFFF); // Wrap around for 16-bit address
    };
#pragma endregion

    // ===== ROL Instructions =====
#pragma region ROL Opcodes

    opcodeTable[0x2A] = [](CPU &cpu) { // ROL - Accumulator Mode
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 1 : 0;
        uint8_t result = (cpu.A << 1) | oldCarry;

        cpu.setFlag(CPU::C, cpu.A & 0x80);  // Carry = Bit 7
        cpu.setFlag(CPU::Z, result == 0);   // Zero
        cpu.setFlag(CPU::N, result & 0x80); // Negative

        cpu.A = result;

        // std::cout << "ROL Accumulator: A = " << std::hex << (int)cpu.A
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x26] = [](CPU &cpu) { // ROL - Zero Page
        uint8_t addr = cpu.fetchByte();
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 1 : 0;
        uint8_t result = (oldValue << 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x80); // Carry = Bit 7
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROL Zero Page: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x36] = [](CPU &cpu) { // ROL - Zero Page, X
        uint8_t addr = (cpu.fetchByte() + cpu.X) & 0xFF;
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 1 : 0;
        uint8_t result = (oldValue << 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x80); // Carry = Bit 7
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROL Zero Page, X: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x2E] = [](CPU &cpu) { // ROL - Absolute
        uint16_t addr = cpu.fetchWord();
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 1 : 0;
        uint8_t result = (oldValue << 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x80); // Carry = Bit 7
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROL Absolute: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x3E] = [](CPU &cpu) { // ROL - Absolute, X
        uint16_t addr = cpu.fetchWord() + cpu.X;
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 1 : 0;
        uint8_t result = (oldValue << 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x80); // Carry = Bit 7
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROL Absolute, X: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

#pragma endregion

// ===== ROR Instructions =====
#pragma region ROR Opcodes

    opcodeTable[0x6A] = [](CPU &cpu) { // ROR - Accumulator Mode
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 0x80 : 0x00;
        uint8_t result = (cpu.A >> 1) | oldCarry;

        cpu.setFlag(CPU::C, cpu.A & 0x01);  // Carry = Bit 0
        cpu.setFlag(CPU::Z, result == 0);   // Zero
        cpu.setFlag(CPU::N, result & 0x80); // Negative

        cpu.A = result;

        // std::cout << "ROR Accumulator: A = " << std::hex << (int)cpu.A
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x66] = [](CPU &cpu) { // ROR - Zero Page
        uint8_t addr = cpu.fetchByte();
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 0x80 : 0x00;
        uint8_t result = (oldValue >> 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x01); // Carry = Bit 0
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROR Zero Page: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x76] = [](CPU &cpu) { // ROR - Zero Page, X
        uint8_t addr = (cpu.fetchByte() + cpu.X) & 0xFF;
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 0x80 : 0x00;
        uint8_t result = (oldValue >> 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x01); // Carry = Bit 0
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROR Zero Page, X: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x6E] = [](CPU &cpu) { // ROR - Absolute
        uint16_t addr = cpu.fetchWord();
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 0x80 : 0x00;
        uint8_t result = (oldValue >> 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x01); // Carry = Bit 0
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROR Absolute: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };

    opcodeTable[0x7E] = [](CPU &cpu) { // ROR - Absolute, X
        uint16_t addr = cpu.fetchWord() + cpu.X;
        uint8_t oldValue = cpu.readMemory(addr);
        uint8_t oldCarry = cpu.getFlag(CPU::C) ? 0x80 : 0x00;
        uint8_t result = (oldValue >> 1) | oldCarry;

        cpu.setFlag(CPU::C, oldValue & 0x01); // Carry = Bit 0
        cpu.setFlag(CPU::Z, result == 0);     // Zero
        cpu.setFlag(CPU::N, result & 0x80);   // Negative

        cpu.writeMemory(addr, result);

        // std::cout << "ROR Absolute, X: Address = " << std::hex << (int)addr
        //           << ", Value = " << (int)result
        //           << ", Carry = " << cpu.getFlag(CPU::C)
        //           << ", Zero = " << cpu.getFlag(CPU::Z)
        //           << ", Negative = " << cpu.getFlag(CPU::N) << std::dec << std::endl;
    };
#pragma endregion
}
