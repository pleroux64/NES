#include "cpu.h"
#include "ppu.h" 
#include "opcode_cycles.h"
#include "cycle_exceptions.h"

#include <iostream>
#include <fstream>




// Reset the CPU to its initial state
void CPU::reset()
{
    // Fetch the RESET vector from memory
    uint8_t resetLow = readMemory(0xFFFC);  // Low byte
    uint8_t resetHigh = readMemory(0xFFFD); // High byte

    // Combine the two bytes to form the address
    PC = (resetHigh << 8) | resetLow;

    // Debugging log for verification
    std::cerr << "[CPU Debug] PC set to RESET vector: 0x" << std::hex << PC << std::endl;
    SP = 0xFF;
    A = X = Y = P = 0;
    initializeOpcodeTable(); // Ensure opcode table is initialized
    cycles = 0;

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
    return readMemory(PC++);
}

// Fetch the next two bytes as a 16-bit word
uint16_t CPU::fetchWord()
{
    uint16_t word = readMemory(PC) | (readMemory(PC + 1) << 8);
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

void CPU::printMemory(uint16_t start, uint16_t end) {
    for (uint16_t addr = start; addr <= end; ++addr) {
        std::cout << "Memory[0x" << std::hex << addr << "] = 0x"
                  << std::hex << static_cast<int>(memory[addr]) << std::endl;
    }
}

void CPU::dumpMemoryToConsole(uint16_t start, uint16_t end) {
    for (uint16_t addr = start; addr <= end; addr++) {
        std::cout << "Memory[0x" << std::hex << addr << "] = 0x" 
                  << std::hex << (int)memory[addr] << std::endl;
    }
}

void CPU::handleUndefinedOpcode(uint8_t opcode) {
    std::cerr << "[Error] Undefined opcode encountered: 0x" 
              << std::hex << static_cast<int>(opcode) << " at PC: 0x"
              << std::hex << PC << std::endl;

    // Skip the undefined opcode
    PC++; 

    // Add optional logging or error handling here, if needed
}

void CPU::addCycles(int cycles) {
    this->cycles += cycles;
}

void CPU::requestNMI() {
    nmiRequested = true;
}

void CPU::handleNMI() {
    std::cerr << "[NMI Debug] NMI Triggered. Starting NMI handler..." << std::endl;

    // Push the current PC to the stack (high byte first)
    uint8_t highByte = (PC >> 8) & 0xFF;
    uint8_t lowByte = PC & 0xFF;

    pushToStack(highByte);
    pushToStack(lowByte);

    std::cerr << "[NMI Debug] PC Pushed to Stack: High = 0x" 
              << std::hex << static_cast<int>(highByte)
              << ", Low = 0x" << static_cast<int>(lowByte) << std::endl;

    // Push the status register with Break cleared and Unused set
    uint8_t flagsToPush = (P & ~0x10) | 0x20; // Clear Break flag, set Unused bit
    pushToStack(flagsToPush);

    std::cerr << "[NMI Debug] Status Register Pushed: 0b" 
              << std::bitset<8>(flagsToPush) << std::endl;

    // Set Interrupt Disable flag (bit 2)
    setFlag(I, true); // Prevent further interrupts during NMI handling
    std::cerr << "[NMI Debug] Interrupt Disable Flag Set." << std::endl;

    // Fetch NMI vector address from memory at 0xFFFA (low byte) and 0xFFFB (high byte)
    uint8_t vectorLow = readMemory(0xFFFA);
    uint8_t vectorHigh = readMemory(0xFFFB);
    PC = (vectorHigh << 8) | vectorLow;

    std::cerr << "[NMI Debug] NMI Vector Loaded: Low = 0x" 
              << std::hex << static_cast<int>(vectorLow)
              << ", High = 0x" << static_cast<int>(vectorHigh)
              << ", New PC = 0x" << PC << std::endl;

    // Add NMI handling cycles (7 cycles for NMI as per NES specs)
    addCycles(7);
    std::cerr << "[NMI Debug] 7 Cycles Added for NMI Handling. Total Cycles: " 
              << cycles << std::endl;
}

void CPU::debugNMIVector() {
    uint8_t vectorLow = memory[0xFFFA];
    uint8_t vectorHigh = memory[0xFFFB];
    std::cerr << "[Debug] NMI Vector: Low = 0x" << std::hex 
              << static_cast<int>(vectorLow) << ", High = 0x" 
              << static_cast<int>(vectorHigh) 
              << ", Full Address = 0x" << ((vectorHigh << 8) | vectorLow) << std::endl;
}


uint8_t CPU::readMemory(uint16_t address) {
    if (address >= 0x2000 && address <= 0x3FFF) {
        uint16_t ppuAddress = 0x2000 + (address % 8);
        if (ppu) {
            uint8_t value = ppu->readRegister(ppuAddress);
            std::cerr << "[CPU Debug] Read from PPU register 0x" << std::hex << ppuAddress
                      << " returning value 0x" << static_cast<int>(value) << ".\n";
            return value;
        }
        std::cerr << "[CPU Debug] Attempted PPU read with no linked PPU.\n";
        return 0; // Return 0 as a placeholder
    }
    // Return general memory value
    return memory[address];
}

void CPU::writeMemory(uint16_t address, uint8_t value) {
    if (address >= 0x2000 && address <= 0x3FFF) {
        // PPU registers are mirrored every 8 bytes in this range
        uint16_t ppuAddress = 0x2000 + (address % 8);
        if (ppu) {
            ppu->writeRegister(ppuAddress, value);
            return; // Handled by PPU
        }
        std::cerr << "[CPU Debug] Attempted PPU write with no linked PPU.\n";
        return;
    }

    // Handle general RAM or other I/O
    memory[address] = value;
}


void CPU::setPPU(PPU* ppuInstance) {
    ppu = ppuInstance;
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
    if (nmiRequested) {
        handleNMI();
        nmiRequested = false; // Clear the signal after handling
        return; // Prevent further opcode execution this cycle
    }

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
