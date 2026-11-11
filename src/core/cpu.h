#pragma once
#include <cstdint>
#include <array>
#include <cstring>
#include "coreoptions.h"
#include "coreerror.h"
#include "machinestate.h"
#include "chip8config.h"
#include "memorybank.h"
#include "registerbank.h"
#include "input.h"
#include "../utils/helper.h"

namespace AtemigoChip8::core
{
    class CPU
    {
        uint16_t pc;
        uint16_t i;
        int16_t sp;

        uint32_t delayTimer;
        uint32_t soundTimer;
        uint32_t generatorStatus;
        uint16_t frameBufferBeginAddress = 0;
        uint16_t stackBeginAddress = 0;
        uint16_t fontBeginAddress = 0;
        size_t frameBufferSize;

        MemoryBank *memory;
        Input *input;
        RegisterBank *registerBank;
        CoreOptions *options;

        uint16_t fetch()
        {
            uint16_t opcode = (memory->read(pc) << 8) | memory->read(pc + 1);
            pc += 2;
            return opcode;
        }

        bool execute(uint16_t opcode, uint16_t pattern)
        {
            uint8_t X = AtemigoChip8::utils::getNibble(opcode, 1);
            uint8_t Y = AtemigoChip8::utils::getNibble(opcode, 2);
            uint8_t N = AtemigoChip8::utils::getNibble(opcode, 3);
            uint8_t NN = opcode & 0x00FF;
            uint16_t NNN = opcode & 0x0FFF;

            if (isValid(pattern))
            {
                switch (pattern)
                {
                case 0x00E0:
                    for (uint32_t k = 0; k < frameBufferSize; k++)
                    {
                        memory->write(frameBufferBeginAddress + k, 0x00);
                    }
                    break;
                case 0x00EE:
                    pc = pop();
                    break;
                case 0x1000:
                    pc = NNN;
                    break;
                case 0x2000:
                    push(pc);
                    pc = NNN;
                    break;
                case 0x3000:
                    if (registerBank->read(X) == NN)
                    {
                        pc += 2;
                    }
                    break;
                case 0x4000:
                    if (registerBank->read(X) != NN)
                    {
                        pc += 2;
                    }
                    break;
                case 0x5000:
                    if (registerBank->read(X) == registerBank->read(Y))
                    {
                        pc += 2;
                    }
                    break;
                case 0x6000:
                    registerBank->write(X, NN);
                    break;
                case 0x7000:
                    registerBank->write(X, registerBank->read(X) + NN);
                    break;
                case 0x8000:
                    registerBank->write(X, registerBank->read(Y));
                    break;
                case 0x8001:
                    registerBank->write(X, registerBank->read(X) | registerBank->read(Y));
                    break;
                case 0x8002:
                    registerBank->write(X, registerBank->read(X) & registerBank->read(Y));
                    break;
                case 0x8003:
                    registerBank->write(X, registerBank->read(X) ^ registerBank->read(Y));
                    break;
                case 0x8004:
                {
                    uint16_t sum = registerBank->read(X) + registerBank->read(Y);
                    registerBank->write(X, sum);
                    registerBank->write(15, static_cast<uint8_t>(sum >= 0x100));
                }
                break;
                case 0x8005:
                {
                    uint8_t yValue = registerBank->read(Y);
                    uint8_t xValue = registerBank->read(X);
                    uint8_t result = xValue - yValue;
                    registerBank->write(X, result);
                    registerBank->write(15, static_cast<uint8_t>(xValue >= yValue));
                }
                break;
                case 0x8006:
                {
                    if (!options->eightXBehavior)
                    {
                        registerBank->write(X, registerBank->read(Y));
                    }

                    uint8_t vX = registerBank->read(X);
                    registerBank->write(X, vX >> 1);
                    registerBank->write(15, vX & 0x01);
                }
                break;
                case 0x8007:
                {
                    uint8_t yValue = registerBank->read(Y);
                    uint8_t xValue = registerBank->read(X);
                    uint8_t result = yValue - xValue;
                    registerBank->write(X, result);
                    registerBank->write(15, static_cast<uint8_t>(yValue >= xValue));
                }
                break;
                case 0x800E:
                {
                    if (!options->eightXBehavior)
                    {
                        registerBank->write(X, registerBank->read(Y));
                    }

                    uint8_t vX = registerBank->read(X);
                    registerBank->write(X, vX << 1);
                    registerBank->write(15, (vX >> 7) & 0x01);
                }
                break;
                case 0x9000:
                    if (registerBank->read(X) != registerBank->read(Y))
                    {
                        pc += 2;
                    }
                    break;
                case 0xA000:
                    i = NNN;
                    break;
                case 0xB000:
                    if (!options->bNNNBehavior)
                    {
                        pc = NNN + registerBank->read(0x00);
                    }
                    else
                    {
                        pc = NNN + registerBank->read(X);
                    }
                    break;
                case 0xC000:
                    registerBank->write(X, AtemigoChip8::utils::xorshift32(&generatorStatus) & NN);
                    break;
                case 0xD000:
                    break;
                case 0xE0A1:
                    if (!input->isKeyPressed(registerBank->read(X)))
                    {
                        pc += 2;
                    }
                    break;
                case 0xE09E:
                    if (input->isKeyPressed(registerBank->read(X)))
                    {
                        pc += 2;
                    }
                    break;
                case 0xF007:
                    registerBank->write(X, delayTimer);
                    break;
                case 0xF015:
                    delayTimer = registerBank->read(X);
                    break;
                case 0xF018:
                    soundTimer = registerBank->read(X);
                    break;
                case 0xF01E:
                    i += registerBank->read(X);
                    if (options->fX1EBehavior && i >= 0x1000)
                    {
                        registerBank->write(15, 1);
                    }
                    break;
                case 0xF00A:
                {
                    uint8_t key;
                    if (input->takeReleasedKey(&key))
                    {
                        registerBank->write(X, key);
                    }
                    else
                    {
                        pc -= 2;
                    }
                }
                break;
                case 0xF029:
                {
                    uint8_t character = registerBank->read(X) & 0x0F;
                    i = fontBeginAddress + (character * 5);
                }
                break;
                case 0xF033:
                {
                    uint8_t vX = registerBank->read(X);
                    memory->write(i + 2, vX % 10);
                    vX /= 10;
                    memory->write(i + 1, vX % 10);
                    vX /= 10;
                    memory->write(i, vX % 10);
                }
                break;
                case 0xF055:
                {
                    uint16_t destinyAddress = i;

                    for (uint8_t k = 0; k <= X; k++)
                    {
                        memory->write(destinyAddress, registerBank->read(k));
                        destinyAddress++;
                    }

                    if (!options->fXsBehavior)
                    {
                        i += X + 1;
                    }
                }
                break;
                case 0xF065:
                {
                    uint16_t sourceAddress = i;
                    for (uint8_t k = 0; k <= X; k++)
                    {
                        registerBank->write(k, memory->read(sourceAddress));
                        sourceAddress++;
                    }

                    if (!options->fXsBehavior)
                    {
                        i += X + 1;
                    }
                }
                break;
                }
                return true;
            }
            return false;
        }

