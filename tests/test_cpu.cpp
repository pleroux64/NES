#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "cpu.h"

TEST_CASE("ADC - Add with Carry")
{
    CPU cpu;
    cpu.reset(); // Initialize CPU to a known state

    cpu.A = 0x10;              // Set the accumulator
    cpu.setFlag(CPU::C, true); // Set the carry flag
    cpu.performADC(0x20);      // Perform ADC with 0x20

    CHECK(cpu.A == 0x31);                // Result of 0x10 + 0x20 + carry = 0x31
    CHECK(cpu.getFlag(CPU::C) == false); // Carry flag should NOT be set (no overflow)
    CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag should not be set
    CHECK(cpu.getFlag(CPU::N) == false); // Negative flag should not be set
    CHECK(cpu.getFlag(CPU::V) == false); // Overflow flag should not be set
}

TEST_CASE("AND - Bitwise AND")
{
    CPU cpu;
    cpu.reset(); // Initialize CPU to a known state

    cpu.A = 0xF0;         // Set the accumulator to 0xF0
    cpu.performAND(0x0F); // Perform AND with 0x0F

    CHECK(cpu.A == 0x00);                // Result of 0xF0 & 0x0F = 0x00
    CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag should be set
    CHECK(cpu.getFlag(CPU::N) == false); // Negative flag should not be set
}

TEST_CASE("Testing getFlag()")
{
    CPU cpu;
    cpu.reset(); // Initialize CPU to a known state

    // Test for Carry Flag (C)
    cpu.setFlag(CPU::C, true);
    REQUIRE(cpu.getFlag(CPU::C) == true); // Carry flag should be set
    cpu.setFlag(CPU::C, false);
    REQUIRE(cpu.getFlag(CPU::C) == false); // Carry flag should be cleared

    // Test for Zero Flag (Z)
    cpu.setFlag(CPU::Z, true);
    REQUIRE(cpu.getFlag(CPU::Z) == true); // Zero flag should be set
    cpu.setFlag(CPU::Z, false);
    REQUIRE(cpu.getFlag(CPU::Z) == false); // Zero flag should be cleared

    // Test for Interrupt Disable Flag (I)
    cpu.setFlag(CPU::I, true);
    REQUIRE(cpu.getFlag(CPU::I) == true); // Interrupt Disable flag should be set
    cpu.setFlag(CPU::I, false);
    REQUIRE(cpu.getFlag(CPU::I) == false); // Interrupt Disable flag should be cleared

    // Test for Decimal Mode Flag (D)
    cpu.setFlag(CPU::D, true);
    REQUIRE(cpu.getFlag(CPU::D) == true); // Decimal Mode flag should be set
    cpu.setFlag(CPU::D, false);
    REQUIRE(cpu.getFlag(CPU::D) == false); // Decimal Mode flag should be cleared

    // Test for Break Flag (B)
    cpu.setFlag(CPU::B, true);
    REQUIRE(cpu.getFlag(CPU::B) == true); // Break flag should be set
    cpu.setFlag(CPU::B, false);
    REQUIRE(cpu.getFlag(CPU::B) == false); // Break flag should be cleared

    // Test for Overflow Flag (V)
    cpu.setFlag(CPU::V, true);
    REQUIRE(cpu.getFlag(CPU::V) == true); // Overflow flag should be set
    cpu.setFlag(CPU::V, false);
    REQUIRE(cpu.getFlag(CPU::V) == false); // Overflow flag should be cleared

    // Test for Negative Flag (N)
    cpu.setFlag(CPU::N, true);
    REQUIRE(cpu.getFlag(CPU::N) == true); // Negative flag should be set
    cpu.setFlag(CPU::N, false);
    REQUIRE(cpu.getFlag(CPU::N) == false); // Negative flag should be cleared
}

TEST_CASE("ASL - Accumulator")
{
    CPU cpu;
    cpu.reset();
    cpu.A = 0x10;
    cpu.performASL(cpu.A);
    CHECK(cpu.A == 0x20);
    CHECK(cpu.getFlag(CPU::C) == false);
    CHECK(cpu.getFlag(CPU::Z) == false);
    CHECK(cpu.getFlag(CPU::N) == false);
}

TEST_CASE("ASL - Zero Page")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x50] = 0x10;
    cpu.performASL(cpu.memory[0x50]);
    CHECK(cpu.memory[0x50] == 0x20);
    CHECK(cpu.getFlag(CPU::C) == false);
}

TEST_CASE("ASL - Zero Page, X")
{
    CPU cpu;
    cpu.reset();
    cpu.X = 0x05;
    cpu.memory[0x50 + cpu.X] = 0x10;
    cpu.performASL(cpu.memory[0x50 + cpu.X]);
    CHECK(cpu.memory[0x50 + cpu.X] == 0x20);
    CHECK(cpu.getFlag(CPU::C) == false);
}

TEST_CASE("ASL - Absolute")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x2000] = 0x10;
    cpu.performASL(cpu.memory[0x2000]);
    CHECK(cpu.memory[0x2000] == 0x20);
    CHECK(cpu.getFlag(CPU::C) == false);
}

TEST_CASE("ASL - Absolute, X")
{
    CPU cpu;
    cpu.reset();
    cpu.X = 0x05;
    cpu.memory[0x2000 + cpu.X] = 0x10;
    cpu.performASL(cpu.memory[0x2000 + cpu.X]);
    CHECK(cpu.memory[0x2000 + cpu.X] == 0x20);
    CHECK(cpu.getFlag(CPU::C) == false);
}

TEST_CASE("BCC - Branch if Carry Clear")
{
    CPU cpu;
    cpu.reset();
    cpu.setFlag(CPU::C, false);    // Ensure carry is clear
    cpu.memory[cpu.PC] = 0x90;     // BCC opcode
    cpu.memory[cpu.PC + 1] = 0x10; // Offset of +16
    uint16_t originalPC = cpu.PC;
    cpu.execute();

    REQUIRE(cpu.PC == originalPC + 2 + 0x10); // PC should jump
}

TEST_CASE("BCS - Branch if Carry Set")
{
    CPU cpu;
    cpu.reset();
    cpu.setFlag(CPU::C, true);     // Ensure carry is set
    cpu.memory[cpu.PC] = 0xB0;     // BCS opcode
    cpu.memory[cpu.PC + 1] = 0x10; // Offset of +16
    uint16_t originalPC = cpu.PC;
    cpu.execute();

    REQUIRE(cpu.PC == originalPC + 2 + 0x10); // PC should jump
}

TEST_CASE("BEQ - Branch if Equal")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0xF0; // BEQ opcode
    cpu.memory[0x8001] = 0x10; // Offset
    cpu.setFlag(CPU::Z, true); // Set Zero flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    CHECK(cpu.PC == originalPC + 0x10 + 2); // PC should branch to new address
}

TEST_CASE("BEQ - No Branch if Zero Flag Clear")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0xF0;  // BEQ opcode
    cpu.memory[0x8001] = 0x10;  // Offset
    cpu.setFlag(CPU::Z, false); // Clear Zero flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    // Ensure PC skips over the opcode and operand
    CHECK(cpu.PC == originalPC + 2);
}

TEST_CASE("BIT - Zero Page")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x00] = 0xF0;   // Memory at address 0x00
    cpu.memory[0x8000] = 0x24; // BIT opcode
    cpu.memory[0x8001] = 0x00; // Zero page address
    cpu.A = 0x0F;              // Set A to ensure Z is correctly set

    cpu.execute();

    CHECK(cpu.getFlag(CPU::Z) == true); // 0x0F & 0xF0 = 0 -> Zero flag set
    CHECK(cpu.getFlag(CPU::V) == true); // Overflow flag set (bit 6 = 1)
    CHECK(cpu.getFlag(CPU::N) == true); // Negative flag set (bit 7 = 1)
}

TEST_CASE("BIT - Absolute")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x1234] = 0x7F; // Memory at address 0x1234
    cpu.memory[0x8000] = 0x2C; // BIT opcode
    cpu.memory[0x8001] = 0x34; // Low byte of address
    cpu.memory[0x8002] = 0x12; // High byte of address
    cpu.A = 0x80;

    cpu.execute();

    CHECK(cpu.getFlag(CPU::Z) == true);  // 0x80 & 0x7F = 0 -> Zero flag set
    CHECK(cpu.getFlag(CPU::V) == true);  // Overflow flag set (bit 6 = 1)
    CHECK(cpu.getFlag(CPU::N) == false); // Negative flag cleared (bit 7 = 0)
}

TEST_CASE("BMI - Branch if Negative Flag Set")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0x30; // BMI opcode
    cpu.memory[0x8001] = 0x10; // Offset
    cpu.setFlag(CPU::N, true); // Set Negative flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    CHECK(cpu.PC == originalPC + 2 + 0x10); // PC should branch
}

TEST_CASE("BMI - No Branch if Negative Flag Clear")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0x30;  // BMI opcode
    cpu.memory[0x8001] = 0x10;  // Offset
    cpu.setFlag(CPU::N, false); // Clear Negative flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    std::cout << "Original PC: " << std::hex << originalPC << ", Final PC: " << cpu.PC << std::dec << std::endl;
    CHECK(cpu.PC == originalPC + 2); // PC should skip to the next instruction
}

TEST_CASE("BNE - Branch if Zero Flag Clear")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0xD0;  // BNE opcode
    cpu.memory[0x8001] = 0x10;  // Offset
    cpu.setFlag(CPU::Z, false); // Clear Zero flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    CHECK(cpu.PC == originalPC + 2 + 0x10); // PC should branch to offset
}

TEST_CASE("BNE - No Branch if Zero Flag Set")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0xD0; // BNE opcode
    cpu.memory[0x8001] = 0x10; // Offset
    cpu.setFlag(CPU::Z, true); // Set Zero flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    CHECK(cpu.PC == originalPC + 2); // PC should skip to next instruction
}

TEST_CASE("BPL - Branch if Negative Flag Clear")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0x10;  // BPL opcode
    cpu.memory[0x8001] = 0x10;  // Offset
    cpu.setFlag(CPU::N, false); // Clear Negative flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    CHECK(cpu.PC == originalPC + 2 + 0x10); // PC should branch to offset
}

TEST_CASE("BPL - No Branch if Negative Flag Set")
{
    CPU cpu;
    cpu.reset();
    cpu.memory[0x8000] = 0x10; // BPL opcode
    cpu.memory[0x8001] = 0x10; // Offset
    cpu.setFlag(CPU::N, true); // Set Negative flag
    uint16_t originalPC = cpu.PC;

    cpu.execute();

    CHECK(cpu.PC == originalPC + 2); // PC should skip to next instruction
}

TEST_CASE("BVC - Branch if Overflow Clear")
{
    CPU cpu;
    cpu.reset();

    // Test case 1: Branch is taken
    cpu.PC = 0x8000;
    cpu.P = 0x00;              // Overflow flag clear
    cpu.memory[0x8000] = 0x50; // BVC opcode
    cpu.memory[0x8001] = 0x10; // Offset (+16)

    std::cout << "Before execution: PC = " << std::hex << cpu.PC << std::dec << std::endl;
    cpu.execute(); // Execute BVC
    std::cout << "After execution: PC = " << std::hex << cpu.PC << std::dec << std::endl;

    // Assertions for branch taken
    CHECK(cpu.PC == 0x8012); // PC should branch to 0x8012

    // Test case 2: Branch is not taken
    cpu.PC = 0x8000;
    cpu.P = 0x40;  // Overflow flag set
    cpu.execute(); // Execute BVC again
    std::cout << "After execution (no branch): PC = " << std::hex << cpu.PC << std::dec << std::endl;

    CHECK(cpu.PC == 0x8002); // PC should not branch (next instruction)
}

