#include <application.hpp>

Application::Application(){
    if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0){
        SDL_Quit();
        return;
    } 

    mWindow = SDL_CreateWindow("prpgb", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160, 144, 0);

    mMemory = Gameboy::Memory::Create();
    mCPU = Gameboy::CPU::Create(mMemory);
    mPPU = Gameboy::PPU::Create(mWindow, mMemory);

    mCPU->LoadROM("cpu_instrs.gb");
    mCPU->LoadBios("dmg_boot.bin");
}

void Application::Run(){
    while(!mExit){

        while(mClock.CurrentFrameCycles < mClock.CyclesPerFrame){
            mCPU->Step(mClock.CurrentFrameCycles);
            mPPU->Step(mClock.CurrentFrameCycles);
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
    SDL_Quit();
}

Application::~Application(){}