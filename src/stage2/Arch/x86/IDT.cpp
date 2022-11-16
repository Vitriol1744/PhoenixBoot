#include "IDT.hpp"

__attribute__((interrupt)) void defaultInterruptHandler(void*)
{
    panic("Captured unhandled interrupt!");
}
extern "C" void raiseException(uint32_t interruptVector, uint32_t ebp, uint32_t eip)
{
    //printf("interruptVector: %d, eip: %x", interruptVector, eip);
    char str[] = "Captured unhandled exception!0";
    str[sizeof(str) - 1] = '0' - interruptVector;
    panic(str);
}

using ExceptionISR = void(*)();
extern "C" ExceptionISR exceptions[32];

void IDT::Initialize()
{
    for (uint32_t vector = 0; vector < 32; vector++)
        SetDescriptor(vector, reinterpret_cast<uintptr_t>(exceptions[vector]), 0x8e);

    for (uint32_t vector = 32; vector < MAX_IDT_ENTRIES; vector++)
        SetDescriptor(vector, reinterpret_cast<uintptr_t>(defaultInterruptHandler), 0x8e);
}
#pragma pack(push, 1)
struct IDT_Descriptor
{
    uint16_t size;
    uint64_t pointer;
};
#pragma pack(pop)
void IDT::Load()
{
    IDT_Descriptor descriptor;
    descriptor.size = sizeof(IDT_Entry) * 256;
    descriptor.pointer = (uint64_t)entries;

    __asm__ volatile("lidt %0\n" : : "m"(descriptor));
}

void IDT::SetDescriptor(uint8_t vector, uintptr_t isr, uint8_t flags)
{
    IDT_Entry* entry = entries + vector;

    entry->isrLow     = isr & 0xffff;
    entry->codeSelector   = 0x08;
    entry->ist         = 0;
    entry->flags  = flags;
    entry->isrMiddle  = (isr & 0xffff0000) >> 16;
    entry->isrHigh    = (isr & 0xffffffff00000000) >> 32;
    entry->reserved    = 0;
}