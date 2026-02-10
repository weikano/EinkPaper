#pragma once

#include "../ui_kit/View.h"
#include <memory>

#include "PageType.h"
#include "../gestures/TouchGestureDetector.h"

/**
 * @brief Page基类 - 所有页面的基础
 * 
 * 提供页面的基本功能管理
 */
class Page {
public:
    /**
     * @brief 构造函数
     * @param pageType 页面类型
     * @param pageName 页面名称（可选，用于调试）
     */
    Page(PageType pageType, const std::string& pageName = "");

    /**
     * @brief 析构函数
     */
    virtual ~Page();

    /**
     * @brief 获取页面类型
     * @return 页面类型
     */
    PageType getType() const { return _pageType; }

    /**
     * @brief 获取页面名称
     * @return 页面名称
     */
    const std::string& getName() const { return _pageName; }

    /**
     * @brief 获取页面根视图
     * @return 页面根视图指针
     */
    View* getRootView() const { return _rootView; }

    /**
     * @brief 设置页面根视图
     * @param rootView 页面根视图指针
     */
    void setRootView(View* rootView) { _rootView = rootView; }

    /**
     * @brief 设置页面参数
     * @param params 页面参数
     */
    void setParams(std::shared_ptr<void> params) { _params = params; }

    /**
     * @brief 获取页面参数
     * @return 页面参数
     */
    std::shared_ptr<void> getParams() const { return _params; }

    /**
     * @brief 检查页面是否需要重绘
     * @return 如果需要重绘返回true，否则返回false
     */
    bool isDirty() const;

    // 生命周期相关方法
    virtual void onCreate();
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onRestart();
    virtual void onDestroy();

    /**
     * @brief 绘制页面内容
     * @param display 显示对象
     */
    void draw(m5gfx::M5GFX& display);

    /**
     * @brief 处理点击事件
     * @param x X坐标
     * @param y Y坐标
     * @return 如果处理了事件返回true，否则返回false
     */
    bool onClick(int16_t x, int16_t y);

    void onSwipe(TouchGestureDetector::SwipeDirection direction);
private:
    virtual void onSwipeDispatched(TouchGestureDetector::SwipeDirection direction);
    PageType _pageType;                    ///< 页面类型
    std::string _pageName;                 ///< 页面名称
    View* _rootView = nullptr;             ///< 页面根视图
    std::shared_ptr<void> _params = nullptr; ///< 页面参数
};