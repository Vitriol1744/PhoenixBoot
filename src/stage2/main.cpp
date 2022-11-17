#include <stddef.h>
#include <stdint.h>

#include "common.hpp"

#include "Arch/x86/x86.h"

#include "Drivers/Disk.hpp"
#include "Drivers/TextModeTerminal.hpp"

#include "Utility/libc.hpp"
#include "Utility/Partition.hpp"
#include "Utility/PhysicalMemoryManager.hpp"

extern symbol __bss_start;
extern symbol __bss_end;

using ConstructorFunction = void(*)();

extern ConstructorFunction __init_array_start[];
extern ConstructorFunction __init_array_end[];

//TODO: Most of this stuff is temporary and just for tests, so don't judge this code
class A
{
    public:
        A() {  }
        ~A() { *c = 0x47; }

        char* c = reinterpret_cast<char*>(0xb800a);
};

A a;

#include "Arch/x86/IDT.hpp"

extern "C"
{
    bool a20_check(void);
    bool a20_enable(void);

    void __cxa_finalize(void*);
}
extern "C" __attribute__((section(".entry"))) __attribute__((cdecl)) void Stage2Main(uint8_t bootDrive, uint16_t stage2Size)
{
    // Zero-out .bss section
    uint8_t* bss_start  = reinterpret_cast<uint8_t*>(__bss_start);
    uint8_t* bss_end    = reinterpret_cast<uint8_t*>(__bss_end);
    for (uint8_t* p = bss_start; p < bss_end; p++) *p = 0;

    PhysicalMemoryManager::SetBelow1M_AllocatorBase(0x50000 + stage2Size);
    TextModeTerminal::Initialize();
    printf("BootDrive: 0x%x\n\n", bootDrive);

    //TODO(very important): implement logging, e9, serial and terminal

    if (!a20_enable()) panic("Failed to enable a20 line!");
    else printf("A20 successfully enabled!\n");

    // Call global constructors
    for (ConstructorFunction* constructor = __init_array_start; constructor < __init_array_end; constructor++) (*constructor)();

    IDT idt;
    idt.Initialize();
    idt.Load();
    __asm__("sti; int 32");

    *(long long*)0xb8f00 = 0x12591241124b124f;
    Terminal::Get()->SetColor(TerminalColor::eCyan, TerminalColor::eBlack);

    Disk::DetectAllDrives();

    Disk* drives = Disk::GetDrives();
    Partition part = drives[0].GetPartition(0);
    
    File* file = part.OpenFile("PhoenixOS.elf");
    if (!file) panic("Failed to open kernel file!");
    
    uint32_t continuationID = 0x00000000;
    constexpr const uint32_t MAX_MMAP_ENTRIES = 256;
    E820MemoryMapEntry entries[MAX_MMAP_ENTRIES];
    uint32_t memoryMapEntriesCount = 0;
    for (uint32_t i = 0; i < MAX_MMAP_ENTRIES; i++)
    {
        e820_get_next_entry(entries + i, &continuationID);
        printf("MemoryMapEntry:\nBase: %x\nLength: %x\nType: %x\n", (uint32_t)entries[i].base, (uint32_t)entries[i].length, entries[i].type);
    
        ++memoryMapEntriesCount;
        if (continuationID == 0) break;
    }

    uint32_t largestEntryBase = 0;
    uint32_t largestEntryLength = 0;
    for (uint32_t i = 0; i < memoryMapEntriesCount; i++)
    {
        if (entries[i].length > largestEntryLength && entries[i].type == 1)
        {
            largestEntryBase = entries[i].base;
            largestEntryLength = entries[i].length;
        }
    }

    Terminal::Get()->ClearScreen();
    PhysicalMemoryManager::Initialize(largestEntryBase, largestEntryLength);
    PhysicalMemoryManager::PrintFreeSpace();
    int* ptr1 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    int* ptr2 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    int* ptr3 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    delete ptr2;
    PhysicalMemoryManager::PrintFreeSpace();
    delete ptr1;
    PhysicalMemoryManager::PrintFreeSpace();
    delete ptr3;
    PhysicalMemoryManager::PrintFreeSpace();
    int* ptr5 = (int*)PhysicalMemoryManager::AllocateAligned(sizeof(int), 0x500);
    printf("ptr5: %x\n", (uint32_t)ptr5);
    PhysicalMemoryManager::PrintFreeSpace();
    delete ptr5;
    PhysicalMemoryManager::PrintFreeSpace();
    int* ptr6 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    delete ptr6;
    PhysicalMemoryManager::PrintFreeSpace();

    printf(" ____  _                      _       ___  ____  \n");
    printf("|  _ \\| |__   ___   ___ _ __ (_)_  __/ _ \\/ ___| \n");
    printf("| |_) | '_ \\ / _ \\ / _ \\ '_ \\| \\ \\/ / | | \\___ \\ \n");
    printf("|  __/| | | | (_) |  __/ | | | |>  <| |_| |___) |\n");
    printf("|_|   |_| |_|\\___/ \\___|_| |_|_/_/\\_\\___/|____/ \n");
    //outb (0x501, 0x31);

    while (true) halt();
    halt();
    __cxa_finalize(nullptr);
}




// Jebać Rust'a :)
