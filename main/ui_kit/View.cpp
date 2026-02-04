#include "View.h"

View::View(int16_t x, int16_t y, int16_t width, int16_t height)
    : _x(x), _y(y), _width(width), _height(height),
      _visibility(VISIBLE), _backgroundColor(TFT_WHITE),
      _borderColor(TFT_BLACK), _borderWidth(1), _isPressed(false), _clickCallback(nullptr) {
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

bool View::contains(int16_t x, int16_t y) const {
    return x >= _x && x < (_x + _width) && y >= _y && y < (_y + _height);
}

void View::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    if (_visibility == VISIBLE) {
        // 绘制背景
        display.fillRect(_x, _y, _width, _height, _backgroundColor);
        
        // 绘制边框
        if (_borderWidth > 0) {
            for (int i = 0; i < _borderWidth; i++) {
                display.drawRect(_x + i, _y + i, _width - 2 * i, _height - 2 * i, _borderColor);
            }
        }
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