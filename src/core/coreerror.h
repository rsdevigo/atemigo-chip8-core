#pragma once
#include <cstdint>

namespace AtemigoChip8::core {
    struct CoreError {
        uint16_t opcode;
        uint16_t pc;

        CoreError()
        {
            opcode = 0x0000;
            pc = 0x0000;
        }       
    };
}