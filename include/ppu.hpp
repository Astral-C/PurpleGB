#ifndef __PPU_H__
#define __PPU_H__
#include <cstdint>
#include <memory>
#include <SDL2/SDL.h>
#include <memory.hpp>

namespace Gameboy {

class PPU {
    SDL_Renderer* mRenderer;
    std::shared_ptr<Memory> mMemory;
    PPU() = default;

public:
    void UpdateGraphics();
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