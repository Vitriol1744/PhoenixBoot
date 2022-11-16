#pragma once

#include "common.hpp"

#if PH_ARCH == PH_ARCH_X86_64
struct IDT_Entry
{
    uint16_t isrLow;
    uint16_t codeSelector;
    uint8_t ist;
    uint8_t flags;
    uint16_t isrMiddle;
    uint32_t isrHigh;
    uint32_t reserved;
};
#else
struct IDT_Entry
{
    uint16_t isrLow;
    uint16_t codeSelector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t isrHigh;
};
#endif

constexpr const uint32_t MAX_IDT_ENTRIES = 256;

class IDT
{
    public:
        void Initialize();
        void Load();

        void SetDescriptor(uint8_t vector, uintptr_t isr, uint8_t flags);

    private:
        alignas(0x10) IDT_Entry entries[MAX_IDT_ENTRIES];
};
