#include <stdint.h>

void FontRenderTestGlyphs();
void FontRenderStr(const char * string, int scale, int xOffset, int yOffset);
void FontRenderChar(char c, int scale, int xOffset, int yOffset);
void FontRenderGlyph(uint8_t glyph, int scale, int xOffset, int yOffset);
