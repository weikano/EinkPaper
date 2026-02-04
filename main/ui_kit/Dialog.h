#pragma once

#include "FrameLayout.h"
#include "Button.h"
#include "TextView.h"
#include <functional>

/**
 * @brief Dialog - 对话框控件
 * 
 * 提供简单的对话框功能
 */
class Dialog : public FrameLayout {
public:
    /**
     * @brief 对话框按钮点击回调类型
     */
    typedef std::function<void(int buttonId)> OnButtonClickListener;

    /**
     * @brief 按钮ID枚举
     */
    enum ButtonId {
        BUTTON_POSITIVE = 0,
        BUTTON_NEGATIVE = 1,
        BUTTON_NEUTRAL = 2
    };

    /**
     * @brief 构造函数
     * @param display 显示对象引用
     */
    Dialog(m5gfx::M5GFX& display);

    /**
     * @brief 设置对话框标题
     * @param title 标题文本
     */
    void setTitle(const std::string& title);

    /**
     * @brief 设置对话框消息
     * @param message 消息文本
     */
    void setMessage(const std::string& message);

    /**
     * @brief 设置按钮文本
     * @param buttonId 按钮ID
     * @param text 按钮文本
     */
    void setButton(ButtonId buttonId, const std::string& text);

    /**
     * @brief 设置按钮点击监听器
     * @param listener 监听器函数
     */
    void setOnButtonClickListener(OnButtonClickListener listener);

    /**
     * @brief 显示对话框
     */
    void show();

    /**
     * @brief 隐藏对话框
     */
    void dismiss();

    /**
     * @brief 检查对话框是否正在显示
     * @return 如果正在显示返回true，否则返回false
     */
    bool isShowing() const;

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

private:
    m5gfx::M5GFX& _display;                    ///< 显示对象引用
    std::string _title;                        ///< 标题
    std::string _message;                      ///< 消息
    std::string _buttonTexts[3];               ///< 按钮文本
    bool _buttonsVisible[3];                   ///< 按钮可见性
    OnButtonClickListener _buttonClickListener; ///< 按钮点击监听器
    bool _isShowing;                           ///< 是否正在显示
    TextView* _titleView;                      ///< 标题视图
    TextView* _messageView;                    ///< 消息视图
    Button* _buttons[3];                       ///< 按钮数组
};