        uint16_t decode(uint16_t opcode)
        {
            uint8_t kind = AtemigoChip8::utils::getNibble(opcode, 0);
            uint16_t mask = 0xFFFF;

            switch (kind)
            {
            case 0x0:
                mask = 0xFFFF;
                break;
            case 0xE:
            case 0xF:
                mask = 0xF0FF;
                break;
            case 0x5:
            case 0x9:
            case 0x8:
                mask = 0xF00F;
                break;
            case 0x1:
            case 0x2:
            case 0x3:
            case 0x4:
            case 0x6:
            case 0x7:
            case 0xA:
            case 0xB:
            case 0xC:
            case 0xD:
                mask = 0xF000;
                break;
            default:
                break;
            }
            uint16_t pattern = opcode & mask;
            return pattern;
        }

        bool isValid(uint16_t pattern)
        {
            for (uint16_t opcode : validsOpcodes)
            {
                if (pattern == opcode)
                    return true;
            }
            return false;
        }

        const std::array<uint16_t, 34> validsOpcodes{
            0x00E0,
            0x00EE,
            0x1000,
            0x2000,
            0x3000,
            0x4000,
            0x5000,
            0x6000,
            0x7000,
            0x8000,
            0x8001,
            0x8002,
            0x8003,
            0x8004,
            0x8005,
            0x8006,
            0x8007,
            0x800E,
            0x9000,
            0xA000,
            0xB000,
            0xC000,
            0xD000,
            0xE0A1,
            0xE09E,
            0xF007,
            0xF015,
            0xF018,
            0xF01E,
            0xF00A,
            0xF029,
            0xF033,
            0xF055,
            0xF065,
        };

        uint16_t pop()
        {
            sp--;
            uint16_t base = stackBeginAddress + sp * -2;
            uint16_t address = (memory->read(base) << 8) | memory->read(base + 1);
            return address;
        }

        void push(uint16_t addressToPush)
        {
            addressToPush = addressToPush & 0x0FFF;
            uint16_t base = stackBeginAddress + sp * -2;
            memory->write(base, addressToPush >> 8);
            memory->write(base + 1, addressToPush & 0x00FF);
            sp++;
        }

    public:
        bool isStopped = false;
        CoreError errorData;
        CPU(
            MemoryBank *m,
            Input *i,
            RegisterBank *r,
            CoreOptions *o,
            uint32_t seed,
            Chip8Config config)
        {
            assert(seed != 0);
            memory = m;
            input = i;
            registerBank = r;
            options = o;
            delayTimer = 0;
            soundTimer = 0;
            pc = config.programBeginAddress;
            frameBufferBeginAddress = config.frameBufferBeginAddress;
            frameBufferSize = config.frameBufferSize;
            sp = 0;
            stackBeginAddress = config.stackBeginAddress;
            generatorStatus = seed;
            fontBeginAddress = config.fontBeginAddress;
        }

        void update(uint16_t inputStatus)
        {
            if (!isStopped)
            {
                // Atualiza o input
                input->updateInputStatus(inputStatus);
                // Atualiza o timer
                if (delayTimer > 0)
                    delayTimer -= 1;
                if (soundTimer > 0)
                    soundTimer -= 1;
                for (int k = 0; k < options->numberOfInstructs; k++)
                {
                    uint16_t opcode = fetch();
                    uint16_t pattern = decode(opcode);
                    bool executed = execute(opcode, pattern);
                    if (!executed)
                    {
                        // Error opcode invalid
                        isStopped = true;
                        errorData.opcode = opcode;
                        errorData.pc = pc - 2;
                        return;
                    }
                }
            }
        }

        bool saveState(MachineState &state) const
        {
            state.pc = this->pc;
            state.sp = this->sp;
            state.i = this->i;
            state.delayTimer = this->delayTimer;
            state.soundTimer = this->soundTimer;
            state.generatorStatus = this->generatorStatus;
            state.isStopped = this->isStopped;
            state.errorData = this->errorData;
            return true;
        }

        bool loadState(const MachineState &state)
        {
            this->pc = state.pc;
            this->sp = state.sp;
            this->i = state.i;
            this->delayTimer = state.delayTimer;
            this->soundTimer = state.soundTimer;
            this->generatorStatus = state.generatorStatus;
            this->isStopped = state.isStopped;
            this->errorData = state.errorData;
            return true;
        }
    };
}

