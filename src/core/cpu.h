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
        uint16_t sp;
        uint32_t delayTimer;
        uint32_t soundTimer;
        uint32_t generatorStatus;

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
            switch (kind)
            {
            case 0x0:
                /* code */
                if (X == 0x0 && Y == 0xE && N == 0x0)
                {
                    // Clear
                }
                else if (X == 0x0 && Y == 0xE && N == 0xE)
                {
                    // Return
                }
                break;
            case 0x1:
                /* code */
                // Jump
                break;
            case 0x2:
                /* code */
                // Subroutine
                break;
            case 0x3:
            case 0x4:
                /* code */
                // Skip conditionally(x)
                break;
            case 0x5:
                if (N == 0x0)
                {
                    // Skip conditionally
                }
                break;
            case 0x6:
                /* code */
                // Set X
                break;
            case 0x7:
                /* code */
                // Add X
                break;
            case 0x8:
                /* code */
                if (N == 0x0)
                {
                }
                else if (N == 0x1)
                {
                }
                else if (N == 0x2)
                {
                }
                else if (N == 0x3)
                {
                }
                else if (N == 0x4)
                {
                }
                else if (N == 0x5)
                {
                }
                else if (N == 0x6)
                {
                }
                else if (N == 0x7)
                {
                }
                else if (N == 0xE)
                {
                }
                break;
            case 0x9:
                /* code */
                // Skip conditionally
                break;
            case 0xA:
                /* code */
                // Set Index
                break;
            case 0xB:
                /* code */
                // Jump offset
                break;
            case 0xC:
                /* code */
                // Random
                break;
            case 0xD:
                /* code */
                // Display
                break;
            case 0xE:
                /* code */
                // Skip if key
                if (Y == 0x9 && N == 0xE)
                {
                }
                else if (Y == 0xA && N == 0x1)
                {
                }
                break;
            case 0xF:
                /* code */
                if (Y == 0x0)
                {
                    if (N == 0x7)
                    {
                    }
                    else if (N == 0xA)
                    {
                    }
                }
                else if (Y == 0x1)
                {
                    if (N == 0x5)
                    {
                    }
                    else if (N == 0x8)
                    {
                    }
                    else if (N == 0xE)
                    {
                    }
                }
                else if (Y == 0x2)
                {
                    //Font Character
                }
                else if (Y == 0x3)
                {
                    //Binary-code
                }
                else if (Y == 0x5)
                {
                    //Store memory
                }
                else if (Y == 0x6)
                {
                    //Load memory
                }
                break;
            default:
                break;
            }
        }

        void execute()
        {
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

    public:
        CPU(MemoryBank *m, Input *i, RegisterBank *r, CoreOptions *o, uint16_t programBeginAddress)
        {
            memory = m;
            input = i;
            registerBank = r;
            options = o;
            delayTimer = 0;
            soundTimer = 0;
            pc = programBeginAddress;
            sp = 0;
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