TEST_CASE("BVS - Branch if Overflow Set")
{
    CPU cpu;
    cpu.reset();

    // Test case 1: Branch is taken
    cpu.PC = 0x8000;
    cpu.P = 0x40;              // Overflow flag set
    cpu.memory[0x8000] = 0x70; // BVS opcode
    cpu.memory[0x8001] = 0x10; // Offset (+16)

    cpu.execute(); // Execute BVS

    // Assertions
    CHECK(cpu.PC == 0x8012); // PC should branch to 0x8012

    // Test case 2: Branch is not taken
    cpu.PC = 0x8000;
    cpu.P = 0x00;  // Overflow flag clear
    cpu.execute(); // Execute BVS again

    CHECK(cpu.PC == 0x8002); // PC should not branch (next instruction)
}

TEST_CASE("CLC - Clear Carry Flag")
{
    CPU cpu;
    cpu.reset(); // Ensure the CPU is in a known state

    // Set the carry flag to 1
    cpu.setFlag(CPU::C, true);
    REQUIRE(cpu.getFlag(CPU::C) == true); // Verify the flag is set initially

    // Set memory for the CLC instruction
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0x18; // CLC opcode

    cpu.execute(); // Execute the CLC instruction

    // Assert that the carry flag is cleared
    CHECK(cpu.getFlag(CPU::C) == false);
    // Assert that the PC incremented by 1 (implied addressing mode)
    CHECK(cpu.PC == 0x8001);
}

TEST_CASE("CLD - Clear Decimal Flag")
{
    CPU cpu;
    cpu.reset(); // Ensure the CPU is in a known state

    // Set the decimal flag to 1
    cpu.P |= (1 << 3);                // Set Decimal flag (bit 3)
    REQUIRE((cpu.P & (1 << 3)) != 0); // Verify the flag is set initially

    // Set memory for the CLD instruction
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xD8; // CLD opcode

    cpu.execute(); // Execute the CLD instruction

    // Assert that the decimal flag is cleared
    CHECK((cpu.P & (1 << 3)) == 0); // Verify Decimal flag is cleared
    // Assert that the PC incremented by 1 (implied addressing mode)
    CHECK(cpu.PC == 0x8001);
}

TEST_CASE("CLI - Clear Interrupt Disable Flag")
{
    CPU cpu;
    cpu.reset(); // Ensure the CPU is in a known state

    // Set the interrupt disable flag to 1
    cpu.P |= (1 << 2);                // Set Interrupt Disable flag (bit 2)
    REQUIRE((cpu.P & (1 << 2)) != 0); // Verify the flag is set initially

    // Set memory for the CLI instruction
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0x58; // CLI opcode

    cpu.execute(); // Execute the CLI instruction

    // Assert that the interrupt disable flag is cleared
    CHECK((cpu.P & (1 << 2)) == 0); // Verify Interrupt Disable flag is cleared
    // Assert that the PC incremented by 1 (implied addressing mode)
    CHECK(cpu.PC == 0x8001);
}

TEST_CASE("CLV - Clear Overflow Flag")
{
    CPU cpu;
    cpu.reset(); // Ensure the CPU is in a known state

    // Set the overflow flag to 1
    cpu.P |= (1 << 6);                // Set Overflow flag (bit 6)
    REQUIRE((cpu.P & (1 << 6)) != 0); // Verify the flag is set initially

    // Set memory for the CLV instruction
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xB8; // CLV opcode

    cpu.execute(); // Execute the CLV instruction

    // Assert that the overflow flag is cleared
    CHECK((cpu.P & (1 << 6)) == 0); // Verify Overflow flag is cleared
    // Assert that the PC incremented by 1 (implied addressing mode)
    CHECK(cpu.PC == 0x8001);
}

TEST_CASE("CMP - Compare A")
{
    CPU cpu;
    cpu.reset(); // Ensure the CPU is in a known state

    // Test CMP Immediate (A == memory)
    cpu.A = 0x10;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xC9; // CMP Immediate opcode
    cpu.memory[0x8001] = 0x10; // Immediate value
    cpu.execute();
    CHECK(cpu.getFlag(CPU::C));  // Carry should be set (A >= memory)
    CHECK(cpu.getFlag(CPU::Z));  // Zero should be set (A == memory)
    CHECK(!cpu.getFlag(CPU::N)); // Negative should not be set

    // Test CMP Zero Page (A > memory)
    cpu.reset();
    cpu.A = 0x20;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xC5; // CMP Zero Page opcode
    cpu.memory[0x8001] = 0x00; // Zero Page address
    cpu.memory[0x0000] = 0x10; // Memory value at address 0x00
    cpu.execute();
    CHECK(cpu.getFlag(CPU::C));  // Carry should be set (A >= memory)
    CHECK(!cpu.getFlag(CPU::Z)); // Zero should not be set
    CHECK(!cpu.getFlag(CPU::N)); // Negative should not be set

    // Test CMP Absolute (A < memory)
    cpu.reset();
    cpu.A = 0x10;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xCD; // CMP Absolute opcode
    cpu.memory[0x8001] = 0x00; // Low byte of address
    cpu.memory[0x8002] = 0x10; // High byte of address
    cpu.memory[0x1000] = 0x20; // Memory value at address 0x1000
    cpu.execute();
    CHECK(!cpu.getFlag(CPU::C)); // Carry should not be set (A < memory)
    CHECK(!cpu.getFlag(CPU::Z)); // Zero should not be set
    CHECK(cpu.getFlag(CPU::N));  // Negative should be set
}

TEST_CASE("CPX - Compare X")
{
    CPU cpu;
    cpu.reset(); // Ensure the CPU is in a known state

    // Test CPX Immediate (X == memory)
    cpu.X = 0x10;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xE0; // CPX Immediate opcode
    cpu.memory[0x8001] = 0x10; // Immediate value
    cpu.execute();
    CHECK(cpu.getFlag(CPU::C));  // Carry should be set (X >= memory)
    CHECK(cpu.getFlag(CPU::Z));  // Zero should be set (X == memory)
    CHECK(!cpu.getFlag(CPU::N)); // Negative should not be set

    // Test CPX Zero Page (X > memory)
    cpu.reset();
    cpu.X = 0x20;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xE4; // CPX Zero Page opcode
    cpu.memory[0x8001] = 0x00; // Zero Page address
    cpu.memory[0x0000] = 0x10; // Memory value at address 0x00
    cpu.execute();
    CHECK(cpu.getFlag(CPU::C));  // Carry should be set (X >= memory)
    CHECK(!cpu.getFlag(CPU::Z)); // Zero should not be set
    CHECK(!cpu.getFlag(CPU::N)); // Negative should not be set

    // Test CPX Absolute (X < memory)
    cpu.reset();
    cpu.X = 0x10;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xEC; // CPX Absolute opcode
    cpu.memory[0x8001] = 0x00; // Low byte of address
    cpu.memory[0x8002] = 0x10; // High byte of address
    cpu.memory[0x1000] = 0x20; // Memory value at address 0x1000
    cpu.execute();
    CHECK(!cpu.getFlag(CPU::C)); // Carry should not be set (X < memory)
    CHECK(!cpu.getFlag(CPU::Z)); // Zero should not be set
    CHECK(cpu.getFlag(CPU::N));  // Negative should be set
}

TEST_CASE("CPY - Compare Y")
{
    CPU cpu;
    cpu.reset(); // Ensure the CPU is in a known state

    // Test CPY Immediate (Y == memory)
    cpu.Y = 0x10;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xC0; // CPY Immediate opcode
    cpu.memory[0x8001] = 0x10; // Immediate value
    cpu.execute();
    CHECK(cpu.getFlag(CPU::C));  // Carry should be set (Y >= memory)
    CHECK(cpu.getFlag(CPU::Z));  // Zero should be set (Y == memory)
    CHECK(!cpu.getFlag(CPU::N)); // Negative should not be set

    // Test CPY Zero Page (Y > memory)
    cpu.reset();
    cpu.Y = 0x20;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xC4; // CPY Zero Page opcode
    cpu.memory[0x8001] = 0x00; // Zero Page address
    cpu.memory[0x0000] = 0x10; // Memory value at address 0x00
    cpu.execute();
    CHECK(cpu.getFlag(CPU::C));  // Carry should be set (Y >= memory)
    CHECK(!cpu.getFlag(CPU::Z)); // Zero should not be set
    CHECK(!cpu.getFlag(CPU::N)); // Negative should not be set

    // Test CPY Absolute (Y < memory)
    cpu.reset();
    cpu.Y = 0x10;
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xCC; // CPY Absolute opcode
    cpu.memory[0x8001] = 0x00; // Low byte of address
    cpu.memory[0x8002] = 0x10; // High byte of address
    cpu.memory[0x1000] = 0x20; // Memory value at address 0x1000
    cpu.execute();
    CHECK(!cpu.getFlag(CPU::C)); // Carry should not be set (Y < memory)
    CHECK(!cpu.getFlag(CPU::Z)); // Zero should not be set
    CHECK(cpu.getFlag(CPU::N));  // Negative should be set
}

TEST_CASE("DEC - Decrement Memory")
{
    CPU cpu;
    cpu.reset();

    // Setup initial state
    cpu.memory[0x0001] = 0x80; // Initial value where bit 7 is set
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xD6; // DEC Zero Page,X opcode
    cpu.memory[0x8001] = 0x00; // Address to decrement (0x00 + X)
    cpu.X = 0x01;

    // Execute instruction
    cpu.execute();

    // Assertions
    CHECK(cpu.memory[0x0001] == 0x7F); // Verify decremented value
    CHECK(!cpu.getFlag(CPU::Z));       // Zero flag should be cleared
    CHECK(!cpu.getFlag(CPU::N));       // Negative flag should be cleared for 0x7F

    // Additional test for N flag
    cpu.memory[0x0002] = 0xFF; // Initial value with bit 7 set
    cpu.PC = 0x8002;
    cpu.memory[0x8002] = 0xC6; // DEC Zero Page opcode
    cpu.memory[0x8003] = 0x02; // Address to decrement
    cpu.execute();

    CHECK(cpu.memory[0x0002] == 0xFE); // Verify decremented value
    CHECK(!cpu.getFlag(CPU::Z));       // Zero flag should be cleared
    CHECK(cpu.getFlag(CPU::N));        // Negative flag should be set for 0xFE
}

TEST_CASE("DEX - Decrement X")
{
    CPU cpu;
    cpu.reset();

    // Test decrementing from a positive value
    cpu.X = 0x02; // Initial value of X
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xCA; // DEX opcode
    cpu.execute();

    CHECK(cpu.X == 0x01);        // X should be decremented
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear

    // Test decrementing to zero
    cpu.X = 0x01;
    cpu.PC = 0x8001;
    cpu.memory[0x8001] = 0xCA; // DEX opcode
    cpu.execute();

    CHECK(cpu.X == 0x00);        // X should be decremented to zero
    CHECK(cpu.getFlag(CPU::Z));  // Z flag should be set
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear

    // Test decrementing to a negative value
    cpu.X = 0x00; // Initial value of X
    cpu.PC = 0x8002;
    cpu.memory[0x8002] = 0xCA; // DEX opcode
    cpu.execute();

    CHECK(cpu.X == 0xFF);        // X should wrap around to 0xFF
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(cpu.getFlag(CPU::N));  // N flag should be set
}

