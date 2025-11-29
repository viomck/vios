#include <stdbool.h>
#include <stdint.h>
#include <gfx.h>
#include <gen/genfont.h>
#include <panic.h>

void FontRenderGlyph(uint8_t glyph, int scale, int xOffset, int yOffset)
{
    if (scale < 0)
    {
        PanicSetData1(scale);
        PanicSetData2(glyph);
        PanicSetData3(xOffset);
        PanicSetData4(yOffset);
        Panic("FontRenderGlyph: Provided scale value (D1) is negative. D2=glyph,D3=xOffset,D4=yOffset");
    }

    int gx = 0;
    int gy = 0;
    int sx = 0;
    int sy = 0;

    for (int y = 0; y < 16 * scale; y++)
    {
        gx = 0;
        sx = 0;
        for (int x = 0; x < 8 * scale; x++)
        {
            GfxPlotPixel(
                x + xOffset, 
                y + yOffset,
                (GenFontGetBitmap(((glyph + 1) * 16) + gy) << gx) & 0b10000000
                    ? GFX_WHITE
                    : GFX_BLACK
            );
            sx++;
            if (sx == scale) 
            {
                sx = 0;
                gx++;
            }
        }
        sy++;
        if (sy == scale) 
        {
            sy = 0; 
            gy++;
        }
    }
}

void FontRenderChar(char c, int scale, int xOffset, int yOffset)
{
    if (c < ' ')
    {
        PanicSetData1(c);
        Panic("FontRenderChar: Provided character (D1) is invalid.");
    }
    FontRenderGlyph(c - ' ', scale, xOffset, yOffset);
}

void FontRenderHex(uint64_t data, int scale, int xOffset, int yOffset)
{
    for (int i = 0; i < 16; i++)
    {
        char piece = ((char) (data >> 60 - (i * 4)) & 0xF);
        char c = piece < 10 ? '0' + piece : 'A' + (piece - 10);
        FontRenderChar(c, scale, xOffset, yOffset);
        xOffset += 8 * scale;
    }
}

void _FontRenderStr(const char * string, int scale, int xOffset, int yOffset, bool wrap)
{
    int xoff = xOffset;
    int yoff = yOffset;

    int width = GfxGetScreenWidth();
    char c = string[0];
    for (int i = 1; c != '\0'; i++)
    {
        FontRenderChar(c, scale, xoff, yoff);
        xoff += 8 * scale;
        if (wrap && xoff >= width - (8 * scale) - xOffset)
        {
            xoff = xOffset;
            yoff += scale * 16;
        }
        c = string[i];
    }
}

void FontRenderStr(const char * string, int scale, int xOffset, int yOffset)
{
    _FontRenderStr(string, scale, xOffset, yOffset, true);
}

void FontRenderStrUnwrapped(const char * string, int scale, int xOffset, int yOffset)
{
    _FontRenderStr(string, scale, xOffset, yOffset, false);
}

void FontRenderTestGlyphs()
{
    const int scale = 4;

    for (int i = 0; i < 9; i++)
    {
        FontRenderGlyph(i, scale, i * 10 * scale, 0);
    }

    for (int x = 0; x < 255; x++)
    {
        for (int y = 0; y < 50; y++)
        {
            GfxPlotPixel(x, 100 + y, GenFontGetBitmap(x));
        }
    }

    FontRenderChar('!', 16, 0, 200);

    FontRenderStr("The quick brown fox jumps over the lazy dog.", 2, 0, 300);
}
