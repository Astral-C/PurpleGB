#include <cpu.hpp>
#include <fstream>
#include <iostream>
#include <format>

namespace Gameboy {
    Clock::Clock() {
        CyclesPerSecond = 4194304;
        FramesPerSecond = 60.0f; // weh
        CyclesPerFrame = CyclesPerSecond / FramesPerSecond;

        CurrentFrameCycles = 0;
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
        PC = 0x00;
    }

    CPU::~CPU(){}

    void CPU::LoadBios(std::string path){
        std::fstream rom;
        rom.open(path, std::ios::binary | std::ios::in);

        rom.read((char*)mMemory->GetBuffer(), 0xFF);
        
        rom.close();
    }
    
    void CPU::LoadROM(std::string path){
        std::fstream rom;
        rom.open(path, std::ios::binary | std::ios::in);
        
        rom.seekg(0, std::ios_base::end);
        std::size_t romSize = rom.tellg();
        rom.seekg(0, std::ios_base::beg);
        rom.read((char*)mMemory->GetBuffer(), romSize);
        
        rom.close();
    }

    void CPU::Step(uint32_t& cycles){
        uint8_t opcode = mMemory->ReadU8(PC++);
        std::cout << std::format("0x{:04x}[{:02x}]: ", PC, opcode);
    
        switch (opcode){
            case 0x00: {
                std::cout << "nop" << std::endl;
                cycles += 4;
                break;
            }
            
            case 0x01: {
                SetBC(static_cast<uint16_t>(mMemory->ReadU8(PC++)) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8));
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
                SetB(mMemory->ReadU8(PC++));
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
            case 0x0B: {
                SetBC(GetBC()-1);
                std::cout << "dec BC" << std::endl;
                cycles += 8;
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
                SetC(mMemory->ReadU8(PC++));
                std::cout << "ld C, " << std::hex << (unsigned int)GetC() << std::dec << std::endl;
                cycles += 8;
                break;
            }
            case 0x11: {
                SetDE(mMemory->ReadU8(PC++) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8));
                std::cout << "ld DE, " << std::hex << (unsigned int)GetDE() << std::dec << std::endl;
                cycles += 12;
                break;
            }
            case 0x12: {
                mMemory->WriteU8(GetDE(), GetA());
                std::cout << "ld (DE), " << std::hex << (unsigned int)GetA() << std::dec << std::endl;
                cycles += 8;
                break;
            }
            case 0x13: {
                SetDE(GetDE()+1);
                std::cout << "inc DE" << std::endl;
                cycles += 8;
                break;
            }
            case 0x15: {
                std::cout << "dec D" << std::endl;
                SetD(GetD()-1);
                SetZF(GetD() == 0x00);
                SetNF(true);
                SetHF((GetD() & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x16: {
                SetD(mMemory->ReadU8(PC++));
                std::cout << "ld D, " << GetD() << std::endl;
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
            case 0x18: {
                int8_t to = static_cast<int8_t>(mMemory->ReadU8(PC++));
                std::cout << "JR " << std::hex << (int)mMemory->ReadU8(PC) << std::dec << std::endl;
                PC += to;
                cycles += 12;
                break;
            }
            case 0x1A: {
                std::cout << "LD A,(DE)" << std::endl;
                SetA(mMemory->ReadU8(GetDE()));
                cycles += 8;
                break;
            }
            case 0x1C: {
                std::cout << "INC E" << std::endl;
                SetE(GetE()+1);
                SetZF(GetE() == 0x00);
                SetNF(false);
                SetHF((GetE() & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x1D: {
                uint8_t e = GetE();
                SetE(e-1);
                SetZF(GetE() == 0x00);
                std::cout << "dec E"<< std::endl;
                SetNF(true);
                SetHF((e & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x1E: {
                SetE(mMemory->ReadU8(PC++));
                std::cout << "LD E, " << (unsigned int)mMemory->ReadU8(PC-1) << std::endl;
                cycles += 8;
                break;
            }
            case 0x20: {
                int8_t to = static_cast<int8_t>(mMemory->ReadU8(PC++));
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
                SetHL(mMemory->ReadU8(PC++) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8));
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
            case 0x24: {
                std::cout << "inc H" << std::endl;
                uint8_t h = GetH();
                SetH(h+1);
                SetZF(GetH() == 0x00);
                SetNF(false);
                SetHF((h & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x26: {
                SetH(mMemory->ReadU8(PC++));
                std::cout << std::format("ld H, {:02x}", GetH()) << std::endl;
                cycles += 8;
                break;
            }
            case 0x28: {
                int8_t to = static_cast<int8_t>(mMemory->ReadU8(PC++));
                if(GetZF()){
                    PC += to;
                    cycles += 12;
                } else {
                    cycles += 8;
                }
                std::cout << "jr Z, " << (int)to << std::endl;
                break;
            }
            case 0x2A: {
                SetA(mMemory->ReadU8(GetHL()+1));
                SetHL(GetHL()+1);
                std::cout << "ld A, (HL+)" << std::endl;
                cycles += 8;
                break;
            }
            case 0x2C: {
                std::cout << "inc L" << std::endl;
                uint8_t l = GetL();
                SetL(l+1);
                SetZF(GetL() == 0x00);
                SetNF(false);
                SetHF((l & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x2D: {
                std::cout << "dec L" << std::endl;
                uint8_t l = GetL();
                SetL(l-1);
                SetZF(GetL() == 0x00);
                SetNF(true);
                SetHF((l & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x2E: {
                SetL(mMemory->ReadU8(PC++));
                std::cout << "LD L, " << (unsigned int)mMemory->ReadU8(PC-1) << std::endl;
                cycles += 8;
                break;
            }
            case 0x2F: {
                SetA(~GetA());
                SetHF(true);
                SetNF(true);
                std::cout << "CPL A" << std::endl;
                cycles += 4;
                break;
            }
            case 0x31: {
                SP = static_cast<uint16_t>(mMemory->ReadU8(PC++)) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8);
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
                std::cout << "inc (HL)" << std::endl;
                cycles += 12;
                break;
            }
            case 0x36: {
                mMemory->WriteU8(GetHL(), mMemory->ReadU8(PC++));
                std::cout << "ld (HL), " << std::hex << mMemory->ReadU8(GetHL()) << std::endl;
                cycles += 12;
                break;
            }
            case 0x3C: {
                std::cout << "inc A" << std::endl;
                uint8_t a = GetA();
                SetA(a+1);
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF((a & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x3D: {
                uint8_t a = GetA();
                SetA(a-1);
                SetZF(GetA() == 0x00);
                std::cout << "dec A"<< std::endl;
                SetNF(true);
                SetHF((a & 0x0F) == 0);
                cycles += 4;
                break;
            }
            case 0x3E: {
                SetA(mMemory->ReadU8(PC++));
                std::cout << "ld A, " << std::hex << (unsigned int)GetA() << std::dec << std::endl;
                cycles += 8;
                break;
            }
            case 0x44: {
                SetB(GetH());
                std::cout << "ld B, H" << std::endl;
                cycles+=4;
                break;
            }
            case 0x46: {
                SetB(mMemory->ReadU8(GetHL()));
                std::cout << std::format("ld B, ({:04x})", GetHL()) << std::endl;
                cycles += 8;
                break;
            }
            case 0x47: {
                SetB(GetA());
                std::cout << "ld B, A" << std::endl;
                cycles+=4;
                break;
            }
            case 0x4E: {
                SetC(mMemory->ReadU8(GetHL()));
                std::cout << std::format("ld C, ({:04x})", GetHL()) << std::endl;
                cycles += 8;
                break;
            }
            case 0x4F: {
                SetC(GetA());
                std::cout << "ld C, A" << std::endl;
                cycles += 4;
                break;
            }
            case 0x56: {
                SetD(mMemory->ReadU8(GetHL()));
                std::cout << std::format("ld D, ({:04x})", GetHL()) << std::endl;
                cycles += 8;
                break;
            }
            case 0x57: {
                SetD(GetA());
                std::cout << "ld D, A" << std::endl;
                cycles += 4;
                break;
            }
            case 0x66: {
                SetH(mMemory->ReadU8(GetHL()));
                std::cout << "ld H, (HL)" << std::endl;
                cycles += 8;
                break;
            }
            case 0x67: {
                SetH(GetA());
                std::cout << "ld H, A" << std::endl;
                cycles += 4;
                break;
            }
            case 0x77: {
                mMemory->WriteU8(GetHL(), GetA());
                std::cout << "ld (HL), A" << std::endl;
                cycles += 8;
                break;
            }
            case 0x78: {
                SetA(GetB());
                std::cout << "ld A, B" << std::endl;
                cycles += 4;
                break;
            }
            case 0x79: {
                SetA(GetC());
                std::cout << "ld A, C" << std::endl;
                cycles += 4;
                break;
            }
            case 0x7A: {
                SetA(GetD());
                std::cout << "ld A, D" << std::endl;
                cycles += 4;
                break;
            }
            case 0x7B: {
                SetA(GetE());
                std::cout << "ld A, E" << std::endl;
                cycles += 4;
                break;
            }
            case 0x7C: {
                SetA(GetH());
                std::cout << "ld A, H" << std::endl;
                cycles += 4;
                break;
            }
            case 0x7D: {
                SetA(GetL());
                std::cout << "ld A, L" << std::endl;
                cycles += 4;
                break;
            }
            case 0x7E: {
                SetA(mMemory->ReadU8(GetHL()));
                std::cout << std::format("ld A, ({:04x})", GetHL()) << std::endl;
                cycles += 8;
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
            case 0x85: {
                uint16_t t = static_cast<uint16_t>(GetA()) + static_cast<uint16_t>(GetL());
                SetNF(false);
                SetHF((GetA() ^ GetL() ^ t) & 0x10);
                SetCF(GetA() & t & 0xFF00);
                SetZF((t & 0xFF) == 0x00);
                SetA(t & 0xFF);
                std::cout << "ADD A, L" << std::endl;
                cycles += 4;           
                break;                
            }
            case 0x86: {
                uint16_t t = static_cast<uint16_t>(GetA()) + static_cast<uint16_t>(mMemory->ReadU8(GetHL()));
                SetNF(false);
                SetHF((GetA() ^ mMemory->ReadU8(GetHL()) ^ t) & 0x10);
                SetCF(GetA() & t & 0xFF00);
                SetA(t & 0xFF);
                SetZF(GetA() == 0x00);
                std::cout << "ADD A, (HL)" << std::endl;   
                cycles += 8;           
                break;
            }
            case 0x90: {
                uint16_t t = static_cast<uint16_t>(GetA()) - static_cast<uint16_t>(GetB());
                SetNF(true);
                SetHF((GetA() ^ GetB() ^ t) & 0x10);
                SetCF(GetB() > GetA());
                SetZF((t & 0xFF) == 0x00);
                SetA(t & 0xFF);
                std::cout << "SUB A, B" << std::endl;
                cycles += 4;
                break;
            }
            case 0x9A: {
                uint16_t t = static_cast<uint16_t>(GetA()) - (static_cast<uint16_t>(GetD() + GetCF()));
                SetNF(true);
                SetHF((GetA() ^ GetD() ^ GetCF() ^ t) & 0x10);
                SetCF(GetD() + GetCF() > GetA());
                SetZF((t & 0xFF) == 0x00);
                SetA(t & 0xFF);
                std::cout << "SBC A, D" << std::endl;
                cycles += 4;
                break;
            }
            case 0x9E: {
                uint16_t t = static_cast<uint16_t>(GetA()) - (static_cast<uint16_t>(mMemory->ReadU8(GetHL()) + GetCF()));
                SetNF(true);
                SetHF((GetA() ^ mMemory->ReadU8(GetHL()) ^ GetCF() ^ t) & 0x10);
                SetCF(mMemory->ReadU8(GetHL()) + GetCF() > GetA());
                SetZF((t & 0xFF) == 0x00);
                SetA(t & 0xFF);
                std::cout << "SBC A, (HL)" << std::endl;
                cycles += 8;
                break;
            }
            case 0xD5: {
                SP -= 2;
                mMemory->WriteU16(SP, GetDE());
                std::cout << std::format("push DE ({:04x})", mMemory->ReadU16(SP)) << std::endl;
                cycles += 16;
                break;
            }
            case 0xD6: {
                uint16_t immediate = static_cast<uint16_t>(mMemory->ReadU8(PC++));
                uint16_t t = static_cast<uint16_t>(GetA()) - immediate;
                SetNF(true);
                SetHF((GetA() ^ immediate ^ t) & 0x10);
                SetCF(immediate > GetA());
                SetZF((t & 0xFF) == 0x00);
                SetA(t & 0xFF);
                std::cout << "SUB A, " << immediate << std::endl;
                cycles += 8;
                break;
            }
            case 0xE0: {
                mMemory->WriteU8(0xFF00 + mMemory->ReadU8(PC++), GetA());
                std::cout << std::format("ld (0xFF00 + 0x{:02x}), A[{:02x}]", mMemory->ReadU8(PC-1), GetA()) << std::dec << std::endl;
                cycles += 12;
                break;
            }
            case 0xE1: {
                SetHL(mMemory->ReadU16(SP));
                SP += 2;
                std::cout << std::format("pop HL ({:04x})", GetHL()) << std::endl;
                cycles += 12;
                break;
            } 
            case 0xE2: {
                mMemory->WriteU8(0xFF00 | GetC(), GetA());
                std::cout << "ld (0xFF00 + C), A" << std::endl;
                cycles += 8;
                break;
            }
            case 0xE5: {
                SP -= 2;
                mMemory->WriteU16(SP, GetHL());
                std::cout << std::format("push HL ({:04x})", mMemory->ReadU16(SP)) << std::endl;
                cycles += 16;
                break;
            }
            case 0xE6: {
                SetA(GetA() & mMemory->ReadU8(PC++));
                std::cout << "AND A, " << mMemory->ReadU8(PC-1) << std::endl;
                SetZF(GetA() == 0);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                cycles += 8;
                break;
            }
            case 0xE9: {
                PC = GetHL();
                cycles += 4;
                std::cout << "jp " << (int)GetHL() << std::endl;
                break;
            }
            case 0xEA: {
                uint16_t addr = mMemory->ReadU8(PC++) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8);
                mMemory->WriteU8(addr, GetA());
                std::cout << std::format("ld ({:04x}), A", addr) << std::endl;
                cycles += 16;
                break;
            }
            case 0xA0: {
                SetA(GetA()&GetB());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                std::cout << "and A, B" << std::endl;
                cycles += 4;
                break;
            }
            case 0xA1: {
                SetA(GetA()&GetC());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                std::cout << "and A, C" << std::endl;
                cycles += 4;
                break;
            }
            case 0xA2: {
                SetA(GetA()&GetD());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                std::cout << "and A, D" << std::endl;
                cycles += 4;
                break;
            }
            case 0xA3: {
                SetA(GetA()&GetE());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                std::cout << "and A, E" << std::endl;
                cycles += 4;
                break;
            }
            case 0xA4: {
                SetA(GetA()&GetH());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                std::cout << "and A, H" << std::endl;
                cycles += 4;
                break;
            }
            case 0xA5: {
                SetA(GetA()&GetL());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                std::cout << "and A, L" << std::endl;
                cycles += 4;
                break;
            }
            case 0xA7: {
                SetA(GetA()&GetA());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(true);
                SetCF(false);
                std::cout << "and A, A" << std::endl;
                cycles += 4;
                break;
            }
            case 0xA9: {
                SetA(GetA()^GetC());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(false);
                SetCF(false);
                std::cout << "xor A, C" << std::endl;
                cycles += 4;
                break;
            }
            case 0xAE: {
                SetA(GetA()^mMemory->ReadU8(GetHL()));
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(false);
                SetCF(false);
                std::cout << std::format("xor A, ({:04x})", GetHL()) << std::endl;
                cycles += 4;
                break;
            }
            case 0xAF: {
                SetA(GetA()^GetA());
                SetZF(GetA() == 0x00);
                SetNF(false);
                SetHF(false);
                SetCF(false);
                std::cout << "xor A, A" << std::endl;
                cycles += 4;
                break;
            }
            case 0xB0: {
                SetA(GetA()|GetB());
                SetZF(GetA() == 0);
                SetNF(0);
                SetHF(0);
                SetCF(0);
                std::cout << "or A, B" << std::endl;
                cycles += 4;
                break;
            }
            case 0xB1: {
                SetA(GetA()|GetC());
                SetZF(GetA() == 0);
                SetNF(0);
                SetHF(0);
                SetCF(0);
                std::cout << "or A, C" << std::endl;
                cycles += 4;
                break;
            }
            case 0xB3: {
                SetA(GetA()|GetE());
                SetZF(GetA() == 0);
                SetNF(0);
                SetHF(0);
                SetCF(0);
                std::cout << "or A, E" << std::endl;
                cycles += 4;
                break;
            }
            case 0xB7: {
                SetA(GetA()|GetA());
                SetZF(GetA() == 0);
                SetNF(0);
                SetHF(0);
                SetCF(0);
                std::cout << "or A, A" << std::endl;
                cycles += 4;
                break;
            }
            case 0xBE: {
                uint8_t u8 = mMemory->ReadU8(GetHL());
                uint16_t t = static_cast<uint16_t>(GetA()) - static_cast<uint16_t>(u8);
                SetZF((t & 0xFF) == 0x00);
                SetNF(true);
                SetHF((GetA() ^ u8 ^ t) & 0x10);
                SetCF(t & 0xFF00);
                std::cout << std::format("CP A[{:02x}], ", GetA()) << std::hex << (unsigned int)u8 << std::dec << std::endl;
                cycles += 8;
                break;
            }
            case 0xC0: {
                std::cout << "RET NZ" << std::endl;
                if(!GetZF()){
                    PC = mMemory->ReadU16(SP);
                    SP += 2;
                    cycles += 20;
                } else {
                    cycles +=  8;
                }
                break;
            }
            case 0xC1: {
                SetBC(mMemory->ReadU16(SP));
                SP += 2;
                std::cout << std::format("pop BC ({:04x})", GetBC()) << std::endl;
                cycles += 12;
                break;
            } 
            case 0xC3: {
                uint16_t to = static_cast<uint16_t>(mMemory->ReadU8(PC++)) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8);
                PC = to;
                cycles += 16;
                std::cout << "jp " << (int)to << std::endl;
                break;
            }
            case 0xC4: {
                uint16_t addr = static_cast<uint16_t>(mMemory->ReadU8(PC++)) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8);
                std::cout << "CALL NZ, " << std::hex << addr << std::dec << std::endl;
                if(!GetZF()){
                    SP -= 2;
                    mMemory->WriteU16(SP, PC);
                    std::cout << std::format("Wrote Return Pointer {:04x}", mMemory->ReadU16(SP)) << std::endl;
                    PC = addr;
                    cycles += 24;
                } else {
                    cycles +=  12;
                }
                break;
            }
            case 0xC5: {
                SP -= 2;
                mMemory->WriteU16(SP, GetBC());
                std::cout << std::format("push BC ({:04x})", mMemory->ReadU16(SP)) << std::endl;
                cycles += 16;
                break;
            }
            case 0xC6: {
                uint16_t immediate = static_cast<uint16_t>(mMemory->ReadU8(PC++));
                uint16_t t = static_cast<uint16_t>(GetA()) + immediate;
                SetNF(false);
                SetHF((GetA() ^ immediate ^ t) & 0x10);
                SetCF(GetA() & t & 0xFF00);
                SetZF((t & 0xFF) == 0x00);
                SetA(t & 0xFF);
                std::cout << "ADD A, " << immediate << std::endl;
                cycles += 8;
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
                uint8_t cbopcode = mMemory->ReadU8(PC++);
                std::cout << std::endl;
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
                    case 0x47: {
                        SetHF(true);
                        SetNF(false);
                        SetZF((GetH() & 0x01) == 0);
                        std::cout << "bit 0, A " << std::hex << (unsigned int)GetA() << " " << (GetA() & 0x01) << std::dec << std::endl;
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
            case 0xCC: {
                uint16_t addr = static_cast<uint16_t>(mMemory->ReadU8(PC++)) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8);
                std::cout << "CALL Z, " << std::hex << addr << std::dec << std::endl;
                if(GetZF()){
                    SP -= 2;
                    mMemory->WriteU16(SP, PC);
                    std::cout << std::format("Wrote Return Pointer {:04x}", mMemory->ReadU16(SP)) << std::endl;
                    PC = addr;
                    cycles += 24;
                } else {
                    cycles +=  12;
                }
                break;
            }
            case 0xCD: {
                uint16_t addr = static_cast<uint16_t>(mMemory->ReadU8(PC++)) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8);
                SP -= 2;
                mMemory->WriteU16(SP, PC);
                std::cout << std::format("Wrote Return Pointer {:04x}", mMemory->ReadU16(SP)) << std::endl;
                PC = addr;
                std::cout << "CALL " << std::hex << addr << std::dec << std::endl;
                cycles += 24;
                break;
            }

            case 0xCE: {
                uint16_t res = GetA() + mMemory->ReadU8(PC++) + GetCF();
                SetZF(GetA() == 0x00);
                SetNF(0);
                SetHF((GetA() ^ mMemory->ReadU8(PC-1) ^ GetCF()) & 0x10);
                SetCF(res & 0xFF00);
                SetA(res & 0xFF);
                std::cout << std::format("adc A, ", mMemory->ReadU8(PC-1)) << std::endl;
                cycles += 8;
                break;
            }

            case 0xF0: {
                SetA(mMemory->ReadU8(0xFF00 | mMemory->ReadU8(PC++)));
                std::cout << std::format("ld A, (0xFF00 + {:02x})", mMemory->ReadU8(PC-1)) << std::endl;
                cycles += 12;
                break;
            }
            case 0xF1: {
                SetAF(mMemory->ReadU16(SP));
                SP += 2;
                std::cout << std::format("pop AF ({:04x})", GetAF()) << std::endl;
                cycles += 12;
                break;
            } 
            case 0xF5: {
                SP -= 2;
                mMemory->WriteU16(SP, GetAF());
                std::cout << std::format("push AF ({:04x})", mMemory->ReadU16(SP)) << std::endl;
                cycles += 16;
                break;
            }
            case 0xFA: {
                uint16_t addr = static_cast<uint16_t>(mMemory->ReadU8(PC++)) | (static_cast<uint16_t>(mMemory->ReadU8(PC++)) << 8);
                SetA(mMemory->ReadU8(addr));
                std::cout << std::format("ld A, ({:04x})", addr) << std::endl;
                cycles += 12;
                break;
            }
            case 0xFE: {
                uint8_t u8 = mMemory->ReadU8(PC++);
                uint16_t t = static_cast<uint16_t>(GetA()) - static_cast<uint16_t>(u8);
                SetZF((t & 0xFF) == 0x00);
                SetNF(true);
                SetHF((GetA() ^ u8 ^ t) & 0x10);
                SetCF(t & 0xFF00);
                std::cout << std::format("CP A[{:02x}], ", GetA()) << std::hex << (unsigned int)u8 << std::dec << std::endl;
                cycles += 8;
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
