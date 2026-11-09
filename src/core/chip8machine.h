#pragma once
#include "coreoptions.h"
#include "memorybank.h"
namespace AtemigoChip8::core {
    class Chip8Machine {
        MemoryBank *memory;
        //CPU
        //MemoryBank
        //RegisterBank
        //Input
        //Font
        CoreOptions options;
        public:
            Chip8Machine()
            {
                memory = new MemoryBank();
            }

            ~Chip8Machine()
            {
                delete memory;
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

            void updateCoreOptions(const CoreOptions& newOptions)
            {
                options = newOptions;
            }
    };
}