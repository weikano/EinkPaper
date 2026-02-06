#include "MessagePage.h"
#include "esp_log.h"
#include "M5Unified.h"

static const char* TAG = "MessagePage";

MessagePage::MessagePage() 
    : Page(PageType::MESSAGE, "Message"), 
      _layout(nullptr), 
      _textView(nullptr), 
      _message("") {
    ESP_LOGI(TAG, "MessagePage constructed");
}

MessagePage::~MessagePage() {
    ESP_LOGI(TAG, "MessagePage destructed");
    
    // 清理资源
    if (_layout) {
        delete _layout;
        _layout = nullptr;
    }
}

void MessagePage::onCreate() {
    ESP_LOGI(TAG, "MessagePage onCreate");
    
    // 从参数中获取消息内容
    auto param = std::static_pointer_cast<std::string>(getParams());
    if (param) {
        _message = *param;
    }
    
    // 获取屏幕尺寸
    auto screenWidth = M5.Display.width();
    auto screenHeight = M5.Display.height();
    
    // 创建主布局
    _layout = new FrameLayout(screenWidth, screenHeight);
    
    // 创建文本显示控件
    _textView = new TextView(screenWidth, screenHeight);
    _textView->setText(_message.c_str());
    _textView->setTextColor(TFT_BLACK);
    _textView->setTextSize(2);  // 默认字体大小
    _textView->setTextAlign(1); // 居中对齐
    _textView->setPadding(20, 20, 20, 20); // 设置内边距
    
    // 将文本视图添加到布局中
    _layout->addChild(_textView);
    
    // 设置页面根视图
    setRootView(_layout);
    
    // 调用父类方法
    Page::onCreate();
}

void MessagePage::onStart() {
    ESP_LOGI(TAG, "MessagePage onStart");
    Page::onStart();
}

void MessagePage::onResume() {
    ESP_LOGI(TAG, "MessagePage onResume");
    Page::onResume();
}

void MessagePage::onPause() {
    ESP_LOGI(TAG, "MessagePage onPause");
    Page::onPause();
}

void MessagePage::onStop() {
    ESP_LOGI(TAG, "MessagePage onStop");
    Page::onStop();
}

void MessagePage::onDestroy() {
    ESP_LOGI(TAG, "MessagePage onDestroy");
    Page::onDestroy();
}

void MessagePage::setMessage(const std::string& message) {
    _message = message;
    
    if (_textView) {
        _textView->setText(message.c_str());
        _textView->markDirty(); // 标记为需要重绘
    }
    
    ESP_LOGD(TAG, "Message updated to: %s", message.c_str());
}

std::string MessagePage::getMessage() const {
    return _message;
}