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

#include "cpu.h"
#include "doctest.h"
#include <bitset>
#include <iostream>

TEST_CASE("CPU - Modify Status Register P") {
    CPU cpu;
    cpu.reset();

     SUBCASE("Set and Clear Break Flag") {
        cpu.P = 0b10100101; // Initialize P with Break flag set
        std::cerr << "[Test Debug] Initial P: 0b" << std::bitset<8>(cpu.P) << std::endl;

        // Shadow P and clear bit 4
        uint8_t tempP = cpu.P;
        tempP &= ~0x10; // Clear Break flag
        cpu.P = tempP;  // Assign back to P

        std::cerr << "[Test Debug] Temp P After Clearing: 0b" << std::bitset<8>(tempP) << std::endl;
        std::cerr << "[Test Debug] P After Assignment:   0b" << std::bitset<8>(cpu.P) << std::endl;

        CHECK(cpu.P == 0b10100001);
    }

    SUBCASE("Set and Clear Interrupt Flag") {
        cpu.P = 0b00000000; // Initialize with no flags set
        cpu.setFlag(CPU::I, true); // Set Interrupt Disable flag (bit 2)

        std::cerr << "[Test Debug] P After Setting Interrupt Flag: 0b" 
                  << std::bitset<8>(cpu.P) << std::endl;

        CHECK(cpu.P == 0b00000100); // Verify Interrupt flag is set

        cpu.setFlag(CPU::I, false); // Clear Interrupt Disable flag
        std::cerr << "[Test Debug] P After Clearing Interrupt Flag: 0b" 
                  << std::bitset<8>(cpu.P) << std::endl;

        CHECK(cpu.P == 0b00000000); // Verify Interrupt flag is cleared
    }
}






