#pragma once

#include <stdint.h>

class PIC
{
    public:
        static void Flush();
        static void SendEOI(uint8_t irq);
        static void MaskAll();

    private:
        PIC() = delete;
};