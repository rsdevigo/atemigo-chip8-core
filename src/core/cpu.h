#pragma once
#include <cstdint>
#include <array>
#include <cstring>
#include "coreoptions.h"
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

        void decode(uint16_t opcode)
        {
            uint8_t kind = AtemigoChip8::utils::getNibble(opcode, 0);
            uint8_t X = AtemigoChip8::utils::getNibble(opcode, 1);
            uint8_t Y = AtemigoChip8::utils::getNibble(opcode, 2);
            uint8_t N = AtemigoChip8::utils::getNibble(opcode, 3);
            uint8_t NN = opcode & 0x00FF;
            uint16_t NNN = opcode & 0x0FFF;

            if (isValid(opcode))
            {
                switch (kind)
                {
                case 0x0:
                    /* code */
                    if (X == 0x0 && Y == 0xE && N == 0x0)
                    {
                        // Clear
                        for (int k = 0; k < frameBufferSize; k++)
                        {
                            memory->write(frameBufferBeginAddress + k, 0x00);
                        }
                    }
                    else if (X == 0x0 && Y == 0xE && N == 0xE)
                    {
                        // Return
                        pc = pop();
                    }
                    break;
                case 0x1:
                    /* code */
                    // Jump
                    pc = NNN;
                    break;
                case 0x2:
                    /* code */
                    // Subroutine
                    push(pc);
                    pc = NNN;
                    break;
                case 0x3:
                    /* code */
                    // Skip conditionally(x)
                    if (registerBank->read(X) == NN)
                    {
                        pc += 2;
                    }
                    break;
                case 0x4:
                    /* code */
                    // Skip conditionally(x)
                    if (registerBank->read(X) != NN)
                    {
                        pc += 2;
                    }
                    break;
                case 0x5:
                    // Skip conditionally
                    if (registerBank->read(X) == registerBank->read(Y))
                    {
                        pc += 2;
                    }
                    break;
                case 0x6:
                    /* code */
                    // Set X
                    registerBank->write(X, NN);
                    break;
                case 0x7:
                    /* code */
                    // Add X
                    registerBank->write(X, registerBank->read(X) + NN);
                    break;
                case 0x8:
                    /* code */
                    if (N == 0x0)
                    {
                        registerBank->write(X, registerBank->read(Y));
                    }
                    else if (N == 0x1)
                    {
                        registerBank->write(X, registerBank->read(X) | registerBank->read(Y));
                    }
                    else if (N == 0x2)
                    {
                        registerBank->write(X, registerBank->read(X) & registerBank->read(Y));
                    }
                    else if (N == 0x3)
                    {
                        registerBank->write(X, registerBank->read(X) ^ registerBank->read(Y));
                    }
                    else if (N == 0x4)
                    {
                        uint16_t sum = registerBank->read(X) + registerBank->read(Y);
                        registerBank->write(X, sum);
                        registerBank->write(15, static_cast<uint8_t>(sum >= 0x100));
                    }
                    else if (N == 0x5)
                    {
                        uint8_t yValue = registerBank->read(Y);
                        uint8_t xValue = registerBank->read(X);
                        uint8_t result = xValue - yValue;
                        registerBank->write(X, result);
                        registerBank->write(15, static_cast<uint8_t>(xValue >= yValue));
                    }
                    else if (N == 0x6)
                    {
                        if (!options->eightXBehavior)
                        {
                            registerBank->write(X, registerBank->read(Y));
                        }

                        uint8_t vx = registerBank->read(X);
                        registerBank->write(X, vx >> 1);
                        registerBank->write(15, vx & 0x01);
                    }
                    else if (N == 0x7)
                    {
                        uint8_t yValue = registerBank->read(Y);
                        uint8_t xValue = registerBank->read(X);
                        uint8_t result = yValue - xValue;
                        registerBank->write(X, result);
                        registerBank->write(15, static_cast<uint8_t>(yValue >= xValue));
                    }
                    else if (N == 0xE)
                    {
                        if (!options->eightXBehavior)
                        {
                            registerBank->write(X, registerBank->read(Y));
                        }

                        uint8_t vx = registerBank->read(X);
                        registerBank->write(X, vx << 1);
                        registerBank->write(15, (vx >> 7) & 0x01);
                    }
                    break;
                case 0x9:
                    /* code */
                    // Skip conditionally
                    if (registerBank->read(X) != registerBank->read(Y))
                    {
                        pc += 2;
                    }
                    break;
                case 0xA:
                    /* code */
                    // Set Index
                    i = NNN;
                    break;
                case 0xB:
                    /* code */
                    // Jump offset
                    if (!options->bNNNBehavior)
                    {
                        pc = NNN + registerBank->read(0x00);
                    }
                    else
                    {
                        pc = NNN + registerBank->read(X);
                    }
                    break;
                case 0xC:
                    /* code */
                    // Random
                    registerBank->write(X, AtemigoChip8::utils::xorshift32(&generatorStatus) & NN);
                    break;
                case 0xD:
                    /* code */
                    // Display
                    break;
                case 0xE:
                    /* code */
                    // Skip if key
                    if (Y == 0x9)
                    {
                        if (input->isKeyPressed(registerBank->read(X)))
                        {
                            pc += 2;
                        }
                    }
                    else if (Y == 0xA)
                    {
                        if (!input->isKeyPressed(registerBank->read(X)))
                        {
                            pc += 2;
                        }
                    }
                    break;
                case 0xF:
                    /* code */
                    if (Y == 0x0)
                    {
                        if (N == 0x7)
                        {
                            registerBank->write(X, delayTimer);
                        }
                        else if (N == 0xA)
                        {
                            uint8_t key;
                            if (input->takeReleasedKey(&key))
                            {
                                registerBank->write(X, key);
                            } else {
                                pc -= 2;
                            }
                        }
                    }
                    else if (Y == 0x1)
                    {
                        if (N == 0x5)
                        {
                            delayTimer = registerBank->read(X);
                            
                        }
                        else if (N == 0x8)
                        {
                            soundTimer = registerBank->read(X);
                        }
                        else if (N == 0xE)
                        {
                            i += registerBank->read(X);
                            if (options->fX1EBehavior && i >= 0x1000)
                            {
                                registerBank->write(15, 1);
                            }
                        }
                    }
                    else if (Y == 0x2)
                    {
                        // Font Character
                        uint8_t character = registerBank->read(X) & 0x0F;
                        i = fontBeginAddress + (character * 5);

                    }
                    else if (Y == 0x3)
                    {
                        // Binary-code
                        uint8_t vX = registerBank->read(X);
                        memory->write(i+2, vX % 10);
                        vX /= 10;
                        memory->write(i+1, vX % 10);
                        vX /= 10;
                        memory->write(i, vX % 10);

                    }
                    else if (Y == 0x5)
                    {
                        // Store memory
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
                    else if (Y == 0x6)
                    {
                        // Load memory
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
                default:
                    break;
                }
            }
        }

        bool isValid(uint16_t opcode)
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

            for (uint16_t k : validsOpcodes)
            {
                if (pattern == k)
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
        CPU(
            MemoryBank *m,
            Input *i,
            RegisterBank *r,
            CoreOptions *o,
            uint16_t pA,
            uint16_t fA,
            size_t fS,
            uint16_t sA,
            uint32_t seed,
            uint16_t font
        )
        {
            memory = m;
            input = i;
            registerBank = r;
            options = o;
            delayTimer = 0;
            soundTimer = 0;
            pc = pA;
            frameBufferBeginAddress = fA;
            frameBufferSize = fS;
            sp = 0;
            stackBeginAddress = sA;
            generatorStatus = seed;
            fontBeginAddress = font;
        }

        void update(uint16_t inputStatus)
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
                decode(opcode);
            }
        }
    };
}
