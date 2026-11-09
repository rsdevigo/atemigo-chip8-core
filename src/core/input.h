#pragma once
#include <cstdint>
#include <bit>

namespace AtemigoChip8::core
{
    class Input
    {
        uint16_t previousStatus = 0x0000;
        uint16_t currentStatus = 0x0000;

    public:
        bool takeReleasedKey(uint8_t *key)
        {
            uint16_t result = previousStatus & ~currentStatus;
            bool status = result != 0;
            uint8_t keyReleased = static_cast<uint8_t>(std::countr_zero(result));
            if (status)
                *key = keyReleased;

            previousStatus = previousStatus & ~(result & -result);
            return status;
        }

        bool isKeyPressed(uint8_t key) const
        {
            if (key >= 0x10)
                return false; 
            return (currentStatus >> (key)) & 1;
        }

        void updateInputStatus(uint16_t newStatus)
        {
            previousStatus = currentStatus;
            currentStatus = newStatus;
        }

        bool loadState(const uint16_t pState, const uint16_t cState)
        {
            currentStatus = cState;
            previousStatus = pState;
            return true;
        }
        bool saveState(uint16_t &pState, uint16_t &cState) const
        {
            pState = previousStatus;
            cState = currentStatus;
            return true;
        }
    };
}
