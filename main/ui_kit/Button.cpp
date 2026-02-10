#include "Button.h"

Button::Button(int16_t width, int16_t height)
    : TextView(width, height) {
    // 设置默认的按钮样式    
    setTextColor(TFT_BLACK);
    setTextAlign(1); // 居中对齐
}



void Button::onDraw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    // 直接调用TextView的绘制方法
    TextView::onDraw(display);
    
    // 绘制边框
    if (_borderWidth > 0) {
        for (int i = 0; i < _borderWidth; i++) {
            display.drawRect(_left + i, _top + i, _width - 2 * i, _height - 2 * i, _borderColor);
        }
    }
}

bool Button::onTouch(int16_t x, int16_t y) {
    if (contains(x, y)) {
        TextView::onTouch(x, y);
        return true;
    }
    
    return false;
}