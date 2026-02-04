#pragma once

#include "View.h"
#include <string>

/**
 * @brief TextView - 文本显示控件
 * 
 * 用于显示文本内容
 */
class TextView : public View {
public:
    /**
     * @brief 构造函数
     * @param width 宽度
     * @param height 高度
     */
    TextView(int16_t width, int16_t height);

    /**
     * @brief 设置文本内容
     * @param text 文本内容
     */
    void setText(const std::string& text);

    /**
     * @brief 获取文本内容
     * @return 文本内容
     */
    const std::string& getText() const;

    /**
     * @brief 设置文本颜色
     * @param color 颜色值
     */
    void setTextColor(uint32_t color);

    /**
     * @brief 设置文本大小
     * @param size 文本大小
     */
    void setTextSize(uint8_t size);

    /**
     * @brief 设置文本对齐方式
     * @param align 对齐方式 (0=左对齐, 1=居中, 2=右对齐)
     */
    void setTextAlign(uint8_t align);

    /**
     * @brief 重写绘制方法
     * @param display 显示对象
     */
    virtual void draw(m5gfx::M5GFX& display) override;

    /**
     * @brief 重写测量方法
     * @param widthMeasureSpec 父容器提供的宽度约束
     * @param heightMeasureSpec 父容器提供的高度约束
     */
    virtual void measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;

    /**
     * @brief 获取类名
     * @return 类名字符串
     */
    virtual std::string className() const override { return "TextView"; }

private:
    std::string _text;        ///< 文本内容
    uint32_t _textColor = TFT_BLACK;      ///< 文本颜色
    uint8_t _textSize;        ///< 文本大小
    uint8_t _textAlign;       ///< 文本对齐方式
};