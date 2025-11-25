#include <stdint.h>

static const uint32_t GFX_BLACK = 0xFF000000;
static const uint32_t GFX_WHITE = 0xFFFFFFFF;

void GfxSetFrameBufferStart(uint64_t frameBufferStart);
void GfxSetPixelsPerScanLine(uint32_t pixelsPerScanLine);
void GfxPlot(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void GfxPlotPixel(int x, int y, uint32_t pixel);
