#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include <array>

class CPU; 

class PPU {
public:
    // PPU Registers
    uint8_t PPUCTRL;    // $2000: Control Register
    uint8_t PPUMASK;    // $2001: Mask Register
    uint8_t PPUSTATUS;  // $2002: Status Register
    uint8_t OAMADDR;    // $2003: OAM Address
    uint8_t OAMDATA;    // $2004: OAM Data
    uint8_t PPUSCROLL;  // $2005: Scroll Position
    uint8_t PPUADDR;    // $2006: Address
    uint8_t PPUDATA;    // $2007: Data

    // PPU Memory
    std::array<uint8_t, 0x4000> memory; // 16KB of PPU memory
    std::array<uint8_t, 256> oam;       // Sprite memory (Object Attribute Memory)

    // Framebuffer
    std::array<uint32_t, 256 * 240> framebuffer; // RGB frame buffer

    // Methods
    PPU();
    void reset();
    void writeRegister(uint16_t address, uint8_t value);
    uint8_t readRegister(uint16_t address);
    void renderFrame();
    void renderBackground();
    void renderSprites();
    void setCPU(CPU* cpuInstance); // Method to link CPU to PPU
    public:
    uint8_t getFineXScroll() const { return fineXScroll; }
    uint8_t getFineYScroll() const { return fineYScroll; }
    void clearVBlankFlag();
    uint16_t resolveNametableAddress(uint16_t address);
    void debugPatternTable();


private:
    // Internal PPU State
    bool addressLatch;      // Tracks high/low byte writes for PPUADDR ($2006)
    bool scrollLatch;       // Tracks first/second write for PPUSCROLL ($2005)
    uint8_t fineXScroll;    // Fine X scroll value
    uint8_t fineYScroll;    // Fine Y scroll value

    CPU* cpu; // Pointer to the CPU for signaling NMI interrupts
};

#endif // PPU_H
