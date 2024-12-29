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
                  << " (NMI enabled: " << ((PPUCTRL & 0x80) != 0 ? "Yes" : "No") << ", "
                  << "Base nametable: " << ((PPUCTRL & 0x03) == 0 ? "0x2000" : (PPUCTRL & 0x03) == 1 ? "0x2400"
                                                                           : (PPUCTRL & 0x03) == 2   ? "0x2800"
                                                                                                     : "0x2C00")
                  << ", "
                  << "Increment mode: " << ((PPUCTRL & 0x04) ? "32 bytes" : "1 byte") << ", "
                  << "Sprite pattern table: " << ((PPUCTRL & 0x08) ? "0x1000" : "0x0000") << ", "
                  << "Background pattern table: " << ((PPUCTRL & 0x10) ? "0x1000" : "0x0000") << ")" << std::endl;

        if ((PPUCTRL & 0x80) == 0x80)
        {
            std::cerr << "[PPU Debug] NMI is now enabled. Waiting for VBlank to trigger.\n";
        }
        else
        {
            std::cerr << "[PPU Debug] NMI remains disabled.\n";
        }
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
        memory[resolveNametableAddress(PPUADDR & 0x3FFF)] = value;
        PPUADDR += (PPUCTRL & 0x04) ? 32 : 1; // Increment by 32 if bit 2 is set
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
    {
        uint16_t resolvedAddr = resolveNametableAddress(PPUADDR & 0x3FFF);
        data = memory[resolvedAddr];
        PPUADDR += (PPUCTRL & 0x04) ? 32 : 1; // Increment based on PPUCTRL
        PPUADDR &= 0x3FFF;                    // Wrap PPUADDR
        break;
    }

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

    debugNametable(0x2000);

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
    uint16_t nametableBase = baseNametable[PPUCTRL & 0x03]; // Determine the base nametable dynamically
    const uint16_t patternTableBase = (PPUCTRL & 0x10) ? 0x1000 : 0x0000;

    const int screenWidth = 256;
    const int screenHeight = 240;

    std::cerr << "[PPU Debug] Rendering Background..." << std::endl;
    std::cerr << "[PPU Debug] Nametable Base Address: 0x" << std::hex << nametableBase << std::endl;
    std::cerr << "[PPU Debug] Pattern Table Base Address: 0x" << std::hex << patternTableBase << std::endl;

    for (int tileY = 0; tileY < 30; ++tileY)
    {
        for (int tileX = 0; tileX < 32; ++tileX)
        {
            uint16_t tileAddr = resolveNametableAddress(nametableBase + (tileY * 32) + tileX);
            uint8_t tileIndex = memory[tileAddr];

            // Debugging tile information
            std::cerr << "[PPU Debug] Tile Position (X: " << tileX << ", Y: " << tileY
                      << ") -> Nametable Address: 0x" << std::hex << tileAddr
                      << ", Tile Index: 0x" << std::hex << static_cast<int>(tileIndex) << std::endl;

            for (int row = 0; row < 8; ++row)
            {
                uint8_t plane1 = memory[patternTableBase + (tileIndex * 16) + row];
                uint8_t plane2 = memory[patternTableBase + (tileIndex * 16) + row + 8];

                // Debugging pattern table information
                std::cerr << "[PPU Debug] Tile Index: 0x" << std::hex << static_cast<int>(tileIndex)
                          << ", Row: " << row
                          << ", Plane 1: 0b" << std::bitset<8>(plane1)
                          << ", Plane 2: 0b" << std::bitset<8>(plane2) << std::endl;

                for (int col = 0; col < 8; ++col)
                {
                    uint8_t pixel = ((plane1 >> (7 - col)) & 1) | (((plane2 >> (7 - col)) & 1) << 1);
                    uint8_t color = pixel * 85; // Grayscale for simplicity

                    int screenX = (tileX * 8 + col) % screenWidth;
                    int screenY = (tileY * 8 + row) % screenHeight;

                    if (screenX >= 0 && screenY >= 0 && screenX < screenWidth && screenY < screenHeight)
                    {
                        framebuffer[screenY * screenWidth + screenX] =
                            (color << 16) | (color << 8) | color; // Set pixel color

                        // Debugging framebuffer writes
                        std::cerr << "[PPU Debug] Framebuffer Write - Screen Position (X: " << screenX
                                  << ", Y: " << screenY << "), Color: 0x" << std::hex << static_cast<int>(color)
                                  << std::endl;
                    }
                }
            }
        }
    }

    std::cerr << "[PPU Debug] Background rendering complete." << std::endl;
}


void PPU::renderSprites()
{
    const uint16_t patternTableBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;
    const int screenWidth = 256;
    const int screenHeight = 240;

    for (int i = 0; i < 64; ++i)
    {
        int spriteIndex = i * 4;
        uint8_t y = oam[spriteIndex] + 1;          // Y-coordinate
        uint8_t tileIndex = oam[spriteIndex + 1];  // Tile index
        uint8_t attributes = oam[spriteIndex + 2]; // Sprite attributes
        uint8_t x = oam[spriteIndex + 3];          // X-coordinate

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
                    continue; // Transparent pixel

                int finalCol = flipHorizontal ? 7 - col : col;
                int finalRow = flipVertical ? 7 - row : row;

                int screenX = x + finalCol;
                int screenY = y + finalRow;

                if (screenX >= 0 && screenX < screenWidth && screenY >= 0 && screenY < screenHeight)
                {
                    framebuffer[screenY * screenWidth + screenX] = 0xFFFFFF; // White for sprite pixels
                }
            }
        }
    }
}

void PPU::writeDMA(uint8_t value)
{
    uint16_t baseAddress = value * 0x100; // Calculate base address
    for (int i = 0; i < 256; i++)
    {
        oam[i] = cpu->readMemory(baseAddress + i); // Copy byte-by-byte from CPU memory
    }
    std::cerr << "[PPU Debug] OAM DMA Transfer complete. Source: 0x" << std::hex << baseAddress << std::endl;
}

uint16_t PPU::resolveNametableAddress(uint16_t address)
{
    if (address >= 0x2000 && address < 0x3000)
    {
        uint16_t offset = (address - 0x2000) % 0x1000;
        if (offset >= 0x800)
        { // Bottom half mirrors top
            // std::cerr << "[PPU Debug] Resolving nametable address: 0x" << std::hex << address
            //           << " -> 0x" << 0x2000 + (offset - 0x800) << std::endl;
            return 0x2000 + (offset - 0x800);
        }
        // std::cerr << "[PPU Debug] Resolving nametable address: 0x" << std::hex << address
        //           << " -> 0x" << 0x2000 + offset << std::endl;
        return 0x2000 + offset;
    }
    return address; // Not a nametable address
}

void PPU::debugPatternTable()
{
    for (uint16_t i = 0x0000; i < 0x2000; i += 16)
    {
        std::cout << "Tile " << (i / 16) << ": ";
        for (int j = 0; j < 16; j++)
        {
            std::cout << std::hex << static_cast<int>(memory[i + j]) << " ";
        }
        std::cout << std::endl;
    }
}

void PPU::debugNametable(uint16_t nametableBase)
{
    std::cerr << "[PPU Debug] Dumping Nametable at 0x" << std::hex << nametableBase << ":" << std::endl;
    for (uint16_t offset = 0; offset < 0x400; offset++)
    {
        uint8_t tileIndex = memory[nametableBase + offset];
        if (offset % 32 == 0)
            std::cerr << std::endl; // New row every 32 tiles
        std::cerr << std::hex << (int)tileIndex << " ";
    }
    std::cerr << std::endl;
}
