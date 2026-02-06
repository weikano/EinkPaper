#pragma once

#include "../page_manager/Page.h"
#include "../ui_kit/LinearLayout.h"
#include "../ui_kit/Button.h"

/**
 * @brief 启动器页面类
 * 
 * 继承自Page类，提供应用启动界面，包含进入设置和文件浏览的入口
 */
class LauncherPage : public Page {
public:
    /**
     * @brief 构造函数
     */
    LauncherPage();

    /**
     * @brief 析构函数
     */
    ~LauncherPage();

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
    LinearLayout* _layout;      ///< 页面主布局
    Button* _settingsButton;    ///< 设置按钮
    Button* _fileBrowserButton; ///< 文件浏览器按钮
    Button* _messsageButton;    ///< 消息按钮
};