#pragma once

#include "Page.h"
#include <deque>
#include <memory>
#include <unordered_map>
#include <functional>
#include "../gestures/TouchGestureDetector.h"

/**
 * @brief 页面管理器 - 管理页面栈和页面生命周期
 * 
 * 类似Android的Activity管理机制，负责页面的创建、切换和生命周期管理
 */
class PageManager {
public:
    /**
     * @brief 获取单例实例
     * @return PageManager单例实例引用
     */
    static PageManager& getInstance();

    /**
     * @brief 构造函数（私有，防止外部直接创建实例）
     */
    PageManager();

    /**
     * @brief 析构函数
     */
    ~PageManager();
    
    // 删除拷贝构造和赋值操作符，确保单例特性
    PageManager(const PageManager&) = delete;
    PageManager& operator=(const PageManager&) = delete;

    /**
     * @brief 注册页面工厂函数
     * @param pageType 页面类型
     * @param factory 页面创建工厂函数
     */
    void registerPage(PageType pageType, 
                      std::function<std::unique_ptr<Page>()> factory);

    /**
     * @brief 启动指定页面
     * @param pageType 页面类型
     * @param params 页面参数（可选）
     */
    void startActivity(PageType pageType, 
                       std::shared_ptr<void> params = nullptr);

    /**
     * @brief 返回到上一个页面（模拟返回键）
     */
    void goBack();

    /**
     * @brief 清空页面栈并启动指定页面
     * @param pageType 页面类型
     * @param params 页面参数（可选）
     */
    void startActivityClearTop(PageType pageType, 
                              std::shared_ptr<void> params = nullptr);

    /**
     * @brief 绘制当前页面
     * @param display 显示对象
     */
    void draw(m5gfx::M5GFX& display);

    /**
     * @brief 处理点击事件
     * @param x X坐标
     * @param y Y坐标
     * @return 如果事件被处理返回true，否则返回false
     */
    bool onClick(int16_t x, int16_t y);

    /**
     * @brief 获取当前页面
     * @return 当前页面指针，如果无页面则返回nullptr
     */
    Page* getCurrentPage();

    /**
     * @brief 获取当前页面名称
     * @return 当前页面名称字符串
     */
    std::string getCurrentPageName();

    /**
     * @brief 检查是否有页面在栈中
     * @return 如果有页面返回true，否则返回false
     */
    bool hasPages() const { return !_pageStack.empty(); }

    /**
     * @brief 获取页面栈大小
     * @return 页面栈中页面数量
     */
    size_t getPageStackSize() const { return _pageStack.size(); }

    /**
     * @brief 检查指定类型页面是否在栈中
     * @param pageType 页面类型
     * @return 如果存在返回true，否则返回false
     */
    bool isPageInStack(PageType pageType) const;

    /**
     * @brief 获取页面栈中特定类型的页面数量
     * @param pageType 页面类型
     * @return 页面数量
     */
    size_t getPageCountByType(PageType pageType) const;
    
    /**
     * @brief 销毁页面管理器
     */
    void destroy();
    
    /**
     * @brief 检查当前页面是否需要重绘
     * @return 如果需要重绘返回true，否则返回false
     */
    bool isDirty();

    void onSwipe(TouchGestureDetector::SwipeDirection direction);

private:
    /**
     * @brief 暂停当前页面
     */
    void pauseCurrentPage();

    /**
     * @brief 恢复指定页面
     * @param page 页面指针
     */
    void resumePage(Page* page);

    /**
     * @brief 停止指定页面
     * @param page 页面指针
     */
    void stopPage(Page* page);

    /**
     * @brief 销毁指定页面
     * @param page 页面指针
     */
    void destroyPage(Page* page);

    std::unordered_map<PageType, std::function<std::unique_ptr<Page>()>> _pageFactories;  ///< 页面工厂映射表
    std::deque<std::unique_ptr<Page>> _pageStack;  ///< 页面栈（使用deque实现，支持遍历）
    bool _pageTransitionOccurred = false;  ///< 页面转换标志
};