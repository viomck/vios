#include <mem.h>
#include <gfx.h>
#include <print.h>

// kernel main entry point
void KMain()
{
    #ifdef FLAG_KERNELPANIC
    GfxSetScreenWidthAndHeight(0x1, 0x2);
    #endif

    MemInit();
    GfxInit();

    KPrintLn("Hello from the kernel!");
    KPrintLn("beans");
}
