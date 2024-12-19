#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <functional>
#include <unordered_map>
#include <string>



class CPU;
class PPU;



// Forward declarations for opcode initialization
void initializeArithmeticOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeBitwiseOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeBranchOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeShiftOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeControlOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeMiscOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeMemoryOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeTransferOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeFlagsOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeComparisonOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);
void initializeStackOpcodes(std::unordered_map<uint8_t, std::function<void(CPU&)>>& opcodeTable);







class CPU {
public:
    // Public StatusFlags enum
    enum StatusFlags {
        C = 0, // Carry
        Z = 1, // Zero
        I = 2, // Interrupt Disable
        D = 3, // Decimal Mode
        B = 4, // Break
        V = 6, // Overflow
        N = 7  // Negative
    };

    // Registers
    uint8_t A = 0; // Accumulator
    uint8_t X = 0; // X Register
    uint8_t Y = 0; // Y Register
    uint8_t SP = 0xFF; // Stack Pointer
    uint16_t PC = 0; // Program Counter
    uint8_t P = 0; // Status Flags

    // Cycles
    int cycles = 0;

    // NMI Flag
    bool nmiRequested = false; // Indicates whether an NMI has been requested


    // Memory
    std::array<uint8_t, 0x10000> memory{}; // 64KB of memory

    // Methods
    void reset();
    void execute();
    void executeWithCycles();
    void loadROM(const std::string& filename);
    void printMemory(uint16_t start, uint16_t end);
    void dumpMemoryToConsole(uint16_t start, uint16_t end);
    void handleUndefinedOpcode(uint8_t opcode);
    void requestNMI();
    void handleNMI();
    void debugNMIVector();
    void writeMemory(uint16_t address, uint8_t value);
    uint8_t readMemory(uint16_t address);
    void setPPU(PPU* ppuInstance);

    // Opcode Table
    using OpcodeFunction = std::function<void(CPU&)>;
    std::unordered_map<uint8_t, OpcodeFunction> opcodeTable;

    void initializeOpcodeTable();

    // Helper methods
    uint8_t fetchByte();
    uint16_t fetchWord();
    void setFlag(uint8_t flag, bool value);
    bool getFlag(uint8_t flag);

    // Arithmetic Instructions
    void performADC(uint8_t value);
    void performSBC(uint8_t value);
    void performINC(uint16_t addr);
    void performINX();
    void performINY();
    void performDEC(uint16_t addr);

    // Bitwise Instructions
    void performAND(uint8_t value);
    void performORA(uint8_t value);
    void performEOR(uint8_t value);
    void performBIT(uint8_t value);

    // Shift Instructions
    void performASL(uint8_t& value);
    void performLSR(uint8_t& value);
    void performLSRMemory(uint16_t address);
    void performROL(uint8_t& value);
    void performROR(uint8_t& value);

    // Branch Instructions
    void performBCC();
    void performBCS();
    void performBEQ();
    void performBNE();
    void performBMI();
    void performBPL();
    void performBVC();
    void performBVS();

    // Jump and Call Instructions
    void performJMP(uint16_t address);
    void performJSR(uint16_t address);
    void performRTI();
    void performRTS();

    // Stack Instructions
    void pushToStack(uint8_t value);
    uint8_t pullFromStack();
    uint8_t popFromStack();
    void performPHA();
    void performPLA();
    void performPHP();
    void performPLP();

    // Control Instructions
    void performBRK();
    void performNOP();
    void performCLC();
    void performSEC();
    void performCLI();
    void performSEI();
    void performCLD();
    void performSED();
    void performCLV();

    // Transfer Instructions
    void performTAX();
    void performTXA();
    void performTAY();
    void performTYA();
    void performTSX();
    void performTXS();

    // Update Flags
    void updateLDAFlags();

    // Comparison Intructions
    void performCMP(uint8_t value);
    void performCPX(uint8_t value);
    void performCPY(uint8_t value);

    void debugStack();
    void addCycles(int cycles);

private:
 PPU* ppu = nullptr;


};

#endif // CPU_H
