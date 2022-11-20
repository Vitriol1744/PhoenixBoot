#pragma once

#include "Drivers/Terminal.hpp"

#include "Arch/arch.hpp"

class GraphicsTerminal : public Terminal
{
    public:
        static void Initialize(FramebufferInfo& framebufferInfo, uint8_t* font);

        virtual void ClearScreen(TerminalColor color) override;
        virtual void PutChar(char c) override;

        virtual TerminalColor GetForegroundColor() const override;
        virtual TerminalColor GetBackgroundColor() const override;

        virtual void SetColor(const TerminalColor foregroundColor, const TerminalColor backgroundColor) override;
        virtual void SetX(const uint32_t x) override;        
        virtual void SetY(const uint32_t y) override;       

        virtual void ScrollDown(uint8_t lines) override; 

    private:
        GraphicsTerminal(FramebufferInfo& framebufferInfo, uint8_t* font);

        void PutPixel(uint32_t pixel, uint32_t _x, uint32_t _y);
        
        FramebufferInfo framebufferInfo;
        uint8_t* font;

        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t foregroundColor;
        uint32_t backgroundColor;
};