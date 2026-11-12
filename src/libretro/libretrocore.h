#pragma once
#include "../core/chip8machine.h"
#include "../core/machineinfo.h"
#include "../core/machinestate.h"
#include "../core/frameinfo.h"
#include "../core/coreoptions.h"
#include "../core/coreerror.h"
#include "../utils/helper.h"
#include "libretro.h"
#include <vector>
#include <cstring>

namespace AtemigoChip8::libretro
{
    using AtemigoChip8::core::Chip8Machine;
    using AtemigoChip8::core::CoreError;
    using AtemigoChip8::core::CoreOptions;
    using AtemigoChip8::core::FrameInfo;
    using AtemigoChip8::core::MachineInfo;
    using AtemigoChip8::core::MachineState;

    struct LibretroCoreOptions
    {
        int soundFrequency = 440;
        int sampleRate = 48000;
        uint32_t pixelColor = 0xFFFFFF;
    };

    class LibretroCore
    {
        Chip8Machine *machine;
        uint8_t *rom = nullptr;
        size_t romSize = 0;
        CoreOptions options;
        std::vector<uint32_t> pixels;
        std::vector<int16_t> audio;
        size_t audioFrames = 0;
        float soundPhase = 0.f;
        bool haltReported = false;
        bool haltPending = false;
        LibretroCoreOptions libretroCoreOptions;
        retro_video_refresh_t videoCB = nullptr;
        retro_audio_sample_batch_t audioCB = nullptr;
        retro_input_poll_t inputPollCB = nullptr;
        retro_input_state_t inputStateCB = nullptr;

        // Monta o bitmask das 16 teclas hexa (bit k = tecla k) a partir do RetroPad.
        uint16_t readKeys() const
        {
            static const unsigned map[16] = {
                RETRO_DEVICE_ID_JOYPAD_B,      // 0
                RETRO_DEVICE_ID_JOYPAD_UP,     // 1
                RETRO_DEVICE_ID_JOYPAD_DOWN,   // 2
                RETRO_DEVICE_ID_JOYPAD_LEFT,   // 3
                RETRO_DEVICE_ID_JOYPAD_RIGHT,  // 4
                RETRO_DEVICE_ID_JOYPAD_A,      // 5
                RETRO_DEVICE_ID_JOYPAD_X,      // 6
                RETRO_DEVICE_ID_JOYPAD_Y,      // 7
                RETRO_DEVICE_ID_JOYPAD_L,      // 8
                RETRO_DEVICE_ID_JOYPAD_R,      // 9
                RETRO_DEVICE_ID_JOYPAD_L2,     // A
                RETRO_DEVICE_ID_JOYPAD_R2,     // B
                RETRO_DEVICE_ID_JOYPAD_SELECT, // C
                RETRO_DEVICE_ID_JOYPAD_START,  // D
                RETRO_DEVICE_ID_JOYPAD_L3,     // E
                RETRO_DEVICE_ID_JOYPAD_R3      // F
            };
            uint16_t keys = 0;
            for (unsigned k = 0; k < 16; k++)
                if (inputStateCB(0, RETRO_DEVICE_JOYPAD, 0, map[k]))
                    keys |= static_cast<uint16_t>(1u << k);
            return keys;
        }

        void makeAudio(bool emitting)
        {
            const float step = static_cast<float>(libretroCoreOptions.soundFrequency) / libretroCoreOptions.sampleRate;
            for (size_t i = 0; i < audioFrames; i++)
            {
                int16_t s = 0;
                if (emitting)
                {
                    s = (soundPhase < 0.5f) ? 8000 : -8000;
                    soundPhase += step;
                    if (soundPhase >= 1.f)
                        soundPhase -= 1.f;
                }
                audio[i * 2] = s;
                audio[i * 2 + 1] = s;
            }
            audioCB(audio.data(), audioFrames);
        }

        void drawPixelBuffer(const FrameInfo &frame)
        {
            const uint32_t on = libretroCoreOptions.pixelColor; // 0xFFFFFF
            const uint32_t off = 0x000000;

            MachineInfo mInfo = machine->getMachineInfo();
            for (unsigned y = 0; y < mInfo.height; y++)
                for (unsigned x = 0; x < mInfo.width; x++)
                {
                    uint8_t byte = frame.frameBuffer[y * (mInfo.width / 8) + x / 8];
                    bool lit = byte & (0x80 >> (x % 8));
                    pixels[y * mInfo.width + x] = lit ? on : off;
                }
            videoCB(pixels.data(), mInfo.width, mInfo.height, mInfo.width * sizeof(uint32_t));
        }

    public:
        LibretroCore()
        {
            machine = new Chip8Machine(AtemigoChip8::utils::makeSeed());
            MachineInfo info = machine->getMachineInfo();
            pixels = std::vector<uint32_t>(info.width * info.height);
            audioFrames = static_cast<size_t>(libretroCoreOptions.sampleRate / info.fps);
            audio.assign(audioFrames * 2, 0);
        }

        void setVideoRefresh(retro_video_refresh_t cb) { videoCB = cb; }
        void setAudioSampleBatch(retro_audio_sample_batch_t cb) { audioCB = cb; }
        void setInputPoll(retro_input_poll_t cb) { inputPollCB = cb; }
        void setInputState(retro_input_state_t cb) { inputStateCB = cb; }

        // true uma única vez depois que a máquina parou (para o frontend avisar o usuário).
        bool takeHaltNotice()
        {
            bool p = haltPending;
            haltPending = false;
            return p;
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
            haltReported = false;
            haltPending = false;
            machine->updateCoreOptions(options);
            return true;
        }

        // Opções vindas do menu do frontend.
        void setOptions(uint32_t pixelColor, int soundFrequency, const CoreOptions &newOptions)
        {
            libretroCoreOptions.pixelColor = pixelColor;
            libretroCoreOptions.soundFrequency = soundFrequency;
            options = newOptions;
            machine->updateCoreOptions(options);
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

        void getMachineInfo(retro_system_av_info *info)
        {
            MachineInfo mInfo = machine->getMachineInfo();
            info->timing.fps = mInfo.fps;
            info->timing.sample_rate = libretroCoreOptions.sampleRate;
            info->geometry.base_height = mInfo.height;
            info->geometry.max_height = mInfo.height;
            info->geometry.base_width = mInfo.width;
            info->geometry.max_width = mInfo.width;
            info->geometry.aspect_ratio = 2.0f;
        }

        void unloadGame()
        {
            haltReported = false;
            haltPending = false;
            delete[] rom;
            rom = nullptr;
            romSize = 0;
        }

        void runFrame()
        {
            if (!videoCB || !audioCB || !inputPollCB || !inputStateCB)
                return;
            if (romSize == 0 || haltReported)
                return;

            inputPollCB();
            FrameInfo frame = machine->runFrame(readKeys());
            if (frame.isStopped)
            {
                haltReported = true;
                haltPending = true;
                return;
            }

            drawPixelBuffer(frame);
            makeAudio(frame.isEmitting);
        }
    };
}