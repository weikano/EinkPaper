#include "FontEngineService.h"

#include "esp_log.h"

namespace font_engine {

namespace {
static const char* TAG = "FontEngineSvc";
static constexpr const char* kDefaultFontPath = "fonts/cjk_20px_SemiBold_1bpp.font";
}

FontEngineService& FontEngineService::getInstance() {
    static FontEngineService instance;
    return instance;
}

esp_err_t FontEngineService::initDefaultFont(size_t cacheGlyphCount) {
    const esp_err_t ret = _engine.loadDefaultFontFromLittleFs(kDefaultFontPath, cacheGlyphCount);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Load default font failed: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Default font loaded: %s", kDefaultFontPath);
    return ESP_OK;
}

}  // namespace font_engine
