#include "LinearLayout.h"
#include "esp_log.h"
#include <algorithm>

LinearLayout::LinearLayout(int16_t width, int16_t height,
                           Orientation orientation)
    : ViewGroup(width, height), _orientation(orientation), _spacing(0) {}

void LinearLayout::setOrientation(Orientation orientation) {
  if (_orientation != orientation) {
    _orientation = orientation;
    markDirty();
  }
}

void LinearLayout::setSpacing(int16_t spacing) {
  if (_spacing != spacing) {
    _spacing = spacing;
    markDirty();
  }
}

void LinearLayout::onLayout(int16_t left, int16_t top, int16_t right,
                            int16_t bottom) {
  ViewGroup::onLayout(left, top, right, bottom);
  if (_orientation == VERTICAL) {
    // 垂直布局 - 从上边距开始
    int16_t currentPos = _top + getPaddingTop();
    int16_t maxBottom =
        _top + _height - getPaddingBottom(); // 最大允许的底部位置

    for (auto child : _children) {
      if (child->getVisibility() == GONE) {
        continue;
      }
      ESP_LOGI("LinearLayout", "layout: child->className() = %s",
               child->className().c_str());

      // int16_t childHeight = child->getHeight() > 0 ? child->getHeight() : 0;
      // // 如果高度未定义，默认为0
      auto childHeight = child->getHeight();
      auto childWidth = child->getWidth();
      auto right = std::min<int16_t>(_left + _width - getPaddingRight(),
                                     _left + childWidth + getPaddingLeft());
      // 确保不超出底部边界
      int16_t childBottom =
          std::min<int16_t>(currentPos + childHeight, maxBottom);
      if (currentPos < maxBottom) { // 只有在还有空间时才布局
        ESP_LOGI("LinearLayout",
                 "layout: child->className() = %s, currentPos = %d, "
                 "childBottom = %d",
                 child->className().c_str(), currentPos, childBottom);
        // 考虑左右padding
        child->layout(_left + getPaddingLeft(), currentPos, right, childBottom);

        // 更新当前位置
        currentPos = childBottom + _spacing;
      }
    }
  } else {
    // 水平布局 - 从左边距开始
    int16_t currentPos = _left + getPaddingLeft();
    int16_t maxRight = _left + _width - getPaddingRight(); // 最大允许的右边位置

    for (auto child : _children) {
      if (child->getVisibility() == GONE) {
        continue;
      }
      ESP_LOGV("LinearLayout", "layout: child->className() = %s",
               child->className().c_str());

      auto childHeight = child->getHeight();
      auto childWidth = child->getWidth();

      // 确保不超出右边界
      int16_t childRight = std::min<int16_t>(currentPos + childWidth, maxRight);
      if (currentPos < maxRight) { // 只有在还有空间时才布局
        // 考虑上下padding
        child->layout(currentPos, _top + getPaddingTop(), childRight,
                      _top + _height - getPaddingBottom());

        // 更新当前位置
        currentPos = childRight + _spacing;
      }
    }
  }
}

void LinearLayout::onDraw(m5gfx::M5GFX &display) {
  if (_visibility == GONE) {
    return;
  }
  View::onDraw(display);
  for (auto child : _children) {
    if (child->getVisibility() != GONE) {
      child->draw(display);
    }
  }
  _isDirty = false;

  // // 只有当自身或子视图需要重绘时才进行绘制
  // if (isDirty()) {
  //     // 确保视图组已正确布局
  //     layout(_left, _top, _left + _width, _top + _height);

  //     // 绘制自身（背景、边框等）- 使用View的绘制方法
  //     View::draw(display);

  //     // 绘制所有可见的子视图
  //     for (auto child : _children) {
  //         if (child->getVisibility() != GONE) {
  //             // 确保子视图只在内容区域内绘制
  //             child->draw(display);
  //         }
  //     }

  //     // 标记为已绘制，清除脏标记
  //     _isDirty = false;
  // }
}

void LinearLayout::onMeasure(int16_t widthMeasureSpec,
                             int16_t heightMeasureSpec) {
  // 首先测量自己
  View::onMeasure(widthMeasureSpec, heightMeasureSpec);

  int16_t maxWidth = 0;
  int16_t maxHeight = 0;
  int16_t totalLength = 0;
  int visibleChildrenCount = 0;

  // 统计可见子视图数量
  for (auto child : _children) {
    if (child->getVisibility() != GONE) {
      visibleChildrenCount++;
    }
  }

  // 测量子视图
  int processedChildren = 0;
  for (auto child : _children) {
    if (child->getVisibility() != GONE) {
      // 测量子视图
      child->measure(widthMeasureSpec, heightMeasureSpec);

      if (_orientation == VERTICAL) {
        // 垂直布局：累加高度，记录最大宽度
        totalLength += child->getHeight();
        maxWidth = std::max(maxWidth, child->getWidth());
        // 如果不是最后一个可见子视图，则添加间距
        if (++processedChildren < visibleChildrenCount) {
          totalLength += _spacing;
        }
      } else {
        // 水平布局：累加宽度，记录最大高度
        totalLength += child->getWidth();
        maxHeight = std::max(maxHeight, child->getHeight());
        // 如果不是最后一个可见子视图，则添加间距
        if (++processedChildren < visibleChildrenCount) {
          totalLength += _spacing;
        }
      }
    }
  }

  // 根据布局方向计算最终尺寸
  if (_orientation == VERTICAL) {
    maxHeight = std::max(_measuredHeight, static_cast<int16_t>(totalLength + getPaddingTop() + getPaddingBottom()));
    maxWidth = std::max(_measuredWidth, static_cast<int16_t>(maxWidth + getPaddingLeft() + getPaddingRight()));
  } else {
    maxWidth = std::max(_measuredWidth, static_cast<int16_t>(totalLength + getPaddingLeft() + getPaddingRight()));
    maxHeight = std::max(_measuredHeight, static_cast<int16_t>(maxHeight + getPaddingTop() + getPaddingBottom()));
  }

  _measuredWidth = maxWidth;
  _measuredHeight = maxHeight;
}