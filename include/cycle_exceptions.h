#ifndef CYCLE_EXCEPTIONS_H
#define CYCLE_EXCEPTIONS_H

#include <unordered_map>
#include <functional>
#include "cpu.h" // Ensure this includes the `CPU` class definition

// Map of cycle exceptions for opcodes
extern std::unordered_map<uint8_t, std::function<int(CPU&)>> cycleExceptions;

// Helper functions for cycle exceptions
int pageBoundaryException(uint16_t baseAddress, uint8_t offset);
int branchCycleExceptions(CPU& cpu);
int indirectYPageBoundaryException(CPU& cpu, uint16_t baseAddress);

#endif // CYCLE_EXCEPTIONS_H
