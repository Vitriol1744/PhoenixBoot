#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define BIT(n) (1 << n)

extern "C"
{
    enum OutputStream
    {
        eTerminal   = BIT(0),
        eSerial     = BIT(1),
        eE9         = BIT(2),
    };

    size_t strlen(const char* str);
    int strcmp(const char* lhs, const char* rhs);
    int strncmp(const char* lhs, const char* rhs, size_t n);
    void* memset(void* dest, char c, size_t n);
    void* memcpy(void* dest, const void* src, size_t bytes);
    
    int atoi(const char* str);
    char* itoa(int32_t value, char* str, int base);
    
    void printf(const char* fmt, ...);
    void vprintf(OutputStream outputStream, const char* fmt, va_list args);
}