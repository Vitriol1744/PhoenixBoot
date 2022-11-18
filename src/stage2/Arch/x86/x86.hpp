#pragma once

#include <stdint.h>

extern "C" bool __attribute__((cdecl)) reset_disk(uint8_t drive);
extern "C" bool __attribute__((cdecl)) get_drive_parameters(uint8_t drive, uint8_t* driveType, uint16_t* cylinders, uint16_t* sectors, uint16_t* heads);
extern "C" bool __attribute__((cdecl)) read_sectors(uint8_t drive, uint16_t cylinder, uint16_t sector, uint16_t head, void* buffer, uint32_t sectors); 

struct E820MemoryMapEntry
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
} __attribute__((packed));

extern "C" uint32_t __attribute__((cdecl)) e820_get_next_entry(E820MemoryMapEntry* entry, uint32_t* continuationID);