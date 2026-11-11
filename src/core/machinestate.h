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

        static constexpr uint8_t signature[7] = {0x41, 0x54, 0x45, 0x4D, 0x49, 0x47, 0x4F}; // A T E M I G O
        static constexpr uint8_t saveVersion = 0x1;
        static constexpr size_t stateSize = sizeof(uint16_t) * 6 + sizeof(uint32_t)*3 + sizeof(uint8_t)+ sizeof(int16_t) + sizeof(uint8_t) * MemoryBank::kSize + sizeof(uint8_t) * RegisterBank::kSize + sizeof(signature) + sizeof(saveVersion);
        
        void sign(uint8_t *&data) const
        {
            memcpy(data, signature, sizeof(signature));
            data += sizeof(signature);
            memcpy(data, &saveVersion, sizeof(saveVersion));
            data += sizeof(saveVersion);
        }

        void serialize(uint8_t *&data) const
        {
            sign(data);
            uint8_t *beginData = data;
            AtemigoChip8::utils::writeBytesBigEndian(data, pc, sizeof(uint16_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, i, sizeof(uint16_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, sp, sizeof(int16_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, delayTimer, sizeof(uint32_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, soundTimer, sizeof(uint32_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, generatorStatus, sizeof(uint32_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, isStopped, sizeof(uint8_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, errorData.opcode, sizeof(uint16_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, errorData.pc, sizeof(uint16_t));
            memcpy(data, memory, MemoryBank::kSize);
            data += MemoryBank::kSize;
            memcpy(data, registerBank, RegisterBank::kSize);
            data += RegisterBank::kSize;
            AtemigoChip8::utils::writeBytesBigEndian(data, inputPreviousStatus, sizeof(uint16_t));
            AtemigoChip8::utils::writeBytesBigEndian(data, inputCurrentStatus, sizeof(uint16_t));
            assert(static_cast<size_t>(data-beginData) == (stateSize-sizeof(signature)-sizeof(saveVersion)));
        }

        bool checkSignature(const uint8_t *&data) const
        {
            if (data == nullptr)
                return false;
            if (memcmp(data, signature, sizeof(signature)) != 0)
                return false;
            if (data[sizeof(signature)] != saveVersion)
                return false;

            data += sizeof(signature) + sizeof(saveVersion);
            return true;
        }

        void deserialize(const uint8_t *&data)
        {
            const uint8_t *beginData = data;
            pc = static_cast<uint16_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint16_t)));
            i = static_cast<uint16_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint16_t)));
            sp = static_cast<int16_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(int16_t)));
            delayTimer = AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint32_t));
            soundTimer = AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint32_t));
            generatorStatus = AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint32_t));
            isStopped = static_cast<uint8_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint8_t)));
            errorData.opcode = static_cast<uint16_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint16_t)));
            errorData.pc = static_cast<uint16_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint16_t)));            
            memcpy(memory, data, MemoryBank::kSize);
            data += MemoryBank::kSize;
            memcpy(registerBank, data, RegisterBank::kSize);
            data += RegisterBank::kSize;
            inputPreviousStatus = static_cast<uint16_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint16_t))); 
            inputCurrentStatus = static_cast<uint16_t>(AtemigoChip8::utils::readBytesBigEndian(data, sizeof(uint16_t))); 
            assert(static_cast<size_t>(data-beginData) == (stateSize-sizeof(signature)-sizeof(saveVersion)));
        }
    };
}