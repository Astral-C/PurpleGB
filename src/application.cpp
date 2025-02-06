#include <application.hpp>
#include <stb_image_write.h>

Application::Application(){
    if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0){
        SDL_Quit();
        return;
    } 

    mWindow = SDL_CreateWindow("prpgb", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160, 144, 0);

    mMemory = Gameboy::Memory::Create();
    mCPU = Gameboy::CPU::Create(mMemory);
    mPPU = Gameboy::PPU::Create(mWindow, mMemory);
    mMemory->SetPPURef(mPPU);

    //mCPU->LoadROM("cpu_instrs.gb");
    mCPU->LoadROM("tetris.gb");
    mCPU->LoadBios("dmg_boot.bin");
}

void Application::Run(){
    while(!mExit){

        while(mClock.CurrentFrameCycles < mClock.CyclesPerFrame){
            uint32_t cpu_cycles = 0;
            mCPU->Step(cpu_cycles);
            mPPU->Step(cpu_cycles);
            mClock.CurrentFrameCycles += cpu_cycles;
        }

        mPPU->RenderFrame();

        //if(mCPU->GetHalted()){
        //    mExit = true;
        //}

        SDL_Event evt;
        while(SDL_PollEvent(&evt) != 0) {
            switch (evt.type) {
                case SDL_QUIT:    
                    mExit = true;        
                    break;
            }
        }


        mPrevTime = mCurTime;
        mCurTime = SDL_GetTicks64();

        uint64_t curFrameTicks = mCurTime - mPrevTime;

        mDelta = ((float)curFrameTicks / 10000.0f);
        mDelta = 0.001f > mDelta ? 0.001f : mDelta;

        if(curFrameTicks < (1000 / mClock.FramesPerSecond)){
            SDL_Delay((1000 / mClock.FramesPerSecond) - curFrameTicks);
        }
        mClock.CurrentFrameCycles = 0; //reset cycles for next frame
    }

    uint32_t palette[4] = {0xC5CAA4FF, 0x8C926BFF, 0x4A5138FF, 0x181818FF};
    std::array<uint32_t, 384*(8*8)> tileImgData;

    for(int t = 0; t < 384; t++){
        for(int y = 0; y < 8; y++){
            for(int x = 0; x < 8; x++){
                tileImgData[(64 * t) + (y * 8) + x] =  palette[mPPU->mTiles[t][y][x]];
            }
        }
    }

    stbi_write_png("tiles.png", 8, 8*384, 4, tileImgData.data(), 8*4);

    SDL_Quit();
}

Application::~Application(){}