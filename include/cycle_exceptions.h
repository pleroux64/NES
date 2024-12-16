#ifndef CYCLE_EXCEPTIONS_H
#define CYCLE_EXCEPTIONS_H

#include <unordered_map>
#include <functional>
#include <cstdint>
#include "cpu.h" // Ensure this includes the `CPU` class definition

// Map of cycle exceptions for opcodes
extern std::unordered_map<uint8_t, std::function<int(CPU&)>> cycleExceptions;

// Helper functions for cycle exceptions
int pageBoundaryException(uint16_t baseAddress, uint8_t offset);

// Updated to use the CPU object and cache the PC
int branchCycleExceptions(CPU& cpu);

// Updated to use CPU memory and Y register
int indirectYPageBoundaryException(CPU& cpu, uint16_t baseAddress);

#endif // CYCLE_EXCEPTIONS_H
