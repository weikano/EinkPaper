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
    FrameLayout(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * @brief 重写布局方法
     * @param left 左边界
     * @param top 上边界
     * @param right 右边界
     * @param bottom 下边界
     */
    virtual void layout(int16_t left, int16_t top, int16_t right, int16_t bottom) override;

    /**
     * @brief 重写测量方法
     * @param widthMeasureSpec 父容器提供的宽度约束
     * @param heightMeasureSpec 父容器提供的高度约束
     */
    virtual void measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;
};