#ifndef __PPU_H__
#define __PPU_H__
#include <cstdint>
#include <memory>
#include <SDL2/SDL.h>
#include <memory.hpp>
#include <array>

namespace Gameboy {

class Memory;

enum class PPUMode {
    Mode0,
    Mode1,
    Mode2,
    Mode3
};

class PPU {
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture;
    std::shared_ptr<Memory> mMemory;
    PPUMode mCurMode;
    PPU() = default;
    uint32_t mDots;
    uint32_t mWindowLines;
    std::array<uint32_t, 160*144> mPixels;

public:

uint8_t mTiles[384][8][8];

void RenderFrame();
    void Step(uint32_t& cycles);
    static std::unique_ptr<PPU> Create(SDL_Window* win, std::shared_ptr<Memory> mem){
        if(win == nullptr) return nullptr;
        if(mem == nullptr) return nullptr;
        return std::make_unique<PPU>(win, mem);
    }
    
    PPU(SDL_Window*, std::shared_ptr<Memory>);
    ~PPU(){}
};

}
#endif