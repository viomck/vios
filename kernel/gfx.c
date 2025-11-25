#include <stdint.h>

uint64_t g_frameBufferStart; 
uint32_t g_pixelsPerScanLine;

void GfxSetFrameBufferStart(uint64_t frameBufferStart)
{
    g_frameBufferStart = frameBufferStart;
}

void GfxSetPixelsPerScanLine(uint32_t pixelsPerScanLine)
{
    g_pixelsPerScanLine = pixelsPerScanLine;
}

void GfxPlotPixel(int x, int y, uint32_t pixel)
{
    // TODO: panic if frameBufferStart or pixelsPerScanLine are not set but we
    // can't panic until we actually have graphics lol
    *((uint32_t*)(g_frameBufferStart + 4 * g_pixelsPerScanLine * y + 4 * x)) = pixel;
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
