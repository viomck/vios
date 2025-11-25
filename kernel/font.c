#include <stdint.h>
#include <gfx.h>
#include <gen/font.h>

void FontRenderGlyph(uint8_t glyph, int scale, int xOffset, int yOffset)
{
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

void FontRenderChar(unsigned char c, int scale, int xOffset, int yOffset)
{
    FontRenderGlyph(c - ' ', scale, xOffset, yOffset);
}

void FontRenderStr(const unsigned char * string, int scale, int xOffset, int yOffset)
{
    char c = string[0];
    for (int i = 1; c != '\0'; i++)
    {
        FontRenderChar(c, scale, xOffset + (i * 8 * scale), yOffset);
        c = string[i];
    }
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
