#include "ListView.h"
#include "TextView.h"

ListView::ListView(int16_t x, int16_t y, int16_t width, int16_t height)
    : ViewGroup(x, y, width, height), _itemHeight(30), _scrollOffset(0), _itemClickListener(nullptr) {
}

void ListView::setItems(const std::vector<std::string>& items) {
    _items = items;
}

size_t ListView::getItemCount() const {
    return _items.size();
}

void ListView::setItemHeight(int16_t itemHeight) {
    _itemHeight = itemHeight;
}

void ListView::setScrollOffset(int16_t scrollOffset) {
    _scrollOffset = scrollOffset;
    // 确保滚动偏移在有效范围内
    if (_scrollOffset < 0) {
        _scrollOffset = 0;
    }
    int maxScroll = std::max(0, (int)(_items.size() * _itemHeight - _height));
    if (_scrollOffset > maxScroll) {
        _scrollOffset = maxScroll;
    }
}

int16_t ListView::getScrollOffset() const {
    return _scrollOffset;
}

void ListView::setOnItemClickListener(OnItemClickListener listener) {
    _itemClickListener = listener;
}

void ListView::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    // 绘制背景
    View::draw(display);

    if (_visibility == VISIBLE) {
        // 计算需要绘制的项目范围
        int startIndex = _scrollOffset / _itemHeight;
        int endIndex = std::min((int)_items.size(), 
                               startIndex + (_height / _itemHeight) + 1);

        // 绘制可见的项目
        for (int i = startIndex; i < endIndex; i++) {
            if (i >= 0 && i < (int)_items.size()) {
                int16_t yPos = _y + (i * _itemHeight) - _scrollOffset;
                
                // 绘制项目背景
                if (i % 2 == 0) {
                    display.fillRect(_x, yPos, _width, _itemHeight, TFT_LIGHTGREY);
                } else {
                    display.fillRect(_x, yPos, _width, _itemHeight, TFT_WHITE);
                }
                
                // 绘制项目文本
                display.setTextColor(TFT_BLACK);
                display.setTextSize(1);
                display.setCursor(_x + 5, yPos + (_itemHeight - display.fontHeight()) / 2);
                display.print(_items[i].c_str());
                
                // 绘制分割线
                if (yPos + _itemHeight < _y + _height) {
                    display.drawFastHLine(_x, yPos + _itemHeight, _width, TFT_DARKGREY);
                }
            }
        }
        
        // 绘制边框
        if (_borderWidth > 0) {
            for (int i = 0; i < _borderWidth; i++) {
                display.drawRect(_x + i, _y + i, _width - 2 * i, _height - 2 * i, _borderColor);
            }
        }
    }
}

bool ListView::onTouch(int16_t x, int16_t y) {
    if (!contains(x, y)) {
        return false;
    }

    // 计算点击的项目索引
    int itemIndex = (y - _y + _scrollOffset) / _itemHeight;
    
    if (itemIndex >= 0 && itemIndex < (int)_items.size()) {
        // 触发项目点击事件
        if (_itemClickListener) {
            _itemClickListener(itemIndex);
        }
        return true;
    }

    return false;
}

void ListView::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    View::measure(widthMeasureSpec, heightMeasureSpec);
}