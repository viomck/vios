#include <stdint.h>

static const uint32_t GFX_BLACK = 0xFF000000;
static const uint32_t GFX_WHITE = 0xFFFFFFFF;

// boot initializer methods
void GfxSetFrameBufferStart(uint64_t frameBufferStart);
void GfxSetPixelsPerScanLine(uint32_t pixelsPerScanLine);
void GfxSetScreenWidthAndHeight(uint16_t width, uint16_t height);

// kernel init method
void GfxInit();

// plot methods
void GfxPlot(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void GfxPlotPixel(int x, int y, uint32_t pixel);
void GfxFullScreenGradientToColor(uint8_t r, uint8_t g, uint8_t b);
void GfxScrollUp(uint16_t px);

uint16_t GfxGetScreenWidth();
uint16_t GfxGetScreenHeight();