TEST_CASE("DEY - Decrement Y")
{
    CPU cpu;
    cpu.reset();

    // Test decrementing from a positive value
    cpu.Y = 0x02; // Initial value of Y
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0x88; // DEY opcode
    cpu.execute();

    CHECK(cpu.Y == 0x01);        // Y should be decremented
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear

    // Test decrementing to zero
    cpu.Y = 0x01;
    cpu.PC = 0x8001;
    cpu.memory[0x8001] = 0x88; // DEY opcode
    cpu.execute();

    CHECK(cpu.Y == 0x00);        // Y should be decremented to zero
    CHECK(cpu.getFlag(CPU::Z));  // Z flag should be set
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear

    // Test decrementing to a negative value
    cpu.Y = 0x00; // Initial value of Y
    cpu.PC = 0x8002;
    cpu.memory[0x8002] = 0x88; // DEY opcode
    cpu.execute();

    CHECK(cpu.Y == 0xFF);        // Y should wrap around to 0xFF
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(cpu.getFlag(CPU::N));  // N flag should be set
}

TEST_CASE("EOR - Bitwise Exclusive OR")
{
    CPU cpu;
    cpu.reset();

    // Immediate mode
    cpu.A = 0x55; // Initial accumulator value
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0x49; // EOR immediate opcode
    cpu.memory[0x8001] = 0xAA; // Immediate value
    cpu.execute();

    CHECK(cpu.A == 0xFF);        // 0x55 XOR 0xAA = 0xFF
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(cpu.getFlag(CPU::N));  // N flag should be set

    // Zero Page
    cpu.A = 0xFF;
    cpu.memory[0x0000] = 0x0F; // Value in zero page
    cpu.PC = 0x8002;
    cpu.memory[0x8002] = 0x45; // EOR zero page opcode
    cpu.memory[0x8003] = 0x00; // Address in zero page
    cpu.execute();

    CHECK(cpu.A == 0xF0);        // 0xFF XOR 0x0F = 0xF0
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(cpu.getFlag(CPU::N));  // N flag should be set
}

TEST_CASE("INC - Increment Memory")
{
    CPU cpu;
    cpu.reset();

    // Zero Page
    cpu.memory[0x00] = 0x01; // Initial value at zero page address 0x00
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xE6; // INC Zero Page opcode
    cpu.memory[0x8001] = 0x00; // Address in zero page
    cpu.execute();

    CHECK(cpu.memory[0x00] == 0x02); // 0x01 + 1 = 0x02
    CHECK(!cpu.getFlag(CPU::Z));     // Z flag should be clear
    CHECK(!cpu.getFlag(CPU::N));     // N flag should be clear

    // Absolute
    cpu.memory[0x1000] = 0x7F; // Initial value at absolute address 0x1000
    cpu.PC = 0x8002;
    cpu.memory[0x8002] = 0xEE; // INC Absolute opcode
    cpu.memory[0x8003] = 0x00; // Low byte of address
    cpu.memory[0x8004] = 0x10; // High byte of address
    cpu.execute();

    CHECK(cpu.memory[0x1000] == 0x80); // 0x7F + 1 = 0x80
    CHECK(!cpu.getFlag(CPU::Z));       // Z flag should be clear
    CHECK(cpu.getFlag(CPU::N));        // N flag should be set (bit 7 = 1)

    // Zero flag test
    cpu.memory[0x10] = 0xFF; // Initial value
    cpu.PC = 0x8005;
    cpu.memory[0x8005] = 0xE6; // INC Zero Page opcode
    cpu.memory[0x8006] = 0x10; // Address in zero page
    cpu.execute();

    CHECK(cpu.memory[0x10] == 0x00); // 0xFF + 1 = 0x00 (overflow)
    CHECK(cpu.getFlag(CPU::Z));      // Z flag should be set
    CHECK(!cpu.getFlag(CPU::N));     // N flag should be clear
}

TEST_CASE("INX - Increment X")
{
    CPU cpu;
    cpu.reset();

    // Test increment without setting flags
    cpu.X = 0x7E; // Initial value
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xE8; // INX opcode
    cpu.execute();

    CHECK(cpu.X == 0x7F);        // 0x7E + 1 = 0x7F
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear

    // Test increment that sets the Negative flag
    cpu.X = 0x7F; // Initial value
    cpu.PC = 0x8001;
    cpu.memory[0x8001] = 0xE8; // INX opcode
    cpu.execute();

    CHECK(cpu.X == 0x80);        // 0x7F + 1 = 0x80
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(cpu.getFlag(CPU::N));  // N flag should be set (bit 7 = 1)

    // Test increment that sets the Zero flag
    cpu.X = 0xFF; // Initial value
    cpu.PC = 0x8002;
    cpu.memory[0x8002] = 0xE8; // INX opcode
    cpu.execute();

    CHECK(cpu.X == 0x00);        // 0xFF + 1 = 0x00 (overflow)
    CHECK(cpu.getFlag(CPU::Z));  // Z flag should be set
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear
}

TEST_CASE("INY - Increment Y")
{
    CPU cpu;
    cpu.reset();

    // Test increment without setting flags
    cpu.Y = 0x7E; // Initial value
    cpu.PC = 0x8000;
    cpu.memory[0x8000] = 0xC8; // INY opcode
    cpu.execute();

    CHECK(cpu.Y == 0x7F);        // 0x7E + 1 = 0x7F
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear

    // Test increment that sets the Negative flag
    cpu.Y = 0x7F; // Initial value
    cpu.PC = 0x8001;
    cpu.memory[0x8001] = 0xC8; // INY opcode
    cpu.execute();

    CHECK(cpu.Y == 0x80);        // 0x7F + 1 = 0x80
    CHECK(!cpu.getFlag(CPU::Z)); // Z flag should be clear
    CHECK(cpu.getFlag(CPU::N));  // N flag should be set (bit 7 = 1)

    // Test increment that sets the Zero flag
    cpu.Y = 0xFF; // Initial value
    cpu.PC = 0x8002;
    cpu.memory[0x8002] = 0xC8; // INY opcode
    cpu.execute();

    CHECK(cpu.Y == 0x00);        // 0xFF + 1 = 0x00 (overflow)
    CHECK(cpu.getFlag(CPU::Z));  // Z flag should be set
    CHECK(!cpu.getFlag(CPU::N)); // N flag should be clear
}

TEST_CASE("JMP - Jump Absolute")
{
    CPU cpu;
    cpu.reset();

    cpu.memory[0x8000] = 0x4C; // JMP Absolute opcode
    cpu.memory[0x8001] = 0x34; // Low byte of address
    cpu.memory[0x8002] = 0x12; // High byte of address
    cpu.execute();

    CHECK(cpu.PC == 0x1234); // PC should be set to 0x1234
}

TEST_CASE("JMP - Jump Indirect")
{
    CPU cpu;
    cpu.reset();

    // Set up memory for indirect addressing
    cpu.memory[0x8000] = 0x6C; // JMP Indirect opcode
    cpu.memory[0x8001] = 0x00; // Low byte of pointer
    cpu.memory[0x8002] = 0x02; // High byte of pointer

    cpu.memory[0x0200] = 0x78; // Low byte of target address
    cpu.memory[0x0201] = 0x56; // High byte of target address
    cpu.execute();

    CHECK(cpu.PC == 0x5678); // PC should be set to 0x5678
}

TEST_CASE("JMP - Jump Indirect with Page Bug")
{
    CPU cpu;
    cpu.reset();

    // Set up memory for indirect addressing with page bug
    cpu.memory[0x8000] = 0x6C; // JMP Indirect opcode
    cpu.memory[0x8001] = 0xFF; // Low byte of pointer
    cpu.memory[0x8002] = 0x02; // High byte of pointer

    cpu.memory[0x02FF] = 0x78; // Low byte of target address
    cpu.memory[0x0200] = 0x56; // High byte of target address (CPU bug)
    cpu.execute();

    CHECK(cpu.PC == 0x5678); // PC should be set to 0x5678 due to bug
}

TEST_CASE("JSR - Jump to Subroutine")
{
    CPU cpu;
    cpu.reset();

    // Setup memory for JSR instruction
    cpu.memory[0x8000] = 0x20; // JSR opcode
    cpu.memory[0x8001] = 0x34; // Low byte of address
    cpu.memory[0x8002] = 0x12; // High byte of address

    uint8_t initialSP = cpu.SP; // Save initial stack pointer
    cpu.execute();

    // Verify the new PC is set correctly
    CHECK(cpu.PC == 0x1234);

    // Verify the stack contains the correct return address (0x8002)
    CHECK(cpu.memory[0x0100 + initialSP] == 0x02);     // Low byte
    CHECK(cpu.memory[0x0100 + initialSP - 1] == 0x80); // High byte

    // Verify stack pointer is decremented correctly
    CHECK(cpu.SP == initialSP - 2);
}

TEST_CASE("LDA - Load Accumulator")
{
    CPU cpu;

    // Setup test memory and reset the CPU
    cpu.reset();
    cpu.memory[0x8000] = 0; // Clear memory before tests

    // Immediate Mode
    SUBCASE("Immediate Mode")
    {
        cpu.memory[0x8000] = 0xA9; // LDA Immediate opcode
        cpu.memory[0x8001] = 0x42; // Value to load
        cpu.execute();
        CHECK(cpu.A == 0x42);
        CHECK(cpu.getFlag(CPU::Z) == false);
        CHECK(cpu.getFlag(CPU::N) == false);

        cpu.memory[0x8001] = 0x00; // Zero value
        cpu.reset();
        cpu.execute();
        CHECK(cpu.A == 0x00);
        CHECK(cpu.getFlag(CPU::Z) == true);
    }

    // Zero Page
    SUBCASE("Zero Page")
    {
        cpu.memory[0x8000] = 0xA5; // LDA Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Address to load from
        cpu.memory[0x0010] = 0x84; // Value to load
        cpu.execute();
        CHECK(cpu.A == 0x84);
        CHECK(cpu.getFlag(CPU::N) == true);
    }

    // Zero Page, X
    SUBCASE("Zero Page,X")
    {
        cpu.X = 0x01;              // Set X register
        cpu.memory[0x8000] = 0xB5; // LDA Zero Page,X opcode
        cpu.memory[0x8001] = 0x10; // Base address
        cpu.memory[0x0011] = 0x32; // Value to load (0x10 + 0x01 = 0x11)
        cpu.execute();
        CHECK(cpu.A == 0x32);
    }

    // Absolute
    SUBCASE("Absolute")
    {
        cpu.memory[0x8000] = 0xAD; // LDA Absolute opcode
        cpu.memory[0x8001] = 0x34; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address (0x1234)
        cpu.memory[0x1234] = 0x77; // Value to load
        cpu.execute();
        CHECK(cpu.A == 0x77);
    }

    // Absolute,X
    SUBCASE("Absolute,X")
    {
        cpu.X = 0x04;              // Set X register
        cpu.memory[0x8000] = 0xBD; // LDA Absolute,X opcode
        cpu.memory[0x8001] = 0x00; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address (0x1200)
        cpu.memory[0x1204] = 0x55; // Value to load (0x1200 + 0x04)
        cpu.execute();
        CHECK(cpu.A == 0x55);
    }

    // Absolute,Y
    SUBCASE("Absolute,Y")
    {
        cpu.Y = 0x02;              // Set Y register
        cpu.memory[0x8000] = 0xB9; // LDA Absolute,Y opcode
        cpu.memory[0x8001] = 0x00; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address (0x1200)
        cpu.memory[0x1202] = 0xAA; // Value to load (0x1200 + 0x02)
        cpu.execute();
        CHECK(cpu.A == 0xAA);
    }

    // (Indirect,X)
    SUBCASE("Indirect,X")
    {
        cpu.X = 0x03;              // Set X register
        cpu.memory[0x8000] = 0xA1; // LDA (Indirect,X) opcode
        cpu.memory[0x8001] = 0x10; // Base address
        cpu.memory[0x0013] = 0xCD; // Low byte of target address (0x10 + 0x03 = 0x13)
        cpu.memory[0x0014] = 0xAB; // High byte of target address
        cpu.memory[0xABCD] = 0x99; // Value to load
        cpu.execute();
        CHECK(cpu.A == 0x99);
    }

    // (Indirect),Y
    SUBCASE("(Indirect),Y")
    {
        cpu.Y = 0x05;              // Set Y register
        cpu.memory[0x8000] = 0xB1; // LDA (Indirect),Y opcode
        cpu.memory[0x8001] = 0x20; // Base address
        cpu.memory[0x0020] = 0x34; // Low byte of target address
        cpu.memory[0x0021] = 0x12; // High byte of target address (0x1234)
        cpu.memory[0x1239] = 0x88; // Value to load (0x1234 + 0x05)
        cpu.execute();
        CHECK(cpu.A == 0x88);
    }
}

