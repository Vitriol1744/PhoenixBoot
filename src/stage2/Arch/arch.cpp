#include "arch.hpp"

#if defined(PH_ARCH_X86)
#include "Arch/x86/x86.hpp"
#include "Arch/x86/IDT.hpp"
#include "Arch/x86/PIC.hpp"

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

IDT idt;
void initializeInterrupts()
{
    PIC::MaskAll();
    PIC::Flush();
    idt.Initialize();
    idt.Load();
}
#endif