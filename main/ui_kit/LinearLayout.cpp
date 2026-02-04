#include "LinearLayout.h"

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

    int16_t currentPos = 0;
    
    for (auto child : _children) {
        if (child->getVisibility() == GONE) {
            continue;
        }
        printf("LinearLayout::layout: child->className() = %s\n", child->className().c_str());

        if (_orientation == VERTICAL) {
            // 垂直布局
            int16_t childHeight = child->getHeight() > 0 ? child->getHeight() : 0; // 如果高度未定义，默认为0
            child->layout(_x, _y + currentPos, _x + _width, _y + currentPos + childHeight);
            currentPos += childHeight + _spacing;
        } else {
            // 水平布局
            int16_t childWidth = child->getWidth() > 0 ? child->getWidth() : 0; // 如果宽度未定义，默认为0
            child->layout(_x + currentPos, _y, _x + currentPos + childWidth, _y + _height);
            currentPos += childWidth + _spacing;
        }
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
            _height = totalHeight - _spacing; // 减去最后一个元素后的间距
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
            _width = totalWidth - _spacing; // 减去最后一个元素后的间距
        }
    }
}