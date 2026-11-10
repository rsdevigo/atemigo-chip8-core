
#pragma once
#include <cstdint>
#include <cassert>

namespace AtemigoChip8::utils
{
    inline uint8_t getNibble(uint16_t instruction, uint8_t position)
    {
        assert(position <= 3);
        return (instruction >> ((3 - position) * 4)) & 0xF;
    }

    inline uint32_t xorshift32(uint32_t *state)
    {
        uint32_t x = *state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        *state = x;
        return x;
    }
}