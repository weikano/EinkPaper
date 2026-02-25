#include "GlyphIndex.h"

#include "esp_heap_caps.h"
#include "esp_log.h"

#include <algorithm>
#include <cstdlib>

namespace font_engine {

namespace {
static const char* TAG = "GlyphIndex";
constexpr uint8_t kEntrySizeV1 = 8;
constexpr uint8_t kEntrySizeV2 = 16;
}

GlyphIndex::~GlyphIndex() { clear(); }

esp_err_t GlyphIndex::loadFromFile(FILE* fp, size_t count, uint8_t entrySize) {
    clear();

    if (!fp || count == 0 || entrySize < kEntrySizeV1 ||
        (entrySize > kEntrySizeV1 && entrySize < kEntrySizeV2)) {
        return ESP_ERR_INVALID_ARG;
    }

    const size_t bytes = count * sizeof(Record);
    _records = static_cast<Record*>(
        heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (_records == nullptr) {
        ESP_LOGW(TAG, "PSRAM alloc failed for index, fallback to internal RAM");
        _records = static_cast<Record*>(malloc(bytes));
    }
    if (_records == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    if (entrySize == kEntrySizeV1) {
        for (size_t i = 0; i < count; ++i) {
            uint8_t raw[kEntrySizeV1] = {0};
            if (fread(raw, 1, sizeof(raw), fp) != sizeof(raw)) {
                clear();
                return ESP_FAIL;
            }
            _records[i].codepoint = static_cast<uint32_t>(raw[0]) |
                                    (static_cast<uint32_t>(raw[1]) << 8) |
                                    (static_cast<uint32_t>(raw[2]) << 16) |
                                    (static_cast<uint32_t>(raw[3]) << 24);
            _records[i].offset = static_cast<uint32_t>(raw[4]) |
                                 (static_cast<uint32_t>(raw[5]) << 8) |
                                 (static_cast<uint32_t>(raw[6]) << 16) |
                                 (static_cast<uint32_t>(raw[7]) << 24);
            _records[i].advanceX = 0;
            _records[i].bearingX = 0;
            _records[i].bearingY = 0;
        }
    } else {
        uint8_t raw[kEntrySizeV2] = {0};
        for (size_t i = 0; i < count; ++i) {
            if (fread(raw, 1, sizeof(raw), fp) != sizeof(raw)) {
                clear();
                return ESP_FAIL;
            }
            if (entrySize > sizeof(raw)) {
                if (fseek(fp, static_cast<long>(entrySize - sizeof(raw)), SEEK_CUR) != 0) {
                    clear();
                    return ESP_FAIL;
                }
            }
            _records[i].codepoint = static_cast<uint32_t>(raw[0]) |
                                    (static_cast<uint32_t>(raw[1]) << 8) |
                                    (static_cast<uint32_t>(raw[2]) << 16) |
                                    (static_cast<uint32_t>(raw[3]) << 24);
            _records[i].offset = static_cast<uint32_t>(raw[4]) |
                                 (static_cast<uint32_t>(raw[5]) << 8) |
                                 (static_cast<uint32_t>(raw[6]) << 16) |
                                 (static_cast<uint32_t>(raw[7]) << 24);
            _records[i].advanceX =
                static_cast<int16_t>(static_cast<uint16_t>(raw[8]) |
                                     (static_cast<uint16_t>(raw[9]) << 8));
            _records[i].bearingX =
                static_cast<int16_t>(static_cast<uint16_t>(raw[10]) |
                                     (static_cast<uint16_t>(raw[11]) << 8));
            _records[i].bearingY =
                static_cast<int16_t>(static_cast<uint16_t>(raw[12]) |
                                     (static_cast<uint16_t>(raw[13]) << 8));
        }
    }

    for (size_t i = 1; i < count; ++i) {
        if (_records[i - 1].codepoint > _records[i].codepoint) {
            ESP_LOGE(TAG, "Glyph index is not sorted at %u: U+%04X > U+%04X",
                     static_cast<unsigned>(i),
                     static_cast<unsigned>(_records[i - 1].codepoint),
                     static_cast<unsigned>(_records[i].codepoint));
            clear();
            return ESP_ERR_INVALID_RESPONSE;
        }
    }

    _count = count;
    return ESP_OK;
}

void GlyphIndex::clear() {
    if (_records != nullptr) {
        free(_records);
        _records = nullptr;
    }
    _count = 0;
}

bool GlyphIndex::findOffset(uint32_t codepoint, uint32_t* offset) const {
    Record record;
    if (!findRecord(codepoint, &record)) {
        return false;
    }
    if (offset) {
        *offset = record.offset;
    }
    return true;
}

bool GlyphIndex::findRecord(uint32_t codepoint, Record* outRecord) const {
    if (_records == nullptr || _count == 0) {
        return false;
    }

    size_t left = 0;
    size_t right = _count;

    while (left < right) {
        const size_t mid = left + (right - left) / 2;
        const uint32_t value = _records[mid].codepoint;
        if (value == codepoint) {
            if (outRecord) {
                *outRecord = _records[mid];
            }
            return true;
        }
        if (value < codepoint) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return false;
}

}  // namespace font_engine
