#include "FrameLayout.h"

FrameLayout::FrameLayout(int16_t width, int16_t height)
    : ViewGroup(width, height) {
}

void FrameLayout::onLayout(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    ViewGroup::onLayout(left, top, right, bottom);

    // 所有子视图都放在FrameLayout的左上角，占据整个空间
    for (auto child : _children) {
        if (child->getVisibility() != GONE) {
            child->layout(_left + _paddingLeft, _top + _paddingTop, 
                         _left + _measuredWidth - _paddingRight, 
                         _top + _measuredHeight - _paddingBottom);
        }
    }
}

void FrameLayout::onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 首先测量自己
    View::onMeasure(widthMeasureSpec, heightMeasureSpec);

    // 测量子视图
    for (auto child : _children) {
        if (child->getVisibility() != GONE) {
            child->measure(widthMeasureSpec, heightMeasureSpec);
        }
    }
}