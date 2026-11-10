#pragma once
#include <cstdint>
#include <array>
#include <cstring>
#include <cassert>
namespace AtemigoChip8::core
{
    class RegisterBank
    {
    public:
        static constexpr uint16_t kSize = 16;
        void write(uint8_t address, uint8_t value)
        {
            assert(address < kSize);
            if (address >= kSize)
                return;
            bank[address] = value;
        }

        uint8_t read(uint8_t address) const
        {
            assert(address < kSize);
            if (address >= kSize)
                return 0;
            return bank[address];
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

        uint8_t flag() const
        {
            return bank[15];
        }

    private:
        std::array<uint8_t, kSize> bank{};
    };
}