#include "ppu.h"
#include "cpu.h"    // Include CPU header for NMI triggering
#include <cstring>  // For memset
#include <iostream> // For debugging logs

PPU::PPU()
{
    cpu = nullptr; // Initialize CPU pointer
    addressLatch = false;
    scrollLatch = false;
    fineXScroll = 0;
    fineYScroll = 0;
    PPUSCROLL = 0;
    PPUADDR = 0;
    reset();
}

void PPU::reset()
{
    PPUCTRL = PPUMASK = PPUSTATUS = OAMADDR = PPUSCROLL = PPUADDR = PPUDATA = 0;

    addressLatch = false;
    scrollLatch = false;
    fineXScroll = 0;
    fineYScroll = 0;

    memory.fill(0);
    oam.fill(0);
    framebuffer.fill(0);
}

void PPU::setCPU(CPU *cpuInstance)
{
    cpu = cpuInstance; // Link CPU instance for NMI signaling
}

// WRITE REGISTER - Handles CPU writes to PPU registers
void PPU::writeRegister(uint16_t address, uint8_t value)
{
    switch (address)
    {
    case 0x2000: // PPUCTRL
        std::cerr << "[PPU Debug] Old PPUCTRL: 0x" << std::hex << static_cast<int>(PPUCTRL) << std::endl;
        PPUCTRL = value;
        std::cerr << "[PPU Debug] New PPUCTRL written: 0x" << std::hex << static_cast<int>(PPUCTRL)
                  << " (NMI enabled: " << ((PPUCTRL & 0x80) != 0) << ")" << std::endl;
        break;

    case 0x2001: // PPUMASK
        PPUMASK = value;
        break;

    case 0x2003: // OAMADDR
        OAMADDR = value;
        break;

    case 0x2004: // OAMDATA
        oam[OAMADDR++] = value;
        break;

    case 0x2005: // PPUSCROLL
        if (!scrollLatch)
        {
            fineXScroll = value;
            scrollLatch = true;
        }
        else
        {
            fineYScroll = value;
            scrollLatch = false;
        }
        break;

    case 0x2006: // PPUADDR
        if (!addressLatch)
        {
            PPUADDR = (PPUADDR & 0x00FF) | (value << 8); // High byte
            addressLatch = true;
        }
        else
        {
            PPUADDR = (PPUADDR & 0xFF00) | value; // Low byte
            addressLatch = false;
        }
        break;

    case 0x2007: // PPUDATA
        memory[PPUADDR & 0x3FFF] = value;
        PPUADDR += (PPUCTRL & 0x04) ? 32 : 1; // Increment based on PPUCTRL
        PPUADDR &= 0x3FFF;                    // Wrap PPUADDR to 14 bits
        break;

    default:
        std::cerr << "[DEBUG] Write to unsupported register: 0x" << std::hex << address << std::endl;
        break;
    }
}

// READ REGISTER - Handles CPU reads from PPU registers
uint8_t PPU::readRegister(uint16_t address)
{
    uint8_t data = 0;

    switch (address)
    {
    case 0x2002: // PPUSTATUS
        data = PPUSTATUS;
        PPUSTATUS &= 0x7F;    // Clear VBlank flag (bit 7)
        addressLatch = false; // Reset latches
        scrollLatch = false;
        std::cerr << "[PPU Debug] $2002 Read: VBlank = "
                  << ((data & 0x80) ? "Set" : "Clear") << std::endl;
        break;

    case 0x2004: // OAMDATA
        data = oam[OAMADDR];
        break;

    case 0x2007: // PPUDATA
        data = memory[PPUADDR & 0x3FFF];
        PPUADDR += (PPUCTRL & 0x04) ? 32 : 1; // Increment based on PPUCTRL
        PPUADDR &= 0x3FFF;                    // Wrap PPUADDR
        break;

    default:
        std::cerr << "[DEBUG] Read from unsupported register: 0x" << std::hex << address << std::endl;
        break;
    }

    return data;
}

