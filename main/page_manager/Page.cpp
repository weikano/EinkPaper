#include "Page.h"
#include "PageManager.h"
#include "esp_log.h"

static const char* TAG = "Page";

Page::Page(PageType pageType, const std::string& pageName) 
    : _pageType(pageType), _pageName(pageName) {
    ESP_LOGI(TAG, "Creating page: %s (type: %d)", _pageName.c_str(), static_cast<int>(_pageType));
}

Page::~Page() {
    ESP_LOGI(TAG, "Destroyed page: %s (type: %d)", _pageName.c_str(), static_cast<int>(_pageType));
    
    // 释放根视图及其所有子视图
    if (_rootView != nullptr) {
        delete _rootView;
        _rootView = nullptr;
    }
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

bool Page::onClick(int16_t x, int16_t y) {
    if (_rootView != nullptr) {
        return _rootView->onTouch(x, y);  // View类仍使用onTouch，因为它是通用的触摸处理
    }
    return false;
}

void Page::onSwipe(TouchGestureDetector::SwipeDirection direction) {
    // 将滑动事件传递给根视图
    if (_rootView && _rootView->onSwipe(direction)) {
        return;
    }    
    onSwipeDispatched(direction);
}

void Page::onSwipeDispatched(TouchGestureDetector::SwipeDirection direction) {
    if(direction == TouchGestureDetector::SwipeDirection::RIGHT) {
        ESP_LOGD(TAG, "Page %s (type: %d) onSwipeDispatched: %d", _pageName.c_str(), static_cast<int>(_pageType), static_cast<int>(direction));
        PageManager::getInstance().finishActivity();
    }
}