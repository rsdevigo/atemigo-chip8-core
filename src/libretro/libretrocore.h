#pragma once
#include "../core/chip8machine.h"
#include "../core/machineinfo.h"
#include "../core/machinestate.h"
#include "../core/frameinfo.h"
#include "../core/coreoptions.h"
#include "../core/coreerror.h"
#include "../utils/helper.h"
#include "libretro.h"

namespace AtemigoChip8::libretro
{
    using AtemigoChip8::core::Chip8Machine;
    using AtemigoChip8::core::CoreError;
    using AtemigoChip8::core::CoreOptions;
    using AtemigoChip8::core::FrameInfo;
    using AtemigoChip8::core::MachineInfo;
    using AtemigoChip8::core::MachineState;

    class LibretroCore
    {
        Chip8Machine *machine;
        uint8_t *rom = nullptr;
        size_t romSize = 0;
        CoreOptions options;
        uint8_t *frameBufferInPixels = nullptr;
        bool haltReported = false;

    public:
        static constexpr int soundFrequency = 440;
        static constexpr int sampleRate = 48000;
        static constexpr uint32_t pixelColor = 0xFFFFFF;
        LibretroCore()
        {
            machine = new Chip8Machine(AtemigoChip8::utils::makeSeed());
        }

        ~LibretroCore()
        {
            delete[] rom;
            delete machine;
        }

        // Dona da máquina e da ROM (ponteiros crus + delete no destrutor):
        // uma cópia faria duas instâncias apagarem os mesmos blocos.
        LibretroCore(const LibretroCore &) = delete;
        LibretroCore &operator=(const LibretroCore &) = delete;

        bool loadGame(const struct retro_game_info *game)
        {
            if (game == nullptr || game->size == 0 || game->data == nullptr)
                return false;

            if (!machine->loadGame(game->data, game->size, AtemigoChip8::utils::makeSeed()))
            {
                return false;
            }
            delete[] rom;
            romSize = 0;
            rom = new uint8_t[game->size];
            romSize = game->size;
            memcpy(rom, static_cast<const uint8_t *>(game->data), game->size);
            return true;
        }

        void *getMachineMemory(unsigned id)
        {
            switch (id)
            {
            case RETRO_MEMORY_SYSTEM_RAM:
                return machine->getMemoryRAM();
            case RETRO_MEMORY_VIDEO_RAM:
                return machine->getMemoryVRAM();
            default:
                return nullptr;
            }
        }

        size_t getMachineMemorySize(unsigned id)
        {
            switch (id)
            {
            case RETRO_MEMORY_SYSTEM_RAM:
                return machine->getMemoryRAMSize();
            case RETRO_MEMORY_VIDEO_RAM:
                return machine->getMemoryVRAMSize();
            default:
                return 0;
            }
        }

        size_t getStateSize()
        {
            return MachineState::stateSize;
        }

        bool saveState(void *data, size_t size)
        {
            return machine->saveState(data, size);
        }

        bool loadState(const void *data, size_t size)
        {
            return machine->loadState(data, size);
        }

        void reset()
        {
            uint32_t seed = AtemigoChip8::utils::makeSeed();
            if (!machine->reset(seed))
                return;

            if (romSize != 0)
            {
                if (!machine->loadGame(rom, romSize, seed))
                {
                    machine->updateCoreOptions(options);
                    haltReported = false;
                }
            }

            machine->updateCoreOptions(options);
            haltReported = false;
        }
    };
}