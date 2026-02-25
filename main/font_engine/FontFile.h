#pragma once

#include "FontTypes.h"
#include "GlyphIndex.h"

#include "esp_err.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

namespace font_engine {

class FontFile {
public:
    struct GlyphInfo {
        uint32_t codepoint = 0;
        uint32_t offset = 0;
        int16_t advanceX = 0;
        int16_t bearingX = 0;
        int16_t bearingY = 0;
    };

    FontFile() = default;
    ~FontFile();

    FontFile(const FontFile&) = delete;
    FontFile& operator=(const FontFile&) = delete;

    esp_err_t open(const std::string& path);
    esp_err_t openFromLittleFs(const std::string& fileName);
    esp_err_t openFromSdCard(const std::string& fileName);
    void close();

    bool isOpen() const { return _fp != nullptr; }
    const FontMetrics& metrics() const { return _metrics; }
    size_t glyphCount() const { return _glyphCount; }
    size_t glyphBitmapSize() const { return _glyphBitmapSize; }

    bool getGlyphInfo(uint32_t requestedCodepoint, GlyphInfo* outInfo) const;

    bool resolveGlyphCodepoint(uint32_t requestedCodepoint,
                               uint32_t* resolvedCodepoint,
                               uint32_t* offset) const;

    bool readGlyphBitmap(uint32_t codepoint,
                         uint8_t* out,
                         size_t outSize,
                         uint32_t* resolvedCodepoint = nullptr) const;

private:
    bool readGlyphBitmapByOffset(uint32_t offset, uint8_t* out, size_t outSize) const;
    static std::string buildLittleFsPath(const std::string& fileName);
    static std::string buildSdCardPath(const std::string& fileName);

    std::string _path;
    mutable FILE* _fp = nullptr;
    FontMetrics _metrics;
    GlyphIndex _index;
    size_t _glyphCount = 0;
    size_t _glyphBitmapSize = 0;
    uint32_t _bitmapStartOffset = 0;
    uint32_t _bitmapBytes = 0;
    uint8_t _indexEntrySize = 0;
};

}  // namespace font_engine
