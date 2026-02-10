#pragma once

#include "ViewGroup.h"

/**
 * @brief FrameLayout - 帧布局
 * 
 * 所有子视图都放置在左上角，后面的视图覆盖前面的视图
 */
class FrameLayout : public ViewGroup {
public:
    /**
     * @brief 构造函数
     * @param x X坐标
     * @param y Y坐标
     * @param width 宽度
     * @param height 高度
     */
    FrameLayout(int16_t width, int16_t height);

    std::string className() const override { return "FrameLayout"; }
protected:
    void onLayout(int16_t left, int16_t top, int16_t right, int16_t bottom) override;
    void onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;
};