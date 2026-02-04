#pragma once

#include "View.h"
#include <vector>

/**
 * @brief ViewGroup - 可包含子视图的容器基类
 * 
 * 提供子视图管理功能
 */
class ViewGroup : public View {
public:
    /**
     * @brief 构造函数
     * @param x X坐标
     * @param y Y坐标
     * @param width 宽度
     * @param height 高度
     */
    ViewGroup(int16_t x, int16_t y, int16_t width, int16_t height);

    virtual ~ViewGroup();

    /**
     * @brief 添加子视图
     * @param child 子视图指针
     */
    void addChild(View* child);

    /**
     * @brief 移除子视图
     * @param child 子视图指针
     */
    void removeChild(View* child);

    /**
     * @brief 移除所有子视图
     */
    void removeAllChildren();

    /**
     * @brief 获取子视图数量
     * @return 子视图数量
     */
    size_t getChildCount() const;

    /**
     * @brief 获取指定索引的子视图
     * @param index 索引
     * @return 子视图指针
     */
    View* getChildAt(size_t index) const;

    /**
     * @brief 重写绘制方法，同时绘制子视图
     * @param display 显示对象
     */
    virtual void draw(m5gfx::M5GFX& display) override;

    /**
     * @brief 重写触摸处理方法，传递给子视图
     * @param x X坐标
     * @param y Y坐标
     * @return 如果处理了事件返回true，否则返回false
     */
    virtual bool onTouch(int16_t x, int16_t y) override;

    /**
     * @brief 重写测量方法，测量所有子视图
     * @param widthMeasureSpec 父容器提供的宽度约束
     * @param heightMeasureSpec 父容器提供的高度约束
     */
    virtual void measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;

protected:
    std::vector<View*> _children;  ///< 子视图列表
};