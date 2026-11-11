#pragma once
#include <cstdint>
#include "coreerror.h"
#include "../utils/helper.h"
#include "memorybank.h"
#include "registerbank.h"
#include <cstring>
#include <cassert>

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

        static constexpr size_t stateSize = sizeof(uint16_t) * 6 + sizeof(uint32_t)*3 + sizeof(uint8_t)+ sizeof(int16_t) + sizeof(uint8_t) * MemoryBank::kSize + sizeof(uint8_t) * RegisterBank::kSize;
        
        void serialize(uint8_t *&data) const
        {
            uint8_t *beginData = data;
            AtemigoChip8::utils::writeByte(data, pc, sizeof(uint16_t));
            AtemigoChip8::utils::writeByte(data, i, sizeof(uint16_t));
            AtemigoChip8::utils::writeByte(data, sp, sizeof(int16_t));
            AtemigoChip8::utils::writeByte(data, delayTimer, sizeof(uint32_t));
            AtemigoChip8::utils::writeByte(data, soundTimer, sizeof(uint32_t));
            AtemigoChip8::utils::writeByte(data, generatorStatus, sizeof(uint32_t));
            AtemigoChip8::utils::writeByte(data, isStopped, sizeof(uint8_t));
            AtemigoChip8::utils::writeByte(data, errorData.opcode, sizeof(uint16_t));
            AtemigoChip8::utils::writeByte(data, errorData.pc, sizeof(uint16_t));
            
            memcpy(data, memory, MemoryBank::kSize);
            data += MemoryBank::kSize;
            memcpy(data, registerBank, RegisterBank::kSize);
            data += RegisterBank::kSize;
            AtemigoChip8::utils::writeByte(data, inputPreviousStatus, sizeof(uint16_t));
            AtemigoChip8::utils::writeByte(data, inputCurrentStatus, sizeof(uint16_t));
            assert(static_cast<size_t>(data-beginData) == stateSize);
        }

        void deserialize(const uint8_t *&data)
        {
            const uint8_t *beginData = data;
            pc = static_cast<uint16_t>(AtemigoChip8::utils::readByte(data, sizeof(uint16_t)));
            i = static_cast<uint16_t>(AtemigoChip8::utils::readByte(data, sizeof(uint16_t)));
            sp = static_cast<int16_t>(AtemigoChip8::utils::readByte(data, sizeof(int16_t)));
            delayTimer = AtemigoChip8::utils::readByte(data, sizeof(uint32_t));
            soundTimer = AtemigoChip8::utils::readByte(data, sizeof(uint32_t));
            generatorStatus = AtemigoChip8::utils::readByte(data, sizeof(uint32_t));
            isStopped = static_cast<uint8_t>(AtemigoChip8::utils::readByte(data, sizeof(uint8_t)));
            errorData.opcode = static_cast<uint16_t>(AtemigoChip8::utils::readByte(data, sizeof(uint16_t)));
            errorData.pc = static_cast<uint16_t>(AtemigoChip8::utils::readByte(data, sizeof(uint16_t)));            
            memcpy(memory, data, MemoryBank::kSize);
            data += MemoryBank::kSize;
            memcpy(registerBank, data, RegisterBank::kSize);
            data += RegisterBank::kSize;
            inputPreviousStatus = static_cast<uint16_t>(AtemigoChip8::utils::readByte(data, sizeof(uint16_t))); 
            inputCurrentStatus = static_cast<uint16_t>(AtemigoChip8::utils::readByte(data, sizeof(uint16_t))); 
            assert(static_cast<size_t>(data-beginData) == stateSize);
        }
    };
}