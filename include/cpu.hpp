#ifndef __CPU_H__
#define __CPU_H__
#include <cstdint>
#include <memory>
#include <memory.hpp>
#include <vector>

namespace Gameboy {

struct Clock {
    uint32_t CyclesPerSecond;
    float FramesPerSecond;
    uint32_t CyclesPerFrame;
    uint32_t CurrentFrameCycles;
    Clock();
    ~Clock(){}
};

class CPU {
    uint8_t A, F, B, C, D, E, H, L;
    uint16_t PC, SP;

    // set 16
    inline void SetAF(uint16_t);
    inline void SetBC(uint16_t);
    inline void SetDE(uint16_t);
    inline void SetHL(uint16_t);

    inline uint16_t GetAF();
    inline uint16_t GetBC();
    inline uint16_t GetDE();
    inline uint16_t GetHL();

    // set 8
    inline void SetA(uint8_t);
    inline void SetF(uint8_t);
    inline void SetB(uint8_t);
    inline void SetC(uint8_t);
    inline void SetD(uint8_t);
    inline void SetE(uint8_t);
    inline void SetH(uint8_t);
    inline void SetL(uint8_t);

    inline uint8_t GetA();
    inline uint8_t GetF();
    inline uint8_t GetB();
    inline uint8_t GetC();
    inline uint8_t GetD();
    inline uint8_t GetE();
    inline uint8_t GetH();
    inline uint8_t GetL();

    inline uint8_t GetZF(){ return F & (1 << 7); }
    inline void SetZF(bool f){ if(f) { F |= (1 << 7); } else { F &= ~(1 << 7); } }
    inline uint8_t GetNF(){ return F & (1 << 6); }
    inline void SetNF(bool f){ if(f) { F |= (1 << 6); } else { F &= ~(1 << 6); } }
    inline uint8_t GetHF(){ return F & (1 << 5); }
    inline void SetHF(bool f){ if(f) { F |= (1 << 5); } else { F &= ~(1 << 5); } }
    inline uint8_t GetCF(){ return F & (1 << 4); }
    inline void SetCF(bool f){ if(f) { F |= (1 << 4); } else { F &= ~(1 << 4); } }

    std::shared_ptr<Memory> mMemory;

    // not a fan but whatever
    std::vector<uint8_t> mROM;

    bool mHalt { false };

    CPU() = default;
public:

    bool GetHalted() { return mHalt; }

    void Step(uint32_t& cycles);

    void LoadBios(std::string);
    void LoadROM(std::string);

    static std::unique_ptr<CPU> Create(std::shared_ptr<Memory> mem){
        return std::make_unique<CPU>(mem);
    }
    
    CPU(std::shared_ptr<Memory>);
    ~CPU();
};

}
#endif