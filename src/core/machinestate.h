#pragma once
#include <cstdint>
#include "coreerror.h"
#include "memorybank.h"
#include "registerbank.h"
namespace AtemigoChip8::core {
    struct MachineState {
        uint16_t pc;
        uint16_t i;
        int16_t sp;
        uint32_t delayTimer;
        uint32_t soundTimer;
        uint32_t generatorStatus;
        uint8_t isStopped = 0;
        CoreError errorData;
        uint8_t memory[MemoryBank::kSize];
        uint8_t registerBank[RegisterBank::kSize];
        uint16_t inputPreviousStatus;
        uint16_t inputCurrentStatus;

        MachineState()
        {
            
        }
        
        void serialize(void *data)
        {
            

        }
    };
}