#include "GlyphCache.h"

#include "esp_heap_caps.h"
#include "esp_log.h"

#include <cstdlib>

namespace font_engine {

namespace {
static const char* TAG = "GlyphCache";
}

GlyphCache::~GlyphCache() { clear(); }

esp_err_t GlyphCache::init(size_t capacity, size_t glyphBitmapSize) {
    clear();

    if (capacity == 0 || glyphBitmapSize == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    _capacity = capacity;
    _glyphBitmapSize = glyphBitmapSize;
    return ESP_OK;
}

void GlyphCache::clear() {
    for (auto& entry : _lru) {
        if (entry.bitmap) {
            free(entry.bitmap);
            entry.bitmap = nullptr;
        }
    }

    _lru.clear();
    _map.clear();
    _capacity = 0;
    _glyphBitmapSize = 0;
}

const uint8_t* GlyphCache::get(uint32_t codepoint, const Loader& loader) {
    if (_capacity == 0 || _glyphBitmapSize == 0 || !loader) {
        return nullptr;
    }

    auto found = _map.find(codepoint);
    if (found != _map.end()) {
        _lru.splice(_lru.begin(), _lru, found->second);
        return found->second->bitmap;
    }

    std::list<Entry>::iterator target;
    if (_lru.size() < _capacity) {
        Entry entry;
        entry.bitmap = allocGlyphBuffer();
        if (!entry.bitmap) {
            return nullptr;
        }

        _lru.push_front(entry);
        target = _lru.begin();
    } else {
        target = std::prev(_lru.end());
        _map.erase(target->codepoint);
        _lru.splice(_lru.begin(), _lru, target);
        target = _lru.begin();
    }

    if (!loader(codepoint, target->bitmap, _glyphBitmapSize)) {
        ESP_LOGW(TAG, "glyph load failed codepoint=U+%04X", static_cast<unsigned>(codepoint));
        free(target->bitmap);
        target->bitmap = nullptr;
        _lru.erase(target);
        return nullptr;
    }

    target->codepoint = codepoint;
    _map[codepoint] = target;
    return target->bitmap;
}

uint8_t* GlyphCache::allocGlyphBuffer() const {
    auto* ptr = static_cast<uint8_t*>(
        heap_caps_malloc(_glyphBitmapSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (ptr == nullptr) {
        ESP_LOGW(TAG, "PSRAM alloc failed for glyph cache, fallback to internal RAM");
        ptr = static_cast<uint8_t*>(malloc(_glyphBitmapSize));
    }
    return ptr;
}

}  // namespace font_engine
