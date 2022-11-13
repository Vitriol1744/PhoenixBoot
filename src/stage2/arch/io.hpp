#pragma once

#include <cstdint>

#include "arch/arch.hpp"

#ifdef PH_ARCH_X86
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("out dx, al" : : "a"(value), "d"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret = 0;
    __asm__ volatile("in al, dx" : "=a"(ret) : "d"(port));

    return ret;
}
#endif