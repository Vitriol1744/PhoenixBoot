#pragma once

#include "Terminal.hpp"
#include "arch/io.hpp"

#include "lib/libc.hpp"
#include "lib/PhysicalMemoryManager.hpp"

#include <cstdint>
#include <new>

class TextModeTerminal : public Terminal
{
    public:
        static void Initialize();

        virtual void ClearScreen(TerminalColor color) override final;
        virtual void PutChar(const char c) noexcept override final;

        virtual TerminalColor GetForegroundColor() const noexcept override final { return static_cast<TerminalColor>(color & 0xffff); }
        virtual TerminalColor GetBackgroundColor() const noexcept override final { return static_cast<TerminalColor>(color >> 4); }

        virtual void SetColor(const TerminalColor foregroundColor, const TerminalColor backgroundColor) noexcept override final;
        virtual inline void SetX(const uint32_t x) noexcept override final { this->x = x; }
        virtual inline void SetY(const uint32_t y) noexcept override final { this->y = y; }
        
        virtual void ScrollDown(uint8_t lines = 1) noexcept override final;

    private:
        TextModeTerminal();

        void UpdateCursor() const;

        uint16_t* videoMemory = nullptr;
        inline static constexpr const uint8_t TERMINAL_WIDTH = 80;
        inline static constexpr const uint8_t TERMINAL_HEIGHT = 25;

        uint32_t x = 0;
        uint32_t y = 0;
        uint8_t color = 0;
};
