#pragma once

#define PH_ARCH_X86_64 0x00001
#define PH_ARCH_X86_32 0x00002

#if PH_ARCH == PH_ARCH_X86_64 || PH_ARCH == PH_ARCH_X86_32
    #define PH_ARCH_X86
#endif

#include <stdint.h>

enum class MemoryMapEntryType : uint32_t
{
    eUsable = 1,
    eReserved = 2,
    eACPI_Reclaimable = 3,
    eACPI_NVS = 4,
    eBootloaderReclaimable,
    eKernelAndModules,
    eFramebuffer,
};

struct MemoryMapEntry
{
    uint64_t base;
    uint64_t length;
    MemoryMapEntryType type;
};

struct FramebufferInfo
{
    uint64_t framebufferBase;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;
};

uint32_t getMemoryMap(MemoryMapEntry* entries, uint64_t maxEntryCount);
void initializeInterrupts();
void getFramebufferInfo(FramebufferInfo& framebufferInfo);