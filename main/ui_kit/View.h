#pragma once

#include "M5Unified.h"
#include <functional>

// 前向声明
class ViewGroup;

/**
 * @brief View基类 - 所有UI控件的基础
 * 
 * 提供基本的绘制、布局和事件处理功能
 */
class View {
public:
    enum Visibility {
        VISIBLE = 0,
        INVISIBLE = 1,
        GONE = 2
    };

    /**
     * @brief 构造函数
     * @param width 宽度
     * @param height 高度
     */
    View(int16_t width, int16_t height);

    virtual ~View();

    /**
     * @brief 获取视图的X坐标
     * @return X坐标
     */
    int16_t getX() const { return _x; }

    /**
     * @brief 获取视图的Y坐标
     * @return Y坐标
     */
    int16_t getY() const { return _y; }

    /**
     * @brief 获取视图的宽度
     * @return 宽度
     */
    int16_t getWidth() const { return _width; }

    /**
     * @brief 获取视图的高度
     * @return 高度
     */
    int16_t getHeight() const { return _height; }

    /**
     * @brief 设置视图的位置
     * @param x X坐标
     * @param y Y坐标
     */
    void setPosition(int16_t x, int16_t y);

    /**
     * @brief 设置视图的尺寸
     * @param width 宽度
     * @param height 高度
     */
    void setSize(int16_t width, int16_t height);

    /**
     * @brief 设置视图的可见性
     * @param visibility 可见性状态
     */
    void setVisibility(Visibility visibility);

    /**
     * @brief 获取视图的可见性
     * @return 可见性状态
     */
    Visibility getVisibility() const { return _visibility; }

    /**
     * @brief 设置边框颜色
     * @param color 颜色值
     */
    void setBorderColor(uint32_t color);

    /**
     * @brief 设置边框宽度
     * @param width 边框宽度
     */
    void setBorderWidth(uint8_t width);

    /**
     * @brief 设置内边距
     * @param left 左侧内边距
     * @param top 顶部内边距
     * @param right 右侧内边距
     * @param bottom 底部内边距
     */
    void setPadding(uint8_t left, uint8_t top, uint8_t right, uint8_t bottom);

    /**
     * @brief 获取左侧内边距
     * @return 左侧内边距值
     */
    uint8_t getPaddingLeft() const { return _paddingLeft; }

    /**
     * @brief 获取顶部内边距
     * @return 顶部内边距值
     */
    uint8_t getPaddingTop() const { return _paddingTop; }

    /**
     * @brief 获取右侧内边距
     * @return 右侧内边距值
     */
    uint8_t getPaddingRight() const { return _paddingRight; }

    /**
     * @brief 获取底部内边距
     * @return 底部内边距值
     */
    uint8_t getPaddingBottom() const { return _paddingBottom; }

    /**
     * @brief 检查点是否在视图范围内
     * @param x X坐标
     * @param y Y坐标
     * @return 如果在范围内返回true，否则返回false
     */
    bool contains(int16_t x, int16_t y) const;

    /**
     * @brief 绘制视图
     * @param display 显示对象
     */
    virtual void draw(m5gfx::M5GFX& display);

    /**
     * @brief 实际执行绘制操作的内部方法
     * @param display 显示对象
     */
    virtual void onDraw(m5gfx::M5GFX& display);

    /**
     * @brief 测量视图所需的空间
     * @param widthMeasureSpec 父容器提供的宽度约束
     * @param heightMeasureSpec 父容器提供的高度约束
     */
    virtual void measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec);

    /**
     * @brief 布局视图
     * @param left 左边界
     * @param top 上边界
     * @param right 右边界
     * @param bottom 下边界
     */
    virtual void layout(int16_t left, int16_t top, int16_t right, int16_t bottom);

    /**
     * @brief 实际执行布局操作的内部方法
     * @param left 左边界
     * @param top 上边界
     * @param right 右边界
     * @param bottom 下边界
     */
    virtual void onLayout(int16_t left, int16_t top, int16_t right, int16_t bottom);

    /**
     * @brief 处理触摸事件
     * @param x X坐标
     * @param y Y坐标
     * @return 如果处理了事件返回true，否则返回false
     */
    virtual bool onTouch(int16_t x, int16_t y);

    /**
     * @brief 设置点击回调函数
     * @param callback 回调函数
     */
    void setOnClickListener(std::function<void()> callback);

    /**
     * @brief 检查视图是否需要重绘
     * @return 如果需要重绘返回true，否则返回false
     */
    virtual bool isDirty() const { return _isDirty; }

    /**
     * @brief 标记视图为需要重绘
     */
    void markDirty();

    /**
     * @brief 获取最后绘制时间
     * @return 最后绘制时间戳
     */
    int32_t getLastDrawTime() const { return _lastDrawTime; }



    /**
     * @brief 通知父视图需要重绘
     */
    virtual void notifyParentOfChange();
    
    /**
     * @brief 设置父视图
     * @param parent 父视图指针
     */
    void setParent(View* parent) { _parent = parent; }

    /**
     * @brief 强制标记整个视图树为需要重绘
     */
    virtual void forceRedraw();

    virtual std::string className() const { return "View"; }

protected:
    int16_t _x = 0, _y = 0;           ///< 视图的左上角坐标
    int16_t _width = 0, _height = 0;  ///< 视图的宽高
    Visibility _visibility = VISIBLE;   ///< 可见性状态
    uint32_t _borderColor = TFT_BLACK;    ///< 边框颜色
    uint8_t _borderWidth = 1;     ///< 边框宽度
    uint8_t _paddingLeft = 0;     ///< 左侧内边距
    uint8_t _paddingTop = 0;      ///< 顶部内边距
    uint8_t _paddingRight = 0;    ///< 右侧内边距
    uint8_t _paddingBottom = 0;   ///< 底部内边距
    bool _isPressed = false;          ///< 是否被按下
    std::function<void()> _clickCallback = nullptr; ///< 点击回调函数
    bool _isDirty = true;            ///< 是否需要重绘
    int32_t _lastDrawTime = 0;    ///< 最后绘制时间戳
    View* _parent = nullptr;            ///< 父视图指针
};