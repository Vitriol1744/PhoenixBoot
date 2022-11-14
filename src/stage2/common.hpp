#pragma once

#include "arch/arch.hpp"

#include "lib/libc.hpp"
#include "drivers/Terminal.hpp"

#define PH_UNUSED [[maybe_unused]]

#ifdef PH_ARCH_X86
inline void halt() { __asm__("hlt"); }
#endif

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

inline void panic(const char* msg)
{
    Terminal::Get()->ClearScreen(TerminalColor::eBlue);
    Terminal::Get()->SetColor(TerminalColor::eWhite, TerminalColor::eRed);
    printf("Bootloader Panic!\n%s\n", msg);
    
    stackTrace();
    halt();
}