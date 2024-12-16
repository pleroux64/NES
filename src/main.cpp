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

    // Get PRG-ROM and CHR-ROM sizes
    int prgSize = header[4] * 16384; // PRG-ROM size (in 16KB units)
    if (prgSize > 0x8000) { // Maximum size for PRG-ROM in memory
        std::cerr << "PRG-ROM size exceeds memory limit!" << std::endl;
        exit(1);
    }

    // Load PRG-ROM into memory at 0x8000
    rom.read(reinterpret_cast<char*>(&cpu.memory[0x8000]), prgSize);

    if (!rom) {
        std::cerr << "Error reading PRG-ROM" << std::endl;
        exit(1);
    }

    std::cout << "ROM loaded successfully: " << filepath << std::endl;
    std::cout << "PRG-ROM size: " << prgSize << " bytes" << std::endl;

    rom.close();
}


int main() {
    CPU cpu;

    // Reset CPU
    cpu.reset();

    // Load Super Mario Bros. ROM
    loadROM(cpu, "roms/super_mario_bros.nes");

    // Execute instructions (simple loop for now)
    for (int i = 0; i < 100; ++i) {
        cpu.execute();
    }

    return 0;
}
