#include "Page.h"
#include "esp_log.h"

static const char* TAG = "Page";

Page::Page(PageType pageType, const std::string& pageName) 
    : _pageType(pageType), _pageName(pageName) {
    ESP_LOGI(TAG, "Creating page: %s (type: %d)", _pageName.c_str(), static_cast<int>(_pageType));
}

Page::~Page() {
    ESP_LOGI(TAG, "Destroyed page: %s (type: %d)", _pageName.c_str(), static_cast<int>(_pageType));
}

void Page::onCreate() {
    ESP_LOGD(TAG, "Page %s (type: %d) onCreate", _pageName.c_str(), static_cast<int>(_pageType));
}

void Page::onStart() {
    ESP_LOGD(TAG, "Page %s (type: %d) onStart", _pageName.c_str(), static_cast<int>(_pageType));
}

void Page::onResume() {
    ESP_LOGD(TAG, "Page %s (type: %d) onResume", _pageName.c_str(), static_cast<int>(_pageType));
    // 当页面恢复时，标记为需要重绘
    if (_rootView != nullptr) {
        _rootView->markDirty();
    }
}

void Page::onPause() {
    ESP_LOGD(TAG, "Page %s (type: %d) onPause", _pageName.c_str(), static_cast<int>(_pageType));
}

void Page::onStop() {
    ESP_LOGD(TAG, "Page %s (type: %d) onStop", _pageName.c_str(), static_cast<int>(_pageType));
}

void Page::onRestart() {
    ESP_LOGD(TAG, "Page %s (type: %d) onRestart", _pageName.c_str(), static_cast<int>(_pageType));
}

void Page::onDestroy() {
    ESP_LOGD(TAG, "Page %s (type: %d) onDestroy", _pageName.c_str(), static_cast<int>(_pageType));
}

bool Page::isDirty() const {
    if (_rootView != nullptr) {
        return _rootView->isDirty();
    }
    return false;
}

void Page::draw(m5gfx::M5GFX& display) {
    if (_rootView != nullptr) {
        _rootView->draw(display);
    }
}

bool Page::onTouch(int16_t x, int16_t y) {
    if (_rootView != nullptr) {
        return _rootView->onTouch(x, y);
    }
    return false;
}