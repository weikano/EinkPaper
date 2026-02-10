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

void TextView::onDraw(m5gfx::M5GFX& display) {    
    if (_visibility == GONE) {
        return;
    }

    // 绘制背景
    View::onDraw(display);

    if (_visibility == VISIBLE && !_text.empty()) {
        display.setTextColor(_textColor);
        display.setTextSize(_textSize);
        
        // 考虑padding的可用绘制区域
        int16_t contentX = _left + _paddingLeft;
        int16_t contentY = _top + _paddingTop;
        int16_t contentWidth = _measuredWidth - _paddingLeft - _paddingRight;
        int16_t contentHeight = _measuredHeight - _paddingTop - _paddingBottom;
        
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

void TextView::onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    int16_t measuredWidth = 0;
    int16_t measuredHeight = 0;
    
    // 处理宽度测量
    if (_width == MATCH_PARENT) {
        // 如果是MATCH_PARENT，则使用父容器的宽度限制
        measuredWidth = MeasureSpec::getSize(widthMeasureSpec);
    } else if (_width == WRAP_CONTENT) {
        // 如果是WRAP_CONTENT，则根据文本内容决定，但不超过父容器限制
        m5gfx::M5GFX tempDisplay; // 创建临时显示对象以获取字体信息
        int16_t textWidth = tempDisplay.textWidth(_text.c_str()) + getPaddingLeft() + getPaddingRight();
        int16_t specSize = MeasureSpec::getSize(widthMeasureSpec);
        MeasureSpecMode specMode = MeasureSpec::getMode(widthMeasureSpec);
        
        if (specMode == MeasureSpecMode::EXACTLY) {
            measuredWidth = specSize;
        } else if (specMode == MeasureSpecMode::AT_MOST) {
            measuredWidth = std::min(textWidth, specSize);
        } else {
            measuredWidth = textWidth;
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
        // 如果是WRAP_CONTENT，则根据文本内容决定，但不超过父容器限制
        m5gfx::M5GFX tempDisplay; // 创建临时显示对象以获取字体信息
        int16_t textHeight = tempDisplay.fontHeight() + getPaddingTop() + getPaddingBottom();
        int16_t specSize = MeasureSpec::getSize(heightMeasureSpec);
        MeasureSpecMode specMode = MeasureSpec::getMode(heightMeasureSpec);
        
        if (specMode == MeasureSpecMode::EXACTLY) {
            measuredHeight = specSize;
        } else if (specMode == MeasureSpecMode::AT_MOST) {
            measuredHeight = std::min(textHeight, specSize);
        } else {
            measuredHeight = textHeight;
        }
    } else {
        // 指定具体数值
        measuredHeight = _height;
    }
    
    _measuredWidth = measuredWidth;
    _measuredHeight = measuredHeight;
}

int16_t TextView::getDesiredWidth() const {
    m5gfx::M5GFX tempDisplay; // 创建临时显示对象以获取字体信息
    return tempDisplay.textWidth(_text.c_str()) + getPaddingLeft() + getPaddingRight();
}

int16_t TextView::getDesiredHeight() const {
    m5gfx::M5GFX tempDisplay; // 创建临时显示对象以获取字体信息
    return tempDisplay.fontHeight() + getPaddingTop() + getPaddingBottom();
}