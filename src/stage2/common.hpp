#pragma once

#include "Arch/arch.hpp"

#include "Drivers/Terminal.hpp"

#include "Utility/libc.hpp"
#include "Utility/Logger.hpp"

#define PH_UNUSED [[maybe_unused]]

using symbol = void*[];

#ifdef PH_ARCH_X86
inline void halt() { while (true) __asm__("hlt"); }
#endif

void* operator new  (size_t count, void* ptr);
void* operator new  (size_t size);
void* operator new[](size_t count, void* ptr);

struct StackFrame
{
    StackFrame* ebp;
    uintptr_t eip;
};

inline void stackTrace()
{
    StackFrame* stackFrame;
    __asm__("mov %0, ebp" : "=r"(stackFrame)); 
    printf("Stack Trace: \n");
    while (stackFrame != nullptr)
    {
        //TODO: Figure out a way to resolve function names
        printf("0x%x\n", stackFrame->eip);
        stackFrame = stackFrame->ebp;
    } 
}

inline void panic(const char* msg, ...)
{
    Terminal::Get()->ClearScreen(TerminalColor::eBlue);
    LOG_FATAL("Bootloader Panic!\n");
    
    va_list args;
    va_start(args, msg);
    Logger::Log(LogLevel::eFatal, msg, args);
    va_end(args);
    LOG_FATAL("\n");

    stackTrace();
    halt();
}