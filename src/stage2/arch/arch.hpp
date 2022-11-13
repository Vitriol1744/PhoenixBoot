#pragma once

#define PH_ARCH_X86_64 0x00001
#define PH_ARCH_X86_32 0x00002

#if PH_ARCH == PH_ARCH_X86_64 || PH_ARCH == PH_ARCH_X86_32
    #define PH_ARCH_X86
#endif