#pragma once
#include "coreoptions.h"
#include "machinestate.h"
#include "coreerror.h"
#include "chip8config.h"
#include "memorybank.h"
#include "registerbank.h"
#include "input.h"
#include "cpu.h"
#include "font.h"
#include "machineinfo.h"
#include "frameinfo.h"
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

    public:
        Chip8Machine(uint32_t seed)
        {
            initMachine(seed);
        }

        // A máquina é dona das peças (ponteiros crus + delete no destrutor):
        // uma cópia faria duas máquinas apagarem as mesmas peças.
        Chip8Machine(const Chip8Machine &) = delete;
        Chip8Machine &operator=(const Chip8Machine &) = delete;

        ~Chip8Machine()
        {
            delete memory;
            delete registerBank;
            delete input;
            delete cpu;
        }

        FrameInfo runFrame(uint16_t inputStatus)
        {
            FrameInfo info;
            cpu->update(inputStatus);
            info.frameBuffer = memory->data() + Chip8Config::frameBufferBeginAddress;
            info.isStopped = cpu->isStopped;
            info.errorData = cpu->errorData;
            info.isEmitting = cpu->isEmittingSound();
            return info;
        }

        bool loadGame(const void *data, size_t size, uint32_t newSeed)
        {
            if (size > Chip8Config::maxProgramSize || data == nullptr || size == 0)
                return false;

            if (!loadInitialState(newSeed))
                return false;

            this->seed = newSeed;

            const uint8_t *d = static_cast<const uint8_t *>(data);
            for (size_t k = 0; k < size; k++)
            {
                memory->write(Chip8Config::programBeginAddress + k, *(d+k));
            }
            return true;
        }

        bool reset(uint32_t newSeed)
        {
            if (!loadInitialState(newSeed))
                return false;
            this->seed = newSeed;
            CoreOptions newOptions;
            options = newOptions;
            return true;
        }

        bool saveState(void *data, size_t size)
        {
            if (size < MachineState::stateSize)
                return false;
            MachineState mState;
            if (!memory->saveState(mState.memory, MemoryBank::kSize))
                return false;
            if (!registerBank->saveState(mState.registerBank, RegisterBank::kSize))
                return false;
            if (!cpu->saveState(mState))
                return false;
            if (!input->saveState(mState.inputPreviousStatus, mState.inputCurrentStatus))
                return false;
            uint8_t *d = static_cast<uint8_t *>(data);
            mState.serialize(d);
            return true;
        }

        bool loadState(const void *data, size_t size)
        {
            if (size < MachineState::stateSize)
                return false;
            MachineState mState;
            const uint8_t *d = static_cast<const uint8_t *>(data);
            if (!mState.checkSignature(d))
                return false;
            mState.deserialize(d);
            return loadMState(mState);
        }

        MachineInfo getMachineInfo() const
        {
            MachineInfo info;
            info.width = Chip8Config::width;
            info.height = Chip8Config::height;
            info.fps = Chip8Config::frequency;
            return info;
        }

        void updateCoreOptions(const CoreOptions &newOptions)
        {
            options = newOptions;
        }

        void *getMemoryRAM()
        {
            return memory->data();
        }

        void *getMemoryVRAM()
        {
            return memory->data()+Chip8Config::frameBufferBeginAddress;
        }

        size_t getMemoryRAMSize()
        {
            return MemoryBank::kSize;
        }

        size_t getMemoryVRAMSize()
        {
            return Chip8Config::frameBufferSize;
        }

    private:
        bool loadInitialState(uint32_t newSeed)
        {
            if (newSeed == 0)
                return false;
            initialState.generatorStatus = newSeed;
            return loadMState(initialState);
        }

        bool loadMState(const MachineState &mState)
        {
            if (mState.generatorStatus == 0)
                return false;
            
            if (!memory->loadState(mState.memory, MemoryBank::kSize))
                return false;
            if (!registerBank->loadState(mState.registerBank, RegisterBank::kSize))
                return false;
            if (!cpu->loadState(mState))
                return false;
            if (!input->loadState(mState.inputPreviousStatus, mState.inputCurrentStatus))
                return false;

            return true;
        }

        void initMachine(uint32_t seed)
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
                this->seed);
            for(size_t k = 0; k < Font::table.size(); k++)
                memory->write(Chip8Config::fontBeginAddress+k, Font::table[k]);
            // Grava o initialState
            memory->saveState(initialState.memory, MemoryBank::kSize);
            registerBank->saveState(initialState.registerBank, RegisterBank::kSize);
            cpu->saveState(initialState);
            input->saveState(initialState.inputPreviousStatus, initialState.inputCurrentStatus);
        }
    };
}