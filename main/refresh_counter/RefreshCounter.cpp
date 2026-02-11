#include "RefreshCounter.h"
#include "esp_log.h"
#include "lgfx/v1/misc/enum.hpp"

static const char* TAG = "RefreshCounter";

RefreshCounter::RefreshCounter() 
    : _refreshCount(0), _fullRefreshThreshold(10), _initialized(false) {
}

void RefreshCounter::init(uint32_t fullRefreshThreshold) {
    _fullRefreshThreshold = fullRefreshThreshold;
    _refreshCount = 0;
    _initialized = true;
    ESP_LOGI(TAG, "RefreshCounter initialized with threshold: %d", fullRefreshThreshold);
}

RefreshCounter& RefreshCounter::getInstance() {
    static RefreshCounter instance;  // C++11标准保证线程安全
    return instance;
}

m5gfx::epd_mode_t RefreshCounter::refresh() {
    if (!_initialized) {
        // 如果未初始化，则使用默认值初始化
        init();
    }
    
    _refreshCount++;
    
    // 打印当前刷新计数和全刷阈值
    ESP_LOGI(TAG, "Refresh count: %d/%d", _refreshCount, _fullRefreshThreshold);
    
    // 检查是否达到全刷阈值
    // if (_refreshCount % _fullRefreshThreshold == 0) {
    //     // 执行全刷并重置计数器
    //     _refreshCount = 0;
    //     ESP_LOGI(TAG, "Full refresh triggered at count: %d", _fullRefreshThreshold);
    //     return m5gfx::epd_mode_t::epd_quality;  // 全刷模式
    // } else {
    //     // 执行快刷
    //     ESP_LOGI(TAG, "Fast refresh at count: %d", _refreshCount);
    //     return m5gfx::epd_mode_t::epd_fast;  // 快刷模式
    // }
    return m5gfx::epd_mode_t::epd_fast;
}

void RefreshCounter::reset() {
    _refreshCount = 0;
    ESP_LOGI(TAG, "Refresh counter reset");
}