#ifndef __CPU_H__
#define __CPU_H__
#include <cstdint>
#include <memory>
#include <memory.hpp>
#include <vector>

namespace Gameboy {

namespace Info {
    extern uint32_t CyclesPerSecond;
    extern uint32_t FramesPerSecond;
    extern uint32_t CyclesPerFrame;
};

class CPU {
    uint8_t A, F, B, C, D, E, H, L;
    uint16_t PC, SP;

    // set 16
    void SetAF(uint16_t);
    void SetBC(uint16_t);
    void SetDE(uint16_t);
    void SetHL(uint16_t);

    uint16_t GetAF();
    uint16_t GetBC();
    uint16_t GetDE();
    uint16_t GetHL();

    // set 8
    void SetA(uint8_t);
    void SetF(uint8_t);
    void SetB(uint8_t);
    void SetC(uint8_t);
    void SetD(uint8_t);
    void SetE(uint8_t);
    void SetH(uint8_t);
    void SetL(uint8_t);

    uint8_t GetA();
    uint8_t GetF();
    uint8_t GetB();
    uint8_t GetC();
    uint8_t GetD();
    uint8_t GetE();
    uint8_t GetH();
    uint8_t GetL();

    uint8_t GetZF(){ return F & (1 << 7); }
    void SetZF(bool f){ if(f) { F |= (1 << 7); } else { F &= ~(1 << 7); } }

    uint8_t GetNF(){ return F & (1 << 6); }
    void SetNF(bool f){ if(f) { F |= (1 << 6); } else { F &= ~(1 << 6); } }

    uint8_t GetHF(){ return F & (1 << 5); }
    void SetHF(bool f){ if(f) { F |= (1 << 5); } else { F &= ~(1 << 5); } }

    uint8_t GetCF(){ return F & (1 << 4); }
    void SetCF(bool f){ if(f) { F |= (1 << 4); } else { F &= ~(1 << 4); } }

    std::shared_ptr<Memory> mMemory;

    // not a fan but whatever
    std::vector<uint8_t> mROM;

    bool mHalt { false };

    CPU() = default;
public:

    bool GetHalted() { return mHalt; }

    void ExecFrame();

    void LoadROM(std::string);

    static std::unique_ptr<CPU> Create(std::shared_ptr<Memory> mem){
        return std::make_unique<CPU>(mem);
    }
    
    CPU(std::shared_ptr<Memory>);
    ~CPU();
};

}
#endif