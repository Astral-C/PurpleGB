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

    mCPU->LoadROM("dmg_boot.bin");
}

void Application::Run(){
    while(!mExit){

        mCPU->ExecFrame();
        mPPU->UpdateGraphics();

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

        if(curFrameTicks < (1000 / Gameboy::Info::FramesPerSecond)){
            SDL_Delay((1000 / Gameboy::Info::FramesPerSecond) - curFrameTicks);
        }
    }
    SDL_Quit();
}

Application::~Application(){}