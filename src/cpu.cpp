#include <cpu.hpp>
#include <fstream>
#include <iostream>
#include <format>

namespace Gameboy {
    namespace Info {
        uint32_t CyclesPerSecond = 4194304;
        uint32_t FramesPerSecond = 60;
        uint32_t CyclesPerFrame = CyclesPerSecond / FramesPerSecond;
    };

    // set 16
    void CPU::SetAF(uint16_t v){ A = (v >> 8) & 0xFF; F = v & 0xFF; } // wrong?
    void CPU::SetBC(uint16_t v){ B = (v >> 8) & 0xFF; C = v & 0xFF; }
    void CPU::SetDE(uint16_t v){ D = (v >> 8) & 0xFF; E = v & 0xFF; }
    void CPU::SetHL(uint16_t v){ H = (v >> 8) & 0xFF; L = v & 0xFF; }
    
    // set 8
    void CPU::SetA(uint8_t v){ A = v; }
    void CPU::SetF(uint8_t v){ F = v; }
    void CPU::SetB(uint8_t v){ B = v; }
    void CPU::SetC(uint8_t v){ C = v; }
    void CPU::SetD(uint8_t v){ D = v; }
    void CPU::SetE(uint8_t v){ E = v; }
    void CPU::SetH(uint8_t v){ H = v; }
    void CPU::SetL(uint8_t v){ L = v; }

    uint16_t CPU::GetAF(){ return static_cast<uint16_t>(A) << 8 | F; }
    uint16_t CPU::GetBC(){ return static_cast<uint16_t>(B) << 8 | C; }
    uint16_t CPU::GetDE(){ return static_cast<uint16_t>(D) << 8 | E; }
    uint16_t CPU::GetHL(){ return static_cast<uint16_t>(H) << 8 | L; }

    uint8_t CPU::GetA() { return A; }
    uint8_t CPU::GetF() { return F; }
    uint8_t CPU::GetB() { return B; }
    uint8_t CPU::GetC() { return C; }
    uint8_t CPU::GetD() { return D; }
    uint8_t CPU::GetE() { return E; }
    uint8_t CPU::GetH() { return H; }
    uint8_t CPU::GetL() { return L; }

    CPU::CPU(std::shared_ptr<Memory> mem){
        mMemory = mem;
    }

    CPU::~CPU(){}

    void CPU::LoadROM(std::string path){
        std::fstream rom;
        rom.open(path, std::ios::binary | std::ios::in);
        
        rom.seekg(0, std::ios_base::end);
        std::size_t romSize = rom.tellg();
        rom.seekg(0, std::ios_base::beg);

        mROM.resize(romSize);

        rom.read(reinterpret_cast<char*>(mROM.data()), mROM.size());
        
        rom.close();
    }

