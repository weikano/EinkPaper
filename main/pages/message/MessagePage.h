#pragma once

#include "../page_manager/Page.h"
#include "../ui_kit/FrameLayout.h"
#include "../ui_kit/TextView.h"

/**
 * @brief 消息页面 - 用于显示全屏文本消息
 * 
 * 继承自Page类，提供全屏文本显示功能
 */
class MessagePage : public Page {
public:
    /**
     * @brief 构造函数
     */
    MessagePage();

    /**
     * @brief 析构函数
     */
    ~MessagePage();

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

    /**
     * @brief 更新显示的消息文本
     * @param message 新的消息文本
     */
    void setMessage(const std::string& message);

    /**
     * @brief 获取当前显示的消息文本
     * @return 当前消息文本
     */
    std::string getMessage() const;    

private:    
    FrameLayout* _layout;     ///< 页面布局
    TextView* _textView;      ///< 文本显示控件
    std::string _message;     ///< 要显示的消息内容
};