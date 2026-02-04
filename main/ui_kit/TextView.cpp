#include "TextView.h"

TextView::TextView(int16_t x, int16_t y, int16_t width, int16_t height)
    : View(x, y, width, height), _text(""), _textColor(TFT_BLACK), _textSize(1), _textAlign(0) {
}

void TextView::setText(const std::string& text) {
    _text = text;
}

const std::string& TextView::getText() const {
    return _text;
}

void TextView::setTextColor(uint32_t color) {
    _textColor = color;
}

void TextView::setTextSize(uint8_t size) {
    _textSize = size;
}

void TextView::setTextAlign(uint8_t align) {
    _textAlign = align;
}

void TextView::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    // 绘制背景
    View::draw(display);

    if (_visibility == VISIBLE && !_text.empty()) {
        display.setTextColor(_textColor);
        display.setTextSize(_textSize);
        
        int16_t textWidth = display.textWidth(_text.c_str());
        int16_t drawX = _x;
        
        // 根据对齐方式计算文本绘制位置
        switch (_textAlign) {
            case 1: // 居中对齐
                drawX = _x + (_width - textWidth) / 2;
                break;
            case 2: // 右对齐
                drawX = _x + _width - textWidth;
                break;
            default: // 左对齐
                drawX = _x;
                break;
        }
        
        int16_t drawY = _y + (_height - display.fontHeight()) / 2;
        
        display.setCursor(drawX, drawY);
        display.print(_text.c_str());
    }
}

void TextView::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 如果宽度或高度未设置，可以根据文本内容计算
    if (_width <= 0) {
        if (widthMeasureSpec > 0) {
            _width = widthMeasureSpec;
        } else {
            // 根据文本内容估算宽度
            m5gfx::M5GFX tempDisplay; // 创建临时显示对象以获取字体信息
            _width = tempDisplay.textWidth(_text.c_str()) + 10; // 添加一些padding
        }
    }
    
    if (_height <= 0) {
        if (heightMeasureSpec > 0) {
            _height = heightMeasureSpec;
        } else {
            // 根据字体大小估算高度
            m5gfx::M5GFX tempDisplay; // 创建临时显示对象以获取字体信息
            _height = tempDisplay.fontHeight() + 10; // 添加一些padding
        }
    }
}