TEST_CASE("LDX - Load X Register")
{
    CPU cpu;

    // Setup test memory and reset the CPU
    cpu.reset();
    cpu.memory[0x8000] = 0; // Clear memory before tests

    // Immediate Mode
    SUBCASE("Immediate Mode")
    {
        cpu.memory[0x8000] = 0xA2; // LDX Immediate opcode
        cpu.memory[0x8001] = 0x42; // Value to load
        cpu.execute();
        CHECK(cpu.X == 0x42);
        CHECK(cpu.getFlag(CPU::Z) == false);
        CHECK(cpu.getFlag(CPU::N) == false);

        cpu.reset();
        cpu.memory[0x8001] = 0x00; // Zero value
        cpu.execute();
        CHECK(cpu.X == 0x00);
        CHECK(cpu.getFlag(CPU::Z) == true);
    }

    // Zero Page
    SUBCASE("Zero Page")
    {
        cpu.memory[0x8000] = 0xA6; // LDX Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Address to load from
        cpu.memory[0x0010] = 0x84; // Value to load
        cpu.execute();
        CHECK(cpu.X == 0x84);
        CHECK(cpu.getFlag(CPU::N) == true);
    }

    // Zero Page,Y
    SUBCASE("Zero Page,Y")
    {
        cpu.Y = 0x03;              // Set Y register
        cpu.memory[0x8000] = 0xB6; // LDX Zero Page,Y opcode
        cpu.memory[0x8001] = 0x10; // Base address
        cpu.memory[0x0013] = 0x32; // Value to load (0x10 + 0x03 = 0x13)
        cpu.execute();
        CHECK(cpu.X == 0x32);
        CHECK(cpu.getFlag(CPU::Z) == false);
    }

    // Absolute
    SUBCASE("Absolute")
    {
        cpu.memory[0x8000] = 0xAE; // LDX Absolute opcode
        cpu.memory[0x8001] = 0x34; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address (0x1234)
        cpu.memory[0x1234] = 0x77; // Value to load
        cpu.execute();
        CHECK(cpu.X == 0x77);
        CHECK(cpu.getFlag(CPU::N) == false);
    }

    // Absolute,Y
    SUBCASE("Absolute,Y")
    {
        cpu.Y = 0x05;              // Set Y register
        cpu.memory[0x8000] = 0xBE; // LDX Absolute,Y opcode
        cpu.memory[0x8001] = 0x00; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address (0x1200)
        cpu.memory[0x1205] = 0xAA; // Value to load (0x1200 + 0x05)
        cpu.execute();
        CHECK(cpu.X == 0xAA);
        CHECK(cpu.getFlag(CPU::N) == true);
    }
}

TEST_CASE("LDY - Load Y Register")
{
    CPU cpu;

    // Setup test memory and reset the CPU
    cpu.reset();
    cpu.memory[0x8000] = 0; // Clear memory before tests

    // Immediate Mode
    SUBCASE("Immediate Mode")
    {
        cpu.memory[0x8000] = 0xA0; // LDY Immediate opcode
        cpu.memory[0x8001] = 0x42; // Value to load
        cpu.execute();
        CHECK(cpu.Y == 0x42);
        CHECK(cpu.getFlag(CPU::Z) == false);
        CHECK(cpu.getFlag(CPU::N) == false);

        cpu.reset();
        cpu.memory[0x8001] = 0x00; // Zero value
        cpu.execute();
        CHECK(cpu.Y == 0x00);
        CHECK(cpu.getFlag(CPU::Z) == true);
    }

    // Zero Page
    SUBCASE("Zero Page")
    {
        cpu.memory[0x8000] = 0xA4; // LDY Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Address to load from
        cpu.memory[0x0010] = 0x84; // Value to load
        cpu.execute();
        CHECK(cpu.Y == 0x84);
        CHECK(cpu.getFlag(CPU::N) == true);
    }

    // Zero Page,X
    SUBCASE("Zero Page,X")
    {
        cpu.X = 0x03;              // Set X register
        cpu.memory[0x8000] = 0xB4; // LDY Zero Page,X opcode
        cpu.memory[0x8001] = 0x10; // Base address
        cpu.memory[0x0013] = 0x32; // Value to load (0x10 + 0x03 = 0x13)
        cpu.execute();
        CHECK(cpu.Y == 0x32);
        CHECK(cpu.getFlag(CPU::Z) == false);
    }

    // Absolute
    SUBCASE("Absolute")
    {
        cpu.memory[0x8000] = 0xAC; // LDY Absolute opcode
        cpu.memory[0x8001] = 0x34; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address (0x1234)
        cpu.memory[0x1234] = 0x77; // Value to load
        cpu.execute();
        CHECK(cpu.Y == 0x77);
        CHECK(cpu.getFlag(CPU::N) == false);
    }

    // Absolute,X
    SUBCASE("Absolute,X")
    {
        cpu.X = 0x05;              // Set X register
        cpu.memory[0x8000] = 0xBC; // LDY Absolute,X opcode
        cpu.memory[0x8001] = 0x00; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address (0x1200)
        cpu.memory[0x1205] = 0xAA; // Value to load (0x1200 + 0x05)
        cpu.execute();
        CHECK(cpu.Y == 0xAA);
        CHECK(cpu.getFlag(CPU::N) == true);
    }
}

TEST_CASE("LSR - Logical Shift Right")
{
    CPU cpu;

    // Reset CPU and clear memory
    cpu.reset();
    cpu.memory.fill(0);

    // Accumulator Mode
    SUBCASE("Accumulator Mode")
    {
        cpu.A = 0b10010110;        // Example value (bit 0 is 0, so Carry should be cleared)
        cpu.memory[0x8000] = 0x4A; // LSR Accumulator opcode
        cpu.execute();
        CHECK(cpu.A == 0b01001011);          // Right shift
        CHECK(cpu.getFlag(CPU::C) == false); // Carry should not be set (bit 0 was 0)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag should not be set
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag should not be set

        // Test with bit 0 set
        cpu.reset();
        cpu.A = 0b00000001;        // Example value (bit 0 is 1, so Carry should be set)
        cpu.memory[0x8000] = 0x4A; // LSR Accumulator opcode
        cpu.execute();
        CHECK(cpu.A == 0b00000000);          // Right shift
        CHECK(cpu.getFlag(CPU::C) == true);  // Carry should be set (bit 0 was 1)
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag should be set
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag should not be set
    }

    // Zero Page Mode
    SUBCASE("Zero Page Mode")
    {
        cpu.memory[0x8000] = 0x46;       // LSR Zero Page opcode
        cpu.memory[0x8001] = 0x10;       // Address
        cpu.memory[0x0010] = 0b00000001; // Initial value
        cpu.execute();
        CHECK(cpu.memory[0x0010] == 0b00000000); // Right shift
        CHECK(cpu.getFlag(CPU::C) == true);      // Carry should be set
        CHECK(cpu.getFlag(CPU::Z) == true);      // Zero flag should be set
        CHECK(cpu.getFlag(CPU::N) == false);     // Negative flag should not be set
    }

    // Zero Page,X Mode
    SUBCASE("Zero Page,X Mode")
    {
        cpu.X = 0x04;                    // Set X register
        cpu.memory[0x8000] = 0x56;       // LSR Zero Page,X opcode
        cpu.memory[0x8001] = 0x10;       // Base address
        cpu.memory[0x0014] = 0b11111110; // Initial value
        cpu.execute();
        CHECK(cpu.memory[0x0014] == 0b01111111); // Right shift
        CHECK(cpu.getFlag(CPU::C) == false);     // Carry should not be set
        CHECK(cpu.getFlag(CPU::Z) == false);     // Zero flag should not be set
        CHECK(cpu.getFlag(CPU::N) == false);     // Negative flag should not be set
    }

    // Absolute Mode
    SUBCASE("Absolute Mode")
    {
        cpu.memory[0x8000] = 0x4E;       // LSR Absolute opcode
        cpu.memory[0x8001] = 0x34;       // Low byte
        cpu.memory[0x8002] = 0x12;       // High byte
        cpu.memory[0x1234] = 0b00000100; // Initial value
        cpu.execute();
        CHECK(cpu.memory[0x1234] == 0b00000010); // Right shift
        CHECK(cpu.getFlag(CPU::C) == false);     // Carry should not be set
        CHECK(cpu.getFlag(CPU::Z) == false);     // Zero flag should not be set
        CHECK(cpu.getFlag(CPU::N) == false);     // Negative flag should not be set
    }

    // Absolute,X Mode
    SUBCASE("Absolute,X Mode")
    {
        cpu.X = 0x05;                    // Set X register
        cpu.memory[0x8000] = 0x5E;       // LSR Absolute,X opcode
        cpu.memory[0x8001] = 0x00;       // Low byte
        cpu.memory[0x8002] = 0x12;       // High byte
        cpu.memory[0x1205] = 0b10000001; // Initial value
        cpu.execute();
        CHECK(cpu.memory[0x1205] == 0b01000000); // Right shift
        CHECK(cpu.getFlag(CPU::C) == true);      // Carry should be set
        CHECK(cpu.getFlag(CPU::Z) == false);     // Zero flag should not be set
        CHECK(cpu.getFlag(CPU::N) == false);     // Negative flag should not be set
    }
}

TEST_CASE("NOP - No Operation")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Implied Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0x42;
        cpu.X = 0x24;
        cpu.Y = 0x12;
        cpu.P = 0x00;
        cpu.SP = 0xFF;

        cpu.memory[0x8000] = 0xEA; // NOP opcode

        cpu.execute();

        CHECK(cpu.PC == 0x8001); // Program counter should increment by 1
        CHECK(cpu.A == 0x42);    // Accumulator should remain unchanged
        CHECK(cpu.X == 0x24);    // X register should remain unchanged
        CHECK(cpu.Y == 0x12);    // Y register should remain unchanged
        CHECK(cpu.P == 0x00);    // Status register should remain unchanged
        CHECK(cpu.SP == 0xFF);   // Stack pointer should remain unchanged
    }
}

TEST_CASE("ORA - Bitwise OR")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Immediate Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b01010101;
        cpu.memory[0x8000] = 0x09;       // ORA opcode
        cpu.memory[0x8001] = 0b10101010; // Immediate value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should be ORed
        CHECK(cpu.PC == 0x8002);             // PC should increment by 2
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative (bit 7 is set)
    }

    SUBCASE("Zero Page Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b00000000;
        cpu.memory[0x8000] = 0x05;       // ORA opcode
        cpu.memory[0x8001] = 0x10;       // Zero Page address
        cpu.memory[0x0010] = 0b00001111; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b00001111);          // A should be ORed
        CHECK(cpu.PC == 0x8002);             // PC should increment by 2
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == false); // Result is not negative
    }

    SUBCASE("Absolute,X Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b11111111;
        cpu.X = 0x01;
        cpu.memory[0x8000] = 0x1D;       // ORA opcode
        cpu.memory[0x8001] = 0x00;       // Low byte of address
        cpu.memory[0x8002] = 0x10;       // High byte of address
        cpu.memory[0x1001] = 0b10101010; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should remain unchanged (OR with 1's is itself)
        CHECK(cpu.PC == 0x8003);             // PC should increment by 3
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }

    // Add similar tests for other addressing modes...
}

