#include "Renderer.h"

#include "FontEngine.h"
#include "M5Unified.h"

namespace font_engine {

void Renderer::drawGlyph1bpp(lgfx::LovyanGFX& display,
                             int16_t x,
                             int16_t y,
                             const uint8_t* bitmap,
                             const FontMetrics& metrics,
                             uint32_t fgColor,
                             uint32_t bgColor,
                             bool opaqueBg) {
    if (bitmap == nullptr || metrics.bpp != 1) {
        return;
    }

    const uint16_t pitch = metrics.pitchBytes;
    const uint16_t width = metrics.glyphWidth;
    const uint16_t height = metrics.glyphHeight;

    for (uint16_t row = 0; row < height; ++row) {
        const uint8_t* rowPtr = bitmap + static_cast<size_t>(row) * pitch;
        for (uint16_t col = 0; col < width; ++col) {
            const uint8_t bit = (rowPtr[col >> 3] >> (7 - (col & 7))) & 0x1u;
            if (bit) {
                display.drawPixel(x + col, y + row, fgColor);
            } else if (opaqueBg) {
                display.drawPixel(x + col, y + row, bgColor);
            }
        }
    }
}

bool Renderer::drawLayout(lgfx::LovyanGFX& display,
                          FontEngine& engine,
                          const TextLayout& layout,
                          int16_t originX,
                          int16_t originY,
                          uint32_t fgColor,
                          uint32_t bgColor,
                          bool opaqueBg) {
    const FontMetrics& metrics = engine.metrics();
    for (const auto& glyph : layout.glyphs) {
        const uint8_t* bitmap = engine.getGlyphBitmap(glyph.codepoint);
        if (bitmap == nullptr) {
            continue;
        }
        drawGlyph1bpp(display,
                      originX + glyph.x,
                      originY + glyph.y,
                      bitmap,
                      metrics,
                      fgColor,
                      bgColor,
                      opaqueBg);
    }
    return true;
}

}  // namespace font_engine
