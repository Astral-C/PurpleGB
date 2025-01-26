#include <ppu.hpp>
#include <bitset>
#include <iostream>
#include <cstring>

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
        mCycles = 0;
    }

    void PPU::RenderFrame(){
        SDL_RenderClear(mRenderer);

        int32_t pitch = 0;
        uint32_t* pixels = nullptr;
        SDL_LockTexture(mTexture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);

        //update
        std::memcpy(pixels, mPixels.data(), mPixels.size());

        SDL_UnlockTexture(mTexture);
        SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);
        SDL_RenderPresent(mRenderer);  
        mCycles = 0;
    }

    void  PPU::Step(uint32_t& cycles){
        std::bitset<8> control = mMemory->ReadU8(0xFF40);
        std::bitset<8> status = mMemory->ReadU8(0xFF41);
        std::bitset<8> palette = mMemory->ReadU8(0xFF47);

        uint16_t backgroundMapAddr = control[1] ? 0x9800 : 0x9C00;
        uint16_t objectTilesBasePtr = 0x8000;
        uint16_t bgTilesBasePtr = control[4] ? 0x8000 : 0x8800;

        //std::cout << "[PPU] LCD Control bits: " << control << std::endl;
        if(control[0]){ // lcd is on
            uint32_t scanline = mMemory->ReadU8(0xFF44);

            while(mCycles < cycles){
                // check if we'ce completed a scanline
                if(mDots >= 456){
                    mDots = 0;
                    scanline++;
                    mMemory->WriteU8(0xFF44, scanline);
                    if(scanline == 144){
                        // entered vblank, do interrupt
                        mCurMode = PPUMode::Mode1;
                    } else if(scanline > 153){
                        scanline = 0;
                        mMemory->WriteU8(0xFF44, 0);
                        mCurMode = PPUMode::Mode2;
                    } else {
                        mCurMode = PPUMode::Mode2;
                    }
                }

                
                if(mCurMode == PPUMode::Mode2){ // first 80 dots is OAM scan
                    
                    if(mDots >= 79) mCurMode = PPUMode::Mode3; // 0-79 first 80 dots 
                } else if(mCurMode == PPUMode::Mode3){
                    uint8_t scrollX = mMemory->ReadU8(0xFF42);
                    uint8_t scrollY = mMemory->ReadU8(0xFF43);

                    uint8_t yPos = scrollY + mMemory->ReadU8(0xFF44);
                    uint16_t tileRow = (yPos / 8) * 32;

                    for(uint8_t p = 0; p < 160; p++){
                        uint8_t xPos = p + scrollX;

                        uint16_t tileCol = (xPos / 8);
                        uint16_t tileAddr = backgroundMapAddr + tileRow + tileCol;

                        int8_t tileNum = (int8_t)(mMemory->ReadU8(tileAddr));

                        uint16_t tileDataAddr = bgTilesBasePtr + ((tileNum + (control[4] ? 0 : 128)) * 16);
                        uint8_t line = (yPos % 8) * 2;

                        std::bitset<8> l1 = mMemory->ReadU8(tileDataAddr  + line);
                        std::bitset<8> l2 = mMemory->ReadU8(tileDataAddr  + line + 1);

                        int colorBit = ((xPos % 8) - 7) * -1;
                        int colorNum = (l2[colorBit] << 1) | l1[colorBit << 1];
                        uint8_t color = 0;

                        std::bitset<8> palette = mMemory->ReadU8(0xFF47);
                        switch (colorNum){
                        case 0:
                            color = (palette[1] << 1) | palette[0];
                            break;
                        case 1:
                            color = (palette[3] << 1) | palette[2];
                            break;
                        case 2:
                            color = (palette[5] << 1) | palette[4];
                            break;
                        case 3:
                            color = (palette[7] << 1) | palette[6];
                            break;
                        }

                        mPixels[(scanline * 144) + p] = mPalette[color];
                    }

                } else if(mCurMode == PPUMode::Mode0) {
                    //hblank
                }

                mDots++;
                mCycles++;
            }
        }

    }
}