TEST_CASE("ORA - Bitwise OR")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Immediate Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b01010101;
        cpu.memory[0x8000] = 0x09;       // ORA opcode
        cpu.memory[0x8001] = 0b10101010; // Immediate value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should be ORed
        CHECK(cpu.PC == 0x8002);             // PC should increment by 2
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }

    SUBCASE("Zero Page Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b00000001;
        cpu.memory[0x8000] = 0x05;       // ORA opcode
        cpu.memory[0x8001] = 0x10;       // Zero Page address
        cpu.memory[0x0010] = 0b00000010; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b00000011);          // A should be ORed
        CHECK(cpu.PC == 0x8002);             // PC should increment by 2
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == false); // Result is not negative
    }

    SUBCASE("Zero Page,X Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b00000000;
        cpu.X = 0x05;
        cpu.memory[0x8000] = 0x15;       // ORA opcode
        cpu.memory[0x8001] = 0x10;       // Base Zero Page address
        cpu.memory[0x0015] = 0b11110000; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b11110000);          // A should be ORed
        CHECK(cpu.PC == 0x8002);             // PC should increment by 2
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }

    SUBCASE("Absolute Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b00001111;
        cpu.memory[0x8000] = 0x0D;       // ORA opcode
        cpu.memory[0x8001] = 0x00;       // Low byte of address
        cpu.memory[0x8002] = 0x20;       // High byte of address
        cpu.memory[0x2000] = 0b11110000; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should be ORed
        CHECK(cpu.PC == 0x8003);             // PC should increment by 3
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }

    SUBCASE("Absolute,X Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b01010101;
        cpu.X = 0x01;
        cpu.memory[0x8000] = 0x1D;       // ORA opcode
        cpu.memory[0x8001] = 0x00;       // Low byte of address
        cpu.memory[0x8002] = 0x20;       // High byte of address
        cpu.memory[0x2001] = 0b10101010; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should be ORed
        CHECK(cpu.PC == 0x8003);             // PC should increment by 3
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }

    SUBCASE("Absolute,Y Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b00001111;
        cpu.Y = 0x01;
        cpu.memory[0x8000] = 0x19;       // ORA opcode
        cpu.memory[0x8001] = 0x00;       // Low byte of address
        cpu.memory[0x8002] = 0x30;       // High byte of address
        cpu.memory[0x3001] = 0b11110000; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should be ORed
        CHECK(cpu.PC == 0x8003);             // PC should increment by 3
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }

    SUBCASE("Indirect,X Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b00001111;
        cpu.X = 0x04;
        cpu.memory[0x8000] = 0x01;       // ORA opcode
        cpu.memory[0x8001] = 0x10;       // Base Zero Page address
        cpu.memory[0x0014] = 0x00;       // Low byte of target address
        cpu.memory[0x0015] = 0x40;       // High byte of target address
        cpu.memory[0x4000] = 0b11110000; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should be ORed
        CHECK(cpu.PC == 0x8002);             // PC should increment by 2
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }

    SUBCASE("Indirect,Y Mode")
    {
        cpu.PC = 0x8000;
        cpu.A = 0b00001111;
        cpu.Y = 0x01;
        cpu.memory[0x8000] = 0x11;       // ORA opcode
        cpu.memory[0x8001] = 0x10;       // Base Zero Page address
        cpu.memory[0x0010] = 0x00;       // Low byte of target address
        cpu.memory[0x0011] = 0x50;       // High byte of target address
        cpu.memory[0x5001] = 0b11110000; // Memory value

        cpu.execute();

        CHECK(cpu.A == 0b11111111);          // A should be ORed
        CHECK(cpu.PC == 0x8002);             // PC should increment by 2
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is non-zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Result is negative
    }
}

TEST_CASE("PHA - Push A")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Push Accumulator to Stack")
    {
        cpu.PC = 0x8000;
        cpu.A = 0x42;              // Accumulator value
        cpu.SP = 0xFF;             // Stack Pointer starts at $FF
        cpu.memory[0x8000] = 0x48; // PHA opcode

        cpu.execute();

        CHECK(cpu.memory[0x0100 + 0xFF] == 0x42); // Value pushed to stack
        CHECK(cpu.SP == 0xFE);                    // Stack Pointer decremented
        CHECK(cpu.PC == 0x8001);                  // Program Counter incremented
    }

    SUBCASE("Push Multiple Values")
    {
        cpu.PC = 0x8000;
        cpu.SP = 0xFF;

        // Push value 1
        cpu.A = 0xAA;
        cpu.memory[0x8000] = 0x48; // PHA opcode
        cpu.execute();
        CHECK(cpu.memory[0x0100 + 0xFF] == 0xAA);
        CHECK(cpu.SP == 0xFE);

        // Push value 2
        cpu.A = 0xBB;
        cpu.memory[0x8001] = 0x48; // PHA opcode
        cpu.execute();
        CHECK(cpu.memory[0x0100 + 0xFE] == 0xBB);
        CHECK(cpu.SP == 0xFD);

        // PC should reflect the next instruction
        CHECK(cpu.PC == 0x8002);
    }
}

TEST_CASE("PHP - Push Processor Status")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Push Processor Status to Stack")
    {
        cpu.PC = 0x8000;
        cpu.SP = 0xFF;             // Stack Pointer starts at $FF
        cpu.P = 0b11001001;        // Sample processor status (NVBDIZC)
        cpu.memory[0x8000] = 0x08; // PHP opcode

        cpu.execute();

        CHECK(cpu.memory[0x0100 + 0xFF] == (cpu.P | 0x30)); // Status pushed with B=1, bit 4=1
        CHECK(cpu.SP == 0xFE);                              // Stack Pointer decremented
        CHECK(cpu.PC == 0x8001);                            // Program Counter incremented
    }

    SUBCASE("Multiple PHP Instructions")
    {
        cpu.PC = 0x8000;
        cpu.SP = 0xFF;

        // First PHP
        cpu.P = 0b10101010;        // NVBDIZC sample
        cpu.memory[0x8000] = 0x08; // PHP opcode
        cpu.execute();
        CHECK(cpu.memory[0x0100 + 0xFF] == (cpu.P | 0x30));
        CHECK(cpu.SP == 0xFE);

        // Second PHP
        cpu.P = 0b11110000;        // NVBDIZC sample
        cpu.memory[0x8001] = 0x08; // PHP opcode
        cpu.execute();
        CHECK(cpu.memory[0x0100 + 0xFE] == (cpu.P | 0x30));
        CHECK(cpu.SP == 0xFD);

        // PC should reflect the next instruction
        CHECK(cpu.PC == 0x8002);
    }
}

TEST_CASE("PLA - Pull A")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Pull Value from Stack into A")
    {
        cpu.PC = 0x8000;
        cpu.SP = 0xFC;                    // Stack Pointer starts at $FC
        cpu.memory[0x0100 + 0xFD] = 0x42; // Value on the stack at $01FD
        cpu.memory[0x8000] = 0x68;        // PLA opcode

        cpu.execute();

        CHECK(cpu.A == 0x42);                // Check accumulator
        CHECK(cpu.SP == 0xFD);               // Stack Pointer incremented
        CHECK(cpu.PC == 0x8001);             // Program Counter incremented
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag not set
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag not set
    }

    SUBCASE("Set Zero Flag when Pulling Zero")
    {
        cpu.PC = 0x8000;
        cpu.SP = 0xFB;
        cpu.memory[0x0100 + 0xFC] = 0x00; // Value on the stack at $01FC
        cpu.memory[0x8000] = 0x68;        // PLA opcode

        cpu.execute();

        CHECK(cpu.A == 0x00);                // Check accumulator
        CHECK(cpu.SP == 0xFC);               // Stack Pointer incremented
        CHECK(cpu.PC == 0x8001);             // Program Counter incremented
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag set
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag not set
    }

    SUBCASE("Set Negative Flag when Pulling Negative Value")
    {
        cpu.PC = 0x8000;
        cpu.SP = 0xFB;
        cpu.memory[0x0100 + 0xFC] = 0x80; // Value on the stack at $01FC (negative in 2's complement)
        cpu.memory[0x8000] = 0x68;        // PLA opcode

        cpu.execute();

        CHECK(cpu.A == 0x80);                // Check accumulator
        CHECK(cpu.SP == 0xFC);               // Stack Pointer incremented
        CHECK(cpu.PC == 0x8001);             // Program Counter incremented
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag not set
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag set
    }
}

// TEST_CASE("PLP - Pull Processor Status") {
//     CPU cpu;
//     cpu.reset();

//     SUBCASE("Pull Processor Status from Stack") {
//         cpu.PC = 0x8000;
//         cpu.SP = 0xFB;
//         cpu.memory[0x0100 + 0xFC] = 0b11110110; // Value on the stack at $01FC
//         cpu.memory[0x8000] = 0x28;             // PLP opcode

//         cpu.execute();

//         CHECK(cpu.P == 0b11010110);    // Expected Processor Status
//         CHECK(cpu.SP == 0xFC);         // Stack Pointer incremented
//         CHECK(cpu.getFlag(CPU::C) == true);  // Carry flag
//         CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag
//         CHECK(cpu.getFlag(CPU::I) == false); // Interrupt Disable flag
//         CHECK(cpu.getFlag(CPU::D) == true);  // Decimal flag
//         CHECK(cpu.getFlag(CPU::V) == true);  // Overflow flag
//         CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag
//     }

//     SUBCASE("Ignore B Flag") {
//         cpu.PC = 0x8000;
//         cpu.SP = 0xFB;
//         cpu.memory[0x0100 + 0xFC] = 0b11110110; // Value on the stack with B set
//         cpu.memory[0x8000] = 0x28;             // PLP opcode

//         cpu.execute();

//         CHECK(cpu.P == 0b11010110);    // B flag ignored
//         CHECK(cpu.SP == 0xFC);         // Stack Pointer incremented
//     }

//     SUBCASE("Clear Processor Status") {
//         cpu.PC = 0x8000;
//         cpu.SP = 0xFB;
//         cpu.memory[0x0100 + 0xFC] = 0b00000000; // Value on the stack to clear all flags
//         cpu.memory[0x8000] = 0x28;             // PLP opcode

//         cpu.execute();

//         CHECK(cpu.P == 0b00000000);    // All flags cleared
//         CHECK(cpu.SP == 0xFC);         // Stack Pointer incremented
//     }
// }

