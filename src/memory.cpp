#include <memory.hpp>
#include <cstring>
#include <iostream>

namespace Gameboy {

uint8_t Memory::ReadU8(uint16_t address){
    // TODO: access control
    return mBuffer[address];
}

uint16_t Memory::ReadU16(uint16_t address){
    return static_cast<uint16_t>(mBuffer[address]) | static_cast<uint16_t>(mBuffer[address+1]) << 8;
}

bool Memory::WriteU8(uint16_t address, uint8_t data){
    mBuffer[address] = data;
    // vram write
    if(address > 0x8000 && address < 0x9800){
        std::cout << "VRAM Write!" << std::endl;
        uint16_t idx = address - 0x8000;
        
        uint8_t b1 = 0;
        uint8_t b2 = 0;

        if((address % 2) == 0){
            b1 = data;
            b2 = mBuffer[address+1];
        } else {
            b1 = mBuffer[address-1];
            b2 = data;
        }

        for(int c = 0; c < 8; c++){
            mPPURef->mTiles[idx/16][(idx%16)/2][c] = (((b2 >> (7 - c)) & 0b1) << 1) | ((b1 >> (7 - c)) & 0b1);
        }
    }
    return true;
}

bool Memory::WriteU16(uint16_t address, uint16_t data){
    mBuffer[address] = data & 0xFF;
    mBuffer[address+1] = (data >> 8) & 0xFF;
    return true;
}

Memory::Memory(){
    std::memset(mBuffer, 0, sizeof(mBuffer));
}

Memory::~Memory(){}

};