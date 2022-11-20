#pragma once

#include <stddef.h>
#include <stdint.h>

#include "Utility/Logger.hpp"

enum class TerminalColor
{
    eBlack              = 0,
    eBlue               = 1,
    eGreen              = 2,
    eCyan               = 3,
    eRed                = 4,
    eMagenta            = 5,
    eBrown              = 6,
    eLightGrey          = 7,
    eDarkGrey           = 8,
    eLightBlue          = 9,
    eLightGreen         = 10,
    eLightCyan          = 11,
    eLightRed           = 12,
    eLightMagenta       = 13,
    eYellow             = 14,
    eWhite              = 15,
};

class Terminal
{
    public:
        virtual ~Terminal() = default;

        virtual void ClearScreen(TerminalColor color) = 0;
        virtual void PutChar(char c) = 0;

        virtual TerminalColor GetForegroundColor() const = 0;
        virtual TerminalColor GetBackgroundColor() const = 0;

        virtual void SetColor(const TerminalColor foregroundColor, const TerminalColor backgroundColor) = 0;
        virtual void SetX(const uint32_t x) = 0;
        virtual void SetY(const uint32_t y) = 0;

        virtual void ScrollDown(uint8_t lines) = 0;        
        
        inline void ClearScreen() { ClearScreen(GetBackgroundColor()); }
        void PrintString(const char* string);
        void PrintString(const char* string, size_t count);

        inline void SetPosition(const uint32_t x, const uint32_t y) { SetX(x); SetY(y); }
        inline static Terminal* Get() { return terminal; }

    protected:
        Terminal() = default;

        static Terminal* terminal;
        static bool flag;
};