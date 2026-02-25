#include "TextView.h"

#include "../font_engine/FontEngineService.h"
#include <algorithm>

TextView::TextView(int16_t width, int16_t height)
    : View(width, height), _text(""), _textColor(TFT_BLACK), _textSize(1), _textAlign(0) {
}

void TextView::setText(const std::string& text) {
    if (_text != text) {
        _text = text;
        markDirty();
    }
}

const std::string& TextView::getText() const {
    return _text;
}

void TextView::setTextColor(uint32_t color) {
    if (_textColor != color) {
        _textColor = color;
        markDirty();
    }
}

void TextView::setTextSize(uint8_t size) {
    if (_textSize != size) {
        _textSize = size;
        markDirty();
    }
}

void TextView::setTextAlign(uint8_t align) {
    if (_textAlign != align) {
        _textAlign = align;
        markDirty();
    }
}

void TextView::onDraw(lgfx::LovyanGFX& display) {
    if (_visibility == GONE) {
        return;
    }

    View::onDraw(display);

    if (_visibility != VISIBLE || _text.empty()) {
        return;
    }

    const int16_t contentX = _left + _paddingLeft;
    const int16_t contentY = _top + _paddingTop;
    const int16_t contentWidth = _measuredWidth - _paddingLeft - _paddingRight;
    const int16_t contentHeight = _measuredHeight - _paddingTop - _paddingBottom;

    if (contentWidth <= 0 || contentHeight <= 0) {
        return;
    }

    auto& fontSvc = font_engine::FontEngineService::getInstance();
    if (!fontSvc.isReady()) {
        return;
    }
    const auto layout = fontSvc.engine().layoutText(_text, contentWidth);

    int16_t drawX = contentX;
    switch (_textAlign) {
        case 1:
            drawX = contentX + (contentWidth - layout.width) / 2;
            break;
        case 2:
            drawX = contentX + contentWidth - layout.width;
            break;
        default:
            drawX = contentX;
            break;
    }

    const int16_t drawY = contentY + (contentHeight - layout.height) / 2;
    fontSvc.engine().drawText(display,
                              _text,
                              drawX,
                              drawY,
                              contentWidth,
                              _textColor,
                              TFT_WHITE,
                              false);
}

void TextView::onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    int16_t measuredWidth = 0;
    int16_t measuredHeight = 0;

    if (_width == MATCH_PARENT) {
        measuredWidth = MeasureSpec::getSize(widthMeasureSpec);
    } else if (_width == WRAP_CONTENT) {
        const int16_t desiredWidth = getDesiredWidth();
        const int16_t specSize = MeasureSpec::getSize(widthMeasureSpec);
        const MeasureSpecMode specMode = MeasureSpec::getMode(widthMeasureSpec);

        if (specMode == MeasureSpecMode::EXACTLY) {
            measuredWidth = specSize;
        } else if (specMode == MeasureSpecMode::AT_MOST) {
            measuredWidth = std::min(desiredWidth, specSize);
        } else {
            measuredWidth = desiredWidth;
        }
    } else {
        measuredWidth = _width;
    }

    if (_height == MATCH_PARENT) {
        measuredHeight = MeasureSpec::getSize(heightMeasureSpec);
    } else if (_height == WRAP_CONTENT) {
        const int16_t desiredHeight = getDesiredHeight();
        const int16_t specSize = MeasureSpec::getSize(heightMeasureSpec);
        const MeasureSpecMode specMode = MeasureSpec::getMode(heightMeasureSpec);

        if (specMode == MeasureSpecMode::EXACTLY) {
            measuredHeight = specSize;
        } else if (specMode == MeasureSpecMode::AT_MOST) {
            measuredHeight = std::min(desiredHeight, specSize);
        } else {
            measuredHeight = desiredHeight;
        }
    } else {
        measuredHeight = _height;
    }

    _measuredWidth = measuredWidth;
    _measuredHeight = measuredHeight;
}

int16_t TextView::getDesiredWidth() const {
    auto& fontSvc = font_engine::FontEngineService::getInstance();
    if (fontSvc.isReady()) {
        const auto layout = fontSvc.engine().layoutText(_text, 32767);
        return static_cast<int16_t>(layout.width + getPaddingLeft() + getPaddingRight());
    }
    return static_cast<int16_t>(_text.size() * 8 + getPaddingLeft() + getPaddingRight());
}

int16_t TextView::getDesiredHeight() const {
    auto& fontSvc = font_engine::FontEngineService::getInstance();
    if (fontSvc.isReady()) {
        const auto layout = fontSvc.engine().layoutText(_text, 32767);
        return static_cast<int16_t>(layout.height + getPaddingTop() + getPaddingBottom());
    }
    return static_cast<int16_t>(16 + getPaddingTop() + getPaddingBottom());
}
