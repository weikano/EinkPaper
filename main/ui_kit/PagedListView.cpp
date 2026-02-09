#include "PagedListView.h"
#include "TextView.h"
#include "esp_log.h"
#include <algorithm>
#include <cmath>

PagedListView::PagedListView(int16_t width, int16_t height)
    : ViewGroup(width, height), 
      _rowCount(3), 
      _columnCount(2),
      _horizontalSpacing(10),  // 默认水平间距
      _verticalSpacing(10),    // 默认垂直间距
      _currentPage(0),
      _totalPages(0),
      _totalItems(0),
      _dataSourceLoader(nullptr),
      _itemRenderer(nullptr),
      _itemClickListener(nullptr),
      _pageChangeListener(nullptr),
      _backCallback(nullptr) {
    _isDirty = true;
    refreshData();
    ESP_LOGD("PagedListView", "created with width=%d, height=%d", width, height);
}

void PagedListView::setRowCount(int16_t rowCount) {
    if (rowCount > 0) {
        _rowCount = rowCount;
        refreshData();
    }
}

int16_t PagedListView::getRowCount() const {
    return _rowCount;
}

void PagedListView::setColumnCount(int16_t columnCount) {
    if (columnCount > 0) {
        _columnCount = columnCount;
        refreshData();
    }
}

int16_t PagedListView::getColumnCount() const {
    return _columnCount;
}

void PagedListView::setDataSourceLoader(DataSourceLoader loader) {
    _dataSourceLoader = loader;
    refreshData();
}

void PagedListView::setItemRenderer(ItemRenderer renderer) {
    _itemRenderer = renderer;
}

void PagedListView::setOnItemClickListener(OnItemClickListener listener) {
    _itemClickListener = listener;
}

void PagedListView::setOnPageChangeListener(OnPageChangeListener listener) {
    _pageChangeListener = listener;
}

void PagedListView::setOnBackCallback(OnBackCallback callback) {
    _backCallback = callback;
}

void PagedListView::setTotalItems(int totalItems) {
    _totalItems = totalItems;
    _totalPages = _rowCount * _columnCount > 0 ? (_totalItems + _rowCount * _columnCount - 1) / (_rowCount * _columnCount) : 0;
}

int PagedListView::getTotalItems() const {
    return _totalItems;
}

void PagedListView::setCurrentPage(int page) {
    if (!_dataSourceLoader) {
        return;
    }

    int oldPage = _currentPage;
    int maxPage = std::max(0, _totalPages - 1);
    
    if (page < 0) {
        _currentPage = 0;
    } else if (page > maxPage) {
        _currentPage = maxPage;
    } else {
        _currentPage = page;
    }

    if (oldPage != _currentPage) {
        _loadCurrentPageData();
        markDirty();
        
        if (_pageChangeListener) {
            _pageChangeListener(_currentPage, _totalPages);
        }
    }
}

int PagedListView::getCurrentPage() const {
    return _currentPage;
}

int PagedListView::getTotalPages() const {
    return _totalPages;
}

void PagedListView::refreshData() {
    if (!_dataSourceLoader) {
        return;
    }
    
    // 先获取总项目数，这里我们假设通过尝试加载第一页来估算总数
    // 实际应用中可能需要额外的API来获取总数
    _loadCurrentPageData();
    markDirty();
}

bool PagedListView::nextPage() {
    if (_currentPage < _totalPages - 1) {
        setCurrentPage(_currentPage + 1);
        return true;
    }
    return false;
}

bool PagedListView::prevPage() {
    if (_currentPage > 0) {
        setCurrentPage(_currentPage - 1);
        return true;
    }
    return false;
}

void PagedListView::_calculatePageSize() {
    if (!_dataSourceLoader) {
        _totalPages = 0;
        _totalItems = 0;
        return;
    }

    // 估算总项目数 - 这里需要改进，实际应用中应该有专门的方法获取总数
    int pageSize = _rowCount * _columnCount;
    int page = 0;
    int total = 0;
    
    // 尝试加载多个页面来计算总数（这是一个简化实现）
    // 在实际应用中，您可能需要一个单独的API来获取总项目数
    while (true) {
        auto items = _dataSourceLoader(page, pageSize);
        if (items.empty()) {
            break;
        }
        total += items.size();
        if (items.size() < pageSize) {
            // 如果返回的项目数少于页面大小，说明这是最后一页
            break;
        }
        page++;
    }
    
    _totalItems = total;
    _totalPages = pageSize > 0 ? (total + pageSize - 1) / pageSize : 0;
}

