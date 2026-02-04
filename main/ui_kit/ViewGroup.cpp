#include "ViewGroup.h"
#include <algorithm>

ViewGroup::ViewGroup(int16_t x, int16_t y, int16_t width, int16_t height)
    : View(x, y, width, height) {
}

ViewGroup::~ViewGroup() {
    removeAllChildren();
}

void ViewGroup::addChild(View* child) {
    if (child != nullptr) {
        _children.push_back(child);
    }
}

void ViewGroup::removeChild(View* child) {
    if (child != nullptr) {
        auto it = std::find(_children.begin(), _children.end(), child);
        if (it != _children.end()) {
            _children.erase(it);
        }
    }
}

void ViewGroup::removeAllChildren() {
    _children.clear();
}

size_t ViewGroup::getChildCount() const {
    return _children.size();
}

View* ViewGroup::getChildAt(size_t index) const {
    if (index < _children.size()) {
        return _children[index];
    }
    return nullptr;
}

void ViewGroup::draw(m5gfx::M5GFX& display) {
    if (_visibility == GONE) {
        return;
    }

    // 绘制自身
    View::draw(display);

    // 绘制所有子视图
    for (auto child : _children) {
        if (child->getVisibility() != GONE) {
            child->draw(display);
        }
    }
}

bool ViewGroup::onTouch(int16_t x, int16_t y) {
    if (!contains(x, y)) {
        return false;
    }

    // 首先检查子视图是否处理触摸事件
    for (auto it = _children.rbegin(); it != _children.rend(); ++it) {
        View* child = *it;
        if (child->onTouch(x, y)) {
            return true;
        }
    }

    // 如果没有子视图处理，则由自己处理
    return View::onTouch(x, y);
}

void ViewGroup::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) {
    // 首先测量自己
    View::measure(widthMeasureSpec, heightMeasureSpec);

    // 然后测量所有子视图
    for (auto child : _children) {
        child->measure(_width, _height);
    }
}