#pragma once

#include "ViewGroup.h"
#include <vector>
#include <functional>

/**
 * @brief ListView - 列表视图
 * 
 * 显示可滚动的项目列表
 */
class ListView : public ViewGroup {
public:
    /**
     * @brief 项目点击回调类型
     */
    typedef std::function<void(int index)> OnItemClickListener;

    /**
     * @brief 构造函数
     * @param width 宽度
     * @param height 高度
     */
    ListView(int16_t width, int16_t height);

    /**
     * @brief 设置数据项
     * @param items 数据项列表
     */
    void setItems(const std::vector<std::string>& items);

    /**
     * @brief 获取数据项数量
     * @return 数据项数量
     */
    size_t getItemCount() const;

    /**
     * @brief 设置显示行数
     * @param rowCount 显示行数
     */
    void setRowCount(int16_t rowCount);

    /**
     * @brief 获取显示行数
     * @return 显示行数
     */
    int16_t getRowCount() const;

    /**
     * @brief 设置滚动偏移
     * @param scrollOffset 滚动偏移
     */
    void setScrollOffset(int16_t scrollOffset);

    /**
     * @brief 获取滚动偏移
     * @return 滚动偏移
     */
    int16_t getScrollOffset() const;

    /**
     * @brief 设置项目点击监听器
     * @param listener 监听器函数
     */
    void setOnItemClickListener(OnItemClickListener listener);

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
     * @brief 重写测量方法
     * @param widthMeasureSpec 父容器提供的宽度约束
     * @param heightMeasureSpec 父容器提供的高度约束
     */
    virtual void measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;

    /**
     * @brief 重写布局方法
     * @param left 左边界
     * @param top 上边界
     * @param right 右边界
     * @param bottom 下边界
     */
    virtual void layout(int16_t left, int16_t top, int16_t right, int16_t bottom) override;

    /**
     * @brief 获取类名
     * @return 类名字符串
     */
    virtual std::string className() const override { return "ListView"; }

private:
    std::vector<std::string> _items;           ///< 数据项列表
    int16_t _rowCount;                         ///< 显示行数
    int16_t _scrollOffset;                     ///< 滚动偏移
    OnItemClickListener _itemClickListener;    ///< 项目点击监听器
};