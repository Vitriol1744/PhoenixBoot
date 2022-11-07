#include "libc.hpp"

#include "drivers/Terminal.hpp"

#include <cstdarg>

void* memset(void* src, int c, size_t bytes)
{
    char* s = (char*)src;

    while (bytes > 0)
    {
        *s++ = c;
        bytes--;
    }

    return src;
}

char* itoa(int32_t value, char* str, int base)
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

void printf(const char* fmt, ...)
{
    char* c;
    va_list args;
    va_start(args, fmt);

    for (c = (char*)fmt; *c != '\0'; c++)
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
            {
                int32_t value = va_arg(args, int32_t);
                char string[20];
                char* str = string;
                itoa(value, str, 2);
                while (*str != '\0')
                {
                    Terminal::Get()->PutChar(*str);
                    ++str;
                }
            }
            break;
            case 'c':
            {
                char c = va_arg(args, char);
                Terminal::Get()->PutChar(c);
            }
            break;
            case 'd':
            case 'i':
            {
                int32_t value = va_arg(args, int32_t);
                char string[20];
                char* str = string;
                itoa(value, str, 10);
                while (*str != '\0')
                {
                    Terminal::Get()->PutChar(*str);
                    ++str;
                }
            }
            break;
            case 'o':
            {
                int32_t value = va_arg(args, int32_t);
                char string[20];
                char* str = string;
                itoa(value, str, 8);
                while (*str != '\0')
                {
                    Terminal::Get()->PutChar(*str);
                    ++str;
                }
            }
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
            {
                char* str = va_arg(args, char*);
                while (*str != '\0')
                    Terminal::Get()->PutChar(*str++);
            }
            break;
            case 'u':
            {
                uint32_t value = va_arg(args, uint32_t);
                char string[20];
                char* str = string;
                itoa(static_cast<int32_t>(value), str, 10);
                while (*str != '\0')
                {
                    Terminal::Get()->PutChar(*str);
                    ++str;
                }
            }
            break;
            case 'x':
            {
                int32_t value = va_arg(args, int32_t);
                char string[20];
                char* str = string;
                itoa(value, str, 16);
                while (*str != '\0')
                {
                    Terminal::Get()->PutChar(*str);
                    ++str;
                }
            }
            break;

            default:
                break;
        }
    }

    loop_end:
    return;
}