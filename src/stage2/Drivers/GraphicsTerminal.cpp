#include "GraphicsTerminal.hpp"

#include "Utility/Logger.hpp"

#define PSF2_MAGIC0         0x72
#define PSF2_MAGIC1         0xb5
#define PSF2_MAGIC2         0x4a
#define PSF2_MAGIC3         0x86

#define PSF2_UNICODE_TABLE  0x01

#define PSF2_MAXVERSION     0

#define PSF2_SEPARATOR      0xff
#define PSF2_STARTSEQ       0xfe

struct PSF2Font
{
    uint32_t magic;
    uint32_t version;
    uint32_t headerSize;
    uint32_t flags;
    uint32_t glyphCount;
    uint32_t bytesPerGlyph;
    uint32_t height;
    uint32_t width;
};

void GraphicsTerminal::Initialize(FramebufferInfo& framebufferInfo, uint8_t* font)
{
    if (terminal)
    {
        delete terminal;
        terminal = nullptr;
    }
    terminal = new GraphicsTerminal(framebufferInfo, font);
}

void GraphicsTerminal::ClearScreen(TerminalColor color)
{
    for (uint32_t y = 0; y < framebufferInfo.height; y++)
    {
        for (uint32_t x = 0; x < framebufferInfo.width; x++)
        {
            uint32_t* pixelOffset = (uint32_t*)(y * framebufferInfo.pitch + (x * (framebufferInfo.bpp / 8)) + framebufferInfo.framebufferBase);
            *pixelOffset = 0x0000ff00;
        }
    }
}
void GraphicsTerminal::PutChar(char c)
{
    PSF2Font* font_ = (PSF2Font*)font;

    uint32_t charactersPerLine = framebufferInfo.width  / (font_->width  + 2);
    uint32_t charactersPerRow  = framebufferInfo.height / (font_->height + 2);

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
        {
            uint32_t bytesPerLine = (font_->width + 7) / 8;

            uint8_t* glyph = (uint8_t*)font + font_->headerSize + (c > 0 && c < font_->glyphCount ? c : 0) * font_->bytesPerGlyph;

            uint32_t ypos = y * font_->height;
            uint32_t xpos = x * font_->width + 1;
            for (uint32_t _y = ypos; _y < font_->height + ypos; _y++)
            {
                uint32_t mask = 1 << (font_->width - 1);
                for (uint32_t _x = xpos; _x < font_->width + xpos; _x++)
                {
                    PutPixel(*((uint32_t*)glyph) & mask ? foregroundColor : backgroundColor, _x, _y);
                    mask >>= 1;
                }
                glyph += bytesPerLine;
            }
            x++;
        }
        break;
    }   
    if (x >= charactersPerLine)
    {
        x -= charactersPerLine;
        y++;
        check_y:
        if (y >= charactersPerRow)
        {
            uint8_t n = y - charactersPerRow + 1;
            ScrollDown(n);
        }
    }
    //TODO:UpdateCursor(font_->width, font_->height);
}

TerminalColor GraphicsTerminal::GetForegroundColor() const
{
    return ToTerminalColor(foregroundColor & 0xffffff);
}
TerminalColor GraphicsTerminal::GetBackgroundColor() const
{
    return ToTerminalColor(backgroundColor & 0xffffff);
}

void GraphicsTerminal::SetColor(const TerminalColor foregroundColor, const TerminalColor backgroundColor)
{
    this->foregroundColor = ToRGB(foregroundColor);
    this->backgroundColor = ToRGB(backgroundColor);
}
void GraphicsTerminal::SetX(const uint32_t _x)
{
    x = _x;
}
void GraphicsTerminal::SetY(const uint32_t _y)
{
    y = _y;
}

void GraphicsTerminal::ScrollDown(uint8_t lines)
{
    PSF2Font* font_ = (PSF2Font*)font;
    size_t bytesToCopy = framebufferInfo.pitch * framebufferInfo.height - framebufferInfo.pitch - font_->height * lines;
    memcpy((void*)framebufferInfo.framebufferBase, (void*)(framebufferInfo.framebufferBase + framebufferInfo.pitch * font_->height * lines), bytesToCopy);
}

GraphicsTerminal::GraphicsTerminal(FramebufferInfo& framebufferInfo, uint8_t* font)
{
    this->framebufferInfo = framebufferInfo;
    this->font = font;
    this->foregroundColor = 0x00ff0000;
    this->backgroundColor = 0x00000000;
    terminal = this;
}

uint32_t GraphicsTerminal::ToRGB(TerminalColor color) const
{
    switch (color)
    {
        case TerminalColor::eBlack:         return 0x00000000;
        case TerminalColor::eBlue:          return 0x000000ff;
        case TerminalColor::eGreen:         return 0x0000ff00;
        case TerminalColor::eCyan:          return 0x0000ffff;
        case TerminalColor::eRed:           return 0x00ff0000;
        case TerminalColor::eMagenta:       return 0x00ff00ff;
        case TerminalColor::eBrown:         return 0x00964b00;
        case TerminalColor::eLightGrey:     return 0x00d3d3d3;
        case TerminalColor::eDarkGrey:      return 0x005a5a5a;
        case TerminalColor::eLightBlue:     return 0x00add8e6;
        case TerminalColor::eLightCyan:     return 0x00e0ffff;
        case TerminalColor::eLightRed:      return 0x00ffcccb;
        case TerminalColor::eLightMagenta:  return 0x00ff80ff;
        case TerminalColor::eYellow:        return 0x00ffff00;
        case TerminalColor::eWhite:         return 0x00ffffff;
    }
    return 0x00ffffff;
}
TerminalColor GraphicsTerminal::ToTerminalColor(uint32_t color) const
{
    switch (color)
    {
        case 0x000000: return TerminalColor::eBlack;
        case 0x0000ff: return TerminalColor::eBlue;
        case 0x00ff00: return TerminalColor::eGreen;
        case 0x00ffff: return TerminalColor::eCyan;
        case 0xff0000: return TerminalColor::eRed;
        case 0xff00ff: return TerminalColor::eMagenta;
        case 0x964b00: return TerminalColor::eBrown;
        case 0xd3d3d3: return TerminalColor::eLightGrey;
        case 0x5a5a5a: return TerminalColor::eDarkGrey;
        case 0xadd8e6: return TerminalColor::eLightBlue;
        case 0xe0ffff: return TerminalColor::eLightCyan;
        case 0xffcccb: return TerminalColor::eLightRed;
        case 0xff80ff: return TerminalColor::eLightMagenta;
        case 0xffff00: return TerminalColor::eYellow;
        case 0xffffff: return TerminalColor::eWhite;
    }

    return TerminalColor::eWhite;
}
void GraphicsTerminal::PutPixel(uint32_t pixel, uint32_t _x, uint32_t _y)
{
    uint32_t* pixelOffset = (uint32_t*)(_y * framebufferInfo.pitch + (_x * (framebufferInfo.bpp / 8)) + framebufferInfo.framebufferBase);
    *pixelOffset = pixel;
}