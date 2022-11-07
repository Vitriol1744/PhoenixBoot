#pragma once

#include <cstddef>
#include <cstdint>

enum class TerminalColor
{
    Black           = 0,
    Blue            = 1,
    Green           = 2,
    Cyan            = 3,
    Red             = 4,
    Magenta         = 5,
    Brown           = 6,
    LightGrey       = 7,
    DarkGrey        = 8,
    LightBlue       = 9,
    LightGreen      = 10,
    LightCyan       = 11,
    LightRed        = 12,
    LightMagenta    = 13,
    LightBrown      = 14,
    White           = 15,
};

class Terminal
{
    public:
        virtual void PutChar(char c) = 0;
        virtual void SetColor(const TerminalColor foregroundColor, const TerminalColor backgroundColor) = 0;
        virtual void SetX(const uint32_t x) = 0;
        virtual void SetY(const uint32_t y) = 0;
        virtual void ScrollDown(const uint8_t lines) = 0;
        
        inline void SetPosition(const uint32_t x, const uint32_t y) { SetX(x); SetY(y); }

        void PrintString(const char* string);
        void PrintString(const char* string, size_t count);

        inline static Terminal* Get() { return terminal; }

    protected:
        Terminal() = default;

        static Terminal* terminal;
};