#pragma once

#include "../page_manager/Page.h"
#include "../ui_kit/LinearLayout.h"

/**
 * @brief 设置页面类
 * 
 * 继承自Page类，提供设置功能
 */
class SettingsPage : public Page {
public:
    /**
     * @brief 构造函数
     */
    SettingsPage();

    /**
     * @brief 析构函数
     */
    ~SettingsPage();

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