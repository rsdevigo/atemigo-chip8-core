#pragma once
#include <cstdint>
#include <cstddef>

namespace AtemigoChip8::core {
    struct Chip8Config {
        static constexpr uint16_t fontBeginAddress = 0x50;
        static constexpr uint16_t stackBeginAddress = 0xEBE;
        static constexpr uint16_t frameBufferBeginAddress = 0xEC0;
        static constexpr uint16_t programBeginAddress = 0x200;
        static constexpr size_t maxProgramSize = 3232;
        static constexpr size_t frameBufferSize = 256;
        static constexpr uint32_t frequency = 60;
        static constexpr uint32_t width = 64;
        static constexpr uint32_t height = 32;
    };
}