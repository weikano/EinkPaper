#include "View.h"
#include "esp_log.h"

View::View(int16_t width, int16_t height)
    : _width(width), _height(height),
      _measuredWidth(width), _measuredHeight(height),
      _isDirty(true) {  // 新视图需要绘制
}

View::~View() {
}

void View::setPosition(int16_t left, int16_t top) {
    _left = left;
    _top = top;
}

void View::setSize(int16_t width, int16_t height) {
    _width = width;
    _height = height;
    _measuredWidth = width;
    _measuredHeight = height;
}

void View::setVisibility(Visibility visibility) {
    _visibility = visibility;
}



void View::setBorderColor(uint32_t color) {
    _borderColor = color;
}

void View::setBorderWidth(uint8_t width) {
    _borderWidth = width;
}

void View::setPadding(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom) {
    _paddingLeft = left;
    _paddingTop = top;
    _paddingRight = right;
    _paddingBottom = bottom;
    markDirty();  // 内边距改变需要重绘
}

bool View::contains(int16_t x, int16_t y) const {
    return x >= _left && x < (_left + _width) && y >= _top && y < (_top + _height);
}

void View::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }
    if(_isDirty) {
        ESP_LOGI(className().c_str(), "View.draw isDirty");
    }else {
        ESP_LOGI(className().c_str(), "View.draw notDirty");
    }    
    if (_visibility == VISIBLE && _isDirty) {
        onDraw(display);
        
        // 标记为已绘制，清除脏标记
        _isDirty = false;
        _lastDrawTime = esp_log_timestamp();
    }
}

void View::onDraw(m5gfx::M5GFX& display) {
    ESP_LOGV(className().c_str(), "onDraw called");
    // 绘制背景（考虑边框宽度）
    int borderWidthOffset = _borderWidth > 0 ? _borderWidth : 0;
    int drawX = _left + borderWidthOffset;
    int drawY = _top + borderWidthOffset;
    int drawWidth = _width - 2 * borderWidthOffset;
    int drawHeight = _height - 2 * borderWidthOffset;
    
    // 墨水屏背景始终为白色
    display.fillRect(drawX, drawY, drawWidth, drawHeight, TFT_WHITE);
    
    // 绘制边框
    if (_borderWidth > 0) {
        for (int i = 0; i < _borderWidth; i++) {
            display.drawRect(_left + i, _top + i, _width - 2 * i, _height - 2 * i, _borderColor);
        }
    }
}

void View::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 子类可以重写此方法以自定义测量逻辑
    // 默认情况下，使用设定的尺寸或父容器的限制
    onMeasure(widthMeasureSpec, heightMeasureSpec);    
}

void View::onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 子类可以重写此方法以自定义测量逻辑
    // 默认情况下，根据设定的尺寸和父容器的限制计算最终尺寸
    
    int16_t measuredWidth = 0;
    int16_t measuredHeight = 0;
    
    // 处理宽度测量
    if (_width == MATCH_PARENT) {
        // 如果是MATCH_PARENT，则使用父容器的宽度限制
        measuredWidth = MeasureSpec::getSize(widthMeasureSpec);
    } else if (_width == WRAP_CONTENT) {
        // 如果是WRAP_CONTENT，则根据内容决定，但不超过父容器限制
        int16_t desiredWidth = getDesiredWidth();
        int16_t specSize = MeasureSpec::getSize(widthMeasureSpec);
        MeasureSpecMode specMode = MeasureSpec::getMode(widthMeasureSpec);
        
        if (specMode == MeasureSpecMode::EXACTLY) {
            measuredWidth = specSize;
        } else if (specMode == MeasureSpecMode::AT_MOST) {
            measuredWidth = std::min(desiredWidth, specSize);
        } else {
            measuredWidth = desiredWidth;
        }
    } else {
        // 指定具体数值
        measuredWidth = _width;
    }
    
    // 处理高度测量
    if (_height == MATCH_PARENT) {
        // 如果是MATCH_PARENT，则使用父容器的高度限制
        measuredHeight = MeasureSpec::getSize(heightMeasureSpec);
    } else if (_height == WRAP_CONTENT) {
        // 如果是WRAP_CONTENT，则根据内容决定，但不超过父容器限制
        int16_t desiredHeight = getDesiredHeight();
        int16_t specSize = MeasureSpec::getSize(heightMeasureSpec);
        MeasureSpecMode specMode = MeasureSpec::getMode(heightMeasureSpec);
        
        if (specMode == MeasureSpecMode::EXACTLY) {
            measuredHeight = specSize;
        } else if (specMode == MeasureSpecMode::AT_MOST) {
            measuredHeight = std::min(desiredHeight, specSize);
        } else {
            measuredHeight = desiredHeight;
        }
    } else {
        // 指定具体数值
        measuredHeight = _height;
    }
    
    _measuredWidth = measuredWidth;
    _measuredHeight = measuredHeight;
}

int16_t View::getDesiredWidth() const {
    // 子类可以重写此方法以提供内容所需的实际宽度
    // 默认返回0，子类应该根据实际内容计算合适的宽度
    return 0;
}

int16_t View::getDesiredHeight() const {
    // 子类可以重写此方法以提供内容所需的实际高度
    // 默认返回0，子类应该根据实际内容计算合适的高度
    return 0;
}

void View::layout(int16_t left, int16_t top, int16_t right, int16_t bottom) {    
    onLayout(left, top, right, bottom);
}

void View::onLayout(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    // 子类可以重写此方法以实现自定义布局逻辑
    // 当前实现为空，保留给子类扩展
    _left = left;
    _top = top;
    _width = right - left;
    _height = bottom - top;
    ESP_LOGI(className().c_str(), "onLayout with left %d, top %d, right %d, bottom %d", left, top, right, bottom);
}

bool View::onTouch(int16_t x, int16_t y) {
    if (contains(x, y)) {
        _isPressed = true;
        if (_clickCallback) {
            _clickCallback();
        }
        return true;
    }
    return false;
}

bool View::onSwipe(TouchGestureDetector::SwipeDirection direction) {
    // 默认实现：不处理滑动事件
    return false;
}

void View::setOnClickListener(std::function<void()> callback) {
    _clickCallback = callback;
}

void View::markDirty() {
    _isDirty = true;
    // 通知父视图需要重绘
    if (_parent && _parent != this) {
        notifyParentOfChange();
    }
}

void View::forceRedraw() {
    _isDirty = true;    
}



void View::notifyParentOfChange() {
    // 通知父视图需要重绘
    // 确保_parent不为空且不等于自身以防止基本的循环引用
    if (_parent && _parent != this) {
        _parent->markDirty();
        _parent->notifyParentOfChange();  // 继续向上通知
    }
}