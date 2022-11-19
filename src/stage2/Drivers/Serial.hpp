#pragma once

#include <stdint.h>

class Serial
{
    public:
        static void Initialize();

        static void WriteByte(uint8_t byte);
        static uint8_t ReadByte(uint8_t byte);

    private:
        static constexpr const uint32_t SERIAL_PORT1 = 0x3f8;

        Serial() = delete;
};