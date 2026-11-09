#pragma once
#include "coreoptions.h"
#include "memorybank.h"
#include "registerbank.h"
#include "input.h"
namespace AtemigoChip8::core
{
    class Chip8Machine
    {
        MemoryBank *memory;
        RegisterBank *registerBank;
        Input *input;
        // CPU
        // Font
        CoreOptions options;

    public:
        static const uint16_t fontBeginAddress = 0x50;
        static const uint16_t stackBeginAddress = 0xEBF;
        static const uint16_t frameBufferBeginAddress = 0xEC0;
        static const uint16_t programBeginAddress = 0x200;
        static const size_t maxProgramSize = 3232;
        
        Chip8Machine()
        {
            memory = new MemoryBank();
            registerBank = new RegisterBank();
            input = new Input();
        }

        ~Chip8Machine()
        {
            delete memory;
            delete registerBank;
            delete input;
        }

        void runFrame()
        {
        }

        bool loadGame()
        {
            return true;
        }

        void reset()
        {
        }

        void saveState()
        {
        }

        void loadState()
        {
        }

        void getMachineInfo()
        {
        }

        void updateCoreOptions(const CoreOptions &newOptions)
        {
            options = newOptions;
        }
    };
}