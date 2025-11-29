#include <stdbool.h>
#include <font.h>
#include <gfx.h>

const static long UNSET_PANIC_DATA = 0xDEADBEEFCAFE1337;

unsigned long g_panicData1 = UNSET_PANIC_DATA;
unsigned long g_panicData2 = UNSET_PANIC_DATA;
unsigned long g_panicData3 = UNSET_PANIC_DATA;
unsigned long g_panicData4 = UNSET_PANIC_DATA;
unsigned long g_panicData5 = UNSET_PANIC_DATA;
unsigned long g_panicData6 = UNSET_PANIC_DATA;
unsigned long g_panicData7 = UNSET_PANIC_DATA;
unsigned long g_panicData8 = UNSET_PANIC_DATA;

void PanicSetData1(unsigned long data) { g_panicData1 = data; }
void PanicSetData2(unsigned long data) { g_panicData2 = data; }
void PanicSetData3(unsigned long data) { g_panicData3 = data; }
void PanicSetData4(unsigned long data) { g_panicData4 = data; }
void PanicSetData5(unsigned long data) { g_panicData5 = data; }
void PanicSetData6(unsigned long data) { g_panicData6 = data; }
void PanicSetData7(unsigned long data) { g_panicData7 = data; }
void PanicSetData8(unsigned long data) { g_panicData8 = data; }

void renderData(
    const short number, 
    unsigned long data, 
    bool top, 
    bool left,
    // this is so awful
    bool row3,
    int xoff, 
    int yoff
)
{
    const static int datascale = 2;

    if (!top) yoff += 64;
    if (row3) yoff += 128;
    if (!left) xoff += (datascale * 8 * (16 + 4) + 64);

    FontRenderChar('D', datascale, xoff, yoff);
    xoff += 8 * datascale;
    FontRenderChar('0' + number, datascale, xoff, yoff);
    xoff += 8 * datascale;
    FontRenderChar(':', datascale, xoff, yoff);
    xoff += 8 * datascale;
    // buffer
    xoff += 8 * datascale;

    FontRenderHex(data, datascale, xoff, yoff);
}

void Panic(const char * reason)
{
    int width = GfxGetScreenWidth();
    int height = GfxGetScreenHeight();

    GfxFullScreenGradientToColor(0xFF, 0, 0);

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
    renderData(1, g_panicData1, true, true, false, xoff, yoff);
    renderData(2, g_panicData2, true, false, false, xoff, yoff);
    renderData(3, g_panicData3, false, true, false, xoff, yoff);
    renderData(4, g_panicData4, false, false, false, xoff, yoff);
    renderData(5, g_panicData5, true, true, true, xoff, yoff);
    renderData(6, g_panicData6, true, false, true, xoff, yoff);
    renderData(7, g_panicData7, false, true, true, xoff, yoff);
    renderData(8, g_panicData8, false, false, true, xoff, yoff);

    for (;;) {}
}
