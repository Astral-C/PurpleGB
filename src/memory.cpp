#include <memory.hpp>
#include <cstring>

namespace Gameboy {

uint8_t Memory::ReadU8(uint16_t address){
    // TODO: access control
    return mBuffer[address];
}

uint16_t Memory::ReadU16(uint16_t address){
    return static_cast<uint16_t>(mBuffer[address]) | static_cast<uint16_t>(mBuffer[address+1]) << 8;
}

bool Memory::WriteU8(uint16_t address, uint8_t data){
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