TEST_CASE("ROL - Rotate Left")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("ROL Accumulator")
    {
        cpu.PC = 0x8000;
        cpu.A = 0x81;              // Initial A
        cpu.setFlag(CPU::C, true); // Initial Carry
        cpu.memory[0x8000] = 0x2A; // ROL opcode

        cpu.execute();

        CHECK(cpu.A == 0x03);                // A = 0x81 << 1 | 1 = 0x03
        CHECK(cpu.getFlag(CPU::C));          // Carry = 1 (Bit 7 of 0x81)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero = false
        CHECK(cpu.getFlag(CPU::N) == false); // Negative = false
    }

    SUBCASE("ROL Zero Page")
    {
        cpu.PC = 0x8000;
        cpu.memory[0x10] = 0x81;    // Initial value at $10
        cpu.setFlag(CPU::C, false); // Initial Carry
        cpu.memory[0x8000] = 0x26;  // ROL opcode
        cpu.memory[0x8001] = 0x10;  // Zero Page address

        cpu.execute();

        CHECK(cpu.memory[0x10] == 0x02);     // 0x81 << 1 = 0x02
        CHECK(cpu.getFlag(CPU::C));          // Carry = 1 (Bit 7 of 0x81)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero = false
        CHECK(cpu.getFlag(CPU::N) == false); // Negative = false
    }

    SUBCASE("ROL Absolute")
    {
        cpu.PC = 0x8000;
        cpu.memory[0x1234] = 0xFF; // Initial value at $1234
        cpu.setFlag(CPU::C, true); // Initial Carry
        cpu.memory[0x8000] = 0x2E; // ROL opcode
        cpu.memory[0x8001] = 0x34; // Absolute address low byte
        cpu.memory[0x8002] = 0x12; // Absolute address high byte

        cpu.execute();

        CHECK(cpu.memory[0x1234] == 0xFF);   // 0xFF << 1 | 1 = 0xFF
        CHECK(cpu.getFlag(CPU::C));          // Carry = 1 (Bit 7 of 0xFF)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero = false
        CHECK(cpu.getFlag(CPU::N));          // Negative = true
    }
}

TEST_CASE("ROR - Rotate Right")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("ROR Accumulator")
    {
        cpu.PC = 0x8000;
        cpu.A = 0x03;              // Initial A
        cpu.setFlag(CPU::C, true); // Initial Carry
        cpu.memory[0x8000] = 0x6A; // ROR opcode

        cpu.execute();

        CHECK(cpu.A == 0x81);                // A = 0x03 >> 1 | 0x80 = 0x81
        CHECK(cpu.getFlag(CPU::C));          // Carry = 1 (Bit 0 of 0x03)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero = false
        CHECK(cpu.getFlag(CPU::N));          // Negative = true
    }

    SUBCASE("ROR Zero Page")
    {
        cpu.PC = 0x8000;
        cpu.memory[0x10] = 0x01;    // Initial value at $10
        cpu.setFlag(CPU::C, false); // Initial Carry
        cpu.memory[0x8000] = 0x66;  // ROR opcode
        cpu.memory[0x8001] = 0x10;  // Zero Page address

        cpu.execute();

        CHECK(cpu.memory[0x10] == 0x00);     // 0x01 >> 1 = 0x00
        CHECK(cpu.getFlag(CPU::C));          // Carry = 1 (Bit 0 of 0x01)
        CHECK(cpu.getFlag(CPU::Z));          // Zero = true
        CHECK(cpu.getFlag(CPU::N) == false); // Negative = false
    }

    SUBCASE("ROR Absolute")
    {
        cpu.PC = 0x8000;
        cpu.memory[0x1234] = 0xFE; // Initial value at $1234
        cpu.setFlag(CPU::C, true); // Initial Carry
        cpu.memory[0x8000] = 0x6E; // ROR opcode
        cpu.memory[0x8001] = 0x34; // Absolute address low byte
        cpu.memory[0x8002] = 0x12; // Absolute address high byte

        cpu.execute();

        CHECK(cpu.memory[0x1234] == 0xFF);   // 0xFE >> 1 | 0x80 = 0xFF
        CHECK(cpu.getFlag(CPU::C) == false); // Carry = 0 (Bit 0 of 0xFE)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero = false
        CHECK(cpu.getFlag(CPU::N));          // Negative = true
    }
}

// TEST_CASE("RTI - Return from Interrupt") {
//     CPU cpu;
//     cpu.reset();

//     SUBCASE("RTI restores flags and PC correctly") {
//         // Setup stack with flags and PC values
//         cpu.memory[0x01FF] = 0xD6; // Processor flags (P)
//         cpu.memory[0x01FE] = 0x34; // Low byte of PC
//         cpu.memory[0x01FD] = 0x12; // High byte of PC
//         cpu.SP = 0xFC;             // SP points below pushed values

//         // Place RTI opcode in memory
//         cpu.memory[0x8000] = 0x40; // RTI opcode
//         cpu.PC = 0x8000;

//         cpu.execute();

//         // Validate restored flags
//         CHECK(cpu.P == 0xD6);
//         CHECK(cpu.getFlag(CPU::C) == false);
//         CHECK(cpu.getFlag(CPU::Z) == true);
//         CHECK(cpu.getFlag(CPU::I) == true);
//         CHECK(cpu.getFlag(CPU::D) == false);
//         CHECK(cpu.getFlag(CPU::V) == true);
//         CHECK(cpu.getFlag(CPU::N) == true);

//         // Validate restored PC
//         CHECK(cpu.PC == 0x1234);
//     }

//     SUBCASE("RTI handles immediate IRQ flag changes correctly") {
//         // Setup stack with different flags and PC values
//         cpu.memory[0x01FF] = 0xF3; // Processor flags (P)
//         cpu.memory[0x01FE] = 0x00; // Low byte of PC
//         cpu.memory[0x01FD] = 0x80; // High byte of PC
//         cpu.SP = 0xFC;             // SP points below pushed values

//         // Place RTI opcode in memory
//         cpu.memory[0x8000] = 0x40; // RTI opcode
//         cpu.PC = 0x8000;

//         cpu.execute();

//         // Validate restored flags
//         CHECK(cpu.P == 0xF3);
//         CHECK(cpu.getFlag(CPU::C) == true);
//         CHECK(cpu.getFlag(CPU::Z) == true);
//         CHECK(cpu.getFlag(CPU::I) == false);
//         CHECK(cpu.getFlag(CPU::D) == true);
//         CHECK(cpu.getFlag(CPU::V) == false);
//         CHECK(cpu.getFlag(CPU::N) == true);

//         // Validate restored PC
//         CHECK(cpu.PC == 0x8000);
//     }
// }

// TEST_CASE("SBC - Subtract with Carry") {
//     CPU cpu;
//     cpu.reset();

//     SUBCASE("SBC Immediate - No Borrow") {
//         cpu.A = 0x10;              // Initial A
//         cpu.setFlag(CPU::C, true); // Set carry to prevent borrow
//         cpu.memory[0x8000] = 0xE9; // SBC Immediate opcode
//         cpu.memory[0x8001] = 0x01; // Value to subtract
//         cpu.PC = 0x8000;

//         cpu.execute();

//         CHECK(cpu.A == 0x0F);          // 0x10 - 0x01 = 0x0F
//         CHECK(cpu.getFlag(CPU::C));    // No borrow
//         CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
//         CHECK(cpu.getFlag(CPU::N) == false); // Positive result
//     }

//     SUBCASE("SBC Zero Page") {
//         cpu.A = 0x50;
//         cpu.setFlag(CPU::C, true);
//         cpu.memory[0x10] = 0x20;       // Value at zero page address
//         cpu.memory[0x8000] = 0xE5;     // SBC Zero Page opcode
//         cpu.memory[0x8001] = 0x10;     // Zero Page address
//         cpu.PC = 0x8000;

//         cpu.execute();

//         CHECK(cpu.A == 0x30);          // 0x50 - 0x20 = 0x30
//         CHECK(cpu.getFlag(CPU::C));    // No borrow
//         CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
//         CHECK(cpu.getFlag(CPU::N) == false); // Positive result
//     }

//     SUBCASE("SBC Absolute - With Borrow") {
//     cpu.A = 0x10;                  // Initialize A
//     cpu.setFlag(CPU::C, false);    // Clear carry to introduce borrow
//     cpu.memory[0x1234] = 0x01;     // Value at absolute address
//     cpu.memory[0x8000] = 0xED;     // SBC Absolute opcode
//     cpu.memory[0x8001] = 0x34;     // Low byte of address
//     cpu.memory[0x8002] = 0x12;     // High byte of address
//     cpu.PC = 0x8000;

//     std::cerr << "Testing SBC Absolute - With Borrow..." << std::endl;

//     cpu.execute();

//     // Check the result
//     CHECK(cpu.A == 0x0E);          // 0x10 - 0x01 - 1 = 0x0E
//     CHECK(cpu.getFlag(CPU::C) == false); // Borrow occurred
//     CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
//     CHECK(cpu.getFlag(CPU::N) == false); // Positive result
// }
// }

TEST_CASE("SEC - Set Carry")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("SEC Implied")
    {
        cpu.setFlag(CPU::C, false); // Clear the carry flag initially
        cpu.memory[0x8000] = 0x38;  // SEC opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.getFlag(CPU::C) == true); // Ensure the carry flag is set
    }
}

TEST_CASE("SED - Set Decimal")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("SED Implied")
    {
        cpu.setFlag(CPU::D, false); // Clear the decimal flag initially
        cpu.memory[0x8000] = 0xF8;  // SED opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.getFlag(CPU::D) == true); // Ensure the decimal flag is set
    }
}

TEST_CASE("SEI - Set Interrupt Disable")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("SEI Implied")
    {
        cpu.setFlag(CPU::I, false); // Clear the interrupt disable flag initially
        cpu.memory[0x8000] = 0x78;  // SEI opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.getFlag(CPU::I) == true); // Ensure the interrupt disable flag is set
    }
}

TEST_CASE("STA - Store Accumulator")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("STA Zero Page")
    {
        cpu.A = 0x42;              // Set the accumulator
        cpu.memory[0x8000] = 0x85; // STA Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Zero Page address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x10] == 0x42); // Ensure value is stored in memory
    }

    SUBCASE("STA Absolute")
    {
        cpu.A = 0x99;              // Set the accumulator
        cpu.memory[0x8000] = 0x8D; // STA Absolute opcode
        cpu.memory[0x8001] = 0x34; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x1234] == 0x99); // Ensure value is stored in memory
    }

    SUBCASE("STA (Indirect,X)")
    {
        cpu.A = 0x77;              // Set the accumulator
        cpu.X = 0x04;              // Set X register
        cpu.memory[0x8000] = 0x81; // STA (Indirect,X) opcode
        cpu.memory[0x8001] = 0x10; // Base zero-page address
        cpu.memory[0x14] = 0x78;   // Low byte of target address
        cpu.memory[0x15] = 0x56;   // High byte of target address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x5678] == 0x77); // Ensure value is stored in memory
    }
}

TEST_CASE("STX - Store X Register")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("STX Zero Page")
    {
        cpu.X = 0x42;              // Set the X register
        cpu.memory[0x8000] = 0x86; // STX Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Zero Page address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x10] == 0x42); // Ensure value is stored in memory
    }

    SUBCASE("STX Zero Page,Y")
    {
        cpu.X = 0x77;              // Set the X register
        cpu.Y = 0x04;              // Set the Y register
        cpu.memory[0x8000] = 0x96; // STX Zero Page,Y opcode
        cpu.memory[0x8001] = 0x10; // Base zero-page address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x14] == 0x77); // Ensure value is stored in memory
    }

    SUBCASE("STX Absolute")
    {
        cpu.X = 0x99;              // Set the X register
        cpu.memory[0x8000] = 0x8E; // STX Absolute opcode
        cpu.memory[0x8001] = 0x34; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x1234] == 0x99); // Ensure value is stored in memory
    }
}

TEST_CASE("STY - Store Y Register")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("STY Zero Page")
    {
        cpu.Y = 0x33;              // Set the Y register
        cpu.memory[0x8000] = 0x84; // STY Zero Page opcode
        cpu.memory[0x8001] = 0x20; // Zero Page address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x20] == 0x33); // Ensure value is stored in memory
    }

    SUBCASE("STY Zero Page,X")
    {
        cpu.Y = 0x55;              // Set the Y register
        cpu.X = 0x04;              // Set the X register
        cpu.memory[0x8000] = 0x94; // STY Zero Page,X opcode
        cpu.memory[0x8001] = 0x10; // Base zero-page address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x14] == 0x55); // Ensure value is stored in memory
    }

    SUBCASE("STY Absolute")
    {
        cpu.Y = 0xAA;              // Set the Y register
        cpu.memory[0x8000] = 0x8C; // STY Absolute opcode
        cpu.memory[0x8001] = 0x34; // Low byte of address
        cpu.memory[0x8002] = 0x12; // High byte of address
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.memory[0x1234] == 0xAA); // Ensure value is stored in memory
    }
}

