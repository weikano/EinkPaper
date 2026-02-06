#include "TouchGestureDetector.h"
#include <cmath>
#include "esp_timer.h"

TouchGestureDetector::TouchGestureDetector()
    : _isTouching(false)
    , _touchStartTime(0)
    , _startX(0)
    , _startY(0)
    , _endX(0)
    , _endY(0) {
}

TouchGestureDetector::SwipeDirection TouchGestureDetector::updateTouch(const m5::touch_detail_t& touch) {
    if (touch.wasPressed()) {
        // 触摸按下
        _isTouching = true;
        _touchStartTime = esp_timer_get_time() / 1000;  // Convert to milliseconds
        _startX = touch.x;
        _startY = touch.y;
        _endX = touch.x;
        _endY = touch.y;
        
        ESP_LOGD("TouchGesture", "Touch pressed at (%d, %d)", _startX, _startY);
    } 
    else if (touch.wasReleased()) {
        // 触摸释放
        if (_isTouching) {
            _endX = touch.x;
            _endY = touch.y;
            
            uint32_t duration = (esp_timer_get_time() / 1000) - _touchStartTime;
            
            ESP_LOGD("TouchGesture", "Touch released at (%d, %d), duration: %lu ms", 
                     _endX, _endY, duration);
            
            if (isValidSwipe() && duration <= MAX_SWIPE_TIME) {
                // 计算滑动距离
                int16_t deltaX = _endX - _startX;
                int16_t deltaY = _endY - _startY;
                
                ESP_LOGD("TouchGesture", "Delta: dx=%d, dy=%d", deltaX, deltaY);
                
                // 确定主导方向
                if (abs(deltaX) > abs(deltaY)) {
                    // 水平滑动
                    if (deltaX > 0) {
                        ESP_LOGD("TouchGesture", "Swipe detected: RIGHT");
                        return SwipeDirection::RIGHT;
                    } else {
                        ESP_LOGD("TouchGesture", "Swipe detected: LEFT");
                        return SwipeDirection::LEFT;
                    }
                } else {
                    // 垂直滑动
                    if (deltaY > 0) {
                        ESP_LOGD("TouchGesture", "Swipe detected: DOWN");
                        return SwipeDirection::DOWN;
                    } else {
                        ESP_LOGD("TouchGesture", "Swipe detected: UP");
                        return SwipeDirection::UP;
                    }
                }
            }
        }
        _isTouching = false;
    } 
    else if (_isTouching && touch.isDragging()) {
        // 触摸移动中
        _endX = touch.x;
        _endY = touch.y;
    }
    
    return SwipeDirection::NONE;
}

bool TouchGestureDetector::isValidSwipe() const {
    if (!_isTouching) {
        return false;
    }
    
    // 计算总滑动距离
    float distance = calculateDistance(_startX, _startY, _endX, _endY);
    
    ESP_LOGD("TouchGesture", "Swipe distance: %.2f, min required: %d", 
             distance, MIN_SWIPE_DISTANCE);
    
    return distance >= MIN_SWIPE_DISTANCE;
}

float TouchGestureDetector::calculateDistance(int16_t x1, int16_t y1, int16_t x2, int16_t y2) const {
    int16_t dx = x2 - x1;
    int16_t dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

void TouchGestureDetector::reset() {
    _isTouching = false;
    _touchStartTime = 0;
    _startX = 0;
    _startY = 0;
    _endX = 0;
    _endY = 0;
}