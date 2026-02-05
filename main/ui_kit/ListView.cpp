#include "ListView.h"
#include "TextView.h"

ListView::ListView(int16_t width, int16_t height)
    : ViewGroup(width, height), _rowCount(5), _scrollOffset(0), _itemClickListener(nullptr) {
    // 初始状态标记为需要重绘，但不在此处调用，避免构造期间的循环引用
    _isDirty = true;
}

void ListView::setItems(const std::vector<std::string>& items) {
    if (_items != items) {
        _items = items;
        markDirty();  // 数据变化时标记为需要重绘
    }
}

size_t ListView::getItemCount() const {
    return _items.size();
}

void ListView::setRowCount(int16_t rowCount) {
    if (rowCount > 0) {
        _rowCount = rowCount;
    }
}

int16_t ListView::getRowCount() const {
    return _rowCount;
}

void ListView::setScrollOffset(int16_t scrollOffset) {
    if (_scrollOffset != scrollOffset) {
        _scrollOffset = scrollOffset;
        // 确保滚动偏移在有效范围内
        if (_scrollOffset < 0) {
            _scrollOffset = 0;
        }
        
        // 计算每项的高度
        int16_t itemHeight = _height / _rowCount;
        int maxScroll = std::max(0, (int)(_items.size() * itemHeight - _height));
        if (_scrollOffset > maxScroll) {
            _scrollOffset = maxScroll;
        }
        markDirty();  // 滚动位置变化时标记为需要重绘
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
        // 计算每项的高度
        int16_t itemHeight = _height / _rowCount;
        
        // 计算需要绘制的项目范围
        int startIndex = _scrollOffset / itemHeight;
        int endIndex = std::min((int)_items.size(), 
                               startIndex + _rowCount + 1);

        // 绘制可见的项目
        for (int i = startIndex; i < endIndex; i++) {
            if (i >= 0 && i < (int)_items.size()) {
                int16_t yPos = _y + (i * itemHeight) - _scrollOffset;
                
                // 只绘制在可见区域内的项目
                if (yPos + itemHeight > _y && yPos < _y + _height) {
                    // 绘制项目背景（考虑padding）
                    int16_t itemBgX = _x + _paddingLeft;
                    int16_t itemBgWidth = _width - _paddingLeft - _paddingRight;
                    display.fillRect(itemBgX, yPos, itemBgWidth, itemHeight, TFT_WHITE);
                    
                    // 绘制项目文本（带省略号处理）
                    display.setTextColor(TFT_BLACK);
                    display.setTextSize(1);
                    
                    // 检查文本是否过长需要截断
                     std::string text = _items[i];
                     int16_t text_width = display.textWidth(text.c_str());
                     int16_t max_width = _width - _paddingLeft - _paddingRight; // 考虑左右padding
                     
                     if (text_width > max_width) {
                         // 需要截断文本并添加省略号
                         std::string ellipsis = "...";
                         int16_t ellipsis_width = display.textWidth(ellipsis.c_str());
                         
                         // 如果原始文本适合空间，直接使用
                          if (display.textWidth(text.c_str()) <= max_width) {
                              // 文本适合空间，无需处理
                          } else {
                              // 使用二分查找来快速找到合适的文本长度
                              int left = 0;
                              int right = text.length();
                              std::string result = ellipsis; // 默认结果是省略号
                              
                              while (left <= right) {
                                  int mid = left + (right - left) / 2;
                                  std::string substr = text.substr(0, mid);
                                  std::string candidate = substr + ellipsis;
                                  
                                  if (display.textWidth(candidate.c_str()) <= max_width) {
                                      result = candidate;
                                      left = mid + 1;
                                  } else {
                                      right = mid - 1;
                                  }
                              }
                              
                              text = result;
                          }
                     }
                    
                    // 使用padding调整文本位置
                    int16_t textX = _x + _paddingLeft;
                    int16_t textY = yPos + (itemHeight - display.fontHeight()) / 2;
                    display.setCursor(textX, textY);
                    display.print(text.c_str());
                    
                    // 绘制分割线
                    if (yPos + itemHeight < _y + _height) {
                        display.drawFastHLine(_x + _paddingLeft, yPos + itemHeight, 
                                             _width - _paddingLeft - _paddingRight, TFT_BLACK);
                    }
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

    // 计算每项的高度
    int16_t itemHeight = _height / _rowCount;
    
    // 计算点击的项目索引
    int itemIndex = (y - _y + _scrollOffset) / itemHeight;
    
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
    // 如果指定了具体尺寸，则使用指定尺寸；否则使用父容器提供的约束
    if (_width <= 0 && widthMeasureSpec > 0) {
        _width = widthMeasureSpec;
    }
    if (_height <= 0 && heightMeasureSpec > 0) {
        _height = heightMeasureSpec;
    }
}

void ListView::layout(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    // 使用父容器指定的布局参数
    _x = left;
    _y = top;
    _width = right - left;
    _height = bottom - top;
}