TEST_CASE("TAX - Transfer A to X")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("TAX - Non-Zero, Positive")
    {
        cpu.A = 0x50;              // Set A to a positive value
        cpu.memory[0x8000] = 0xAA; // TAX opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.X == 0x50);                // Ensure X matches A
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == false); // Positive result
    }

    SUBCASE("TAX - Zero Value")
    {
        cpu.A = 0x00;              // Set A to zero
        cpu.memory[0x8000] = 0xAA; // TAX opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.X == 0x00);                // Ensure X matches A
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag set
        CHECK(cpu.getFlag(CPU::N) == false); // Not negative
    }

    SUBCASE("TAX - Negative Value")
    {
        cpu.A = 0x80;              // Set A to a negative value
        cpu.memory[0x8000] = 0xAA; // TAX opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.X == 0x80);                // Ensure X matches A
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag set
    }
}

TEST_CASE("TAY - Transfer A to Y")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("TAY - Non-Zero, Positive")
    {
        cpu.A = 0x40;              // Set A to a positive value
        cpu.memory[0x8000] = 0xA8; // TAY opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.Y == 0x40);                // Ensure Y matches A
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == false); // Positive result
    }

    SUBCASE("TAY - Zero Value")
    {
        cpu.A = 0x00;              // Set A to zero
        cpu.memory[0x8000] = 0xA8; // TAY opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.Y == 0x00);                // Ensure Y matches A
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag set
        CHECK(cpu.getFlag(CPU::N) == false); // Not negative
    }

    SUBCASE("TAY - Negative Value")
    {
        cpu.A = 0x80;              // Set A to a negative value
        cpu.memory[0x8000] = 0xA8; // TAY opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.Y == 0x80);                // Ensure Y matches A
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag set
    }
}
TEST_CASE("TSX - Transfer Stack Pointer to X")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("TSX - Non-Zero, Positive")
    {
        cpu.SP = 0x40;             // Set SP to a positive value
        cpu.memory[0x8000] = 0xBA; // TSX opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.X == 0x40);                // Ensure X matches SP
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == false); // Positive result
    }

    SUBCASE("TSX - Zero Value")
    {
        cpu.SP = 0x00;             // Set SP to zero
        cpu.memory[0x8000] = 0xBA; // TSX opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.X == 0x00);                // Ensure X matches SP
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag set
        CHECK(cpu.getFlag(CPU::N) == false); // Not negative
    }

    SUBCASE("TSX - Negative Value")
    {
        cpu.SP = 0x80;             // Set SP to a negative value
        cpu.memory[0x8000] = 0xBA; // TSX opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.X == 0x80);                // Ensure X matches SP
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag set
    }
}

TEST_CASE("TXA - Transfer X to A")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("TXA - Non-Zero, Positive")
    {
        cpu.X = 0x40;              // Set X to a positive value
        cpu.memory[0x8000] = 0x8A; // TXA opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.A == 0x40);                // Ensure A matches X
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == false); // Positive result
    }

    SUBCASE("TXA - Zero Value")
    {
        cpu.X = 0x00;              // Set X to zero
        cpu.memory[0x8000] = 0x8A; // TXA opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.A == 0x00);                // Ensure A matches X
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag set
        CHECK(cpu.getFlag(CPU::N) == false); // Not negative
    }

    SUBCASE("TXA - Negative Value")
    {
        cpu.X = 0x80;              // Set X to a negative value
        cpu.memory[0x8000] = 0x8A; // TXA opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.A == 0x80);                // Ensure A matches X
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag set
    }
}
TEST_CASE("TXS - Transfer X to Stack Pointer")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("TXS - Regular Value")
    {
        cpu.X = 0x50;              // Set X to a typical value
        cpu.memory[0x8000] = 0x9A; // TXS opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.SP == 0x50); // Ensure SP matches X
    }

    SUBCASE("TXS - Zero Value")
    {
        cpu.X = 0x00;              // Set X to zero
        cpu.memory[0x8000] = 0x9A; // TXS opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.SP == 0x00); // Ensure SP matches X
    }

    SUBCASE("TXS - Maximum Value")
    {
        cpu.X = 0xFF;              // Set X to maximum value
        cpu.memory[0x8000] = 0x9A; // TXS opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.SP == 0xFF); // Ensure SP matches X
    }
}

TEST_CASE("TYA - Transfer Y to A")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("TYA - Positive Value")
    {
        cpu.Y = 0x50;              // Set Y to a positive value
        cpu.memory[0x8000] = 0x98; // TYA opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.A == 0x50);                // Ensure A matches Y
        CHECK(cpu.getFlag(CPU::Z) == false); // Not zero
        CHECK(cpu.getFlag(CPU::N) == false); // Positive result
    }

    SUBCASE("TYA - Zero Value")
    {
        cpu.Y = 0x00;              // Set Y to zero
        cpu.memory[0x8000] = 0x98; // TYA opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.A == 0x00);                // Ensure A matches Y
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag should be set
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag should not be set
    }

    SUBCASE("TYA - Negative Value")
    {
        cpu.Y = 0x80;              // Set Y to a negative value
        cpu.memory[0x8000] = 0x98; // TYA opcode
        cpu.PC = 0x8000;

        cpu.execute();

        CHECK(cpu.A == 0x80);                // Ensure A matches Y
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag should not be set
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag should be set
    }
}

TEST_CASE("BRK - Break (Software IRQ)")
{
    CPU cpu;
    cpu.reset();

    // Set up the IRQ handler address at $FFFE/$FFFF
    cpu.memory[0xFFFE] = 0x34; // Low byte of handler address
    cpu.memory[0xFFFF] = 0x12; // High byte of handler address

    SUBCASE("BRK - Normal Operation")
    {
        cpu.PC = 0x8000;           // Starting PC
        cpu.memory[0x8000] = 0x00; // BRK opcode
        cpu.P = 0x30;              // Initial processor status flags
        cpu.SP = 0xFF;             // Stack pointer at initial position

        cpu.execute();

        // Check pushed values on the stack
        CHECK(cpu.memory[0x01FF] == 0x80);          // High byte of PC+1 (0x8002)
        CHECK(cpu.memory[0x01FE] == 0x02);          // Low byte of PC+1
        CHECK(cpu.memory[0x01FD] == (0x30 | 0x10)); // Flags with Break set

        // Check SP
        CHECK(cpu.SP == 0xFC);

        // Check PC after BRK
        CHECK(cpu.PC == 0x1234); // PC should jump to handler address $1234

        // Check I flag
        CHECK(cpu.getFlag(CPU::I) == true); // Interrupt Disable flag should be set
    }

    SUBCASE("BRK - Nested Interrupt")
    {
        // Simulate a nested interrupt scenario
        cpu.PC = 0x8000;
        cpu.memory[0x8000] = 0x00; // BRK opcode
        cpu.P = 0x20;              // Processor flags with Interrupt Disable already set
        cpu.SP = 0xFE;             // Stack pointer with one entry already used

        cpu.execute();

        // Check pushed values on the stack
        CHECK(cpu.memory[0x01FE] == 0x80);          // High byte of PC+1 (0x8002)
        CHECK(cpu.memory[0x01FD] == 0x02);          // Low byte of PC+1
        CHECK(cpu.memory[0x01FC] == (0x20 | 0x10)); // Flags with Break set

        // Check SP
        CHECK(cpu.SP == 0xFB);

        // Check PC after BRK
        CHECK(cpu.PC == 0x1234);

        // Check I flag remains set
        CHECK(cpu.getFlag(CPU::I) == true);
    }
}

TEST_CASE("Stack Operations")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Push to Stack")
    {
        // Initial stack pointer should be 0xFF
        CHECK(cpu.SP == 0xFF);

        // Push values onto the stack
        cpu.pushToStack(0x42);
        CHECK(cpu.memory[0x01FF] == 0x42);
        CHECK(cpu.SP == 0xFE);

        cpu.pushToStack(0x37);
        CHECK(cpu.memory[0x01FE] == 0x37);
        CHECK(cpu.SP == 0xFD);

        cpu.pushToStack(0x99);
        CHECK(cpu.memory[0x01FD] == 0x99);
        CHECK(cpu.SP == 0xFC);
    }

    SUBCASE("Pop from Stack")
    {
        // Preload stack with values and adjust SP
        cpu.memory[0x01FF] = 0x42;
        cpu.memory[0x01FE] = 0x37;
        cpu.memory[0x01FD] = 0x99;
        cpu.SP = 0xFC; // Stack pointer after 3 pushes

        // Pop values from the stack
        CHECK(cpu.popFromStack() == 0x99);
        CHECK(cpu.SP == 0xFD);

        CHECK(cpu.popFromStack() == 0x37);
        CHECK(cpu.SP == 0xFE);

        CHECK(cpu.popFromStack() == 0x42);
        CHECK(cpu.SP == 0xFF);
    }

    SUBCASE("Push and Pop Combination")
    {
        // Push values onto the stack
        cpu.pushToStack(0x42);
        cpu.pushToStack(0x37);

        // Pop values from the stack
        CHECK(cpu.popFromStack() == 0x37);
        CHECK(cpu.popFromStack() == 0x42);

        // Stack pointer should return to its initial value
        CHECK(cpu.SP == 0xFF);
    }
}

TEST_CASE("Stack - Push and Pop RTI Values")
{
    CPU cpu;
    cpu.reset();

    // Push flags and PC values to stack
    cpu.pushToStack(0x12); // Low byte of PC
    cpu.pushToStack(0x34); // High byte of PC
    cpu.pushToStack(0xA0); // Flags

    // Pop values back and verify
    uint8_t flags = cpu.popFromStack();
    CHECK(flags == 0xA0); // Check flags

    uint8_t pcl = cpu.popFromStack();
    uint8_t pch = cpu.popFromStack();
    uint16_t pc = (pch << 8) | pcl;
    CHECK(pc == 0x1234); // Check PC
}

TEST_CASE("RTI - Stack Interaction")
{
    CPU cpu;
    cpu.reset();

    // Set the RTI opcode at the current PC
    cpu.memory[cpu.PC] = 0x40; // RTI opcode (0x40)

    // Simulate stack values pushed during an interrupt
    cpu.pushToStack(0x12); // Low byte of PC
    cpu.pushToStack(0x34); // High byte of PC
    cpu.pushToStack(0xA0); // Flags

    // Execute RTI (should pop these values)
    cpu.execute(); // Executes the 0x40 opcode (RTI)

    // Verify restored values
    CHECK(cpu.PC == 0x1234);    // Program counter restored
    CHECK(cpu.P == 0xA0);       // Flags restored
    CHECK(cpu.getFlag(CPU::N)); // Negative flag set
}

TEST_CASE("RTI - Simplified Debug Test")
{
    CPU cpu;
    cpu.reset();

    cpu.memory[cpu.PC] = 0x40; // RTI opcode
    cpu.pushToStack(0x12);     // Low byte of PC
    cpu.pushToStack(0x34);     // High byte of PC
                               // cpu.pushToStack(0xA0); // Flags (10100000)
    cpu.pushToStack(0xB0);     // Flags: 10110000 (sets both N and V)

    cpu.execute(); // Execute RTI

    std::cerr << "[Test] PC After RTI: " << std::hex << cpu.PC << "\n";
    std::cerr << "[Test] Flags After RTI: " << std::bitset<8>(cpu.P) << "\n";

    CHECK(cpu.PC == 0x1234);
    CHECK(cpu.P == 0xB0);
    CHECK(cpu.getFlag(CPU::N) == true);
    // CHECK(cpu.getFlag(CPU::V) == true);
}
TEST_CASE("CPU - getFlag Method - Single Flag Test")
{
    CPU cpu;

    cpu.P = 0b01000000;                 // Set only V (bit 6)
    CHECK(cpu.getFlag(CPU::V) == true); // Should pass

    cpu.P = 0b00000000;                  // Clear all flags
    CHECK(cpu.getFlag(CPU::V) == false); // Should pass
}

