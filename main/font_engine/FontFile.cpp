#include "FontFile.h"

#include "esp_log.h"

#include <algorithm>
#include <cstring>

namespace font_engine {

namespace {
static const char* TAG = "FontFile";
constexpr uint32_t kCommonHeaderBytes = 8;  // magic + glyph count
constexpr uint8_t kMagicSize = 4;
constexpr uint8_t kMetricsBytesV1 = 5;      // width + height + bpp
constexpr uint8_t kMetricsBytesV2 = 6;      // width + height + bpp + entry_size
constexpr uint8_t kIndexEntryBytesV1 = 8;
constexpr uint8_t kIndexEntryBytesV2 = 16;

uint16_t readU16LE(const uint8_t* bytes) {
    return static_cast<uint16_t>(bytes[0]) |
           (static_cast<uint16_t>(bytes[1]) << 8);
}

uint32_t readU32LE(const uint8_t* bytes) {
    return static_cast<uint32_t>(bytes[0]) |
           (static_cast<uint32_t>(bytes[1]) << 8) |
           (static_cast<uint32_t>(bytes[2]) << 16) |
           (static_cast<uint32_t>(bytes[3]) << 24);
}

bool isAbsolutePath(const std::string& path) {
    return !path.empty() && path[0] == '/';
}
}  // namespace

FontFile::~FontFile() { close(); }

esp_err_t FontFile::open(const std::string& path) {
    close();

    _fp = fopen(path.c_str(), "rb");
    if (_fp == nullptr) {
        ESP_LOGE(TAG, "Open font failed: %s", path.c_str());
        return ESP_ERR_NOT_FOUND;
    }

    uint8_t commonHeader[kCommonHeaderBytes] = {0};
    if (fread(commonHeader, 1, sizeof(commonHeader), _fp) != sizeof(commonHeader)) {
        close();
        return ESP_FAIL;
    }

    bool isV2 = false;
    if (memcmp(commonHeader, "CJK1", kMagicSize) == 0) {
        isV2 = false;
    } else if (memcmp(commonHeader, "CJK2", kMagicSize) == 0) {
        isV2 = true;
    } else {
        ESP_LOGE(TAG, "Invalid font magic: %s", path.c_str());
        close();
        return ESP_ERR_INVALID_RESPONSE;
    }

    _glyphCount = readU32LE(&commonHeader[4]);

    uint8_t metricsBuffer[kMetricsBytesV2] = {0};
    const size_t metricsBytesToRead = isV2 ? kMetricsBytesV2 : kMetricsBytesV1;
    if (fread(metricsBuffer, 1, metricsBytesToRead, _fp) != metricsBytesToRead) {
        close();
        return ESP_FAIL;
    }

    _metrics.glyphWidth = readU16LE(&metricsBuffer[0]);
    _metrics.glyphHeight = readU16LE(&metricsBuffer[2]);
    _metrics.bpp = metricsBuffer[4];
    _indexEntrySize = isV2 ? metricsBuffer[5] : kIndexEntryBytesV1;

    if (_metrics.bpp != 1 || _metrics.glyphWidth == 0 || _metrics.glyphHeight == 0 ||
        _glyphCount == 0 || _indexEntrySize < kIndexEntryBytesV1 ||
        (_indexEntrySize > kIndexEntryBytesV1 && _indexEntrySize < kIndexEntryBytesV2)) {
        ESP_LOGE(TAG, "Unsupported font format: %s", path.c_str());
        close();
        return ESP_ERR_NOT_SUPPORTED;
    }

    _metrics.pitchBytes = static_cast<uint16_t>((_metrics.glyphWidth + 7) / 8);
    _metrics.lineHeight = _metrics.glyphHeight;
    _metrics.baseline = _metrics.glyphHeight;
    _glyphBitmapSize = static_cast<size_t>(_metrics.pitchBytes) * _metrics.glyphHeight;

    const uint32_t headerBytes = kCommonHeaderBytes + metricsBytesToRead;
    _bitmapStartOffset = headerBytes + static_cast<uint32_t>(_glyphCount) * _indexEntrySize;

    if (fseek(_fp, 0, SEEK_END) != 0) {
        close();
        return ESP_FAIL;
    }
    const long totalBytes = ftell(_fp);
    if (totalBytes < 0 || static_cast<uint32_t>(totalBytes) < _bitmapStartOffset) {
        close();
        return ESP_FAIL;
    }

    _bitmapBytes = static_cast<uint32_t>(totalBytes) - _bitmapStartOffset;

    if (fseek(_fp, headerBytes, SEEK_SET) != 0) {
        close();
        return ESP_FAIL;
    }

    const esp_err_t indexErr = _index.loadFromFile(_fp, _glyphCount, _indexEntrySize);
    if (indexErr != ESP_OK) {
        close();
        return indexErr;
    }

    _path = path;
    ESP_LOGI(TAG,
             "Loaded font=%s glyphs=%u size=%ux%u bpp=%u index_entry=%u",
             path.c_str(),
             static_cast<unsigned>(_glyphCount),
             static_cast<unsigned>(_metrics.glyphWidth),
             static_cast<unsigned>(_metrics.glyphHeight),
             static_cast<unsigned>(_metrics.bpp),
             static_cast<unsigned>(_indexEntrySize));

    return ESP_OK;
}

esp_err_t FontFile::openFromLittleFs(const std::string& fileName) {
    return open(buildLittleFsPath(fileName));
}

esp_err_t FontFile::openFromSdCard(const std::string& fileName) {
    return open(buildSdCardPath(fileName));
}

void FontFile::close() {
    if (_fp != nullptr) {
        fclose(_fp);
        _fp = nullptr;
    }

    _path.clear();
    _index.clear();
    _glyphCount = 0;
    _glyphBitmapSize = 0;
    _bitmapStartOffset = 0;
    _bitmapBytes = 0;
    _indexEntrySize = 0;
    _metrics = {};
}

bool FontFile::getGlyphInfo(uint32_t requestedCodepoint, GlyphInfo* outInfo) const {
    if (!isOpen()) {
        return false;
    }

    GlyphIndex::Record record;
    if (!_index.findRecord(requestedCodepoint, &record)) {
        return false;
    }

    if (outInfo) {
        outInfo->codepoint = record.codepoint;
        outInfo->offset = record.offset;
        outInfo->advanceX =
            (record.advanceX > 0) ? record.advanceX : static_cast<int16_t>(_metrics.glyphWidth);
        outInfo->bearingX = record.bearingX;
        outInfo->bearingY = record.bearingY;
    }

    return true;
}

bool FontFile::resolveGlyphCodepoint(uint32_t requestedCodepoint,
                                     uint32_t* resolvedCodepoint,
                                     uint32_t* offset) const {
    GlyphInfo info;
    if (!getGlyphInfo(requestedCodepoint, &info)) {
        return false;
    }

    if (resolvedCodepoint) {
        *resolvedCodepoint = info.codepoint;
    }
    if (offset) {
        *offset = info.offset;
    }
    return true;
}

bool FontFile::readGlyphBitmap(uint32_t codepoint,
                               uint8_t* out,
                               size_t outSize,
                               uint32_t* resolvedCodepoint) const {
    GlyphInfo info;
    if (!getGlyphInfo(codepoint, &info)) {
        return false;
    }

    if (resolvedCodepoint) {
        *resolvedCodepoint = info.codepoint;
    }

    return readGlyphBitmapByOffset(info.offset, out, outSize);
}

bool FontFile::readGlyphBitmapByOffset(uint32_t offset, uint8_t* out, size_t outSize) const {
    if (!isOpen() || out == nullptr || outSize < _glyphBitmapSize) {
        return false;
    }

    if (offset > _bitmapBytes || static_cast<size_t>(_bitmapBytes - offset) < _glyphBitmapSize) {
        return false;
    }

    const long absoluteOffset = static_cast<long>(_bitmapStartOffset + offset);
    if (fseek(_fp, absoluteOffset, SEEK_SET) != 0) {
        return false;
    }

    return fread(out, 1, _glyphBitmapSize, _fp) == _glyphBitmapSize;
}

std::string FontFile::buildLittleFsPath(const std::string& fileName) {
    if (isAbsolutePath(fileName)) {
        return fileName;
    }
    return std::string("/littlefs/") + fileName;
}

std::string FontFile::buildSdCardPath(const std::string& fileName) {
    if (isAbsolutePath(fileName)) {
        return fileName;
    }
    return std::string("/sdcard/") + fileName;
}

}  // namespace font_engine
