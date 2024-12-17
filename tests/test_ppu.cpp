#include "ppu.h"
#include "doctest.h"
#include <iostream>
#include <bitset>

// Helper for VRAM address setup
void setPPUAddress(PPU &ppu, uint16_t address)
{
    ppu.writeRegister(0x2006, (address >> 8) & 0xFF); // High byte
    ppu.writeRegister(0x2006, address & 0xFF);        // Low byte
}

bool hasNonZeroPixels(const PPU &ppu)
{
    return std::any_of(ppu.framebuffer.begin(), ppu.framebuffer.end(),
                       [](uint32_t pixel)
                       { return pixel != 0; });
}

// Helper for Simplified Tile Data Initialization
void initializeTileData(PPU &ppu, uint8_t tileIndex, uint8_t pattern)
{
    // Fill the pattern table with known data for the tile
    for (int row = 0; row < 8; ++row)
    {
        ppu.memory[0x0000 + tileIndex * 16 + row] = pattern;     // Plane 1
        ppu.memory[0x0000 + tileIndex * 16 + row + 8] = pattern; // Plane 2
    }
}

// PPU Register Tests
TEST_CASE("PPU - Register Read/Write")
{
    PPU ppu;
    ppu.reset();

    SUBCASE("Write and Read PPUCTRL ($2000)")
    {
        ppu.writeRegister(0x2000, 0xFF);
        CHECK(ppu.PPUCTRL == 0xFF);
    }

    SUBCASE("Write and Read PPUMASK ($2001)")
    {
        ppu.writeRegister(0x2001, 0xAA);
        CHECK(ppu.PPUMASK == 0xAA);
    }

    SUBCASE("Write and Read PPUSCROLL ($2005)")
    {
        ppu.writeRegister(0x2005, 0x55); // First write (X Scroll)
        CHECK(ppu.getFineXScroll() == 0x55);

        ppu.writeRegister(0x2005, 0xAA); // Second write (Y Scroll)
        CHECK(ppu.getFineYScroll() == 0xAA);
    }

    SUBCASE("Write and Read PPUADDR ($2006)")
    {
        setPPUAddress(ppu, 0x2400);
        CHECK(ppu.PPUADDR == 0x00); // PPUADDR holds last written byte
    }

    SUBCASE("Write and Read PPUDATA ($2007)")
    {
        setPPUAddress(ppu, 0x2000);
        ppu.writeRegister(0x2007, 0x42);
        setPPUAddress(ppu, 0x2000);
        uint8_t value = ppu.readRegister(0x2007);
        CHECK(value == 0x42);
    }
}

// VRAM Memory Access Tests
TEST_CASE("PPU - VRAM Memory Access")
{
    PPU ppu;
    ppu.reset();

    SUBCASE("Write and Read VRAM Data")
    {
        setPPUAddress(ppu, 0x2000);
        ppu.writeRegister(0x2007, 0x33);
        setPPUAddress(ppu, 0x2000);
        uint8_t value = ppu.readRegister(0x2007);
        CHECK(value == 0x33);
    }

    SUBCASE("Auto-increment PPUADDR")
    {
        ppu.writeRegister(0x2000, 0x04); // Set PPUCTRL to enable increment by 32
        setPPUAddress(ppu, 0x2000);
        ppu.writeRegister(0x2007, 0x11); // Write to $2000
        ppu.writeRegister(0x2007, 0x22); // Write to $2001

        setPPUAddress(ppu, 0x2000); // Reset PPUADDR to $2000
        uint8_t value1 = ppu.readRegister(0x2007);
        uint8_t value2 = ppu.readRegister(0x2007);

        // Debugging output
        std::cerr << "[DEBUG] Test Auto-increment: Value1=" << std::dec << (int)value1
                  << ", Value2=" << (int)value2 << std::endl;

        CHECK(value1 == 0x11);
        CHECK(value2 == 0x22);
    }
}

// OAM Tests
TEST_CASE("PPU - OAM Access")
{
    PPU ppu;
    ppu.reset();

    SUBCASE("Write and Read OAM Data")
    {
        ppu.writeRegister(0x2003, 0x00); // Set OAMADDR
        ppu.writeRegister(0x2004, 0x12);
        ppu.writeRegister(0x2004, 0x34);
        ppu.writeRegister(0x2004, 0x56);
        ppu.writeRegister(0x2004, 0x78);

        ppu.writeRegister(0x2003, 0x00); // Reset OAMADDR
        CHECK(ppu.oam[0] == 0x12);
        CHECK(ppu.oam[1] == 0x34);
        CHECK(ppu.oam[2] == 0x56);
        CHECK(ppu.oam[3] == 0x78);
    }
}

// Reset Tests
TEST_CASE("PPU - Reset Behavior")
{
    PPU ppu;
    ppu.writeRegister(0x2000, 0xFF);
    ppu.writeRegister(0x2001, 0xFF);
    ppu.writeRegister(0x2003, 0xFF);
    ppu.writeRegister(0x2005, 0xFF);
    ppu.writeRegister(0x2006, 0xFF);
    ppu.writeRegister(0x2007, 0xFF);

    ppu.reset();

    CHECK(ppu.PPUCTRL == 0x00);
    CHECK(ppu.PPUMASK == 0x00);
    CHECK(ppu.PPUSTATUS == 0x00);
    CHECK(ppu.OAMADDR == 0x00);
    CHECK(ppu.PPUSCROLL == 0x00);
    CHECK(ppu.PPUADDR == 0x00);
    CHECK(ppu.PPUDATA == 0x00);
    CHECK(std::all_of(ppu.memory.begin(), ppu.memory.end(), [](uint8_t x)
                      { return x == 0; }));
    CHECK(std::all_of(ppu.oam.begin(), ppu.oam.end(), [](uint8_t x)
                      { return x == 0; }));
    CHECK(std::all_of(ppu.framebuffer.begin(), ppu.framebuffer.end(), [](uint32_t x)
                      { return x == 0; }));

    std::cout << "PPU reset test passed!" << std::endl;
}

TEST_CASE("PPU - Background Rendering")
{
    PPU ppu;
    ppu.reset();

    // Initialize nametable and pattern table
    memset(ppu.memory.data() + 0x2000, 1, 1024);
    initializeTileData(ppu, 1, 0xFF); // Tile 1: All pixels set to 1 (visible)

    // Render background
    ppu.renderBackground();

    // Verify that framebuffer has non-zero pixels
    bool foundNonZero = hasNonZeroPixels(ppu);
    std::cerr << "[DEBUG] Background Rendering: Non-zero pixels found = " << foundNonZero << std::endl;

    CHECK(foundNonZero);
}

// PPU Sprite Rendering Test
TEST_CASE("PPU - Sprite Rendering")
{
    PPU ppu;
    ppu.reset();

    // Initialize sprite data
    memset(ppu.oam.data(), 0, 256); // Clear OAM
    ppu.oam[0] = 100;               // Y-coordinate
    ppu.oam[1] = 1;                 // Tile index 1
    ppu.oam[2] = 0;                 // Attributes (palette, no flip)
    ppu.oam[3] = 50;                // X-coordinate

    initializeTileData(ppu, 1, 0xFF); // Tile 1: All pixels set to 1 (visible)

    // Render sprites
    ppu.renderSprites();

    // Verify that framebuffer has non-zero pixels
    bool foundNonZero = hasNonZeroPixels(ppu);
    std::cerr << "[DEBUG] Sprite Rendering: Non-zero pixels found = " << foundNonZero << std::endl;

    CHECK(foundNonZero);
}