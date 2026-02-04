#include "LinearLayout.h"

LinearLayout::LinearLayout(int16_t x, int16_t y, int16_t width, int16_t height, Orientation orientation)
    : ViewGroup(x, y, width, height), _orientation(orientation), _spacing(0) {
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
    int16_t childSize = 0;
    
    for (auto child : _children) {
        if (child->getVisibility() == GONE) {
            continue;
        }

        if (_orientation == VERTICAL) {
            // 垂直布局
            childSize = child->getHeight();
            child->layout(_x, _y + currentPos, _x + _width, _y + currentPos + childSize);
            currentPos += childSize + _spacing;
        } else {
            // 水平布局
            childSize = child->getWidth();
            child->layout(_x + currentPos, _y, _x + currentPos + childSize, _y + _height);
            currentPos += childSize + _spacing;
        }
    }
}

void LinearLayout::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 首先测量自己
    View::measure(widthMeasureSpec, heightMeasureSpec);

    // 测量子视图
    for (auto child : _children) {
        child->measure(_width, _height);
    }

    // 如果尺寸未确定，根据子视图计算
    if (_orientation == VERTICAL) {
        // 垂直布局：高度根据子视图总高度计算
        if (_height <= 0) {
            int16_t totalHeight = 0;
            int childCount = 0;
            
            for (auto child : _children) {
                if (child->getVisibility() != GONE) {
                    totalHeight += child->getHeight() + _spacing;
                    childCount++;
                }
            }
            
            if (childCount > 0) {
                _height = totalHeight - _spacing; // 减去最后一个元素后的间距
            }
        }
    } else {
        // 水平布局：宽度根据子视图总宽度计算
        if (_width <= 0) {
            int16_t totalWidth = 0;
            int childCount = 0;
            
            for (auto child : _children) {
                if (child->getVisibility() != GONE) {
                    totalWidth += child->getWidth() + _spacing;
                    childCount++;
                }
            }
            
            if (childCount > 0) {
                _width = totalWidth - _spacing; // 减去最后一个元素后的间距
            }
        }
    }
}