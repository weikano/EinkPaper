#include "TextView.h"

TextView::TextView(int16_t width, int16_t height)
    : View(width, height), _text(""), _textColor(TFT_BLACK), _textSize(1), _textAlign(0) {
}

void TextView::setText(const std::string& text) {
    if (_text != text) {
        _text = text;
        markDirty();  // 文本变化时标记为需要重绘
    }
}

const std::string& TextView::getText() const {
    return _text;
}

void TextView::setTextColor(uint32_t color) {
    if (_textColor != color) {
        _textColor = color;
        markDirty();  // 颜色变化时标记为需要重绘
    }
}

void TextView::setTextSize(uint8_t size) {
    if (_textSize != size) {
        _textSize = size;
        markDirty();  // 字体大小变化时标记为需要重绘
    }
}

void TextView::setTextAlign(uint8_t align) {
    if (_textAlign != align) {
        _textAlign = align;
        markDirty();  // 对齐方式变化时标记为需要重绘
    }
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
        
        // 考虑padding的可用绘制区域
        int16_t contentX = _x + _paddingLeft;
        int16_t contentY = _y + _paddingTop;
        int16_t contentWidth = _width - _paddingLeft - _paddingRight;
        int16_t contentHeight = _height - _paddingTop - _paddingBottom;
        
        int16_t textWidth = display.textWidth(_text.c_str());
        int16_t drawX = contentX;
        
        // 根据对齐方式计算文本绘制位置（在内容区域内）
        switch (_textAlign) {
            case 1: // 居中对齐
                drawX = contentX + (contentWidth - textWidth) / 2;
                break;
            case 2: // 右对齐
                drawX = contentX + contentWidth - textWidth;
                break;
            default: // 左对齐
                drawX = contentX;
                break;
        }
        
        int16_t drawY = contentY + (contentHeight - display.fontHeight()) / 2;
        
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