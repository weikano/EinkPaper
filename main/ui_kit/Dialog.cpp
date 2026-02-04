#include "Dialog.h"
#include <algorithm>

Dialog::Dialog(m5gfx::M5GFX& display)
    : FrameLayout(0, 0), _display(display), _isShowing(false), 
      _titleView(nullptr), _messageView(nullptr) {
    // 初始化按钮状态
    for (int i = 0; i < 3; i++) {
        _buttonTexts[i] = "";
        _buttonsVisible[i] = false;
        _buttons[i] = nullptr;
    }
}

void Dialog::setTitle(const std::string& title) {
    _title = title;
    if (!_titleView) {
        _titleView = new TextView(0, 0);
        _titleView->setTextSize(2);
        _titleView->setTextColor(TFT_BLACK);
        _titleView->setTextAlign(1); // 居中对齐
        addChild(_titleView);
    }
    _titleView->setText(title);
}

void Dialog::setMessage(const std::string& message) {
    _message = message;
    if (!_messageView) {
        _messageView = new TextView(0, 0);
        _messageView->setTextColor(TFT_BLACK);
        addChild(_messageView);
    }
    _messageView->setText(message);
}

void Dialog::setButton(ButtonId buttonId, const std::string& text) {
    _buttonTexts[buttonId] = text;
    _buttonsVisible[buttonId] = !text.empty();
    
    if (!text.empty() && !_buttons[buttonId]) {
        _buttons[buttonId] = new Button(0, 0);
        _buttons[buttonId]->setText(text);
        addChild(_buttons[buttonId]);
        
        // 设置按钮点击事件
        int id = buttonId; // 保存id到lambda中
        _buttons[buttonId]->setOnClickListener([this, id]() {
            if (_buttonClickListener) {
                _buttonClickListener(id);
            }
            dismiss(); // 点击按钮后自动隐藏对话框
        });
    } else if (text.empty() && _buttons[buttonId]) {
        removeChild(_buttons[buttonId]);
        delete _buttons[buttonId];
        _buttons[buttonId] = nullptr;
        _buttonsVisible[buttonId] = false;
    }
}

void Dialog::setOnButtonClickListener(OnButtonClickListener listener) {
    _buttonClickListener = listener;
}

void Dialog::show() {
    // 计算对话框尺寸和位置（居中显示）
    int16_t dialogWidth = std::min<int16_t>((int16_t)_display.width() - 40, (int16_t)300);
    int16_t dialogHeight = 150; // 初始高度，后续根据内容调整
    
    // 重新计算尺寸
    int16_t titleHeight = _titleView ? 30 : 0;
    int16_t messageHeight = _messageView ? 60 : 0;
    int16_t buttonHeight = 30;
    
    // 计算实际对话框高度
    dialogHeight = titleHeight + messageHeight + buttonHeight + 30; // 添加padding
    
    int16_t dialogX = (_display.width() - dialogWidth) / 2;
    int16_t dialogY = (_display.height() - dialogHeight) / 2;
    
    // 设置对话框位置和尺寸
    setPosition(dialogX, dialogY);
    setSize(dialogWidth, dialogHeight);
    
    // 设置背景色
    setBackgroundColor(TFT_WHITE);
    setBorderWidth(2);
    setBorderColor(TFT_BLACK);
    
    // 布局子视图
    if (_titleView) {
        _titleView->setPosition(dialogX + 10, dialogY + 10);
        _titleView->setSize(dialogWidth - 20, 25);
    }
    
    if (_messageView) {
        _messageView->setPosition(dialogX + 10, dialogY + 10 + (titleHeight > 0 ? 30 : 0));
        _messageView->setSize(dialogWidth - 20, 50);
    }
    
    // 计算按钮布局
    int visibleButtons = 0;
    for (int i = 0; i < 3; i++) {
        if (_buttonsVisible[i]) {
            visibleButtons++;
        }
    }
    
    if (visibleButtons > 0) {
        int buttonWidth = (dialogWidth - 20 - (visibleButtons - 1) * 10) / visibleButtons;
        int currentX = dialogX + 10;
        
        for (int i = 0; i < 3; i++) {
            if (_buttons[i] && _buttonsVisible[i]) {
                _buttons[i]->setPosition(currentX, dialogY + dialogHeight - 40);
                _buttons[i]->setSize(buttonWidth, 30);
                currentX += buttonWidth + 10;
            }
        }
    }
    
    _isShowing = true;
}

void Dialog::dismiss() {
    _isShowing = false;
}

bool Dialog::isShowing() const {
    return _isShowing;
}

void Dialog::draw(m5gfx::M5GFX& display) {
    if (_isShowing) {
        // 绘制半透明遮罩
        display.fillRect(0, 0, display.width(), display.height(), 0x80000000 | (TFT_BLACK & 0xFFFFFF));
        
        // 绘制对话框本身
        FrameLayout::draw(display);
    }
}

bool Dialog::onTouch(int16_t x, int16_t y) {
    if (!_isShowing) {
        return false;
    }
    
    // 检查是否点击在对话框外部
    if (!contains(x, y)) {
        // 点击外部区域，隐藏对话框
        dismiss();
        return true;
    }
    
    // 否则处理对话框内部的触摸事件
    return FrameLayout::onTouch(x, y);
}