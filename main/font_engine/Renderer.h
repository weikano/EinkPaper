#pragma once

#include "FontTypes.h"
#include "M5Unified.h"

#include <cstdint>

namespace font_engine {

class FontEngine;

class Renderer {
public:
    static void drawGlyph1bpp(lgfx::LovyanGFX& display,
                              int16_t x,
                              int16_t y,
                              const uint8_t* bitmap,
                              const FontMetrics& metrics,
                              uint32_t fgColor,
                              uint32_t bgColor,
                              bool opaqueBg);

    static bool drawLayout(lgfx::LovyanGFX& display,
                           FontEngine& engine,
                           const TextLayout& layout,
                           int16_t originX,
                           int16_t originY,
                           uint32_t fgColor,
                           uint32_t bgColor,
                           bool opaqueBg);
};

}  // namespace font_engine
