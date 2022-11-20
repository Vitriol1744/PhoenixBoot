#pragma once

#include <cstdint>

struct PageMap
{
    uint8_t levels;
    uint64_t topLevel = 0;
};

using VirtAddr = uint64_t;
using PhysAddr = uint64_t;

class VirtualMemoryManager
{
    public:
        void mapPage(VirtAddr virtualAddress, PhysAddr physicalAddress);
};