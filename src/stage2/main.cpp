#include <cstddef>
#include <cstdint>

#include "arch/x86/x86.h"

#include "drivers/TextModeTerminal.hpp"

#include "lib/libc.hpp"
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
        ~A() { *c = 'B'; }

        char* c = reinterpret_cast<char*>(0xb800a);
};

A a;

#define UNUSED [[maybe_unused]]

extern "C"
{
    struct Destructor
    {
        void(*destructor)(void*);
        void* objptr;
    };
    Destructor destructors[128];

    void* __dso_handle = reinterpret_cast<void*>(0x7e00);
    int __cxa_atexit(void(*destructor)(void* objptr), void* objptr, UNUSED void* dso)
    {
        static uint32_t currentIndex = 0;
        destructors[currentIndex].destructor = destructor;
        destructors[currentIndex].objptr = objptr;

        currentIndex++;
        return 0;
    }

    void __cxa_finalize()
    {
        for (int i = 0; i < 128; i++)
        {
            destructors[i].destructor(destructors[i].objptr);
        }
    }

    void __cxa_pure_virtual()
    {
        // Do nothing or print an error message.
    }
}

extern "C" __attribute__((section(".entry"))) __attribute__((cdecl)) void Stage2Main(uint8_t bootDrive, uint16_t stage2Size)
{
    // Zero-out .bss section
    uint8_t* bss_start  = reinterpret_cast<uint8_t*>(__bss_start);
    uint8_t* bss_end    = reinterpret_cast<uint8_t*>(__bss_end);
    for (uint8_t* p = bss_start; p < bss_end; p++) *p = 0;

    PhysicalMemoryManager::SetBelow1M_AllocatorBase(0x7e00 + stage2Size + 0x100);
    TextModeTerminal::Initialize();
    Terminal::Get()->SetY(15);
    printf("BootDrive: %x", bootDrive);
    
    // Call global constructors
    for (ConstructorFunction* f = __init_array_start; f < __init_array_end; f++) (*f)();

    //TODO: Render some cool ASCII art

    *(long long*)0xb80f0 = 0x12591241124b124f;
    Terminal::Get()->SetColor(TerminalColor::eCyan, TerminalColor::eBlack);

    char buffer[20];
    itoa(146, buffer, 16);
    Terminal::Get()->PrintString(buffer);
    printf("some_value: %c%c%x%x%r%d", 'a', 'b', 0xf, 0xf, "123", 3);
    printf("\nBootDrive: 0x%x, Stage2Size: %x", bootDrive, stage2Size);

    uint8_t drive_type;
    uint16_t cylinders = 0, sectors = 0, heads = 0;
    get_drive_parameters(0x80, &drive_type, &cylinders, &sectors, &heads);

    printf("Cylinders: %d, Sectors: %d, Heads: %d", cylinders, sectors, heads);

    __cxa_finalize();
    __asm__("hlt");
}




// Jebać C :)
