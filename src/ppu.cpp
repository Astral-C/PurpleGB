#include <ppu.hpp>
#include <bitset>

namespace Gameboy {
    uint32_t mPalette[4] = {0xC5CAA4FF, 0x8C926bFF, 0x4A5138FF, 0x181818FF};

    PPU::PPU(SDL_Window* win, std::shared_ptr<Memory> mem){
        mRenderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 160, 144);
        int32_t pitch = 0;
        uint32_t* pixels = nullptr;
        SDL_LockTexture(mTexture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);
        
        for(int x = 0; x < (160*144); x++){
            pixels[x] = mPalette[0];
        }

        SDL_UnlockTexture(mTexture);
        mMemory = mem;
        mCurMode = PPUMode::Mode2;
        mDots = 456;
    }

    void PPU::RenderFrame(){
        SDL_RenderClear(mRenderer);

        int32_t pitch = 0;
        uint32_t* pixels = nullptr;
        SDL_LockTexture(mTexture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);

        //update

        SDL_UnlockTexture(mTexture);
        SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);
        SDL_RenderPresent(mRenderer);  
    }

    void  PPU::Step(uint32_t& cycles){
        
        std::bitset<8> control = mMemory->ReadU8(0xFF40);
        std::bitset<8> status = mMemory->ReadU8(0xFF41);
        std::bitset<8> palette = mMemory->ReadU8(0xFF47);
        uint16_t backgroundMapAddr = control[1] ? 0x9800 : 0x9C00;
        uint16_t objectTilesBasePtr = 0x8000;
        uint16_t bgTilesBasePtr = control[4] == 1 ? 0x8000 : 0x9000;

        if(control[0]){ // lcd is on
            uint32_t scanline = mMemory->ReadU8(0xFF44);

            // check if we'ce completed a scanline
            if(mDots >= 456){
                mDots = 0;
                scanline++;
                mMemory->WriteU8(0xFF44, scanline);
                if(scanline == 144){
                    mCurMode = PPUMode::Mode1;
                } else if(scanline > 153){
                    scanline = 0;
                    mMemory->WriteU8(0xFF44, 0);
                    mCurMode = PPUMode::Mode2;
                } else {
                    mCurMode = PPUMode::Mode2;
                }
            }

            
            for(int dot = mDots; dot < mDots+cycles; dot++){ // per scanline
                if(mCurMode == PPUMode::Mode2){ // first 80 dots is OAM scan
                    
                    if(dot >= 79) mCurMode = PPUMode::Mode3; // 0-79 first 80 dots 
                } else if(mCurMode == PPUMode::Mode3){
                    
                } else if(mCurMode == PPUMode::Mode0) {
                    //hblank
                }
            }
            mDots += cycles;
        }

    }
}