#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <cstdint>
#include <memory>
#include <optional>

namespace Gameboy {

class Memory {
    uint8_t mBuffer[0xFFFF];
public:

    uint8_t* GetBuffer() { return &mBuffer[0]; }

    uint8_t ReadU8(uint16_t);
    uint16_t ReadU16(uint16_t);

    bool WriteU8(uint16_t, uint8_t);
    bool WriteU16(uint16_t, uint16_t);

    static std::shared_ptr<Memory> Create(){
        return std::make_shared<Memory>();
    }

    Memory();
    ~Memory();
};

}

#endif