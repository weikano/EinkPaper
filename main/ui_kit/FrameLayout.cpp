#include "FrameLayout.h"

FrameLayout::FrameLayout(int16_t width, int16_t height)
    : ViewGroup(width, height) {
}

void FrameLayout::layout(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    ViewGroup::layout(left, top, right, bottom);

    // 所有子视图都放在FrameLayout的左上角，占据整个空间
    for (auto child : _children) {
        if (child->getVisibility() != GONE) {
            child->layout(_left, _top, _left + _width, _top + _height);
        }
    }
}

void FrameLayout::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 首先测量自己
    View::measure(widthMeasureSpec, heightMeasureSpec);

    // 测量子视图
    for (auto child : _children) {
        child->measure(_width, _height);
    }
}