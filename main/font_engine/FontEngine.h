#pragma once

#include "FontFile.h"
#include "GlyphCache.h"
#include "TextLayouter.h"
#include "M5Unified.h"

#include "esp_err.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace font_engine {

class FontEngine {
public:
    FontEngine() = default;

    esp_err_t loadDefaultFontFromLittleFs(const std::string& fontName,
                                          size_t cacheGlyphCount = 256);

    esp_err_t loadFontFromSdCard(const std::string& fontPath,
                                 size_t cacheGlyphCount = 256);

    void unload();

    bool isReady() const { return _fontFile.isOpen(); }
    const FontMetrics& metrics() const { return _fontFile.metrics(); }
    bool getGlyphInfo(uint32_t codepoint, FontFile::GlyphInfo* outInfo) const {
        return _fontFile.getGlyphInfo(codepoint, outInfo);
    }

    const uint8_t* getGlyphBitmap(uint32_t codepoint);

    TextLayout layoutText(const std::string& utf8,
                          int16_t maxWidth,
                          int16_t lineSpacing = 0) const;

    bool drawText(lgfx::LovyanGFX& display,
                  const std::string& utf8,
                  int16_t x,
                  int16_t y,
                  int16_t maxWidth,
                  uint32_t fgColor,
                  uint32_t bgColor,
                  bool opaqueBg = false,
                  int16_t lineSpacing = 0);

private:
    esp_err_t loadFromPath(bool useLittleFs,
                           const std::string& path,
                           size_t cacheGlyphCount);

    FontFile _fontFile;
    GlyphCache _glyphCache;
    TextLayouter _layouter;
};

}  // namespace font_engine
