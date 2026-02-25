#pragma once

#include "esp_err.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace font_engine {

class GlyphIndex {
public:
    /**
     * @brief 字形索引记录结构体 - 存储每个字形的信息
     * @param codepoint 字形的 Unicode 码点
     * @param offset 字形在字体文件中的偏移量
     * @param advanceX 字形的水平Advance宽度。cjk等宽，非cjk需要
     * @param bearingX 字形的水平 bearings 宽度。cjk等宽，非cjk需要
     * @param bearingY 字形的垂直 bearings 高度。cjk等宽，非cjk需要
     */
    struct Record {
        uint32_t codepoint = 0;
        uint32_t offset = 0;        
        int16_t advanceX = 0;
        int16_t bearingX = 0;
        int16_t bearingY = 0;
    };

    GlyphIndex() = default;
    ~GlyphIndex();

    GlyphIndex(const GlyphIndex&) = delete;
    GlyphIndex& operator=(const GlyphIndex&) = delete;

    esp_err_t loadFromFile(FILE* fp, size_t count, uint8_t entrySize);
    void clear();

    bool findOffset(uint32_t codepoint, uint32_t* offset) const;
    bool findRecord(uint32_t codepoint, Record* outRecord) const;
    size_t size() const { return _count; }

private:
    Record* _records = nullptr;
    size_t _count = 0;
};

}  // namespace font_engine
