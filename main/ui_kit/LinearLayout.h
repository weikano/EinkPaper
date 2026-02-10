#pragma once

#include "ViewGroup.h"

/**
 * @brief LinearLayout - 线性布局
 * 
 * 将子视图按水平或垂直方向排列
 */
class LinearLayout : public ViewGroup {
public:
    enum Orientation {
        HORIZONTAL = 0,  ///< 水平排列
        VERTICAL = 1     ///< 垂直排列
    };

    /**
     * @brief 构造函数
     * @param width 宽度
     * @param height 高度
     * @param orientation 布局方向
     */
    LinearLayout(int16_t width, int16_t height, Orientation orientation = VERTICAL);

    /**
     * @brief 设置布局方向
     * @param orientation 排列方向
     */
    void setOrientation(Orientation orientation);

    /**
     * @brief 设置子视图之间的间距
     * @param spacing 间距
     */
    void setSpacing(int16_t spacing);

    std::string className() const override { return "LinearLayout"; }
protected:
    void onDraw(m5gfx::M5GFX& display) override;
    void onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;
    /**
     * @brief 重写布局方法
     * @param left 左边界
     * @param top 上边界
     * @param right 右边界
     * @param bottom 下边界
     */
    virtual void onLayout(int16_t left, int16_t top, int16_t right, int16_t bottom) override;

private:
    Orientation _orientation;  ///< 布局方向
    int16_t _spacing;          ///< 子视图间距
};