#pragma once
#include <cstdint>

namespace AtemigoChip8::core {
    struct Chip8Config {
        const uint16_t fontBeginAddress = 0x50;
        const uint16_t stackBeginAddress = 0xEBE;
        const uint16_t frameBufferBeginAddress = 0xEC0;
        const uint16_t programBeginAddress = 0x200;
        const size_t maxProgramSize = 3232;
        const size_t frameBufferSize = 256;
        const uint32_t frequency = 60;
        const uint32_t width = 64;
        const uint32_t height = 32;
    };
}