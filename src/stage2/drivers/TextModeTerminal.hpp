#pragma once

#include "Terminal.hpp"

#include "lib/PhysicalMemoryManager.hpp"

#include <cstdint>
#include <new>

class TextModeTerminal : public Terminal
{
    public:
        inline static void Initialize()
        {
            terminal = reinterpret_cast<Terminal*>(PhysicalMemoryManager::Allocate(sizeof(TextModeTerminal)));
            new(terminal)TextModeTerminal;
        }

        void PutChar(const char c) noexcept override final
        {
            switch (c)
            {
                //TODO: Bell
                case '\a': break;
                case '\b': x--; PutChar(' '); x--; break;
                case '\f': break;
                case '\n': y++; goto check_y;
                case '\r': x = 0; break;
                case '\t': x += 4; break;
                case '\v': y += 4; goto check_y;

                default:
                    videoMemory[y * TERMINAL_WIDTH + x] = color << 8 | c;
                    x++;
                    break;
            }

            if (x >= TERMINAL_WIDTH)
            {
                x -= TERMINAL_WIDTH;
                y++;
                check_y:
                if (y >= TERMINAL_HEIGHT)
                {
                    ScrollDown(1);
                    y--;
                }
            }
        }
        void SetColor(const TerminalColor foregroundColor, const TerminalColor backgroundColor) noexcept override final
        {
            color = (static_cast<uint8_t>(backgroundColor) << 4) | static_cast<uint8_t>(foregroundColor);
        }
        virtual void SetX(const uint32_t x) noexcept override final
        {
            this->x = x;
        }
        virtual void SetY(const uint32_t y) noexcept override final
        {
            this->y = y;
        }
        virtual void ScrollDown(const uint8_t lines) noexcept override final
        {
            //TODO: ScrollDown
        }

    private:
        TextModeTerminal()
        {
            videoMemory = reinterpret_cast<uint16_t*>(0xb8000);
            x = 0;
            y = 0;
            color = 0x02;
        }

        uint16_t* videoMemory;
        static constexpr const uint8_t TERMINAL_WIDTH = 80;
        static constexpr const uint8_t TERMINAL_HEIGHT = 25;

        uint32_t x;
        uint32_t y;
        uint8_t color;
};
