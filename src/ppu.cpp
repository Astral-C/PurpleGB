#include <ppu.hpp>

namespace Gameboy {
    PPU::PPU(SDL_Window* win, std::shared_ptr<Memory> mem){
        mRenderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        mMemory = mem;
    }

    void  PPU::UpdateGraphics(){
        SDL_RenderClear(mRenderer);

        SDL_RenderPresent(mRenderer);  
    }
}