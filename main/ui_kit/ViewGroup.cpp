#include "ViewGroup.h"
#include <algorithm>

ViewGroup::ViewGroup(int16_t width, int16_t height)
    : View(width, height) {
}

ViewGroup::~ViewGroup() {
    removeAllChildren();
}

void ViewGroup::addChild(View* child) {
    if (child != nullptr) {
        _children.push_back(child);
        child->setParent(this);  // 设置父视图引用
        

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

    // 只有当自身或子视图需要重绘时才进行绘制
    if (isDirty()) {
        // 确保视图组已正确布局
        layout(_x, _y, _x + _width, _y + _height);

        // 绘制自身（背景、边框等）
        View::draw(display);

        // 绘制所有可见的子视图
        for (auto child : _children) {
            if (child->getVisibility() != GONE) {
                child->draw(display);
            }
        }
        
        // 标记为已绘制，清除脏标记
        _isDirty = false;
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

void ViewGroup::forceRedraw() {
    _isDirty = true;
    // 递归标记所有子视图也需要重绘
    for (auto child : _children) {
        child->forceRedraw();
    }
    // 通知父视图也需要重绘
    if (_parent) {
        _parent->markDirty();
        _parent->notifyParentOfChange();  // 继续向上通知
    }
}

bool ViewGroup::isDirty() const {
    // 如果自身是脏的，返回true
    if (_isDirty) {
        return true;
    }
    
    // 检查所有可见的子视图，如果有任意一个需要重绘，返回true
    for (const auto& child : _children) {
        if (child->getVisibility() != GONE && child->isDirty()) {
            return true;
        }
    }
    
    // 如果自身和所有可见子视图都不需要重绘，返回false
    return false;
}

void ViewGroup::notifyParentOfChange() {
    // 调用基类的实现
    View::notifyParentOfChange();
}