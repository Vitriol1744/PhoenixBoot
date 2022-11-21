#include <stddef.h>
#include <stdint.h>

#include "common.hpp"

#include "Filesystem/Volume.hpp"

#include "Drivers/BlockDevice.hpp"
#include "Drivers/GraphicsTerminal.hpp"
#include "Drivers/TextModeTerminal.hpp"
#include "Drivers/Serial.hpp"

#include "Utility/font.hpp"
#include "Utility/libc.hpp"
#include "Utility/Logger.hpp"
#include "Memory/PhysicalMemoryManager.hpp"

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
    
    PhysicalMemoryManager::SetBelow1M_AllocatorBase(85480 + 0x7e00);
    TextModeTerminal::Initialize();
    Logger::SetOutputStreams(static_cast<OutputStream>(Logger::GetOutputStreams() | OutputStream::eTerminal));
    LOG_INFO("BootDrive: 0x%x\n\n", bootDrive);
    LOG_INFO("Stage2 size: %d\n");

    LOG_TRACE("Enabling A20 Line...\t");
    if (!a20_enable()) panic("Failed to enable a20 line!");
    else LOG_INFO("[OK]\n");

    PhysicalMemoryManager::Initialize();

    Serial::Initialize();
    Logger::SetOutputStreams(static_cast<OutputStream>(Logger::GetOutputStreams() | OutputStream::eSerial));

    // Call global constructors
    initializeConstructors();

    initializeInterrupts();
    __asm__("sti;");

    *(long long*)0xb8f00 = 0x12591241124b124f;
    Terminal::Get()->SetColor(TerminalColor::eCyan, TerminalColor::eBlack);

    Volume::DetectVolumes();

    const char* kernelFileName = "PhoenixOS.elf";
    File* kernelFile = Volume::SearchForFile(kernelFileName);
    
    LOG_TRACE("Searching for %s file...\t", kernelFileName);
    if (!kernelFile) panic("Failed to open kernel file!");
    else LOG_INFO("[OK]\n");
    
    Volume::CloseFile(kernelFile);
    void* kernel = PhysicalMemoryManager::Allocate(kernelFile->GetSize());
    kernelFile->ReadAll(kernel);
    delete kernel;
    printf("SIZE: %d\n", kernelFile->GetSize());
    
    FramebufferInfo framebufferInfo;
    getFramebufferInfo(framebufferInfo);

    //TODO: Add support for psf1 fonts
    GraphicsTerminal::Initialize(framebufferInfo, zap_light20_psf);
    Terminal::Get()->ClearScreen(TerminalColor::eCyan);
    LOG_INFO("Yo! Graphics Terminal is up and running! x: %d, y: %d\n", 17, 477);

    LOG_INFO("BlockDevice: %d\n", bootDrive);
    LOG_INFO("STAGE2_SIZE: %d\n", stage2Size);

    printf("TESTS:\n");
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Allocating bytes...\n");
    int* ptr1 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Allocating bytes...\n");
    int* ptr17 = (int*)PhysicalMemoryManager::Allocate(4600);
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Allocating bytes...\n");
    int* ptr2 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Allocating bytes...\n");
    int* ptr3 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Allocating bytes...\n");
    long* ptr4 = new long;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Freeing bytes...\n");
    delete ptr2;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Freeing bytes...\n");
    delete ptr1;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Freeing bytes...\n");
    delete ptr3;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Freeing bytes...\n");
    delete ptr17;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Freeing bytes...\n");
    delete ptr4;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Allocating bytes...\n");
    int* ptr5 = (int*)PhysicalMemoryManager::AllocateAligned(sizeof(int), 0x500);
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Freeing bytes...\n");
    delete ptr5;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Allocating bytes...\n");
    int* ptr6 = new int;
    PhysicalMemoryManager::PrintFreeSpace();
    printf("Freeing bytes...\n");
    delete ptr6;
    PhysicalMemoryManager::PrintFreeSpace();
    //__asm__("int 0x10");
    LOG_INFO(" ____  _                      _       ___  ____  \n");
    LOG_INFO("|  _ \\| |__   ___   ___ _ __ (_)_  __/ _ \\/ ___| \n");
    LOG_INFO("| |_) | '_ \\ / _ \\ / _ \\ '_ \\| \\ \\/ / | | \\___ \\ \n");
    LOG_INFO("|  __/| | | | (_) |  __/ | | | |>  <| |_| |___) |\n");
    LOG_INFO("|_|   |_| |_|\\___/ \\___|_| |_|_/_/\\_\\___/|____/ \n");

    halt();
    QemuExit();
    __cxa_finalize(nullptr);
}




// Jebać Rust'a :)
