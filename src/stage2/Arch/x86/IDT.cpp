#include "IDT.hpp"

__attribute__((interrupt)) void defaultInterruptHandler(void*)
{
    panic("Captured unhandled interrupt!");
}

const char* exceptionNames[]
{
    "Divide-by-zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device not available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Triple Fault",
    "FPU Error Interrupt",
};

extern "C" void raiseException(uint32_t exceptionVector, uint32_t errorCode, uint32_t ebp, uint32_t eip)
{
    panic("Captured unhandled exception: '%s'\nError Code: %d\neip: %x", exceptionNames[exceptionVector], errorCode, eip);
}

using ExceptionISR = void(*)();
extern "C" ExceptionISR exceptions[32];

constexpr const uint8_t CODE_SELECTOR32 = 0x08;

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