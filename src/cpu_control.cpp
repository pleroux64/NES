#include "cpu.h"
#include <fstream>
#include <iostream>

void CPU::performJMP(uint16_t address)
{
    PC = address;
    std::cout << "JMP: New PC = " << std::hex << PC << std::dec << std::endl;
}

void CPU::debugStack() {
    for (int i = 0xFF; i >= SP; --i) {
        std::cerr << "Stack[" << std::hex << i << "]: " << std::hex << (int)memory[0x0100 + i] << "\n";
    }
}


void CPU::performJSR(uint16_t address)
{
    uint16_t returnAddress = PC - 1; // Address to return to (JSR stores the address minus 1)

    // Push low byte of return address
    memory[0x0100 + SP] = returnAddress & 0xFF;
    SP--;

    // Push high byte of return address
    memory[0x0100 + SP] = (returnAddress >> 8) & 0xFF;
    SP--;

    // Set PC to new address
    PC = address;

    std::cout << "JSR: Pushed return address: Low byte = "
              << std::hex << (returnAddress & 0xFF)
              << ", High byte = " << ((returnAddress >> 8) & 0xFF)
              << ", New PC: " << PC << std::dec << std::endl;
}

void CPU::performRTI()
{
    // Pull processor flags (P) from stack
    SP++;
    P = memory[0x0100 + SP];

    // Pull low and high bytes of PC from stack
    SP++;
    uint8_t pcl = memory[0x0100 + SP];
    SP++;
    uint8_t pch = memory[0x0100 + SP];

    // Combine the bytes to form the full PC
    PC = static_cast<uint16_t>(pch) << 8 | pcl;

    // Debug output for verification
    std::cout << "RTI: Restored PC = " << std::hex << PC
              << ", P = " << std::hex << static_cast<int>(P)
              << ", Flags: "
              << "C=" << getFlag(CPU::C)
              << " Z=" << getFlag(CPU::Z)
              << " I=" << getFlag(CPU::I)
              << " D=" << getFlag(CPU::D)
              << " V=" << getFlag(CPU::V)
              << " N=" << getFlag(CPU::N) << "\n";
}
void CPU::pushToStack(uint8_t value) {
    // Confirm the value being pushed
    std::cerr << "[Stack Debug] Value to Push: 0x" 
              << std::hex << static_cast<int>(value)
              << " to Address: 0x" << (0x0100 + SP)
              << ", SP Before: 0x" << static_cast<int>(SP) 
              << std::endl;

    // Write value to stack memory
    memory[0x0100 + SP] = value;

    // Validate the write
    uint8_t writtenValue = memory[0x0100 + SP];
    if (writtenValue != value) {
        std::cerr << "[Error] Stack Write Mismatch: Expected 0x"
                  << std::hex << static_cast<int>(value) 
                  << ", Found 0x" << static_cast<int>(writtenValue) << std::endl;
    }

    // Decrement SP with wrap-around
    SP = (SP - 1) & 0xFF;

    // Confirm SP after decrement
    std::cerr << "[Stack Debug] SP After Decrement: 0x" 
              << std::hex << static_cast<int>(SP) << std::endl;
}



uint8_t CPU::popFromStack() {
    SP = (SP + 1) & 0xFF;        // Increment SP with wrap-around (0x00 to 0xFF)
    return memory[0x0100 + SP];  // Stack is located at $0100-$01FF
}





