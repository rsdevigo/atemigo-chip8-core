#pragma once
#include <cstdint>
#include <array>
#include <cstring>


namespace AtemigoChip8::core
{
    class MemoryBank
    {
    public:
        static constexpr uint16_t kSize = 1 << 12;
        static constexpr uint16_t kMask = kSize - 1;
        void write(uint16_t a, uint8_t d)
        {
            bank[a & kMask] = d;
        }

        uint8_t read(uint16_t a) const
        {
            return bank[a & kMask];
        }

        uint8_t *data() { return bank.data(); }
        
        void reset()
        {
            bank.fill(0x00);
        }

        bool loadState(const uint8_t *state, size_t size)
        {
            if (size != kSize || state == nullptr)
                return false;
            memcpy(bank.data(), state, kSize);
            return true;
        }

        bool saveState(uint8_t *state, size_t size) const
        {
            if (size != kSize || state == nullptr)
                return false;
            memcpy(state, bank.data(), kSize);
            return true;
        }

        size_t getSize() const { return kSize; }

    private:
        std::array<uint8_t, kSize> bank{};
    };
}