#pragma once

#include "../page_manager/Page.h"
#include "../ui_kit/LinearLayout.h"

/**
 * @brief 文件浏览器页面类
 * 
 * 继承自Page类，提供文件浏览功能
 */
class FileBrowserPage : public Page {
public:
    /**
     * @brief 构造函数
     */
    FileBrowserPage();

    /**
     * @brief 析构函数
     */
    ~FileBrowserPage();

    /**
     * @brief 页面创建回调
     */
    void onCreate() override;

    /**
     * @brief 页面启动回调
     */
    void onStart() override;

    /**
     * @brief 页面恢复回调
     */
    void onResume() override;

    /**
     * @brief 页面暂停回调
     */
    void onPause() override;

    /**
     * @brief 页面停止回调
     */
    void onStop() override;

    /**
     * @brief 页面销毁回调
     */
    void onDestroy() override;

private:
    LinearLayout* _layout;  ///< 页面布局
};