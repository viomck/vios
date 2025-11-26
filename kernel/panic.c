#include <stdbool.h>
#include <font.h>
#include <gfx.h>

const static long UNSET_PANIC_DATA = 0xDEADBEEFCAFE1337;

unsigned long g_panicData1 = UNSET_PANIC_DATA;
unsigned long g_panicData2 = UNSET_PANIC_DATA;
unsigned long g_panicData3 = UNSET_PANIC_DATA;
unsigned long g_panicData4 = UNSET_PANIC_DATA;

void PanicSetData1(unsigned long data) { g_panicData1 = data; }
void PanicSetData2(unsigned long data) { g_panicData2 = data; }
void PanicSetData3(unsigned long data) { g_panicData3 = data; }
void PanicSetData4(unsigned long data) { g_panicData4 = data; }

void renderData(
    const short number, 
    unsigned long data, 
    bool top, 
    bool left, 
    int xoff, 
    int yoff
)
{
    const static int datascale = 2;

    if (!top) yoff += 64;
    if (!left) xoff += (datascale * 8 * (16 + 4) + 64);

    FontRenderChar('D', datascale, xoff, yoff);
    xoff += 8 * datascale;
    FontRenderChar('0' + number, datascale, xoff, yoff);
    xoff += 8 * datascale;
    FontRenderChar(':', datascale, xoff, yoff);
    xoff += 8 * datascale;
    // buffer
    xoff += 8 * datascale;

    // 0xDEADBEEFCAFE1337
    for (int i = 0; i < 16; i++)
    {
        char piece = ((char) (data >> 60 - (i * 4)) & 0xF);
        char c = piece < 10 ? '0' + piece : 'A' + (piece - 10);
        FontRenderChar(c, datascale, xoff, yoff);
        xoff += 8 * datascale;
    }
}

void Panic(const char * reason)
{
    int width = GfxGetScreenWidth();
    int height = GfxGetScreenHeight();

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            GfxPlot(x, y, width - (x / (width / 255)), 0, 0);
        }
    }

    const static int panicscale = 4;
    const static int reasonscale = 2;
    int yoff = 128 + 64;
    int xoff = 32;

    FontRenderStr("Panic!", panicscale, xoff, yoff);
    yoff += panicscale * 16;
    yoff += 32;  // add some visual buffer
    FontRenderStr(reason, reasonscale, xoff, yoff);
    yoff += reasonscale * 16;
    yoff += 64;  // add some visual buffer
    renderData(1, g_panicData1, true, true, xoff, yoff);
    renderData(2, g_panicData2, true, false, xoff, yoff);
    renderData(3, g_panicData3, false, true, xoff, yoff);
    renderData(4, g_panicData4, false, false, xoff, yoff);

    for (;;) {}
}
