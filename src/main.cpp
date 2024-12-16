#include <iostream>
#include <fstream>
#include "cpu.h"

void loadROM(CPU& cpu, const std::string& filepath) {
    std::ifstream rom(filepath, std::ios::binary);
    if (!rom.is_open()) {
        std::cerr << "Failed to open ROM: " << filepath << std::endl;
        exit(1);
    }

    // Read the NES header
    char header[16];
    rom.read(header, 16);

    // Verify that it's a valid NES file
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        std::cerr << "Invalid NES file: " << filepath << std::endl;
        exit(1);
    }

    // Get PRG-ROM size (in 16KB units)
    int prgSize = header[4] * 16384; // PRG-ROM size in bytes
    if (prgSize > 0x8000) { // Maximum size for PRG-ROM in NES memory
        std::cerr << "PRG-ROM size exceeds memory limit!" << std::endl;
        exit(1);
    }

    // Load PRG-ROM into memory at 0x8000
    rom.read(reinterpret_cast<char*>(&cpu.memory[0x8000]), prgSize);

    if (!rom) {
        std::cerr << "Error reading PRG-ROM" << std::endl;
        exit(1);
    }

    // Mirror PRG-ROM for smaller sizes
    if (prgSize == 0x4000) { // If the PRG-ROM is 16KB, mirror it to 0xC000
        for (size_t i = 0; i < 0x4000; ++i) {
            cpu.memory[0xC000 + i] = cpu.memory[0x8000 + i];
        }
    }

    // Set the reset vector in memory to start at 0xC000
    cpu.memory[0xFFFC] = 0x00; // Low byte of 0xC000
    cpu.memory[0xFFFD] = 0xC0; // High byte of 0xC000

    // Debugging information
    uint8_t resetLow = cpu.memory[0xFFFC];
    uint8_t resetHigh = cpu.memory[0xFFFD];
    uint16_t resetVector = resetLow | (resetHigh << 8);

    std::cout << "ROM loaded successfully: " << filepath << std::endl;
    std::cout << "PRG-ROM size: " << prgSize << " bytes" << std::endl;
    std::cout << "Reset vector set to: 0x" << std::hex << resetVector << std::endl;

    rom.close();
}





void debugCPU(const CPU& cpu) {
    // Print out CPU state
    std::cout << "PC: 0x" << std::hex << cpu.PC
              << " A: 0x" << static_cast<int>(cpu.A)
              << " X: 0x" << static_cast<int>(cpu.X)
              << " Y: 0x" << static_cast<int>(cpu.Y)
              << " SP: 0x" << static_cast<int>(cpu.SP)
              << " P: 0x" << std::bitset<8>(cpu.P)
              << " Cycles: " << std::dec << cpu.cycles
              << std::endl;
}

int main() {
    CPU cpu;

    // Reset CPU
    cpu.reset();

    // Load ROM
    const std::string romPath = "roms/nestest.nes";
    loadROM(cpu, romPath);

    // Execute instructions
    for (int i = 0; i < 100; ++i) { // Adjust this number as needed
        cpu.execute();
        debugCPU(cpu); // Debug CPU state after each instruction
        
        // Check if we've reached the end of the test
        
    }
    //cpu.dumpMemoryToConsole(0x6000, 0x7FFF);

    return 0;
}
