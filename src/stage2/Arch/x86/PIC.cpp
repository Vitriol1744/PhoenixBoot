#include "PIC.hpp"

#include "Arch/io.hpp"

void PIC::Flush()
{
    for (uint8_t irq = 0; irq < 16; irq++) SendEOI(irq);
}
void PIC::SendEOI(uint8_t irq)
{
    if (irq >= 8) outb(0xa0, 0x20);
    outb(0x20, 0x20);
}

void PIC::MaskAll()
{
    outb(0xa1, 0xff);
    outb(0x21, 0xff);
}