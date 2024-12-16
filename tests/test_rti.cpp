#include "doctest.h"
#include "cpu.h"

TEST_CASE("RTI - Return from Interrupt")
{
    CPU cpu;
    cpu.reset();

    // Set up the stack with a saved PC and flags
    cpu.memory[cpu.PC] = 0x40; // RTI opcode (0x40)

    // Push PC to the stack (low byte, then high byte)
    cpu.pushToStack(0x78); // High byte of PC
    cpu.pushToStack(0x56);

    // Push flags to the stack (set specific flags: N and V)
    cpu.pushToStack(0b10100101); // N=1, V=1, I=0, D=0, Z=0, C=1

    // Execute RTI
    cpu.execute();

    // Check the restored program counter
    CHECK(cpu.PC == 0x7856); // Verify PC is restored

    CHECK(cpu.P == 0b10100101);          // Verify the entire flags register
    CHECK(cpu.getFlag(CPU::N) == true);  // N (Negative) flag is set
    CHECK(cpu.getFlag(CPU::V) == false); // V (Overflow) flag is clear
    CHECK(cpu.getFlag(CPU::D) == false); // D (Decimal) flag is clear
    CHECK(cpu.getFlag(CPU::I) == true);  // I (Interrupt disable) flag is set
    CHECK(cpu.getFlag(CPU::Z) == false); // Z (Zero) flag is clear
    CHECK(cpu.getFlag(CPU::C) == true);  // C (Carry) flag is set

    // Ensure stack pointer is correctly updated
    CHECK(cpu.SP == 0xFF); // Original stack pointer after popping 3 bytes
}
