#include "FontEngine.h"

#include "Renderer.h"

namespace font_engine {

esp_err_t FontEngine::loadDefaultFontFromLittleFs(const std::string& fontName,
                                                  size_t cacheGlyphCount) {
    return loadFromPath(true, fontName, cacheGlyphCount);
}

esp_err_t FontEngine::loadFontFromSdCard(const std::string& fontPath,
                                         size_t cacheGlyphCount) {
    return loadFromPath(false, fontPath, cacheGlyphCount);
}

void FontEngine::unload() {
    _glyphCache.clear();
    _fontFile.close();
}

const uint8_t* FontEngine::getGlyphBitmap(uint32_t codepoint) {
    if (!_fontFile.isOpen()) {
        return nullptr;
    }

    uint32_t resolvedCodepoint = 0;
    if (!_fontFile.resolveGlyphCodepoint(codepoint, &resolvedCodepoint, nullptr)) {
        return nullptr;
    }

    return _glyphCache.get(
        resolvedCodepoint,
        [this](uint32_t cp, uint8_t* out, size_t outSize) {
            return _fontFile.readGlyphBitmap(cp, out, outSize, nullptr);
        });
}

TextLayout FontEngine::layoutText(const std::string& utf8,
                                  int16_t maxWidth,
                                  int16_t lineSpacing) const {
    const FontMetrics metrics = _fontFile.metrics();
    return _layouter.layoutUtf8(
        utf8,
        metrics,
        maxWidth,
        [this, metrics](uint32_t codepoint) -> int16_t {
            FontFile::GlyphInfo info;
            if (_fontFile.getGlyphInfo(codepoint, &info) && info.advanceX > 0) {
                return info.advanceX;
            }
            return static_cast<int16_t>(metrics.glyphWidth);
        },
        lineSpacing);
}

bool FontEngine::drawText(lgfx::LovyanGFX& display,
                          const std::string& utf8,
                          int16_t x,
                          int16_t y,
                          int16_t maxWidth,
                          uint32_t fgColor,
                          uint32_t bgColor,
                          bool opaqueBg,
                          int16_t lineSpacing) {
    if (!_fontFile.isOpen()) {
        ESP_LOGW("FontEngine", "No font loaded, cannot draw text");
        return false;
    }

    const TextLayout layout = layoutText(utf8, maxWidth, lineSpacing);
    return Renderer::drawLayout(display, *this, layout, x, y, fgColor, bgColor, opaqueBg);
}

esp_err_t FontEngine::loadFromPath(bool useLittleFs,
                                   const std::string& path,
                                   size_t cacheGlyphCount) {
    unload();

    esp_err_t ret = ESP_FAIL;
    if (useLittleFs) {
        ret = _fontFile.openFromLittleFs(path);
    } else {
        ret = _fontFile.openFromSdCard(path);
    }

    if (ret != ESP_OK) {
        return ret;
    }

    ret = _glyphCache.init(cacheGlyphCount, _fontFile.glyphBitmapSize());
    if (ret != ESP_OK) {
        _fontFile.close();
        return ret;
    }

    return ESP_OK;
}

}  // namespace font_engine
