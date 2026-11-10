
#pragma once
#include <cstdint>
#include <cassert>
#include <random>
#include <chrono>

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

    // splitmix32: espalha bem os bits de uma entrada fraca (ex.: relógio)
    inline uint32_t splitmix32(uint32_t x)
    {
        x += 0x9E3779B9u;
        x = (x ^ (x >> 16)) * 0x85EBCA6Bu;
        x = (x ^ (x >> 13)) * 0xC2B2AE35u;
        return x ^ (x >> 16);
    }

    inline uint32_t make_seed()
    {
        uint32_t s = std::random_device{}();
        s ^= (uint32_t)std::chrono::high_resolution_clock::now()
                 .time_since_epoch()
                 .count();
        s = splitmix32(s);
        return s ? s : 0xDEADBEEFu; // xorshift32 não pode ter estado 0
    }
}