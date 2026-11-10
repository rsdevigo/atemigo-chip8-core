#pragma once
#include "coreoptions.h"
#include "machinestate.h"
#include "coreerror.h"
#include "chip8config.h"
#include "memorybank.h"
#include "registerbank.h"
#include "input.h"
#include "cpu.h"
namespace AtemigoChip8::core
{
    class Chip8Machine
    {
        MemoryBank *memory;
        RegisterBank *registerBank;
        Input *input;
        CPU *cpu;
        // Font
        CoreOptions options;
        MachineState initialState;
        uint32_t seed;
        Chip8Config cConfig;
    public:
        

        Chip8Machine(uint32_t seed)
        {
            this->seed = seed;
            memory = new MemoryBank();
            registerBank = new RegisterBank();
            input = new Input();
            cpu = new CPU(
                memory, 
                input, 
                registerBank, 
                &options, 
                this->seed,
                cConfig
            );

            //Grava o initialState
            memory->saveState(initialState.memory, MemoryBank::kSize);
            registerBank->saveState(initialState.registerBank, RegisterBank::kSize);
            cpu->saveState(initialState);
            input->saveState(initialState.inputPreviousStatus, initialState.inputCurrentStatus);
        }

        ~Chip8Machine()
        {
            delete memory;
            delete registerBank;
            delete input;
            delete cpu;
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

        bool saveState(void *data, size_t size)
        {
            MachineState mState;
            memory->saveState(mState.memory, MemoryBank::kSize);
            registerBank->saveState(mState.registerBank, RegisterBank::kSize);
            cpu->saveState(mState);
            input->saveState(mState.inputPreviousStatus, mState.inputCurrentStatus);
            mState.serialize(data);
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