void initializeControlOpcodes(std::unordered_map<uint8_t, std::function<void(CPU &)>> &opcodeTable)
{
    // ===== JMP Instructions =====
#pragma region JMP Opcodes

    // JMP Absolute (Opcode 0x4C)
    opcodeTable[0x4C] = [](CPU &cpu)
    {
        uint16_t address = cpu.fetchWord(); // Fetch the 16-bit address
        cpu.performJMP(address);
    };

    // JMP Indirect (Opcode 0x6C)
    opcodeTable[0x6C] = [](CPU &cpu)
    {
        uint16_t pointer = cpu.fetchWord();     // Fetch the pointer address
        uint16_t lowByte = cpu.memory[pointer]; // Fetch low byte
        uint16_t highByte;

        // Handle the CPU bug for page crossing
        if ((pointer & 0x00FF) == 0x00FF)
        {
            highByte = cpu.memory[pointer & 0xFF00]; // Wrap around the page
        }
        else
        {
            highByte = cpu.memory[pointer + 1]; // Fetch high byte
        }

        uint16_t address = lowByte | (highByte << 8);
        cpu.performJMP(address);
    };

#pragma endregion

// ===== JSR Instructions =====
#pragma region JSR Opcodes

    // JSR Absolute (Opcode 0x20)
    opcodeTable[0x20] = [](CPU &cpu)
    {
        uint16_t address = cpu.fetchWord(); // Fetch the new address
        cpu.performJSR(address);            // Perform the JSR operation
    };

#pragma endregion

// ===== RTS Instructions =====
#pragma region RTS Opcodes

opcodeTable[0x60] = [](CPU &cpu) {
    // Debug: Initial stack state
    std::cerr << "[RTS Debug] Initial Stack State:\n";
    cpu.debugStack();

    // Pull program counter from the stack
    uint8_t pcl = cpu.popFromStack();
    uint8_t pch = cpu.popFromStack();
    cpu.PC = (pch << 8) | pcl;

    // Increment the PC by 1
    cpu.PC++;

    // Debug: Final state
    std::cerr << "[RTS Debug] Restored PC: " << std::hex << cpu.PC << "\n";
};

#pragma endregion

// ===== BRK Instructions =====
#pragma region BRK Opcodes

   opcodeTable[0x00] = [](CPU &cpu) {
    // Calculate return address (PC + 1, since BRK is considered 2 bytes)
    uint16_t returnAddress = cpu.PC + 1;

    // Push return address to the stack
    cpu.pushToStack((returnAddress >> 8) & 0xFF); // High byte of return address
    cpu.pushToStack(returnAddress & 0xFF);        // Low byte of return address
    std::cout << "[BRK Debug] Return Address Pushed: High = " 
              << std::hex << ((returnAddress >> 8) & 0xFF) 
              << ", Low = " << (returnAddress & 0xFF) << std::endl;

    // Push processor flags to the stack (Break flag set)
    uint8_t flags = cpu.P | 0x10; // Set Break (B) flag
    cpu.pushToStack(flags);
    std::cout << "[BRK Debug] Flags Pushed to Stack: " << std::bitset<8>(flags) << std::endl;

    // Set the Interrupt Disable flag
    cpu.setFlag(CPU::I, true);

    // Fetch the IRQ/BRK handler address from $FFFE/$FFFF
    uint16_t handlerAddress = cpu.memory[0xFFFE] | (cpu.memory[0xFFFF] << 8);
    std::cout << "[BRK Debug] Handler Address Loaded: " << std::hex << handlerAddress << std::endl;

    // Jump to the handler
    cpu.PC = handlerAddress;

    // Debug output for verification
    std::cout << "[BRK Debug] PC = " << std::hex << returnAddress
              << ", Handler Address = " << handlerAddress
              << ", Flags = " << std::bitset<8>(flags)
              << std::endl;
};



#pragma endregion

// ===== RTI Instructions =====
#pragma region RTI Opcodes

  opcodeTable[0x40] = [](CPU &cpu) {
    std::cerr << "[RTI Debug] Initial Stack State:\n";
    cpu.debugStack();

    // Pull flags
    uint8_t flags = cpu.popFromStack();
    cpu.P = flags;
    std::cerr << "[RTI Debug] Restored Flags: " << std::bitset<8>(flags) << "\n";

    // Debugging specific flags
    std::cerr << "[Debug] P Register After Restoration: " << std::bitset<8>(cpu.P) << "\n";
    std::cerr << "[Debug] V Flag (bit 6): " << cpu.getFlag(CPU::V) << "\n";

    // Pull program counter
    uint8_t pcl = cpu.popFromStack();
    uint8_t pch = cpu.popFromStack();
    cpu.PC = (pch << 8) | pcl;
    std::cerr << "[RTI Debug] Restored PC: " << std::hex << cpu.PC << "\n";

    std::cerr << "[RTI Debug] Final Stack State:\n";
    cpu.debugStack();
};





#pragma endregion
}