TEST_CASE("RTS - Return from Subroutine")
{
    CPU cpu;
    cpu.reset();

    // Set up the RTS opcode at the current PC
    cpu.memory[cpu.PC] = 0x60; // RTS opcode

    // Simulate a subroutine call with a return address pushed onto the stack
    cpu.pushToStack(0x12);
    cpu.pushToStack(0x34); // Low byte of PC

    // Execute RTS (should pull PC and increment it)
    cpu.execute();

    // Verify the results
    CHECK(cpu.PC == 0x1235); // Program counter restored and incremented
}

TEST_CASE("PLP - Pull Processor Status")
{
    CPU cpu;
    cpu.reset();

    // Set initial stack pointer
    cpu.SP = 0xFD;

    // Push flags to the stack
    cpu.memory[0x0100 + 0xFE] = 0b10101010; // Flags to pull (NVxxDIZC)

    // Execute PLP
    cpu.memory[cpu.PC] = 0x28; // PLP opcode
    cpu.execute();

    // Verify the stack pointer
    CHECK(cpu.SP == 0xFE);

    // Verify the status register
    CHECK(cpu.P == 0b10101010);

    // Verify individual flags
    CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag
    CHECK(cpu.getFlag(CPU::V) == false); // Overflow flag
    CHECK(cpu.getFlag(CPU::D) == true);  // Decimal flag
    CHECK(cpu.getFlag(CPU::I) == false); // Interrupt disable flag
    CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag
    CHECK(cpu.getFlag(CPU::C) == false); // Carry flag
}

TEST_CASE("SBC Instruction")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("Immediate Mode")
    {
        cpu.A = 0x50;              // Accumulator
        cpu.setFlag(CPU::C, true); // Set carry flag to ensure no extra subtraction
        cpu.memory[0x8000] = 0xE9; // SBC Immediate opcode
        cpu.memory[0x8001] = 0x10; // Immediate value to subtract
        cpu.execute();

        CHECK(cpu.A == 0x40);                // Check accumulator value (0x50 - 0x10 = 0x40)
        CHECK(cpu.getFlag(CPU::C) == true);  // Carry flag (unsigned underflow did not occur)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag
        CHECK(cpu.getFlag(CPU::V) == false); // Overflow flag
    }

    SUBCASE("Zero Page Mode")
    {
        cpu.A = 0x10;
        cpu.setFlag(CPU::C, true);
        cpu.memory[0x8000] = 0xE5; // SBC Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Address in Zero Page
        cpu.memory[0x0010] = 0x05; // Value at address 0x0010
        cpu.execute();

        CHECK(cpu.A == 0x0B);                // Check accumulator value (0x10 - 0x05 = 0x0B)
        CHECK(cpu.getFlag(CPU::C) == true);  // Carry flag
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag
        CHECK(cpu.getFlag(CPU::V) == false); // Overflow flag
    }

    SUBCASE("Overflow Detection")
    {
        cpu.A = 0x50;              // Accumulator
        cpu.setFlag(CPU::C, true); // Set carry flag
        cpu.memory[0x8000] = 0xE9; // SBC Immediate opcode
        cpu.memory[0x8001] = 0xB0; // Immediate value to subtract
        cpu.execute();

        CHECK(cpu.A == 0xA0);                // Check accumulator value (0x50 - 0xB0 = 0xA0)
        CHECK(cpu.getFlag(CPU::C) == false); // Carry flag cleared (unsigned underflow occurred)
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag
        CHECK(cpu.getFlag(CPU::V) == true);  // Overflow flag
    }

    SUBCASE("Zero Result with Carry")
    {
        cpu.A = 0x10;              // Accumulator
        cpu.setFlag(CPU::C, true); // Set carry flag
        cpu.memory[0x8000] = 0xE9; // SBC Immediate opcode
        cpu.memory[0x8001] = 0x10; // Immediate value to subtract
        cpu.execute();

        CHECK(cpu.A == 0x00);                // Result should be zero
        CHECK(cpu.getFlag(CPU::C) == true);  // Carry flag
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag
        CHECK(cpu.getFlag(CPU::V) == false); // Overflow flag
    }

    SUBCASE("Borrow Handling (Carry Clear)")
    {
        cpu.A = 0x10;               // Accumulator
        cpu.setFlag(CPU::C, false); // Clear carry flag
        cpu.memory[0x8000] = 0xE9;  // SBC Immediate opcode
        cpu.memory[0x8001] = 0x05;  // Immediate value to subtract
        cpu.execute();

        CHECK(cpu.A == 0x0A);                // Check accumulator value (0x10 - 0x05 - 1 = 0x0A)
        CHECK(cpu.getFlag(CPU::C) == true);  // Carry flag
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag
        CHECK(cpu.getFlag(CPU::V) == false); // Overflow flag
    }
}

TEST_CASE("CPU - Reset Cycle Count")
{
    CPU cpu;
    cpu.cycles = 100;       // Set cycleCount to a non-zero value
    cpu.reset();            // Reset the CPU
    CHECK(cpu.cycles == 0); // Ensure cycleCount is reset to 0
}

TEST_CASE("Opcode Cycles - Basic Cycle Count Test")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("ADC Immediate - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0x69; // ADC Immediate opcode
        cpu.memory[0x8001] = 0x10; // Immediate value
        cpu.cycles = 0;

        cpu.execute(); // Execute the opcode

        CHECK(cpu.cycles == 2); // ADC Immediate should take 2 cycles
    }
}

TEST_CASE("Opcode Cycles - Base Cycle Count Test")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("ADC Immediate - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0x69; // ADC Immediate opcode
        cpu.memory[0x8001] = 0x10; // Immediate value
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2); // ADC Immediate should take 2 cycles
    }

    SUBCASE("LDA Zero Page - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0xA5; // LDA Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Address in Zero Page
        cpu.memory[0x0010] = 0x42; // Value at address 0x0010
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 3); // LDA Zero Page should take 3 cycles
    }

    SUBCASE("STA Absolute - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0x8D; // STA Absolute opcode
        cpu.memory[0x8001] = 0x00; // Low byte of address
        cpu.memory[0x8002] = 0x80; // High byte of address
        cpu.A = 0x55;              // Accumulator value
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 4);            // STA Absolute should take 4 cycles
        CHECK(cpu.memory[0x8000] == 0x55); // Ensure the value was written
    }

    SUBCASE("ASL Accumulator - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0x0A; // ASL Accumulator opcode
        cpu.A = 0x01;              // Initial accumulator value
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2); // ASL Accumulator should take 2 cycles
        CHECK(cpu.A == 0x02);   // Result should be shifted left
    }

    SUBCASE("LDX Immediate - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0xA2; // LDX Immediate opcode
        cpu.memory[0x8001] = 0x10; // Immediate value
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2); // LDX Immediate should take 2 cycles
        CHECK(cpu.X == 0x10);   // Ensure X register is updated
    }

    SUBCASE("DEX - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0xCA; // DEX opcode
        cpu.X = 0x05;              // Set X register
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2); // DEX should take 2 cycles
        CHECK(cpu.X == 0x04);   // X register decremented
    }

    SUBCASE("RTS - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0x60; // RTS opcode
        cpu.pushToStack(0x12);     // Push return address (high byte)

        cpu.pushToStack(0x34); // Push return address (low byte)
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 6);  // RTS should take 6 cycles
        CHECK(cpu.PC == 0x1235); // PC restored and incremented
    }
}

TEST_CASE("Opcode Cycles - Base Cycle Count Test with Flags")
{
    CPU cpu;
    cpu.reset();

    SUBCASE("ADC Immediate - Base Cycle Count and Flags")
    {
        cpu.memory[0x8000] = 0x69; // ADC Immediate opcode
        cpu.memory[0x8001] = 0x10; // Immediate value
        cpu.A = 0x20;              // Initial accumulator value
        cpu.setFlag(CPU::C, false); // Ensure Carry is clear
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2); // ADC Immediate should take 2 cycles
        CHECK(cpu.A == 0x30);   // Accumulator should be updated
        CHECK(cpu.getFlag(CPU::C) == false); // No carry
        CHECK(cpu.getFlag(CPU::Z) == false); // Result is not zero
    }

    SUBCASE("LDA Zero Page - Base Cycle Count and Flags")
    {
        cpu.memory[0x8000] = 0xA5; // LDA Zero Page opcode
        cpu.memory[0x8001] = 0x10; // Address in Zero Page
        cpu.memory[0x0010] = 0x00; // Value at address 0x0010 (Zero)
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 3); // LDA Zero Page should take 3 cycles
        CHECK(cpu.A == 0x00);   // Accumulator should be loaded with value
        CHECK(cpu.getFlag(CPU::Z) == true); // Zero flag should be set
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag should be clear
    }

    SUBCASE("STA Absolute - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0x8D; // STA Absolute opcode
        cpu.memory[0x8001] = 0x00; // Low byte of address
        cpu.memory[0x8002] = 0x80; // High byte of address
        cpu.A = 0x55;              // Accumulator value
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 4);             // STA Absolute should take 4 cycles
        CHECK(cpu.memory[0x8000] == 0x55);  // Value should be stored in memory
    }

    SUBCASE("ASL Accumulator - Base Cycle Count and Flags")
    {
        cpu.memory[0x8000] = 0x0A; // ASL Accumulator opcode
        cpu.A = 0x80;              // Initial accumulator value
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2);       // ASL Accumulator should take 2 cycles
        CHECK(cpu.A == 0x00);         // Result should be zero
        CHECK(cpu.getFlag(CPU::C) == true);  // Carry should be set
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag should be set
    }

    SUBCASE("LDX Immediate - Base Cycle Count and Flags")
    {
        cpu.memory[0x8000] = 0xA2; // LDX Immediate opcode
        cpu.memory[0x8001] = 0xFF; // Immediate value (negative in signed context)
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2);      // LDX Immediate should take 2 cycles
        CHECK(cpu.X == 0xFF);        // X register should be updated
        CHECK(cpu.getFlag(CPU::Z) == false); // Zero flag should be clear
        CHECK(cpu.getFlag(CPU::N) == true);  // Negative flag should be set
    }

    SUBCASE("DEX - Base Cycle Count and Flags")
    {
        cpu.memory[0x8000] = 0xCA; // DEX opcode
        cpu.X = 0x01;              // Set X register to 1
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 2);      // DEX should take 2 cycles
        CHECK(cpu.X == 0x00);        // X register decremented
        CHECK(cpu.getFlag(CPU::Z) == true);  // Zero flag should be set
        CHECK(cpu.getFlag(CPU::N) == false); // Negative flag should be clear
    }

    SUBCASE("RTS - Base Cycle Count")
    {
        cpu.memory[0x8000] = 0x60; // RTS opcode
        cpu.pushToStack(0x12);     // Push return address (high byte)
        cpu.pushToStack(0x34);     // Push return address (low byte)
        cpu.cycles = 0;

        cpu.execute();

        CHECK(cpu.cycles == 6);  // RTS should take 6 cycles
        CHECK(cpu.PC == 0x1235); // PC restored and incremented
    }
}
