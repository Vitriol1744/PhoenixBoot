#pragma once

#include <stdint.h>
#include <stddef.h>

class PhysicalMemoryManager
{
    public:
        static void Initialize(uintptr_t base, uintptr_t length);

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
        static void* AllocateBelow1M(size_t bytes);
        static void* AllocateBelow1M_Aligned(size_t bytes, uintptr_t alignment);
        static void  FreeBelow1M(size_t bytes);

        static void PrintFreeSpace();

    private:
        PhysicalMemoryManager() = default;

        static uint8_t* allocatorBase;
        static uint8_t* currentPointer;
        static uint8_t* below1M_AllocatorBase;
        static uint8_t* below1M_AllocatorTop;
        static uint8_t* below1M_CurrentPointer;
};