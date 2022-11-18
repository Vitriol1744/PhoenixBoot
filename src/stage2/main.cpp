#include <stddef.h>
#include <stdint.h>

#include "common.hpp"

#include "Drivers/Disk.hpp"
#include "Drivers/TextModeTerminal.hpp"
#include "Drivers/Serial.hpp"

#include "Utility/libc.hpp"
#include "Utility/Logger.hpp"
#include "Utility/Partition.hpp"
#include "Utility/PhysicalMemoryManager.hpp"

extern symbol __bss_start;
extern symbol __bss_end;

using ConstructorFunction = void(*)();

extern ConstructorFunction __init_array_start[];
extern ConstructorFunction __init_array_end[];

#define QemuExit() outb (0x501, 0x31)

#include "Arch/x86/IDT.hpp"

extern "C"
{
    bool a20_check(void);
    bool a20_enable(void);

    void initializeConstructors()
    {
        for (ConstructorFunction* entry = __init_array_start; entry < __init_array_end; entry++)
        {
            ConstructorFunction constructor = *entry;
            constructor();
        }
    }
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
    Logger::SetOutputStreams(static_cast<OutputStream>(Logger::GetOutputStreams() | OutputStream::eTerminal));
    LOG_INFO("BootDrive: 0x%x\n\n", bootDrive);

    LOG_TRACE("Enabling A20 Line...\t");
    Terminal::Get()->SetX(76);
    if (!a20_enable()) panic("Failed to enable a20 line!");
    else LOG_INFO("[OK]\n");

    PhysicalMemoryManager::Initialize();

    Serial::Initialize();
    Logger::SetOutputStreams(static_cast<OutputStream>(Logger::GetOutputStreams() | OutputStream::eSerial));

    // Call global constructors
    initializeConstructors();

    initializeInterrupts();

    *(long long*)0xb8f00 = 0x12591241124b124f;
    Terminal::Get()->SetColor(TerminalColor::eCyan, TerminalColor::eBlack);

    Disk::DetectAllDrives();

    Disk* drives = Disk::GetDrives();
    Partition part = drives[0].GetPartition(0);
    
    const char* kernelFileName = "PhoenixOS.elf";
    File* file = part.OpenFile(kernelFileName);
    LOG_TRACE("Searching for %s file...\t", kernelFileName);
    Terminal::Get()->SetX(76);
    if (!file) panic("Failed to open kernel file!");
    else LOG_INFO("[OK]\n");

    halt();
    Terminal::Get()->ClearScreen();
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

    LOG_INFO(" ____  _                      _       ___  ____  \n");
    LOG_INFO("|  _ \\| |__   ___   ___ _ __ (_)_  __/ _ \\/ ___| \n");
    LOG_INFO("| |_) | '_ \\ / _ \\ / _ \\ '_ \\| \\ \\/ / | | \\___ \\ \n");
    LOG_INFO("|  __/| | | | (_) |  __/ | | | |>  <| |_| |___) |\n");
    LOG_INFO("|_|   |_| |_|\\___/ \\___|_| |_|_/_/\\_\\___/|____/ \n");
    //QemuExit();

    halt();
    __cxa_finalize(nullptr);
}




// Jebać Rust'a :)
