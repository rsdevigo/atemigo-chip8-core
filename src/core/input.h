#pragma once
#include <cstdint>
#include <array>
#include <cstring>


namespace AtemigoChip8::core
{
    class Input {
        uint16_t lastStatus = 0x0000;
        uint16_t actualStatus = 0x0000;
    };
}
