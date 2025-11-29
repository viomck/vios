#include <stddef.h>
#include <stdint.h>
#include <panic.h>
#include <mem.h>

uint32_t g_pixelsPerScanLine;
uint16_t g_screenWidth;
uint16_t g_screenHeight;
uint64_t g_frameBufferStart; 
uint64_t g_frameDoubleBufferStart;

void GfxSetFrameBufferStart(uint64_t frameBufferStart)
{
    if (g_frameBufferStart != 0)
    {
        PanicSetData1(frameBufferStart);
        PanicSetData2((unsigned long) g_frameBufferStart);
        Panic("GfxSetFrameBufferStart: g_frameBufferStart (D2) already set. D1=provided frameBufferStart.");
    }
    g_frameBufferStart = frameBufferStart;
    // fake double buffer for now (until kernel takes over and calls GfxInit)
    g_frameDoubleBufferStart = frameBufferStart;
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

    *((uint32_t*)(g_frameBufferStart + 4 * g_pixelsPerScanLine * y + 4 * x)) = pixel;
    *((uint32_t*)(g_frameDoubleBufferStart + 4 * g_pixelsPerScanLine * y + 4 * x)) = pixel;

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

void GfxFullScreenGradientToColor(uint8_t r, uint8_t g, uint8_t b)
{
    int width = GfxGetScreenWidth();
    int height = GfxGetScreenHeight();

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            GfxPlot(
                (width - x),  // light color -> dark color
                y, 
                x == 0 || r == 0 ? 0 : r * ((float) x / width), 
                x == 0 || g == 0 ? 0 : g * ((float) x / width), 
                x == 0 || b == 0 ? 0 : b * ((float) x / width)
            );
        }
    }
}

void GfxBlankScreen()
{
    for (int x = 0; x < GfxGetScreenWidth(); x++)
    {
        for (int y = 0; y < GfxGetScreenHeight(); y++)
        {
            GfxPlot(x, y, 0, 0, 0);
        }
    }
}

uint64_t frameBufferOffset(uint16_t x, uint16_t y)
{
    return 4 * g_pixelsPerScanLine * y + 4 * x;
}

uint32_t pixelAt(uint16_t x, uint16_t y)
{
    return *((uint32_t*)(g_frameDoubleBufferStart + frameBufferOffset(x, y)));
}

void GfxScrollUp(uint16_t px)
{
    for (uint16_t y = 0; y < GfxGetScreenHeight(); y++)
    {
        for (uint16_t x = 0; x < GfxGetScreenWidth(); x++)
        {
            if (y > GfxGetScreenHeight() - px)
            {
                GfxPlot(x, y, 0, 0, 0);
            }
            else
            {
                GfxPlotPixel(x, y, pixelAt(x, y + px));
            }
        }
    }
}

void GfxInit()
{
    uint64_t fbs;

    MemAlloc(
        4 * g_pixelsPerScanLine * GfxGetScreenHeight() + 4 * GfxGetScreenWidth(),
        &g_frameDoubleBufferStart
    );

    // so that the buffer and double buffer are in sync
    GfxBlankScreen();
}
