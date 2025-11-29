#include <font.h>
#include <gfx.h>
#include <mem.h>

uint16_t g_xoff = 0;
uint16_t g_yoff = 0;

void newline()
{
    if (g_yoff + 16 >= GfxGetScreenHeight())
    {
        GfxScrollUp(16);
    }
    else
    {
        g_yoff += 16;
    }

    g_xoff = 0;
}

void KPrint(char * string)
{
    char c = string[0];
    for (int i = 1; c != '\0'; i++)
    {
        if (c == '\n' || g_xoff + 8 >= GfxGetScreenWidth())
        {
            newline();
        }
        if (c != '\n')
        {
            FontRenderChar(c, 1, g_xoff, g_yoff);
            g_xoff += 8;
        }
        c = string[i];
    }
}

void KPrintLn(char * string)
{
    KPrint(string);
    newline();
}
