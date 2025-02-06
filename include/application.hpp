#include <SDL2/SDL.h>
#include <cpu.hpp>
#include <ppu.hpp>
#include <memory.hpp>

class Application {
    bool mExit { false };
    SDL_Window* mWindow { nullptr };

    std::unique_ptr<Gameboy::CPU> mCPU { nullptr };
    std::shared_ptr<Gameboy::PPU> mPPU { nullptr };
    std::shared_ptr<Gameboy::Memory> mMemory { nullptr };

	uint64_t mPrevTime { 0 };
	uint64_t mCurTime { 0 };
	uint64_t mTargetFps { 60 };
	float mDelta { 0.0 };
    Gameboy::Clock mClock;

public:
    static std::unique_ptr<Application> Create(){
        return std::make_unique<Application>();
    }

    void Run();
    Application();
    ~Application();
};