#pragma once

#include <stdint.h>
#include <stddef.h>

struct MemorySegment
{
    MemorySegment* previousSegment = 0;
    MemorySegment* nextSegment = 0;
    uint32_t length = 0;
    bool free = true;
};

class PhysicalMemoryManager
{
    public:
        static void Initialize();

        inline static void  SetBelow1M_AllocatorBase(uintptr_t base) 
        { 
            below1M_AllocatorBase = reinterpret_cast<uint8_t*>(base); 
            below1M_CurrentPointer = below1M_AllocatorBase; 
        } 
        static void* Allocate(size_t bytes);
        static void* AllocateAligned(size_t bytes, uintptr_t alignment);
        static void* Callocate(size_t bytes);
        static void* CallocateAligned(size_t bytes, uintptr_t alignment);
        static void  Free(void* memory);
        // Most of the bios functions require real mode addresses, so we have to be able to allocate memory below 1M mark
        static void* AllocateBelow1M(size_t bytes);
        static void* AllocateBelow1M_Aligned(size_t bytes, uintptr_t alignment);
        static void  FreeBelow1M(size_t bytes);

        static void PrintFreeSpace();

    private:
        PhysicalMemoryManager() = default;

        static MemorySegment* firstSegment;
        static uint8_t* below1M_AllocatorBase;
        static uint8_t* below1M_AllocatorTop;
        static uint8_t* below1M_CurrentPointer;
};