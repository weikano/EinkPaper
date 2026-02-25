#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace font_engine {

struct FontMetrics {
    uint16_t glyphWidth = 0;
    uint16_t glyphHeight = 0;
    uint16_t pitchBytes = 0;
    uint8_t bpp = 0;
    uint16_t lineHeight = 0;
    uint16_t baseline = 0;
};

struct GlyphPos {
    uint32_t codepoint = 0;
    int16_t x = 0;
    int16_t y = 0;
};

struct TextLayout {
    std::vector<GlyphPos> glyphs;
    int16_t width = 0;
    int16_t height = 0;
    uint16_t lineHeight = 0;
    uint16_t baseline = 0;
    uint16_t lineCount = 0;
};

}  // namespace font_engine
