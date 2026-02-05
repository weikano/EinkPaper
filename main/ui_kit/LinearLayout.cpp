#include "LinearLayout.h"
#include <algorithm>

LinearLayout::LinearLayout(int16_t width, int16_t height, Orientation orientation)
    : ViewGroup(width, height), _orientation(orientation), _spacing(0) {
}

void LinearLayout::setOrientation(Orientation orientation) {
    _orientation = orientation;
}

void LinearLayout::setSpacing(int16_t spacing) {
    _spacing = spacing;
}

void LinearLayout::layout(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    ViewGroup::layout(left, top, right, bottom);

    if (_orientation == VERTICAL) {
        // 垂直布局 - 从上边距开始
        int16_t currentPos = _y + getPaddingTop();
        int16_t maxBottom = _y + _height - getPaddingBottom(); // 最大允许的底部位置
        
        for (auto child : _children) {
            if (child->getVisibility() == GONE) {
                continue;
            }
            printf("LinearLayout::layout: child->className() = %s\n", child->className().c_str());

            int16_t childHeight = child->getHeight() > 0 ? child->getHeight() : 0; // 如果高度未定义，默认为0
            
            // 确保不超出底部边界
            int16_t childBottom = std::min<int16_t>(currentPos + childHeight, maxBottom);
            if (currentPos < maxBottom) { // 只有在还有空间时才布局
                // 考虑左右padding
                child->layout(_x + getPaddingLeft(), currentPos, 
                             _x + _width - getPaddingRight(), childBottom);
                
                // 更新当前位置
                currentPos = childBottom + _spacing;
            }
        }
    } else {
        // 水平布局 - 从左边距开始
        int16_t currentPos = _x + getPaddingLeft();
        int16_t maxRight = _x + _width - getPaddingRight(); // 最大允许的右边位置
        
        for (auto child : _children) {
            if (child->getVisibility() == GONE) {
                continue;
            }
            printf("LinearLayout::layout: child->className() = %s\n", child->className().c_str());

            int16_t childWidth = child->getWidth() > 0 ? child->getWidth() : 0; // 如果宽度未定义，默认为0
            
            // 确保不超出右边界
            int16_t childRight = std::min<int16_t>(currentPos + childWidth, maxRight);
            if (currentPos < maxRight) { // 只有在还有空间时才布局
                // 考虑上下padding
                child->layout(currentPos, _y + getPaddingTop(), 
                             childRight, _y + _height - getPaddingBottom());
                
                // 更新当前位置
                currentPos = childRight + _spacing;
            }
        }
    }
}

void LinearLayout::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    // 只有当自身或子视图需要重绘时才进行绘制
    if (isDirty()) {
        // 确保视图组已正确布局
        layout(_x, _y, _x + _width, _y + _height);

        // 绘制自身（背景、边框等）- 使用View的绘制方法
        View::draw(display);

        // 绘制所有可见的子视图
        for (auto child : _children) {
            if (child->getVisibility() != GONE) {
                // 确保子视图只在内容区域内绘制
                child->draw(display);
            }
        }
        
        // 标记为已绘制，清除脏标记
        _isDirty = false;
    }
}

void LinearLayout::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 首先测量自己
    View::measure(widthMeasureSpec, heightMeasureSpec);

    // 测量子视图
    for (auto child : _children) {
        // 对于LinearLayout，在布局方向上不限制子视图的尺寸，只限制垂直方向
        if (_orientation == VERTICAL) {
            // 垂直布局：限制子视图的宽度，但不限制高度
            child->measure(_width, 0);  // 0表示无限制
        } else {
            // 水平布局：限制子视图的高度，但不限制宽度
            child->measure(0, _height);  // 0表示无限制
        }
    }

    // 根据子视图计算自己的尺寸
    if (_orientation == VERTICAL) {
        // 垂直布局：高度根据子视图总高度计算
        int16_t totalHeight = 0;
        int childCount = 0;
        
        for (auto child : _children) {
            if (child->getVisibility() != GONE) {
                // 使用子视图的实际测量高度
                totalHeight += child->getHeight() > 0 ? child->getHeight() : 0;
                totalHeight += _spacing;  // 添加间距
                childCount++;
            }
        }
        
        if (childCount > 0 && _height <= 0) {
            // 加上上下padding
            _height = totalHeight - _spacing + getPaddingTop() + getPaddingBottom(); // 减去最后一个元素后的间距，加上上下padding
        }
    } else {
        // 水平布局：宽度根据子视图总宽度计算
        int16_t totalWidth = 0;
        int childCount = 0;
        
        for (auto child : _children) {
            if (child->getVisibility() != GONE) {
                // 使用子视图的实际测量宽度
                totalWidth += child->getWidth() > 0 ? child->getWidth() : 0;
                totalWidth += _spacing;  // 添加间距
                childCount++;
            }
        }
        
        if (childCount > 0 && _width <= 0) {
        // 加上左右padding
        _width = totalWidth - _spacing + getPaddingLeft() + getPaddingRight(); // 减去最后一个元素后的间距，加上左右padding
    }
}
}