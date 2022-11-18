#include "TextModeTerminal.hpp"

void TextModeTerminal::Initialize()
{
    terminal = new TextModeTerminal;
}

void TextModeTerminal::ClearScreen(TerminalColor tcolor)
{
    uint8_t _color = static_cast<uint8_t>(tcolor);
    _color <<= 4;
    _color |= static_cast<uint8_t>(tcolor);
    uint32_t terminalSize = TERMINAL_WIDTH * TERMINAL_HEIGHT * 2;
    for (uint64_t c = 0; c < TERMINAL_WIDTH * TERMINAL_HEIGHT; c++)
    {
        videoMemory[c] = (static_cast<uint32_t>(_color) << 8) | ' ';
    }
    x = y = 0;
}
void TextModeTerminal::PutChar(const char c)
{
    switch (c)
    {
        case '\a':                          break;
        case '\b': x--; PutChar(' '); x--;  break;
        case '\f':                          break;
        case '\n': y++;                     goto check_y;
        case '\r': x = 0;                   break;
        case '\t':
        {
            static constexpr const uint8_t tabSize = 8;
            uint8_t toSkip = x % tabSize;
            if (toSkip % 8 == 0) x += 8;
            else x += tabSize - toSkip;
        }
        break;
        case '\v': y += 4;                  goto check_y;   
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
            uint8_t n = y - TERMINAL_HEIGHT + 1;
            ScrollDown(n);
        }
    }
    UpdateCursor();
}
void TextModeTerminal::SetColor(const TerminalColor foregroundColor, const TerminalColor backgroundColor)
{
    color = (static_cast<uint8_t>(backgroundColor) << 4) | static_cast<uint8_t>(foregroundColor);
}
void TextModeTerminal::ScrollDown(uint8_t lines)
{
    uint8_t* src = reinterpret_cast<uint8_t*>(videoMemory + TERMINAL_WIDTH * lines);
    uint32_t toCopySize = (TERMINAL_HEIGHT - lines) * TERMINAL_WIDTH * 2;
    memcpy(videoMemory, src, toCopySize);

    memset(videoMemory + TERMINAL_WIDTH * (TERMINAL_HEIGHT - lines), 0x00, lines * TERMINAL_WIDTH * 2);
    if (y >= lines) y -= lines;
    else y = 0;
}

TextModeTerminal::TextModeTerminal()
{
    videoMemory = reinterpret_cast<uint16_t*>(0xb8000);
    uint16_t pos = 0;
    outb(0x3d4, 0x0f);
    pos |= inb(0x3d5);
    outb(0x3d4, 0x0e);
    pos |= static_cast<uint16_t>(inb(0x3D5)) << 8;
    
    x = 0;
    y = pos / TERMINAL_WIDTH;
    color = 0x02;
}

void TextModeTerminal::UpdateCursor() const
{
    uint16_t pos = y * TERMINAL_WIDTH + x;
	
    outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xff));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xff));
}