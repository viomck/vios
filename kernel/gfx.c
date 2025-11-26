#include <stdint.h>
#include <panic.h>

uint64_t g_frameBufferStart; 
uint32_t g_pixelsPerScanLine;
uint16_t g_screenWidth;
uint16_t g_screenHeight;

void GfxSetFrameBufferStart(uint64_t frameBufferStart)
{
    if (g_frameBufferStart != 0)
    {
        PanicSetData1(frameBufferStart);
        PanicSetData2(g_frameBufferStart);
        Panic("GfxSetFrameBufferStart: g_frameBufferStart (D2) already set. D1=provided frameBufferStart.");
    }
    g_frameBufferStart = frameBufferStart;
}

void GfxSetPixelsPerScanLine(uint32_t pixelsPerScanLine)
{
    if (g_pixelsPerScanLine != 0)
    {
        PanicSetData1(pixelsPerScanLine);
        PanicSetData2(g_pixelsPerScanLine);
        Panic("GfxSetPixelsPerScanLine: g_pixelsPerScanLine (D2) already set. D1=provided pixelsPerScanLine.");
    }
    g_pixelsPerScanLine = pixelsPerScanLine;
}

void GfxSetScreenWidthAndHeight(uint16_t width, uint16_t height)
{
    if (g_screenWidth != 0 || g_screenHeight != 0)
    {
        PanicSetData1(width);
        PanicSetData2(height);
        PanicSetData3(g_screenWidth);
        PanicSetData4(g_screenHeight);
        Panic("GfxSetScreenWidthAndHeight: g_screenWidth (D3) &/or g_screenHeight (D4) already set. D1=provided width, D2=provided height");
    }
    g_screenWidth = width;
    g_screenHeight = height;
}

uint16_t GfxGetScreenWidth()
{
    if (g_screenWidth == 0) Panic("GfxGetScreenWidth: g_screenWidth not set!");
    return g_screenWidth;
}

uint16_t GfxGetScreenHeight()
{
    if (g_screenWidth == 0) Panic("GfxGetScreenHeight: g_screenHeight not set!");
    return g_screenHeight;
}

void GfxPlotPixel(int x, int y, uint32_t pixel)
{
    if (x < 0 || y < 0)
    {
        PanicSetData1(x);
        PanicSetData2(y);
        PanicSetData3(pixel);
        Panic("GfxPlotPixel: x (D1) or y (D2) are negative. D3=pixel");
    }

    // TODO: panic if frameBufferStart or pixelsPerScanLine are not set but we
    // can't panic until we actually have graphics lol
    *((uint32_t*)(g_frameBufferStart + 4 * g_pixelsPerScanLine * y + 4 * x)) = pixel;

    #ifdef FLAG_SLOWGFX
    for (int i = 0; i < 10000; i++) { __asm__("nop"); }
    #endif
}

void GfxPlot(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    // assume full alpha for now
    uint32_t pixel = 0xFF000000;

    pixel |= r << 16;
    pixel |= g << 8;
    pixel |= b;

    GfxPlotPixel(x, y, pixel);
}