void PPU::renderFrame()
{
    // Render the background and sprites
    renderBackground();
    renderSprites();

    // Set VBlank flag in PPUSTATUS (bit 7)
    PPUSTATUS |= 0x80; // Indicates the start of VBlank
    std::cerr << "[PPU Debug] VBlank flag set. PPUSTATUS: 0b"
              << std::bitset<8>(PPUSTATUS) << std::endl;

    // Check if NMI is enabled in PPUCTRL (bit 7)
    if (PPUCTRL & 0x80) // NMI enable flag
    {
        if (cpu)
        {
            // Trigger NMI if the CPU is linked
            cpu->requestNMI();
            std::cerr << "[PPU Debug] NMI requested by PPU." << std::endl;
        }
        else
        {
            // Log if CPU is not linked
            std::cerr << "[PPU Debug] CPU is not linked to PPU. NMI not triggered." << std::endl;
        }
    }
    else
    {
        // Log if NMI is not enabled
        std::cerr << "[PPU Debug] NMI not enabled in PPUCTRL." << std::endl;
    }

    // Optionally clear the VBlank flag before the next frame (based on timing in the main loop)
    // PPUSTATUS &= ~0x80; // Uncomment if clearing before rendering the next frame
}

void PPU::renderBackground()
{
    const uint16_t baseNametable[4] = {0x2000, 0x2400, 0x2800, 0x2C00};
    const uint16_t patternTableBase = (PPUCTRL & 0x10) ? 0x1000 : 0x0000;

    const int screenWidth = 256;
    const int screenHeight = 240;

    int scrollX = fineXScroll;
    int scrollY = fineYScroll;

    for (int tileY = 0; tileY < 30; ++tileY)
    {
        for (int tileX = 0; tileX < 32; ++tileX)
        {
            int currentNametable = 0;
            if (scrollX >= 256)
                currentNametable |= 1;
            if (scrollY >= 240)
                currentNametable |= 2;

            uint16_t nametableBase = baseNametable[currentNametable];
            uint16_t tileAddr = nametableBase + (tileY * 32) + tileX;

            uint8_t tileIndex = memory[tileAddr];
            int adjustedX = (tileX * 8 - scrollX) % screenWidth;
            int adjustedY = (tileY * 8 - scrollY) % screenHeight;

            for (int row = 0; row < 8; ++row)
            {
                uint8_t plane1 = memory[patternTableBase + (tileIndex * 16) + row];
                uint8_t plane2 = memory[patternTableBase + (tileIndex * 16) + row + 8];

                for (int col = 0; col < 8; ++col)
                {
                    uint8_t pixel = ((plane1 >> (7 - col)) & 1) | (((plane2 >> (7 - col)) & 1) << 1);
                    uint8_t color = pixel * 85; // Grayscale for simplicity

                    int screenX = adjustedX + col;
                    int screenY = adjustedY + row;

                    if (pixel != 0 && screenX >= 0 && screenY >= 0 &&
                        screenX < screenWidth && screenY < screenHeight)
                    {
                        framebuffer[screenY * screenWidth + screenX] =
                            (color << 16) | (color << 8) | color; // Grayscale color
                    }
                }
            }
        }
    }
}

void PPU::renderSprites()
{
    const uint16_t patternTableBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;
    const int screenWidth = 256;
    const int screenHeight = 240;

    for (int i = 0; i < 64; ++i)
    {
        int spriteIndex = i * 4;
        uint8_t y = oam[spriteIndex] + 1;
        uint8_t tileIndex = oam[spriteIndex + 1];
        uint8_t attributes = oam[spriteIndex + 2];
        uint8_t x = oam[spriteIndex + 3];

        bool flipHorizontal = attributes & 0x40;
        bool flipVertical = attributes & 0x80;

        for (int row = 0; row < 8; ++row)
        {
            uint8_t plane1 = memory[patternTableBase + (tileIndex * 16) + row];
            uint8_t plane2 = memory[patternTableBase + (tileIndex * 16) + row + 8];

            for (int col = 0; col < 8; ++col)
            {
                uint8_t pixel = ((plane1 >> (7 - col)) & 1) | (((plane2 >> (7 - col)) & 1) << 1);
                if (pixel == 0)
                    continue;

                int finalCol = flipHorizontal ? 7 - col : col;
                int finalRow = flipVertical ? 7 - row : row;

                int screenX = x + finalCol;
                int screenY = y + finalRow;

                if (screenX >= 0 && screenX < screenWidth && screenY >= 0 && screenY < screenHeight)
                {
                    framebuffer[screenY * screenWidth + screenX] = 0xFFFFFF;
                }
            }
        }
    }
}
