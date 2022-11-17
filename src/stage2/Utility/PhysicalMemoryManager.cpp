#include "PhysicalMemoryManager.hpp"

#include <stddef.h>

#include "Utility/libc.hpp"

uint8_t* PhysicalMemoryManager::below1M_AllocatorBase = reinterpret_cast<uint8_t*>(0x10000);
uint8_t* PhysicalMemoryManager::below1M_AllocatorTop = reinterpret_cast<uint8_t*>(0x0007ffff);
uint8_t* PhysicalMemoryManager::below1M_CurrentPointer = reinterpret_cast<uint8_t*>(0x10000);
 
void*   operator new(size_t size)   { return PhysicalMemoryManager::Allocate(size); }
void*   operator new[](size_t size) { return PhysicalMemoryManager::Allocate(size); }
void    operator delete(void* p)    noexcept { PhysicalMemoryManager::Free(p); }
void    operator delete[](void* p)  noexcept { PhysicalMemoryManager::Free(p); }

struct MemorySegment
{
    MemorySegment* previousSegment = 0;
    MemorySegment* nextSegment = 0;
    uint32_t length = 0;
    bool free = true;
};
static MemorySegment* firstSegment = nullptr;

void PhysicalMemoryManager::Initialize(uintptr_t base, uintptr_t length)
{
    firstSegment = (MemorySegment*)base;
    firstSegment->previousSegment = 0;
    firstSegment->nextSegment = 0;
    firstSegment->length = length - sizeof(MemorySegment);
    firstSegment->free = true;
}        

void* PhysicalMemoryManager::Allocate(size_t bytes)
{
    return AllocateAligned(bytes, 0);
}
void* PhysicalMemoryManager::AllocateAligned(size_t bytes, uintptr_t alignment)
{
    if (!firstSegment) return AllocateBelow1M(bytes);
    MemorySegment* currentSegment = firstSegment;

    // Search for the suitable block that can be aligned to specifed boundary
    uint32_t toAlign = 0;
    while (currentSegment)
    {
        toAlign = alignment ? alignment - (uintptr_t)(currentSegment + 1) % alignment : 0;
        if (currentSegment->free && currentSegment->length >= bytes + toAlign) break;
        currentSegment = currentSegment->nextSegment;
    }

    // Calculate new segment size
    uint32_t nextSegmentLength = currentSegment->length - bytes - sizeof(MemorySegment);
    // Mark block as used
    currentSegment->free = false;
    currentSegment->length = bytes;
    
    // Create new segment
    MemorySegment* nextSegment = (MemorySegment*)((uint8_t*)currentSegment + sizeof(MemorySegment) + currentSegment->length);
    nextSegment->nextSegment = currentSegment->nextSegment;
    if (currentSegment->nextSegment) currentSegment->nextSegment->previousSegment = nextSegment;
    currentSegment->nextSegment = nextSegment;
    
    nextSegment->previousSegment = currentSegment;
    nextSegment->length = nextSegmentLength;
    nextSegment->free = true;

    // This method of aligning might be a little bit wastefull
    return (uint8_t*)(currentSegment + 1) + toAlign;
}
void* PhysicalMemoryManager::Callocate(size_t bytes)
{
    void* ret = Allocate(bytes);
    memset(ret, 0, bytes);
    
    return ret;
}
void* PhysicalMemoryManager::CallocateAligned(size_t bytes, uintptr_t alignment)
{
    void* ret = AllocateAligned(bytes, alignment);
    memset(ret, 0, bytes);

    return ret;
}
void  PhysicalMemoryManager::Free(void* memory)
{
    MemorySegment* currentSegment = (MemorySegment*)((uint8_t*)memory - sizeof(MemorySegment));
    currentSegment->free = true;

    // Defragmentate
    while (currentSegment)
    {
        if (currentSegment->previousSegment != nullptr && currentSegment->previousSegment->free)
        {
            currentSegment = currentSegment->previousSegment;
            continue;
        }
        break;
    }
    if (!currentSegment->nextSegment->free) return;
    MemorySegment* toConcatSegment = currentSegment->nextSegment;
    while (toConcatSegment && toConcatSegment->free)
    {
        currentSegment->nextSegment = toConcatSegment->nextSegment;
        currentSegment->length += toConcatSegment->length + sizeof(MemorySegment);
        toConcatSegment = currentSegment->nextSegment;
    }
}
void* PhysicalMemoryManager::AllocateBelow1M(size_t bytes)
{
    if (below1M_CurrentPointer >= below1M_AllocatorTop) return nullptr;
    void* ret = below1M_CurrentPointer;
    below1M_CurrentPointer += bytes;

    return ret;
}
void* PhysicalMemoryManager::AllocateBelow1M_Aligned(size_t bytes, uintptr_t alignment)
{
    below1M_CurrentPointer += alignment - (reinterpret_cast<uintptr_t>(below1M_CurrentPointer) % alignment);

    return AllocateBelow1M(bytes);
}
void  PhysicalMemoryManager::FreeBelow1M(size_t bytes)
{
    below1M_CurrentPointer -= bytes;
}
// Just for debugging
void PhysicalMemoryManager::PrintFreeSpace()
{
    uint32_t freeSpace = 0;
    uint32_t segmentCount = 0;
    MemorySegment* currentSegment = firstSegment;
    while (currentSegment)
    {
        segmentCount++;
        if (currentSegment->free) freeSpace += currentSegment->length;
        currentSegment = currentSegment->nextSegment;
    }
    printf("Free Space: %d, SegmentCount: %d\n", freeSpace, segmentCount);
}