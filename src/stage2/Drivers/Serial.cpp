#include "Serial.hpp"

#include "Arch/io.hpp"

#include "Drivers/Terminal.hpp"

#include "Utility/libc.hpp"
#include "Utility/Logger.hpp"

void Serial::Initialize()
{
    static bool initialized = false;
    if (initialized) return;

    LOG_TRACE("Initializing Serial Port...\t");
    outb(SERIAL_PORT1 + 1, 0x00);    // Disable all interrupts
    outb(SERIAL_PORT1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_PORT1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(SERIAL_PORT1 + 1, 0x00);    //                  (hi byte)
    outb(SERIAL_PORT1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_PORT1 + 2, 0xc7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_PORT1 + 4, 0x0b);    // IRQs enabled, RTS/DSR set
    outb(SERIAL_PORT1 + 4, 0x1e);    // Set in loopback mode, test the serial chip
    outb(SERIAL_PORT1 + 0, 0xae);    // Test serial chip (send byte 0xae and check if serial returns same byte)
    
    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(SERIAL_PORT1 + 0) != 0xae)
    {
        LOG_ERROR("[FAILED]\n");
        return;
    }
    
    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(SERIAL_PORT1 + 4, 0x0f);
    Terminal::Get()->SetX(76);
    LOG_INFO("[OK]\n");
    initialized = true;
}

void Serial::WriteByte(uint8_t byte)
{
    while ((inb(SERIAL_PORT1 + 5) & 0x20) == 0)
        ;
    outb(SERIAL_PORT1, byte);
}
uint8_t Serial::ReadByte(uint8_t byte)
{
    while ((inb(SERIAL_PORT1 + 5) & 0x01) == 0)
        ;
    return inb(SERIAL_PORT1);
}