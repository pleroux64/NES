#include <iostream>
#include <fstream>
#include "cpu.h"
#include "controller.h"
#include <SDL2/SDL.h>
#include "ppu.h"

const int SCREEN_WIDTH = 256;      // NES screen width
const int SCREEN_HEIGHT = 240;     // NES screen height
const int FRAME_DELAY = 1000 / 60; // ~60 FPS delay

void loadROM(CPU &cpu, PPU &ppu, const std::string &filepath)
{
    std::ifstream rom(filepath, std::ios::binary);
    if (!rom.is_open())
    {
        std::cerr << "Failed to open ROM: " << filepath << std::endl;
        exit(1);
    }

    // Read the NES header
    char header[16];
    rom.read(header, 16);

    // Verify that it's a valid NES file
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A)
    {
        std::cerr << "Invalid NES file: " << filepath << std::endl;
        exit(1);
    }

    // Get PRG-ROM size (in 16KB units)
    int prgSize = header[4] * 16384; // PRG-ROM size in bytes
    if (prgSize > 0x8000)
    {
        std::cerr << "PRG-ROM size exceeds memory limit!" << std::endl;
        exit(1);
    }

    // Load PRG-ROM into memory at 0x8000
    rom.read(reinterpret_cast<char *>(&cpu.memory[0x8000]), prgSize);

    if (!rom)
    {
        std::cerr << "Error reading PRG-ROM" << std::endl;
        exit(1);
    }

    // Mirror PRG-ROM for smaller sizes
    if (prgSize == 0x4000)
    {
        for (size_t i = 0; i < 0x4000; ++i)
        {
            cpu.writeMemory(0xC000 + i, cpu.readMemory(0x8000 + i));
        }
    }

    // Load CHR-ROM into PPU memory (if available)
    int chrSize = header[5] * 8192; // CHR-ROM size in 8KB units
    if (chrSize > 0)
    {
        rom.read(reinterpret_cast<char *>(&ppu.memory[0x0000]), chrSize);
        if (!rom)
        {
            std::cerr << "Error reading CHR-ROM" << std::endl;
            exit(1);
        }
    }
    else
    {
        // Handle CHR-RAM if no CHR-ROM is present
        std::cerr << "[Debug] CHR-ROM size is 0. Assuming CHR-RAM." << std::endl;
        std::fill(ppu.memory.begin(), ppu.memory.begin() + 0x2000, 0); // Initialize 8KB CHR-RAM
    }

    // Calculate offset for the last 6 bytes of PRG-ROM
    int vectorOffset = 16 + prgSize - 6; // 16 bytes for header
    rom.seekg(vectorOffset, std::ios::beg);

    // Read the interrupt vectors
    uint8_t vectorBytes[6];
    rom.read(reinterpret_cast<char *>(vectorBytes), 6);

    // Assign the vectors to the correct memory locations
    cpu.writeMemory(0xFFFA, vectorBytes[0]); // NMI Vector Low
    cpu.writeMemory(0xFFFB, vectorBytes[1]); // NMI Vector High
    cpu.writeMemory(0xFFFC, vectorBytes[2]); // RESET Vector Low
    cpu.writeMemory(0xFFFD, vectorBytes[3]); // RESET Vector High
    cpu.writeMemory(0xFFFE, vectorBytes[4]); // IRQ/BRK Vector Low
    cpu.writeMemory(0xFFFF, vectorBytes[5]); // IRQ/BRK Vector High

    // Fetch vectors for debugging
    uint16_t nmiVector = (cpu.readMemory(0xFFFB) << 8) | cpu.readMemory(0xFFFA);
    uint16_t resetVector = (cpu.readMemory(0xFFFD) << 8) | cpu.readMemory(0xFFFC);
    uint16_t irqVector = (cpu.readMemory(0xFFFF) << 8) | cpu.readMemory(0xFFFE);

    // Debugging: Print vectors
    std::cout << "[Debug] Fetched NMI vector: 0x" << std::hex << nmiVector << std::endl;
    std::cout << "[Debug] Fetched RESET vector: 0x" << std::hex << resetVector << std::endl;
    std::cout << "[Debug] Fetched IRQ/BRK vector: 0x" << std::hex << irqVector << std::endl;

    std::cout << "ROM loaded successfully: " << filepath << std::endl;
    std::cout << "PRG-ROM size: " << prgSize << " bytes" << std::endl;

    rom.close();

    // Dump NMI vector for further debugging
    cpu.debugNMIVector();
    //ppu.debugPatternTable();
}


void displayFramebuffer(SDL_Renderer *renderer, SDL_Texture *texture, const PPU &ppu)
{
    // Update the texture with the framebuffer data
    SDL_UpdateTexture(texture, nullptr, ppu.framebuffer.data(), SCREEN_WIDTH * sizeof(uint32_t));

    // Clear and present the renderer
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void debugCPU(const CPU &cpu)
{
    // Print out CPU state
    std::cout << "PC: 0x" << std::hex << cpu.PC
              << " A: 0x" << static_cast<int>(cpu.A)
              << " X: 0x" << static_cast<int>(cpu.X)
              << " Y: 0x" << static_cast<int>(cpu.Y)
              << " SP: 0x" << static_cast<int>(cpu.SP)
              << " P: 0x" << std::bitset<8>(cpu.P)
              << " Cycles: " << std::dec << cpu.cycles
              << std::endl;
}

int main()
{
    CPU cpu;
    Controller controller;
    PPU ppu;

    // Link the PPU to the CPU
    cpu.setPPU(&ppu);
    ppu.setCPU(&cpu);

    // SDL Initialization with error checking
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Scale the window dimensions
    const int WINDOW_WIDTH = SCREEN_WIDTH * 3;
    const int WINDOW_HEIGHT = SCREEN_HEIGHT * 3;

    SDL_Window *window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                             SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!texture)
    {
        std::cerr << "Failed to create SDL texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load ROM
    const std::string romPath = "roms/hello_world.nes";
    loadROM(cpu,ppu,romPath);

    // Reset CPU and PPU
    cpu.reset();
    ppu.reset();

    // Main emulation loop
    bool running = true;
    Uint32 frameStart, frameTime;

    while (running)
    {
        frameStart = SDL_GetTicks();

        // Poll controller input
        controller.pollKeyboard();
        cpu.writeMemory(0x4016, controller.getButtonState());

        // Execute CPU instructions and render PPU frame
        cpu.execute();
        ppu.renderFrame();

        // Update the screen
        displayFramebuffer(renderer, texture, ppu);

        // Handle events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Frame timing for ~60 FPS
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // Cleanup SDL resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
