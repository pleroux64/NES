<<<markdown
# NES Emulator

## **Overview**
A custom NES emulator written in C++ that emulates the Nintendo Entertainment System, capable of loading and running NES ROMs. It includes a CPU emulation for the 6502 processor, a partially implemented Picture Processing Unit (PPU), and basic rendering features.

---

## **Features**
- **CPU Emulation**: Implements the 6502 instruction set, including arithmetic, bitwise, branching, and memory management.
- **PPU (Picture Processing Unit)**: Handles nametable rendering, pattern tables, and basic palette application.
- **ROM Loading**: Parses iNES ROM headers and supports horizontal and vertical mirroring.
- **Debugging Tools**: Logs CPU instructions, PPU register operations, and rendering data for analysis.
- **Test ROMs Supported**: Runs various test ROMs, including basic "Hello, World!" output.

---

## **Technologies Used**
- **C++**: Core implementation.
- **Debugging**: Extensive use of logging for CPU and PPU debugging.
- **Memory Management**: Simulates VRAM and OAM structures.
- **Graphics Rendering**: Produces a framebuffer for graphical output.

---

## **Getting Started**

### **Prerequisites**
- **C++17** or newer.
- A terminal or development environment (e.g., VSCode, CLion).
- Basic understanding of NES architecture (optional but helpful).

### **Installation**
1. Clone the repository:
   <<<bash
   git clone https://github.com/your-username/nes-emulator.git
   cd nes-emulator
   <<<
2. Compile the project using your preferred method (e.g., g++ or a provided build system):
   <<<bash
   g++ -std=c++17 -o nes_emulator src/main.cpp src/cpu/cpu.cpp src/ppu.cpp
   <<<
### **Usage**
Run the emulator with a test ROM:
   <<<bash
   ./nes_emulator roms/hello_world.nes
   <<<

---

## **Current Progress**
- **Implemented**:
  - CPU emulation with several 6502 instructions.
  - PPU basics: rendering background tiles and pattern tables.
  - Debugging tools for CPU and PPU states.

- **In Progress**:
  - Accurate palette handling and advanced mirroring logic.
  - Enhanced PPU features like sprite rendering and scroll support.
  - Comprehensive support for NES test ROMs and real games.

---

## **How It Works**
- The CPU executes 6502 instructions fetched from PRG-ROM.
- The PPU simulates graphical output by rendering tiles and applying patterns based on VRAM and palette data.
- Test ROMs like `hello_world.nes` validate the implementation by writing data to PPU registers and VRAM.

---

## **Test ROMs**
The repository includes several test ROMs for debugging:
- **hello_world.nes**: Basic "Hello, World!" text rendering.
- **nestest.nes**: A comprehensive CPU instruction test.
- **02-immediate.nes**: Tests specific addressing modes.

---

## **Known Issues**
- Limited PPU functionality (e.g., no sprite rendering).
- Timing inaccuracies between CPU and PPU cycles.
- Incomplete handling of edge cases for advanced games.

---

## **Contributing**
Contributions are welcome! To get started:
1. Fork the repository.
2. Create a feature branch:
   <<<bash
   git checkout -b feature-name
   <<<
3. Commit and push your changes, then open a pull request.

---

## **License**
This project is licensed under the MIT License. See `LICENSE` for more details.
<<<
