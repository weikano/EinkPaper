#include "View.h"

View::View(int16_t width, int16_t height)
    : _x(0), _y(0), _width(width), _height(height),
      _visibility(VISIBLE), _backgroundColor(TFT_WHITE),
      _borderColor(TFT_BLACK), _borderWidth(1), 
      _paddingLeft(0), _paddingTop(0), _paddingRight(0), _paddingBottom(0),
      _isPressed(false), _clickCallback(nullptr),
      _isDirty(true), _lastDrawTime(0), _parent(nullptr), _isInitialized(false) {
    // 构造完成后再设置为true，避免在构造期间调用markDirty时触发父视图更新
    _isInitialized = true;
}

View::~View() {
}

void View::setPosition(int16_t x, int16_t y) {
    _x = x;
    _y = y;
}

void View::setSize(int16_t width, int16_t height) {
    _width = width;
    _height = height;
}

void View::setVisibility(Visibility visibility) {
    _visibility = visibility;
}

void View::setBackgroundColor(uint32_t color) {
    _backgroundColor = color;
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
    return x >= _x && x < (_x + _width) && y >= _y && y < (_y + _height);
}

void View::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    if (_visibility == VISIBLE) {
        // 绘制背景（考虑边框宽度）
        int borderWidthOffset = _borderWidth > 0 ? _borderWidth : 0;
        int drawX = _x + borderWidthOffset;
        int drawY = _y + borderWidthOffset;
        int drawWidth = _width - 2 * borderWidthOffset;
        int drawHeight = _height - 2 * borderWidthOffset;
        
        display.fillRect(drawX, drawY, drawWidth, drawHeight, _backgroundColor);
        
        // 绘制边框
        if (_borderWidth > 0) {
            for (int i = 0; i < _borderWidth; i++) {
                display.drawRect(_x + i, _y + i, _width - 2 * i, _height - 2 * i, _borderColor);
            }
        }
        
        // 标记为已绘制，清除脏标记
        _isDirty = false;
        _lastDrawTime = esp_log_timestamp();
    }
}

void View::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 子类可以重写此方法以自定义测量逻辑
    // 默认情况下，使用设定的尺寸或父容器的限制
    if (_width <= 0 && widthMeasureSpec > 0) {
        _width = widthMeasureSpec;
    }
    if (_height <= 0 && heightMeasureSpec > 0) {
        _height = heightMeasureSpec;
    }
}

void View::layout(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    _x = left;
    _y = top;
    _width = right - left;
    _height = bottom - top;
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

void View::setOnClickListener(std::function<void()> callback) {
    _clickCallback = callback;
}

void View::markDirty() {
    _isDirty = true;
    // 通知父视图需要重绘，但仅在对象完全初始化后
    if (_isInitialized && _parent && _parent != this) {
        notifyParentOfChange();
    }
}

void View::forceRedraw() {
    _isDirty = true;
    // 通知父视图需要重绘
    notifyParentOfChange();
}

void View::notifyParentOfChange() {
    // 通知父视图需要重绘
    if (_parent && _parent != this) {
        _parent->markDirty();
        _parent->notifyParentOfChange();  // 继续向上通知
    }
}