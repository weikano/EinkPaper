#include "Button.h"

Button::Button(int16_t x, int16_t y, int16_t width, int16_t height)
    : TextView(x, y, width, height), _pressedColor(TFT_LIGHTGREY) {
    // 设置默认的按钮样式
    setBackgroundColor(TFT_DARKGREY);
    setTextColor(TFT_WHITE);
    setTextAlign(1); // 居中对齐
}

void Button::setPressedColor(uint32_t color) {
    _pressedColor = color;
}

void Button::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    // 根据按下状态选择背景颜色
    uint32_t bgColor = _isPressed ? _pressedColor : _backgroundColor;
    
    // 临时修改背景色进行绘制
    uint32_t originalBgColor = _backgroundColor;
    _backgroundColor = bgColor;
    TextView::draw(display);
    _backgroundColor = originalBgColor; // 恢复原始背景色
    
    // 绘制边框
    if (_borderWidth > 0) {
        for (int i = 0; i < _borderWidth; i++) {
            display.drawRect(_x + i, _y + i, _width - 2 * i, _height - 2 * i, _borderColor);
        }
    }
}

bool Button::onTouch(int16_t x, int16_t y) {
    if (contains(x, y)) {
        _isPressed = true;
        TextView::onTouch(x, y);
        return true;
    }
    _isPressed = false;
    return false;
}