#include <stddef.h>
#include <stdint.h>

#include "common.hpp"

#include "arch/x86/x86.h"

#include "drivers/Disk.hpp"
#include "drivers/TextModeTerminal.hpp"

#include "lib/libc.hpp"
#include "lib/Partition.hpp"
#include "lib/PhysicalMemoryManager.hpp"

extern uint8_t* __bss_start[];
extern uint8_t* __bss_end[];

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

    PhysicalMemoryManager::SetBelow1M_AllocatorBase(0x7e00 + stage2Size + 0x400);
    TextModeTerminal::Initialize();
    printf("BootDrive: 0x%x\n\n", bootDrive);

    if (!a20_enable()) panic("Failed to enable a20 line!");
    else printf("A20 successfully enabled!\n");
    
    // Call global constructors
    for (ConstructorFunction* f = __init_array_start; f < __init_array_end; f++) (*f)();

    //TODO: Render some cool ASCII art
    *(long long*)0xb8f00 = 0x12591241124b124f;
    Terminal::Get()->SetColor(TerminalColor::eCyan, TerminalColor::eBlack);

    Disk::DetectAllDrives();

    Disk* drives = Disk::GetDrives();
    Partition part = drives[0].GetPartition(0);
    
    File* file = part.OpenFile("PhoenixOS.elf");
    if (!file) panic("Failed to open kernel file!");
    //panic("Testing stuff...");

    #if defined __x86_64__ || defined(_M_X64)
    printf("X86_64\n");
    #endif

    halt();
    __cxa_finalize(nullptr);
}




// Jebać Rust'a :)
