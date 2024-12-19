#include "ppu.h"
#include "cpu.h"
#include "doctest.h"
#include <iostream>

// Helper to reset CPU and PPU before tests
void resetSystem(PPU &ppu, CPU &cpu)
{
    cpu.reset();
    ppu.reset();
    ppu.setCPU(&cpu); // Link PPU to CPU
}

// TEST_CASE("PPU - NMI Trigger and CPU Communication - Detailed Steps")
// {
//     PPU ppu;
//     CPU cpu;

//     SUBCASE("NMI pushes correct values to the stack and updates PC")
//     {
//         resetSystem(ppu, cpu);

//         // Set the NMI vector to 0x8000
//         cpu.memory[0xFFFA] = 0x00; // Low byte
//         cpu.memory[0xFFFB] = 0x80; // High byte

//         // Set initial CPU state
//         cpu.PC = 0x1234;          // Arbitrary PC value
//         cpu.P = 0b10100101;       // Random status register state
//         uint8_t sp_before = cpu.SP; // Save SP before NMI

//         // Trigger NMI
//         cpu.requestNMI();  // Flag the NMI request
//         cpu.execute();     // CPU checks for NMI and handles it

//         // Verify values pushed onto the stack
//         CHECK(cpu.memory[0x0100 + ((sp_before) & 0xFF)] == (0b10100101 & ~0x10)); // Flags with B cleared
//         CHECK(cpu.memory[0x0100 + ((sp_before - 1) & 0xFF)] == 0x34);            // Low byte of PC
//         CHECK(cpu.memory[0x0100 + ((sp_before - 2) & 0xFF)] == 0x12);            // High byte of PC

//         // Verify SP decremented by 3
//         CHECK(cpu.SP == static_cast<uint8_t>(sp_before - 3));

//         // Verify PC is updated to the NMI vector (0x8000)
//         CHECK(cpu.PC == 0x8000);

//         // Debug prints for clarity
//         std::cerr << "[Debug] SP After NMI: 0x" << std::hex << static_cast<int>(cpu.SP) << std::endl;
//         std::cerr << "[Debug] PC After NMI: 0x" << std::hex << cpu.PC << std::endl;
//     }
// }

TEST_CASE("CPU - Stack Push Behavior") {
    CPU cpu;
    cpu.reset();
    cpu.SP = 0xFF; // Start SP at reset value

    SUBCASE("Push single byte to stack") {
        cpu.pushToStack(0xAB); // Push test value
        CHECK(cpu.memory[0x01FF] == 0xAB);  // Verify stack location
        CHECK(cpu.SP == 0xFE);              // SP decrements
    }

    SUBCASE("Push multiple bytes to stack") {
        cpu.pushToStack(0x12);
        cpu.pushToStack(0x34);
        CHECK(cpu.memory[0x01FF] == 0x12);
        CHECK(cpu.memory[0x01FE] == 0x34);
        CHECK(cpu.SP == 0xFD); // SP decrements twice
    }
}

// TEST_CASE("CPU - Status Register Push with Break Flag Cleared") {
//     CPU cpu;
//     cpu.reset();

//     // Set P to a known value with the Break flag set
//     cpu.P = 0b10100101; // Initial value of P (Break flag is set)
//     std::cerr << "[Test Debug] Initial P: 0b" << std::bitset<8>(cpu.P) << std::endl;

//     uint8_t flagsToPush = cpu.P & ~0x10; // Clear the Break flag
//     std::cerr << "[Test Debug] Flags to Push (Break Cleared): 0b" 
//               << std::bitset<8>(flagsToPush) << std::endl;

//     // Save the initial SP
//     uint8_t initialSP = cpu.SP;

//     // Push the flags onto the stack
//     cpu.pushToStack(flagsToPush);

//     // Validate the stack content at the initial SP position
//     uint8_t pushedValue = cpu.memory[0x0100 + initialSP];
//     std::cerr << "[Test Debug] Value on Stack: 0b" 
//               << std::bitset<8>(pushedValue) 
//               << " at Address: 0x" << std::hex << (0x0100 + initialSP) << std::endl;

//     // Verify that the Break flag has been cleared
//     CHECK(pushedValue == 0b10100001); // Expect Break flag cleared

//     // Verify that SP was decremented correctly
//     CHECK(cpu.SP == static_cast<uint8_t>(initialSP - 1));
// }


TEST_CASE("CPU - NMI Vector Fetch") {
    CPU cpu;
    cpu.reset();

    // Set the NMI vector to 0x8000
    cpu.memory[0xFFFA] = 0x00; // Low byte
    cpu.memory[0xFFFB] = 0x80; // High byte

    cpu.handleNMI();

    CHECK(cpu.PC == 0x8000); // PC should be set to NMI vector
}



