# NES Emulator

## **Overview**
A custom NES emulator written in C++ that emulates the Nintendo Entertainment System. It is capable of loading and running NES ROMs and includes:
- CPU emulation for the 6502 processor.
- A partially implemented Picture Processing Unit (PPU).
- Basic rendering features.

---

## **Features**
- **CPU Emulation**: Implements the 6502 instruction set, including arithmetic, bitwise operations, branching, and memory management.
- **PPU (Picture Processing Unit)**: Renders nametables, processes pattern tables, and applies basic palettes.
- **ROM Loading**: Parses iNES ROM headers and supports horizontal and vertical mirroring.
- **Debugging Tools**: Provides detailed logs for CPU instructions, PPU registers, and rendering states.


---

## **Technologies Used**
- **C++**: Core implementation language.
- **Memory Management**: Simulates VRAM and OAM structures for NES architecture.
- **Debugging Tools**: Logs for efficient emulator development.
- **Graphics Rendering**: Generates a framebuffer for NES graphics.

---

## **Current Progress**
- **Implemented**:
  - CPU emulation with many 6502 instructions.
  - PPU basics: rendering background tiles and pattern tables.
  - Debugging tools for analyzing CPU and PPU states.

- **In Progress**:
  - Enhanced PPU features: sprite rendering and scrolling.
  - Accurate palette application and advanced mirroring logic.
  - Comprehensive support for NES test ROMs and real games.

---

## **File Organization**
- **Makefile**: Contains build instructions for compiling the emulator.
- **README.md**: Provides an overview, setup instructions, and project details.
- **build/**: Directory for compiled object files and the final emulator binary.
  - `nes_emulator`: The compiled emulator executable.
  - `.o` files: Compiled object files for different modules.
- **include/**: Header files defining interfaces for the emulator components.
  - `controller.h`, `cpu.h`, `ppu.h`: Core headers for the emulator modules.
  - Supporting utilities like cycle exceptions and opcode cycles.
- **roms/**: Test ROMs for validating the emulator's functionality.
  - `hello_world.nes`: A simple ROM for testing text rendering.
  - Other ROMs include `nestest.nes` for CPU validation and popular games.
- **src/**: Source code for the emulator.
  - `main.cpp`: The entry point for the emulator.
  - `controller.cpp`, `ppu.cpp`: Implementation of the controller and PPU.
  - **cpu/**: Subdirectory containing all CPU-related implementations (e.g., arithmetic, branching, memory operations).
- **tests/**: Unit tests for different components of the emulator.
  - Tests validate the CPU, PPU, and overall system behavior.

---

## **How It Works**
- The **CPU** executes 6502 instructions fetched from PRG-ROM.
- The **PPU** simulates graphical output by rendering tiles and applying patterns using VRAM and palette data.
- Test ROMs like `hello_world.nes` validate the implementation by writing data to PPU registers and VRAM.

---

## **Test ROMs**
This repository includes several test ROMs for debugging:
- **hello_world.nes**: Displays "Hello, World!" text using background tiles.
- **nestest.nes**: Comprehensive 6502 CPU instruction test.
- **02-immediate.nes**: Validates addressing modes for instructions.

---

## **Known Issues**
- Limited PPU functionality (e.g., no sprite rendering).
- Inaccuracies in CPU and PPU cycle synchronization.
- Partial handling of edge cases for advanced games.

---

## **Getting Started**

### **Prerequisites**
- **C++17** or newer.
- A terminal or development environment (e.g., VSCode, CLion).
- Basic understanding of NES architecture (optional but helpful).

### **Installation**
1. Clone the repository:
   ```bash
   git clone https://github.com/pleroux64/NES.git
   cd nes_emulator
   ```
2. Compile the project using your preferred build system:
   ```bash
   g++ -std=c++17 -o nes_emulator src/main.cpp src/cpu/cpu.cpp src/ppu.cpp
   ```

### **Usage**
Run the emulator with a test ROM:
   ```bash
   ./nes_emulator roms/hello_world.nes
   ```

---

## **Contributing**
Contributions are welcome! To get started:
1. Fork the repository.
2. Create a feature branch:
   ```bash
   git checkout -b feature-name
   ```
3. Commit and push your changes, then open a pull request.

---

## **License**
This project is licensed under the MIT License. See the `LICENSE` file for details.
