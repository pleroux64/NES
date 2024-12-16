#include "cpu.h"
#include "opcode_cycles.h"
#include "cycle_exceptions.h"

#include <iostream>
#include <fstream>



// Reset the CPU to its initial state
void CPU::reset()
{
    // Fetch the reset vector (stored at 0xFFFC and 0xFFFD)
    uint8_t resetLow = memory[0xFFFC];
    uint8_t resetHigh = memory[0xFFFD];
    uint16_t resetVector = resetLow | (resetHigh << 8);

    PC = resetVector;  // Set PC to the address from the reset vector
    SP = 0xFF;
    A = X = Y = P = 0; // Reset registers and status flags
    initializeOpcodeTable(); // Ensure opcode table is initialized
    cycles = 0;

    // Debug outputs
    std::cerr << "[Debug] CPU Reset: Memory[0xFFFC] = 0x" 
              << std::hex << static_cast<int>(resetLow) << std::endl;
    std::cerr << "[Debug] CPU Reset: Memory[0xFFFD] = 0x" 
              << std::hex << static_cast<int>(resetHigh) << std::endl;
    std::cerr << "[Debug] CPU Reset: PC set to 0x" << std::hex << PC << std::endl;
}
// Load a ROM into memory starting at address 0x8000
void CPU::loadROM(const std::string &filename)
{
    std::ifstream romFile(filename, std::ios::binary);
    if (!romFile)
    {
        std::cerr << "Failed to open ROM file: " << filename << std::endl;
        return;
    }

    // Load up to 32KB of ROM into memory at 0x8000
    romFile.read(reinterpret_cast<char *>(&memory[0x8000]), 0x8000);
    std::cout << "ROM loaded successfully: " << filename << std::endl;
}

// Fetch the next byte and increment the program counter
uint8_t CPU::fetchByte()
{
    return memory[PC++];
}

// Fetch the next two bytes as a 16-bit word
uint16_t CPU::fetchWord()
{
    uint16_t word = memory[PC] | (memory[PC + 1] << 8);
    PC += 2;
    return word;
}

// Set a status flag in the P register
void CPU::setFlag(uint8_t flag, bool value)
{
    if (value)
        P |= (1 << flag);
    else
        P &= ~(1 << flag);
}

// Get the value of a status flag in the P register
bool CPU::getFlag(uint8_t flag) {
    bool result = (P & (1 << flag)) != 0;
    std::cerr << "[Debug] getFlag(" << static_cast<int>(flag)
              << "): P = " << std::bitset<8>(P)
              << ", Bit Values: [N=" << ((P & 0x80) != 0)
              << ", V=" << ((P & 0x40) != 0)
              << ", ...]"
              << ", (1 << flag) = " << std::bitset<8>(1 << flag)
              << ", Result = " << result << "\n";
    return result;
}

void CPU::addCycles(int cycles) {
    this->cycles += cycles;
}


std::function<void(CPU&)> withBaseCycles(uint8_t opcode, int baseCycles, std::function<void(CPU&)> handler) {
    return [opcode, baseCycles, handler](CPU& cpu) {
        // Add base cycles
        cpu.addCycles(baseCycles);
        // Execute the original handler
        handler(cpu);

        // Debugging output (optional)
        std::cerr << "Opcode 0x" << std::hex << static_cast<int>(opcode)
                  << " executed. Base Cycles: " << baseCycles
                  << ", Total Cycles: " << cpu.cycles << std::dec << '\n';
    };
}

void addCycleLogic(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable,
                   const std::unordered_map<uint8_t, int>& opcodeCycles,
                   const std::unordered_map<uint8_t, std::function<int(CPU&)>>& cycleExceptions) {
    for (const auto& [opcode, baseCycles] : opcodeCycles) {
        const uint8_t capturedOpcode = opcode;
        const int capturedBaseCycles = baseCycles;

        if (opcodeTable.count(opcode)) {
            auto originalHandler = opcodeTable[opcode];

            // Wrap the handler with cycle logic
            opcodeTable[opcode] = [capturedOpcode, capturedBaseCycles, originalHandler, &cycleExceptions](CPU& cpu) {
                cpu.addCycles(capturedBaseCycles); // Add base cycles
                originalHandler(cpu);              // Call the original handler logic

                // Dynamically find the exception handler
                auto exceptionIt = cycleExceptions.find(capturedOpcode);
                if (exceptionIt != cycleExceptions.end()) {
                    int extraCycles = exceptionIt->second(cpu); // Call the exception handler
                    cpu.addCycles(extraCycles);                // Add the extra cycles
                }
            };
        } else {
            std::cerr << "Warning: No handler defined for opcode 0x"
                      << std::hex << static_cast<int>(opcode) << '\n';
        }
    }
}






// Execute a single instruction
void CPU::execute()
{
    uint8_t opcode = fetchByte();
    if (opcodeTable.count(opcode))
    {
        opcodeTable[opcode](*this); // Execute the corresponding function
    }
    else
    {
        std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << std::dec << std::endl;
    }
}



// Initialize the opcode table
void CPU::initializeOpcodeTable()
{
    initializeArithmeticOpcodes(opcodeTable);
    initializeBitwiseOpcodes(opcodeTable);
    initializeBranchOpcodes(opcodeTable);
    initializeShiftOpcodes(opcodeTable);
    initializeControlOpcodes(opcodeTable);
    initializeMiscOpcodes(opcodeTable);
    initializeComparisonOpcodes(opcodeTable);
    initializeMemoryOpcodes(opcodeTable);
    initializeTransferOpcodes(opcodeTable);
    initializeStackOpcodes(opcodeTable);
    initializeFlagsOpcodes(opcodeTable);

    addCycleLogic(opcodeTable, opcodeCycles, cycleExceptions);


}