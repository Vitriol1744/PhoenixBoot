#include "arch.hpp"

#if defined(PH_ARCH_X86)
#include "Arch/x86/x86.hpp"
#include "Arch/x86/IDT.hpp"
#include "Arch/x86/PIC.hpp"
#include "Arch/x86/vbe.hpp"

#include "common.hpp"

uint32_t getMemoryMap(MemoryMapEntry* entries, uint64_t maxEntryCount)
{
    uint32_t continuationID = 0x00000000;
    E820MemoryMapEntry entry;
    uint32_t entryCount = 0;

    for (uint32_t i = 0; i < maxEntryCount; i++)
    {
        if (e820_get_next_entry(&entry, &continuationID) < 0) break;
    
        entries[i].base = entry.base;
        entries[i].length = entry.length;
        entries[i].type = static_cast<MemoryMapEntryType>(entry.type);

        ++entryCount;
        if (continuationID == 0) break;
    }

    return entryCount;
}

static IDT idt;
void initializeInterrupts()
{
    PIC::MaskAll();
    PIC::Flush();
    idt.Initialize();
    idt.Load();
}
void getFramebufferInfo(FramebufferInfo& framebufferInfo)
{
    VbeControllerInfo controllerInfo;
    LOG_TRACE("%x\n", (uint32_t)(&controllerInfo));
    LOG_TRACE("Initializing framebuffer...");
    if (!vbe_get_controller_info(&controllerInfo)) LOG_ERROR("[FAILED]\n");
    LOG_INFO("[OK]\n");

    VbeModeInfo modeInfo;
    vbeInitializeGraphicsMode(modeInfo);
    framebufferInfo.framebufferBase = modeInfo.framebufferBase;
    framebufferInfo.width = modeInfo.width;
    framebufferInfo.height = modeInfo.height;
    framebufferInfo.bpp = modeInfo.bpp;
    framebufferInfo.pitch = modeInfo.pitch;
}
#endif