    void CPU::ExecFrame(){
        uint32_t cycles = 0;
        while(cycles < Info::CyclesPerFrame){
            uint8_t opcode = mROM[PC++];
            //std::cout << "Program Counter: " << std::hex << PC << std::dec << std::endl;
        
            switch (opcode){
                case 0x00: {
                    std::cout << "nop" << std::endl;
                    cycles += 4;
                    break;
                }
                
                case 0x01: {
                    SetBC(static_cast<uint16_t>(mROM[PC++]) | (static_cast<uint16_t>(mROM[PC++]) << 8));
                    std::cout << "ld BC, " << std::hex << (unsigned int)GetBC() << std::dec << std::endl;
                    cycles += 12;
                    break;
                }

                case 0x02: {
                    SetA(mMemory->ReadU8(GetBC()));
                    std::cout << "ld A, (" << std::hex << mMemory->ReadU8(GetBC()) << std::dec << ")" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x03: {
                    std::cout << "inc BC" << std::endl;
                    SetBC(GetBC()+1);
                    cycles += 8;
                    break;
                }

                case 0x04: {
                    std::cout << "inc B" << std::endl;
                    uint8_t b = GetB();
                    SetB(b+1);
                    SetZF(GetB() == 0x00);
                    SetNF(false);
                    SetHF((b & 0x0F) == 0);
                    cycles += 4;
                    break;
                }

                case 0x05: {
                    std::cout << "dec B" << std::endl;
                    uint8_t b = GetB();
                    SetB(b-1);
                    SetZF(GetB() == 0x00);
                    std::cout << "Zero Flag is now " << (bool)GetZF() << std::endl;
                    SetNF(true);
                    SetHF((b & 0x0F) == 0);
                    cycles += 4;
                    break;
                }

                case 0x06: {
                    SetB(mROM[PC++]);
                    std::cout << "ld B, " << std::hex << (unsigned int)GetB() << std::dec << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x07: {
                    std::cout << "rlca" << std::endl;
                    SetA(std::rotl(GetA(), 1));
                    SetZF(0x00);
                    SetNF(0x00);
                    SetHF(0x00);
                    SetCF(GetA() & 0x01);
                    cycles += 4;
                    break;
                }

                case 0x0C: {
                    std::cout << "inc C" << std::endl;
                    SetC(GetC()+1);
                    SetZF(GetC() == 0x00);
                    SetNF(false);
                    SetHF((GetC() & 0x0F) == 0);
                    cycles += 4;
                    break;
                }

                case 0x0D: {
                    std::cout << "dec C" << std::endl;
                    SetC(GetC()-1);
                    SetZF(GetC() == 0x00);
                    SetNF(true);
                    SetHF((GetC() & 0x0F) == 0);
                    cycles += 4;
                    break;
                }

                case 0x0E: {
                    SetC(mROM[PC++]);
                    std::cout << "ld C, " << std::hex << (unsigned int)GetC() << std::dec << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x11: {
                    SetDE(mROM[PC++] | (static_cast<uint16_t>(mROM[PC++]) << 8));
                    std::cout << "ld DE, " << std::hex << (unsigned int)GetDE() << std::dec << std::endl;
                    cycles += 12;
                    break;
                }

                case 0x13: {
                    SetDE(GetDE()+1);
                    std::cout << "inc DE" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x17: {
                    uint8_t t = GetA();
                    SetA((GetA() << 1) | GetCF());
                    SetZF(false);
                    SetNF(false);
                    SetHF(false);
                    SetCF((t >> 7) & 0x01);
                    std::cout << "rla" << std::endl;
                    cycles += 4;
                    break;
                }

                case 0x1A: {
                    std::cout << "LD A,(DE)" << std::endl;
                    SetA(mMemory->ReadU8(GetDE()));
                    cycles += 8;
                    break;
                }

                case 0x20: {
                    int8_t to = static_cast<int8_t>(mROM[PC++]);
                    if(!GetZF()){
                        PC += to;
                        cycles += 12;
                    } else {
                        cycles += 8;
                    }
                    std::cout << "jr NZ, " << (int)to << std::endl;
                    break;
                }

                case 0x21: {
                    SetHL(mROM[PC++] | (static_cast<uint16_t>(mROM[PC++]) << 8));
                    std::cout << "ld HL, " << std::hex << (unsigned int)GetHL() << std::dec << std::endl;
                    cycles += 12;
                    break;
                }

                case 0x22: {
                    mMemory->WriteU8(GetHL()+1, GetA());
                    SetHL(GetHL()+1);
                    std::cout << "ld (HL+), A" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x23: {
                    SetHL(GetHL()+1);
                    std::cout << "inc HL" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x31: {
                    SP = static_cast<uint16_t>(mROM[PC++]) | (static_cast<uint16_t>(mROM[PC++]) << 8);
                    std::cout << "ld SP, " << std::hex << SP << std::dec << std::endl;
                    cycles += 12;
                    break;
                }

                case 0x32: {
                    mMemory->WriteU8(GetHL()-1, GetA());
                    SetHL(GetHL()-1);
                    std::cout << "ld (HL-), A" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x34: {
                    uint8_t t = mMemory->ReadU8(GetHL()) + 1;
                    SetNF(false);
                    SetZF(t == 0);
                    SetHF(mMemory->ReadU8(GetHL()) & 0x0F);
                    mMemory->WriteU8(GetHL(), t);
                    break;
                }

                case 0x3E: {
                    SetA(mROM[PC++]);
                    std::cout << "ld A, " << std::hex << (unsigned int)GetA() << std::dec << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x4F: {
                    SetC(GetA());
                    std::cout << "ld C, A" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x77: {
                    mMemory->WriteU8(GetHL(), GetA());
                    std::cout << "ld (HL), A" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0x7B: {
                    SetA(GetE());
                    std::cout << "ld A, E" << std::endl;
                    break;
                }

                case 0x80: {
                    uint16_t t = static_cast<uint16_t>(GetA()) + static_cast<uint16_t>(GetB());
                    SetNF(false);
                    SetHF((GetA() ^ GetB() ^ t) & 0x10);
                    SetCF(GetA() & t & 0xFF00);
                    SetZF((t & 0xFF) == 0x00);
                    SetA(t & 0xFF);
                    std::cout << "ADD A, B" << std::endl;
                    cycles += 4;
                    break;
                }

                case 0xE0: {
                    SetA(mMemory->ReadU8(0xFF00 | mROM[PC++]));
                    std::cout << "ld (0xFF00 + " << (unsigned int) mROM[PC-1] << "), A" << std::endl;
                    cycles += 12;
                    break;
                }

                case 0xE2: {
                    mMemory->WriteU8(0xFF00 | GetC(), GetA());
                    std::cout << "ld (0xFF00 + C), A" << std::endl;
                    cycles += 8;
                    break;
                }

                case 0xAF: {
                    SetA(GetA()^GetA());
                    std::cout << "xor A, A" << std::endl;
                    cycles += 4;
                    break;
                }

                case 0xC1: {
                    SetBC(mMemory->ReadU16(SP));
                    SP += 2;
                    std::cout << std::format("pop BC ({:04x})", GetBC()) << std::endl;
                    cycles += 12;
                    break;
                } 

                case 0xC5: {
                    SP -= 2;
                    mMemory->WriteU16(SP, GetBC());
                    std::cout << std::format("push BC ({:04x})", mMemory->ReadU16(SP)) << std::endl;
                    cycles += 16;
                    break;
                }

                case 0xC9: {
                    PC = mMemory->ReadU16(SP);
                    SP += 2;
                    std::cout << std::format("RET ({:04x})", PC) << std::endl;
                    cycles += 16;
                    break;
                }

                case 0xCB: {
                    uint8_t cbopcode = mROM[PC++];
                    cycles += 4;
                    switch(cbopcode){
                        case 0x11: {
                            uint8_t t = GetC();
                            SetC((GetC() << 1) | GetCF());
                            SetNF(false);
                            SetHF(false);
                            SetCF((t >> 7) & 0x01); // if bit 7 was set before rl we end up w a carry
                            SetZF(GetC() == 0x00);
                            std::cout << "rl C" << std::endl;
                            cycles += 4;
                            break;
                        }
                        case 0x7C: {
                            SetHF(true);
                            SetNF(false);
                            SetZF((GetH() >> 7) == 0);
                            std::cout << "bit 7, H " << std::hex << (unsigned int)GetH() << " " << (GetH() & 0x80) << std::dec << std::endl;
                            cycles += 4;
                            break;
                        }
                    }

                    break;
                }

                case 0xCD: {
                    uint16_t addr = static_cast<uint16_t>(mROM[PC++]) | (static_cast<uint16_t>(mROM[PC++]) << 8);
                    SP -= 2;
                    mMemory->WriteU16(SP, PC);
                    std::cout << std::format("Wrote Return Pointer {:04x}", mMemory->ReadU16(SP)) << std::endl;
                    PC = addr;
                    std::cout << "CALL " << std::hex << addr << std::dec << std::endl;
                    cycles += 24;
                    break;
                }

                case 0xFE: {
                    uint8_t u8 = mROM[PC++];
                    uint8_t t = GetA() - u8;
                    SetZF(t == 0);
                    SetNF(true);
                    SetHF((GetA() & 0x0F) == 0);
                    SetCF(u8 > GetA());
                    std::cout << "CP A, " << std::hex << (unsigned int)u8 << std::dec << std::endl;
                    break;
                }

                default: {
                    std::cout << "Unimplemented Opcode: " << std::hex << (unsigned int)opcode << std::dec << std::endl;
                    cycles += 4;
                    break;
                }
            }
        }
    }

}