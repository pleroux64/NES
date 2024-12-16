#include "cycle_exceptions.h"
#include <unordered_map>
#include <cstdint>


int pageBoundaryException(uint16_t baseAddress, uint8_t offset) {
    uint16_t newAddress = baseAddress + offset;
    return (baseAddress & 0xFF00) != (newAddress & 0xFF00) ? 1 : 0;
}

int branchCycleExceptions(CPU& cpu) {
    int extraCycles = 0;
    uint16_t originalPC = cpu.PC;
    int8_t offset = static_cast<int8_t>(cpu.fetchByte());
    uint16_t newPC = originalPC + offset;

    if ((originalPC & 0xFF00) != (newPC & 0xFF00)) {
        extraCycles += 1; // Page boundary crossed
    }
    return extraCycles;
}

int indirectYPageBoundaryException(CPU& cpu, uint16_t baseAddress) {
    uint16_t address = cpu.memory[baseAddress] | (cpu.memory[baseAddress + 1] << 8);
    uint16_t finalAddress = address + cpu.Y;
    return (address & 0xFF00) != (finalAddress & 0xFF00) ? 1 : 0;
}



std::unordered_map<uint8_t, std::function<int(CPU&)>> cycleExceptions = {
    // ADC (Add with Carry)
    {0x7D, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // ADC Absolute,X
    {0x79, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.Y); 
    }}, // ADC Absolute,Y
    {0x71, [](CPU& cpu) { 
        uint16_t baseAddr = cpu.fetchByte(); 
        return indirectYPageBoundaryException(cpu, baseAddr); 
    }}, // ADC (Indirect),Y

    // AND (Bitwise AND)
    {0x3D, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // AND Absolute,X
    {0x39, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.Y); 
    }}, // AND Absolute,Y
    {0x31, [](CPU& cpu) { 
        uint16_t baseAddr = cpu.fetchByte(); 
        return indirectYPageBoundaryException(cpu, baseAddr); 
    }}, // AND (Indirect),Y

    // Branch Instructions (e.g., BCC, BCS, BEQ, etc.)
    {0x90, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BCC
    {0xB0, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BCS
    {0xF0, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BEQ
    {0x30, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BMI
    {0xD0, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BNE
    {0x10, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BPL
    {0x50, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BVC
    {0x70, [](CPU& cpu) { return branchCycleExceptions(cpu); }}, // BVS

    // CMP (Compare A)
    {0xDD, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // CMP Absolute,X
    {0xD9, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.Y); 
    }}, // CMP Absolute,Y
    {0xD1, [](CPU& cpu) { 
        uint16_t baseAddr = cpu.fetchByte(); 
        return indirectYPageBoundaryException(cpu, baseAddr); 
    }}, // CMP (Indirect),Y

    // LDA (Load A)
    {0xBD, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // LDA Absolute,X
    {0xB9, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.Y); 
    }}, // LDA Absolute,Y
    {0xB1, [](CPU& cpu) { 
        uint16_t baseAddr = cpu.fetchByte(); 
        return indirectYPageBoundaryException(cpu, baseAddr); 
    }}, // LDA (Indirect),Y

    // EOR (Bitwise Exclusive OR)
    {0x5D, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // EOR Absolute,X
    {0x59, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.Y); 
    }}, // EOR Absolute,Y
    {0x51, [](CPU& cpu) { 
        uint16_t baseAddr = cpu.fetchByte(); 
        return indirectYPageBoundaryException(cpu, baseAddr); 
    }}, // EOR (Indirect),Y

    // SBC (Subtract with Carry)
    {0xFD, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // SBC Absolute,X
    {0xF9, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.Y); 
    }}, // SBC Absolute,Y
    {0xF1, [](CPU& cpu) { 
        uint16_t baseAddr = cpu.fetchByte(); 
        return indirectYPageBoundaryException(cpu, baseAddr); 
    }}, // SBC (Indirect),Y

    // ORA (Bitwise OR)
    {0x1D, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // ORA Absolute,X
    {0x19, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.Y); 
    }}, // ORA Absolute,Y
    {0x11, [](CPU& cpu) { 
        uint16_t baseAddr = cpu.fetchByte(); 
        return indirectYPageBoundaryException(cpu, baseAddr); 
    }}, // ORA (Indirect),Y

    // ROL (Rotate Left)
    {0x3E, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // ROL Absolute,X

    // ROR (Rotate Right)
    {0x5E, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // ROR Absolute,X

    // INC (Increment Memory)
    {0xFE, [](CPU& cpu) { 
        uint16_t addr = cpu.fetchWord(); 
        return pageBoundaryException(addr, cpu.X); 
    }}, // INC Absolute,X
};
