#pragma once

#include "TextView.h"

/**
 * @brief Button - 按钮控件
 * 
 * 继承自TextView，具有点击效果
 */
class Button : public TextView {
public:
    /**
     * @brief 构造函数
     * @param width 宽度
     * @param height 高度
     */
    Button(int16_t width, int16_t height);

    /**
     * @brief 设置按钮按下时的背景颜色
     * @param color 颜色值
     */
    void setPressedColor(uint32_t color);

    /**
     * @brief 重写绘制方法
     * @param display 显示对象
     */
    virtual void draw(m5gfx::M5GFX& display) override;

    /**
     * @brief 重写触摸处理方法
     * @param x X坐标
     * @param y Y坐标
     * @return 如果处理了事件返回true，否则返回false
     */
    virtual bool onTouch(int16_t x, int16_t y) override;

    /**
     * @brief 获取类名
     * @return 类名字符串
     */
    virtual std::string className() const override { return "Button"; }

private:
    uint32_t _pressedColor;   ///< 按下时的背景颜色
};