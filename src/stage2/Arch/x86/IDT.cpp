#include "IDT.hpp"

__attribute__((interrupt)) void defaultInterruptHandler(void*)
{
    panic("Captured unhandled interrupt!");
}
extern "C" void raiseException(uint32_t exceptionVector, uint32_t errorCode, uint32_t ebp, uint32_t eip)
{
    panic("Captured unhandled exception!\ninterrupt vector: %d, eip: %x", exceptionVector, eip);
}

using ExceptionISR = void(*)();
extern "C" ExceptionISR exceptions[32];

constexpr const uint8_t CODE_SELECTOR32 = 0x08;

void IDT::Initialize()
{
    for (uint32_t vector = 32; vector < MAX_IDT_ENTRIES; vector++)
        SetDescriptor(vector, reinterpret_cast<uintptr_t>(defaultInterruptHandler), 0x8e);

    for (uint32_t vector = 0; vector < 32; vector++)
        SetDescriptor(vector, reinterpret_cast<uintptr_t>(exceptions[vector]), 0x8e);

    //for (uint32_t vector = 0; vector < 32; vector++)
    //{
    //    printf("Vector: %i, Addr: %x", vector, (uint32_t)entries[vector].isrLow);
    //}
    //while (true) halt();
}
#pragma pack(push, 1)
struct IDT_Descriptor
{
    uint16_t size;
    uint32_t pointer;
};
#pragma pack(pop)
void IDT::Load()
{
    IDT_Descriptor descriptor;
    descriptor.size = sizeof(IDT_Entry) * 256;
    descriptor.pointer = (uint64_t)entries;

    __asm__ volatile("lidt %0\n" : : "m"(descriptor));
}

void IDT::SetDescriptor(uint8_t vector, uint64_t isr, uint8_t flags)
{
    IDT_Entry* entry = entries + vector;

    entry->isrLow           = (uint32_t)isr & 0xffff;
    entry->codeSelector     = CODE_SELECTOR32;
    entry->flags            = flags;
    entry->reserved         = 0;

    #if 0
    entry->ist              = 0;
    entry->isrMiddle        = (isr & 0xffff0000) >> 16;
    entry->isrHigh          = (isr & 0xffffffff00000000) >> 32;
    #else
    entry->isrHigh          = (uint32_t)isr >> 16;
    #endif
}