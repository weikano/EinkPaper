#pragma once

#include "ViewGroup.h"
#include <vector>
#include <functional>
#include <memory>

/**
 * @brief 分页列表视图 - 专为墨水屏优化
 * 
 * 提供分页浏览功能，避免滚动操作，适用于墨水屏设备
 */
class PagedListView : public ViewGroup {
public:
    /**
     * @brief 项目点击回调类型
     */
    typedef std::function<void(int index)> OnItemClickListener;

    /**
     * @brief 数据源加载回调类型
     */
    typedef std::function<std::vector<std::string>(int page, int pageSize)> DataSourceLoader;

    /**
     * @brief 项目绘制回调类型
     */
    typedef std::function<void(m5gfx::M5GFX& display, int index, const std::string& item, 
                              int16_t x, int16_t y, int16_t width, int16_t height)> ItemRenderer;

    /**
     * @brief 页码变化回调类型
     */
    typedef std::function<void(int currentPage, int totalPages)> OnPageChangeListener;

    /**
     * @brief 返回按钮点击回调类型
     */
    typedef std::function<void()> OnBackCallback;

    /**
     * @brief 构造函数
     * @param width 宽度
     * @param height 高度
     */
    PagedListView(int16_t width, int16_t height);

    /**
     * @brief 设置显示行数
     * @param rowCount 每页显示的行数
     */
    void setRowCount(int16_t rowCount);

    /**
     * @brief 获取显示行数
     * @return 每页显示的行数
     */
    int16_t getRowCount() const;

    /**
     * @brief 设置每页显示的列数
     * @param columnCount 每页显示的列数
     */
    void setColumnCount(int16_t columnCount);

    /**
     * @brief 获取每页显示的列数
     * @return 每页显示的列数
     */
    int16_t getColumnCount() const;

    /**
     * @brief 设置水平间距
     * @param spacing 水平间距
     */
    void setHorizontalSpacing(int16_t spacing);

    /**
     * @brief 获取水平间距
     * @return 水平间距
     */
    int16_t getHorizontalSpacing() const;

    /**
     * @brief 设置垂直间距
     * @param spacing 垂直间距
     */
    void setVerticalSpacing(int16_t spacing);

    /**
     * @brief 获取垂直间距
     * @return 垂直间距
     */
    int16_t getVerticalSpacing() const;

    /**
     * @brief 设置数据源加载器
     * @param loader 数据源加载回调函数
     */
    void setDataSourceLoader(DataSourceLoader loader);

    /**
     * @brief 设置项目渲染器
     * @param renderer 项目渲染回调函数
     */
    void setItemRenderer(ItemRenderer renderer);

    /**
     * @brief 设置项目点击监听器
     * @param listener 监听器函数
     */
    void setOnItemClickListener(OnItemClickListener listener);

    /**
     * @brief 设置页码变化监听器
     * @param listener 监听器函数
     */
    void setOnPageChangeListener(OnPageChangeListener listener);

    /**
     * @brief 设置返回按钮回调
     * @param callback 回调函数
     */
    void setOnBackCallback(OnBackCallback callback);

    /**
     * @brief 设置当前页码
     * @param page 页码（从0开始）
     */
    void setCurrentPage(int page);

    /**
     * @brief 获取当前页码
     * @return 当前页码（从0开始）
     */
    int getCurrentPage() const;

    /**
     * @brief 获取总页数
     * @return 总页数
     */
    int getTotalPages() const;

    /**
     * @brief 设置总项目数
     * @param totalItems 总项目数
     */
    void setTotalItems(int totalItems);

    /**
     * @brief 获取总项目数
     * @return 总项目数
     */
    int getTotalItems() const;

    /**
     * @brief 刷新当前页数据
     */
    void refreshData();

    /**
     * @brief 跳转到下一页
     * @return 是否成功跳转
     */
    bool nextPage();

    /**
     * @brief 跳转到上一页
     * @return 是否成功跳转
     */
    bool prevPage();

    /**
     * @brief 重写触摸处理方法
     * @param x X坐标
     * @param y Y坐标
     * @return 如果处理了事件返回true，否则返回false
     */
    virtual bool onTouch(int16_t x, int16_t y) override;

    /**
     * @brief 重写滑动处理方法
     * @param direction 滑动方向
     * @return 如果处理了事件返回true，否则返回false
     */
    virtual bool onSwipe(TouchGestureDetector::SwipeDirection direction) override;        

    /**
     * @brief 获取类名
     * @return 类名字符串
     */
    virtual std::string className() const override { return "PagedListView"; }
protected:
    void onDraw(m5gfx::M5GFX& display) override;
    void onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;
private:
    int16_t _rowCount;                           ///< 每页显示的行数
    int16_t _columnCount;                        ///< 每页显示的列数
    int16_t _horizontalSpacing;                  ///< 水平间距
    int16_t _verticalSpacing;                    ///< 垂直间距
    int _currentPage;                            ///< 当前页码
    int _totalPages;                             ///< 总页数
    int _totalItems;                             ///< 总项目数
    std::vector<std::string> _currentPageItems;  ///< 当前页的数据项
    DataSourceLoader _dataSourceLoader;          ///< 数据源加载器
    ItemRenderer _itemRenderer;                  ///< 项目渲染器
    OnItemClickListener _itemClickListener;      ///< 项目点击监听器
    OnPageChangeListener _pageChangeListener;    ///< 页码变化监听器
    OnBackCallback _backCallback;                ///< 返回按钮回调
    
    void _calculatePageSize();
    void _loadCurrentPageData();
    int16_t _getItemX(int index) const;
    int16_t _getItemY(int index) const;
    int16_t _getItemWidth() const;
    int16_t _getItemHeight() const;
    int _getIndexFromPosition(int16_t x, int16_t y) const;
};