TEST_CASE("CPU - Set and Clear Break Flag in Status Register P") {
    CPU cpu;

    SUBCASE("Set Break Flag") {
        cpu.P = 0b10100000; // Initial state with Break flag cleared
        cpu.setFlag(CPU::B, true); // Set Break flag
        std::cerr << "[Test Debug] After Setting Break Flag: P = 0b" << std::bitset<8>(cpu.P) << std::endl;
        CHECK(cpu.P == 0b10110000); // Verify Break flag is set
    }

    SUBCASE("Clear Break Flag") {
        cpu.P = 0b10110000; // Initial state with Break flag set
        cpu.setFlag(CPU::B, false); // Clear Break flag
        std::cerr << "[Test Debug] After Clearing Break Flag: P = 0b" << std::bitset<8>(cpu.P) << std::endl;
        CHECK(cpu.P == 0b10100000); // Verify Break flag is cleared
    }

    SUBCASE("No Unexpected Overwrites to P") {
    cpu.P = 0b10100101; // Set some flags
    cpu.setFlag(CPU::B, false); // Clear Break flag

    uint8_t expectedP = cpu.P & ~0x10; // Calculate expected result
    std::cerr << "[Test Debug] After Clearing Break Flag (No Overwrites): P = 0b" 
              << std::bitset<8>(cpu.P) << ", Expected P = 0b" 
              << std::bitset<8>(expectedP) << std::endl;

    CHECK(cpu.P == expectedP); // Compare using the precomputed value
}

}

TEST_CASE("CPU - NMI Pushes Correct PC and Flags to Stack") {
    CPU cpu;

    // Initial state
    cpu.PC = 0x1234; // Example program counter
    cpu.P = 0b10100101; // Status register
    cpu.SP = 0xFF;

    cpu.requestNMI(); // Trigger NMI
    cpu.handleNMI(); // Handle NMI

    // Check values pushed to the stack
    CHECK(cpu.memory[0x1FF] == 0x12); // High byte of PC
    CHECK(cpu.memory[0x1FE] == 0x34); // Low byte of PC
    CHECK(cpu.memory[0x1FD] == (0b10100101 & ~0x10)); // Flags with Break cleared

    // Verify stack pointer decrement
    CHECK(cpu.SP == 0xFC);

    // Check new PC value (loaded from vector)
    uint16_t expectedPC = (cpu.memory[0xFFFB] << 8) | cpu.memory[0xFFFA];
    CHECK(cpu.PC == expectedPC);
}

TEST_CASE("PPU - NMI Trigger and CPU Communication") {
    CPU cpu;
    PPU ppu;

    // Reset both CPU and PPU
    cpu.reset();
    ppu.reset();
    ppu.setCPU(&cpu); // Link PPU to CPU

    // Set the NMI vector to 0x8000
    cpu.memory[0xFFFA] = 0x00; // Low byte
    cpu.memory[0xFFFB] = 0x80; // High byte

    // Enable NMI in PPUCTRL
    ppu.writeRegister(0x2000, 0x80); // Set NMI enable bit

    // Initialize CPU state
    cpu.PC = 0x1234;           // Example program counter
    cpu.P = 0b10100101;        // Status register
    uint8_t initialSP = cpu.SP; // Save initial SP

    // Render a frame (triggers VBlank and NMI)
    ppu.renderFrame();
    cpu.execute();

    // Check values pushed to the stack
    CHECK(cpu.memory[0x0100 + (initialSP - 0)] == 0x12); // High byte of PC
    CHECK(cpu.memory[0x0100 + (initialSP - 1)] == 0x34); // Low byte of PC
    CHECK(cpu.memory[0x0100 + (initialSP - 2)] == ((cpu.P & ~0x10) | 0x20)); // Flags with B cleared, Unused set

    // Verify SP decremented by 3
    CHECK(cpu.SP == static_cast<uint8_t>(initialSP - 3));

    // Verify PC is updated to the NMI vector (0x8000)
    CHECK(cpu.PC == 0x8000);

    // Debug logs
    std::cerr << "[Debug] SP After NMI: 0x" << std::hex << static_cast<int>(cpu.SP) << std::endl;
    std::cerr << "[Debug] PC After NMI: 0x" << std::hex << cpu.PC << std::endl;
}


TEST_CASE("PPU - NMI Trigger and CPU Communication2") {
    CPU cpu;
    PPU ppu;

    // Link PPU to CPU
    cpu.reset();
    ppu.reset();
    ppu.setCPU(&cpu);

    // Set the NMI vector to 0x8000 in memory
    cpu.memory[0xFFFA] = 0x00; // Low byte
    cpu.memory[0xFFFB] = 0x80; // High byte

    // Enable NMI in PPUCTRL
    ppu.writeRegister(0x2000, 0x80); // Set NMI enable flag

    // Initialize CPU state
    cpu.PC = 0x1234;           // Arbitrary program counter
    cpu.P = 0b10100101;        // Status register
    uint8_t initialSP = cpu.SP; // Save initial stack pointer

    // Simulate rendering a frame, which sets VBlank and triggers NMI
    ppu.renderFrame();
    cpu.execute(); // Execute CPU cycle to handle the NMI

    // Verify stack values
    CHECK(cpu.memory[0x0100 + initialSP - 0] == 0x12); // High byte of PC
    CHECK(cpu.memory[0x0100 + initialSP - 1] == 0x34); // Low byte of PC
    CHECK(cpu.memory[0x0100 + initialSP - 2] == ((cpu.P & ~0x10) | 0x20)); // Status register

    // Verify SP decrement
    CHECK(cpu.SP == static_cast<uint8_t>(initialSP - 3));

    // Verify PC is updated to the NMI vector (0x8000)
    CHECK(cpu.PC == 0x8000);

    // Debug output
    std::cerr << "[Test Debug] Stack Pointer After NMI: 0x" << std::hex << static_cast<int>(cpu.SP) << std::endl;
    std::cerr << "[Test Debug] Program Counter After NMI: 0x" << std::hex << cpu.PC << std::endl;
}
