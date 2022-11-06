#include <cstddef>
#include <cstdint>

inline void* operator new(std::size_t, void* p)     noexcept { return p; }
inline void* operator new[](std::size_t, void* p)   noexcept { return p; }
inline void  operator delete(void*, void*)          noexcept { };
inline void  operator delete[](void*, void*)        noexcept { };

extern uint8_t* __bss_start[];
extern uint8_t* __bss_end[];

using ConstructorFunction = void(*)();

extern ConstructorFunction __init_array_start[];
extern ConstructorFunction __init_array_end[];

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
}

extern "C" __attribute__((section(".entry"))) void Stage2Main(uint8_t bootDrive, size_t stage2Size)
{
    // Zero-out .bss section
    uint8_t* bss_start  = reinterpret_cast<uint8_t*>(__bss_start);
    uint8_t* bss_end    = reinterpret_cast<uint8_t*>(__bss_end);
    for (uint8_t* p = bss_start; p < bss_end; p++) *p = 0;
    
    // Call global constructors
    for (ConstructorFunction* f = __init_array_start; f < __init_array_end; f++) (*f)();

    *(long long*)0xb8000 = 0x12591241124b124f;

    __cxa_finalize();
    __asm__("hlt");
}