#include <ppu.hpp>
#include <bitset>
#include <iostream>
#include <cstring>
#include <filesystem>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <format>

namespace Gameboy {
    uint32_t mPalette[4] = {0xC5CAA4FF, 0x8C926BFF, 0x4A5138FF, 0x181818FF};

    PPU::PPU(SDL_Window* win, std::shared_ptr<Memory> mem){
        mRenderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
        SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_NONE);

        std::fill(mPixels.begin(), mPixels.end(), mPalette[0]);

        int32_t pitch = 0;
        uint32_t* pixels = nullptr;
        SDL_LockTexture(mTexture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);
        
        std::memcpy(pixels, mPixels.data(), mPixels.size()*4);

        SDL_UnlockTexture(mTexture);
        mMemory = mem;
        mCurMode = PPUMode::Mode2;
        mDots = 0;
        mWindowLines = 0;
    }

    void PPU::RenderFrame(){
        SDL_RenderClear(mRenderer);

        int32_t pitch = 0;
        uint32_t* pixels = nullptr;
        SDL_LockTexture(mTexture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);

        //update
        std::memcpy(pixels, mPixels.data(), mPixels.size()*4);

        SDL_UnlockTexture(mTexture);
        SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);
        SDL_RenderPresent(mRenderer);  
    }

    void  PPU::Step(uint32_t& cycles){
        std::bitset<8> control = mMemory->ReadU8(0xFF40);
        std::bitset<8> status = mMemory->ReadU8(0xFF41);
        std::bitset<8> palette = mMemory->ReadU8(0xFF47);

        uint16_t objectTilesBasePtr = 0x8000;

        uint16_t tilesBasePtr = control[4] ? 0x8000 : 0x8800;
        uint16_t backgroundMapAddr = control[3] ? 0x9C00 : 0x9800;
        
        uint16_t oamMemPtr = 0xFE00;

        std::array<uint32_t, 10> ScanlineOAMEntries;

        mDots += cycles;

        //std::cout << "[PPU] LCD Control bits: " << control << std::endl;
        uint32_t scanline = mMemory->ReadU8(0xFF44);

            // check if we'ce completed a scanline
        if(mDots >= 456){
            mDots = 0;
            scanline++;
            mMemory->WriteU8(0xFF44, scanline);
            if(scanline >= 144){
                // entered vblank, do interrupt
                mCurMode = PPUMode::Mode1;
                mWindowLines = 0;
            } else if(scanline >= 153){
                scanline = 0;
                mMemory->WriteU8(0xFF44, 0);
                mCurMode = PPUMode::Mode2;
            } else {
                mCurMode = PPUMode::Mode2;
            }
        }
        if(control[0]){ // lcd is on

                
            if(mCurMode == PPUMode::Mode2){ // first 80 dots is OAM scan
                if(mDots >= 80){
                    mDots = 0;

                    // do oam search

                    mCurMode = PPUMode::Mode3; // 0-79 first 80 dots
                } 
            } else if(mCurMode == PPUMode::Mode3){
                if(mDots >= 172){
                    mDots = 0;

                    uint8_t scrollY = mMemory->ReadU8(0xFF42);
                    uint8_t scrollX = mMemory->ReadU8(0xFF43);

                    for(uint8_t c = 0; c < 160; c++){

                        uint16_t x = (c + scrollX) % 255;
                        uint16_t y = (scanline + scrollY) % 255;

                        uint16_t col = floor(x / 8);
                        uint16_t row = floor(y / 8);
                        uint16_t tileOffset = backgroundMapAddr + (row * 32) + col;
                        uint16_t tileID = (mMemory->ReadU8(tileOffset));

                        mPixels[(scanline * 160) + c] =  mPalette[mTiles[tileID][y%8][x%8]];

                    }
                    mCurMode = PPUMode::Mode0;
                }
            } else if(mCurMode == PPUMode::Mode0) {
                //hblank
            }
        }

    }
}