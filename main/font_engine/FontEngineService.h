#pragma once

#include "FontEngine.h"

#include "esp_err.h"

namespace font_engine {

class FontEngineService {
public:
    static FontEngineService& getInstance();

    esp_err_t initDefaultFont(size_t cacheGlyphCount = 512);

    bool isReady() const { return _engine.isReady(); }
    FontEngine& engine() { return _engine; }
    const FontEngine& engine() const { return _engine; }

private:
    FontEngineService() = default;
    FontEngine _engine;
};

}  // namespace font_engine
