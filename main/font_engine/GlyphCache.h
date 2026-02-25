#pragma once

#include "esp_err.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <unordered_map>

namespace font_engine {

class GlyphCache {
public:
    using Loader = std::function<bool(uint32_t codepoint, uint8_t* out, size_t outSize)>;

    GlyphCache() = default;
    ~GlyphCache();

    GlyphCache(const GlyphCache&) = delete;
    GlyphCache& operator=(const GlyphCache&) = delete;

    esp_err_t init(size_t capacity, size_t glyphBitmapSize);
    void clear();

    const uint8_t* get(uint32_t codepoint, const Loader& loader);

    size_t capacity() const { return _capacity; }
    size_t size() const { return _map.size(); }

private:
    struct Entry {
        uint32_t codepoint = 0;
        uint8_t* bitmap = nullptr;
    };

    uint8_t* allocGlyphBuffer() const;

    size_t _capacity = 0;
    size_t _glyphBitmapSize = 0;
    std::list<Entry> _lru;  // front = MRU, back = LRU
    std::unordered_map<uint32_t, std::list<Entry>::iterator> _map;
};

}  // namespace font_engine
