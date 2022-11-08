#include "PhysicalMemoryManager.hpp"

uint8_t* PhysicalMemoryManager::allocatorBase = reinterpret_cast<uint8_t*>(0x200000);
uint8_t* PhysicalMemoryManager::currentPointer = reinterpret_cast<uint8_t*>(0x200000);
uint8_t* PhysicalMemoryManager::below1M_AllocatorBase = reinterpret_cast<uint8_t*>(0x10000);
uint8_t* PhysicalMemoryManager::below1M_AllocatorTop = reinterpret_cast<uint8_t*>(0x0007ffff);
uint8_t* PhysicalMemoryManager::below1M_CurrentPointer = reinterpret_cast<uint8_t*>(0x10000);

void* PhysicalMemoryManager::Allocate(size_t bytes)
{
    void* ret = currentPointer;
    currentPointer += bytes;

    return ret;
}
void* PhysicalMemoryManager::AllocateAligned(size_t bytes, uintptr_t alignment)
{
    currentPointer += alignment - (reinterpret_cast<uintptr_t>(currentPointer) % alignment);

    return Allocate(bytes);
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
