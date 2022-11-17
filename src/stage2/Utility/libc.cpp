#include "libc.hpp"

#include "Drivers/Terminal.hpp"

#include <stdarg.h>

#define CFUNC extern "C"

CFUNC long pow(long num, long power)
{
	int invert = 0;

	long powMult = 1;

	if ((invert = (power < 0)))
		power = -power;

	for (long x = 0; x < power; x++)
		powMult *= num;

	return (invert) ? 1 / powMult : powMult;
}

CFUNC size_t strlen(const char* str)
{
    size_t bytes = 0;
    while (*str != '\0')
    {
        bytes++;
        ++str;
    }

    return bytes;
}
CFUNC int strcmp(const char* lhs, const char* rhs)
{
    while (*lhs != '\0' && *lhs == *rhs)
    {
        ++lhs;
        ++rhs;
    }
    if (*lhs == *rhs) return 0;
    return (*lhs - *rhs);
}
CFUNC int strncmp(const char* lhs, const char* rhs, size_t n)
{
    while (*lhs != '\0' && *lhs == *rhs && n > 0)
    {
        ++lhs;
        ++rhs;
        --n;
    }
    if (*lhs == *rhs) return 0;
    return (*lhs - *rhs);
}
CFUNC void* memset(void* src, char c, size_t bytes)
{
    char* s = (char*)src;

    while (bytes > 0)
    {
        *s++ = c;
        bytes--;
    }

    return src;
}
CFUNC void* memcpy(void* dest, const void* src, size_t bytes)
{
    char* d = static_cast<char*>(dest);
    char* s = static_cast<char*>(const_cast<void*>(src));
    while (bytes > 0)
    {
        *d++ = *s++;
        bytes--;
    }

    return dest;
}

CFUNC int atoi(const char* str)
{
    int integer = 0;
    bool isNegative = str[0] == '-';

    int index = isNegative;
    size_t stringLength = strlen(str), power = stringLength - isNegative;

    for (; index < stringLength; index++)
        integer += (str[index] - 48) * pow(10, --power);


    return (isNegative) ? -integer : integer;
}
CFUNC char* itoa(int value, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    if (value == 0)
    {
        str[i++] = '0';
        str[i] = 0;
        return str;
    }

    if (value < 0 && base == 10)
    {
        isNegative = true;
        value = -value;
    }

    while (value != '\0')
    {
        int rem = value % base;
        str[i++] = (rem > 9) ? (char)((rem - 10) + 'a') : (char)(rem + '0');
        value = value / base;
    }

    if (isNegative) str[i++] = '-';
    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end)
    {
        char c = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = c;
        start++;
        end--;
    }

    return str;
}

CFUNC void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    
    va_end(args);
}
template<typename T>
void printnum(va_list args, uint32_t base)
{
    T value = va_arg(args, T);
    char str[20];
    itoa(value, str, base);
    Terminal::Get()->PrintString(str);
}
CFUNC void vprintf(const char* fmt, va_list args)
{
    for (char* c = (char*)fmt; *c != '\0'; c++)
    {
        while (*c != '%')
        {
            if (*c == '\0') goto loop_end;
            if (*c == '\n') Terminal::Get()->PutChar('\r');
            Terminal::Get()->PutChar(*c);
            c++;
        }
        c++;

        switch (*c)
        {
            case 'b':
                printnum<int>(args, 2);
                break;
            case 'c':
                Terminal::Get()->PutChar(va_arg(args, int));
                break;
            case 'd':
            case 'i':
                printnum<int>(args, 10);
                break;
            case 'l':
                printnum<int64_t>(args, 10);
                break;
            case 'o':
                printnum<int>(args, 8);
                break;
            case 'r':
            {
                char* str = va_arg(args, char*);
                char* str_start = str;
                while (*str != '\0') str++;
                while (str >= str_start)
                {
                    Terminal::Get()->PutChar(*str);
                    str--;
                }
            }
            break;
            case 's':
                Terminal::Get()->PrintString(va_arg(args, char*));
                break;
            case 'u':
                printnum<unsigned int>(args, 10);
                break;
            case 'x':
                printnum<int>(args, 16);
                break;

            default:
                break;
        }
    }

loop_end:
    return;
}