void PagedListView::_loadCurrentPageData() {
    if (!_dataSourceLoader) {
        _currentPageItems.clear();
        return;
    }

    _calculatePageSize();
    
    int pageSize = _rowCount * _columnCount;
    _currentPageItems = _dataSourceLoader(_currentPage, pageSize);
}

void PagedListView::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    // 绘制背景
    View::draw(display);

    if (_visibility == VISIBLE && _parent != nullptr) {
        int itemCount = _currentPageItems.size();        
        
        // 绘制当前页的所有项目
        for (int i = 0; i < itemCount; i++) {
            // 重新计算每个项目的实际位置和尺寸，确保不会为0
            int16_t itemX = _getItemX(i);
            int16_t itemY = _getItemY(i);
            int16_t itemW = _getItemWidth();
            int16_t itemH = _getItemHeight();
            
            // 确保尺寸不为0
            if (itemW <= 0) itemW = 10;  // 设置最小宽度
            if (itemH <= 0) itemH = 10;  // 设置最小高度
            
            // 确保坐标在合理范围内
            itemX = std::max(static_cast<int16_t>(_left), itemX);
            itemY = std::max(static_cast<int16_t>(_top), itemY);
            
            if (_itemRenderer) {
                // 使用自定义渲染器绘制项目
                _itemRenderer(display, i, _currentPageItems[i], itemX, itemY, itemW, itemH);
            } else {
                // 使用默认渲染器绘制项目
                // 绘制项目背景
                display.fillRect(itemX, itemY, itemW, itemH, TFT_WHITE);
                display.drawRect(itemX, itemY, itemW, itemH, TFT_BLACK);
                
                // 绘制项目文本
                display.setTextColor(TFT_BLACK);
                display.setTextSize(1);
                
                // 简单的文本绘制，带省略号处理
                std::string text = _currentPageItems[i];
                int16_t text_width = display.textWidth(text.c_str());
                int16_t max_width = itemW - 10; // 考虑内边距
                
                if (text_width > max_width && max_width > 0) {
                    std::string ellipsis = "...";
                    int16_t ellipsis_width = display.textWidth(ellipsis.c_str());
                    
                    int left = 0;
                    int right = text.length();
                    std::string result = ellipsis;
                    
                    while (left <= right && max_width > ellipsis_width) {
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
                
                int16_t textX = itemX + 5;
                int16_t textY = itemY + (itemH - display.fontHeight()) / 2;
                display.setCursor(textX, textY);
                display.print(text.c_str());
            }
        }
        
        // 绘制底部控制栏
        int16_t controlBarHeight = 30;
        int16_t controlBarY = _top + _height - controlBarHeight;
        
        // 绘制控制栏背景
        display.fillRect(_left, controlBarY, _width, controlBarHeight, TFT_WHITE);
        display.drawRect(_left, controlBarY, _width, controlBarHeight, TFT_BLACK);
        
        // 计算各按钮和文本的位置
        int16_t buttonWidth = 60;
        int16_t buttonHeight = 20;
        int16_t padding = 5;
        
        // "上一页"按钮
        int16_t prevButtonX = _left + padding;
        int16_t prevButtonY = controlBarY + (controlBarHeight - buttonHeight) / 2;
        display.fillRect(prevButtonX, prevButtonY, buttonWidth, buttonHeight, TFT_WHITE);
        display.drawRect(prevButtonX, prevButtonY, buttonWidth, buttonHeight, TFT_BLACK);
        display.setTextColor(TFT_BLACK);
        display.setTextSize(1);
        display.setCursor(prevButtonX + (buttonWidth - display.textWidth("上一页")) / 2, 
                         prevButtonY + (buttonHeight - display.fontHeight()) / 2);
        display.print("上一页");
        
        // 页码信息
        std::string pageInfo = "第 " + std::to_string(_currentPage + 1) + "/" + std::to_string(_totalPages) + " 页";
        int16_t pageInfoWidth = display.textWidth(pageInfo.c_str());
        int16_t pageInfoX = _left + padding + buttonWidth + padding;
        int16_t pageInfoY = controlBarY + (controlBarHeight - display.fontHeight()) / 2;
        display.setCursor(pageInfoX, pageInfoY);
        display.print(pageInfo.c_str());
        
        // 总数信息（在页码旁边）
        std::string totalCountInfo = "(" + std::to_string(_totalItems) + "项)";        
        int16_t totalCountX = pageInfoX + pageInfoWidth + padding;
        int16_t totalCountY = pageInfoY;
        display.setCursor(totalCountX, totalCountY);
        display.print(totalCountInfo.c_str());
        
        // "下一页"按钮
        int16_t nextButtonX = _left + _width - buttonWidth * 2 - padding * 2;  // 调整位置为倒数第二个按钮
        int16_t nextButtonY = controlBarY + (controlBarHeight - buttonHeight) / 2;
        display.fillRect(nextButtonX, nextButtonY, buttonWidth, buttonHeight, TFT_WHITE);
        display.drawRect(nextButtonX, nextButtonY, buttonWidth, buttonHeight, TFT_BLACK);
        display.setTextColor(TFT_BLACK);
        display.setCursor(nextButtonX + (buttonWidth - display.textWidth("下一页")) / 2, 
                         nextButtonY + (buttonHeight - display.fontHeight()) / 2);
        display.print("下一页");
        
        // "返回"按钮
        int16_t backButtonX = _left + _width - buttonWidth - padding;  // 最右边的按钮
        int16_t backButtonY = controlBarY + (controlBarHeight - buttonHeight) / 2;
        display.fillRect(backButtonX, backButtonY, buttonWidth, buttonHeight, TFT_WHITE);
        display.drawRect(backButtonX, backButtonY, buttonWidth, buttonHeight, TFT_BLACK);
        display.setTextColor(TFT_BLACK);
        display.setCursor(backButtonX + (buttonWidth - display.textWidth("返回")) / 2, 
                         backButtonY + (buttonHeight - display.fontHeight()) / 2);
        display.print("返回");
        
        // 绘制边框
        if (_borderWidth > 0) {
            for (int i = 0; i < _borderWidth; i++) {
                display.drawRect(_left + i, _top + i, _width - 2 * i, _height - 2 * i, _borderColor);
            }
        }
    }
}

bool PagedListView::onTouch(int16_t x, int16_t y) {
    if (!contains(x, y)) {
        return false;
    }

    // 检查是否点击了项目
    int index = _getIndexFromPosition(x, y);
    if (index >= 0 && index < (int)_currentPageItems.size()) {
        if (_itemClickListener) {
            _itemClickListener(index);
        }
        return true;
    }
    
    // 检查是否点击了底部控制按钮
        int16_t controlBarHeight = 30;
        int16_t controlBarY = _top + _height - controlBarHeight;
        
        if (y >= controlBarY && y < controlBarY + controlBarHeight) {
            int16_t buttonWidth = 60;
            int16_t padding = 5;
            
            // 检查是否点击了"上一页"按钮
            int16_t prevButtonX = _left + padding;
            if (x >= prevButtonX && x < prevButtonX + buttonWidth) {
                prevPage();
                return true;
            }
            
            // 检查是否点击了"下一页"按钮
            int16_t nextButtonX = _left + _width - buttonWidth * 2 - padding * 2;
            if (x >= nextButtonX && x < nextButtonX + buttonWidth) {
                nextPage();
                return true;
            }
            
            // 检查是否点击了"返回"按钮
            int16_t backButtonX = _left + _width - buttonWidth - padding;
            if (x >= backButtonX && x < backButtonX + buttonWidth) {
                // 这里需要外部传入的回调函数来处理返回操作
                // 由于我们无法直接访问外部的返回逻辑，我们可以使用一个回调函数
                // 这里暂时不做处理，将在下面添加一个返回回调函数
                if (_backCallback) {
                    _backCallback();
                }
                return true;
            }
        }

    return false;
}

bool PagedListView::onSwipe(TouchGestureDetector::SwipeDirection direction) {
    switch (direction) {
        case TouchGestureDetector::SwipeDirection::LEFT:
        case TouchGestureDetector::SwipeDirection::UP:
            // 向左滑动，跳转到下一页
            return nextPage();
        case TouchGestureDetector::SwipeDirection::RIGHT:
        case TouchGestureDetector::SwipeDirection::DOWN:
            // 向右滑动，跳转到上一页
            return prevPage();
        default:
            // 其他方向不处理
            return false;
    }
}

int16_t PagedListView::_getItemX(int index) const {
    if (_columnCount <= 0) {
        return std::max(static_cast<int16_t>(_left), static_cast<int16_t>(_left + _paddingLeft));
    }
    
    int col = index % _columnCount;
    int16_t itemWidth = _getItemWidth();
    
    // 使用固定的水平间距计算项目X坐标
    int16_t calculatedX = _left + _paddingLeft + col * (itemWidth + _horizontalSpacing);
    
    // 确保计算的X坐标在合理的范围内
    return std::max(static_cast<int16_t>(_left + _paddingLeft), 
                   std::min(calculatedX, static_cast<int16_t>(_left + _width - itemWidth - _paddingRight)));
}

int16_t PagedListView::_getItemY(int index) const {
    if (_rowCount <= 0) {
        return std::max(static_cast<int16_t>(_top), static_cast<int16_t>(_top + _paddingTop));
    }
    
    int row = index / _columnCount;
    int16_t itemHeight = _getItemHeight();
    
    // 使用固定的垂直间距计算项目Y坐标
    int16_t calculatedY = _top + _paddingTop + row * (itemHeight + _verticalSpacing);
    
    // 确保计算的Y坐标在合理的范围内
    return std::max(static_cast<int16_t>(_top + _paddingTop), 
                   std::min(calculatedY, static_cast<int16_t>(_top + _height - itemHeight - 30 - _paddingBottom)));
}

int16_t PagedListView::_getItemWidth() const {
    if (_columnCount <= 0) {
        int16_t result = _width - _paddingLeft - _paddingRight;
        return std::max(static_cast<int16_t>(10), result);  // 确保最小宽度为10
    }
    
    int16_t availableWidth = _width - _paddingLeft - _paddingRight;
    if (availableWidth <= 0) return 10;  // 确保最小宽度
    
    // 计算总间距（考虑列间距）
    int16_t totalSpacing = _horizontalSpacing * (_columnCount - 1);
    
    // 计算单个项目宽度
    int16_t itemWidth = _columnCount > 1 ? 
        (availableWidth - totalSpacing) / _columnCount : 
        availableWidth - _horizontalSpacing;  // 单列时只减去一个边距
    
    return std::max(static_cast<int16_t>(10), itemWidth);  // 确保最小宽度
}

int16_t PagedListView::_getItemHeight() const {
    if (_rowCount <= 0) {
        int16_t result = _height - 30 - _paddingTop - _paddingBottom; // 预留底部控制栏空间
        return std::max(static_cast<int16_t>(10), result);  // 确保最小高度
    }
    
    int16_t availableHeight = _height - 30 - _paddingTop - _paddingBottom; // 预留底部控制栏空间
    if (availableHeight <= 0) return 10;  // 确保最小高度
    
    // 计算总间距（考虑行间距）
    int16_t totalSpacing = _verticalSpacing * (_rowCount - 1);
    
    // 计算单个项目高度
    int16_t itemHeight = _rowCount > 1 ? 
        (availableHeight - totalSpacing) / _rowCount : 
        availableHeight - _verticalSpacing;  // 单行时只减去一个边距
    
    return std::max(static_cast<int16_t>(10), itemHeight);  // 确保最小高度
}

void PagedListView::setHorizontalSpacing(int16_t spacing) {
    _horizontalSpacing = spacing >= 0 ? spacing : 0;
    refreshData();  // 重新计算布局
}

int16_t PagedListView::getHorizontalSpacing() const {
    return _horizontalSpacing;
}

void PagedListView::setVerticalSpacing(int16_t spacing) {
    _verticalSpacing = spacing >= 0 ? spacing : 0;
    refreshData();  // 重新计算布局
}

int16_t PagedListView::getVerticalSpacing() const {
    return _verticalSpacing;
}

int PagedListView::_getIndexFromPosition(int16_t x, int16_t y) const {    
    
    for (int i = 0; i < (int)_currentPageItems.size(); i++) {
        int16_t itemX = _getItemX(i);
        int16_t itemY = _getItemY(i);
        int16_t itemW = _getItemWidth();
        int16_t itemH = _getItemHeight();
        
        if (x >= itemX && x < itemX + itemW && y >= itemY && y < itemY + itemH) {
            return i;
        }
    }
    
    return -1;
}

void PagedListView::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 如果指定了具体尺寸，则使用指定尺寸；否则使用父容器提供的约束
    if (_width <= 0 && widthMeasureSpec > 0) {
        _width = widthMeasureSpec;
    }
    if (_height <= 0 && heightMeasureSpec > 0) {
        _height = heightMeasureSpec;
    }
}

void PagedListView::layout(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    // 使用父容器指定的布局参数
    _left = left;
    _top = top;
    _width = right - left;
    _height = bottom - top;
}