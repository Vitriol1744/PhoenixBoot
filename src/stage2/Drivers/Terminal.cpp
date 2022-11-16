#include "Terminal.hpp"

Terminal* Terminal::terminal = nullptr;

void Terminal::PrintString(const char* string)
{
    while (*string != '\0')
        PutChar(*string++);
}
void Terminal::PrintString(const char* string, size_t count)
{
    while (count > 0)
    {
        PutChar(*string++);
        count--;
    }
}