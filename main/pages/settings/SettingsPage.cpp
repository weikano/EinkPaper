#include "SettingsPage.h"
#include "page_manager/PageManager.h"
#include "esp_log.h"

static const char* TAG = "SettingsPage";

SettingsPage::SettingsPage() 
    : Page(PageType::SETTINGS, "Settings"), _layout(nullptr), _backButton(nullptr) {
    ESP_LOGI(TAG, "SettingsPage constructed");
}

SettingsPage::~SettingsPage() {
    ESP_LOGI(TAG, "SettingsPage destructed");
    // 不需要手动删除控件，因为它们是_layout 的子控件，会在_layout 析构时自动清理
}

void SettingsPage::onCreate() {
    ESP_LOGI(TAG, "SettingsPage onCreate");
    
    // 创建主布局
    auto screenWidth = M5.Display.width();
    auto screenHeight = M5.Display.height();
    _layout = new FrameLayout(screenWidth, screenHeight);
    
    // 创建返回按钮
    _backButton = new Button(200, 60);
    _backButton->setText("返回");
    _backButton->setOnClickListener([this]() {
        ESP_LOGI(TAG, "Back button clicked");
        // 使用PageManager返回上一个页面
        PageManager::getInstance().goBack();
    });
    
    // 手动设置按钮居中位置
    int16_t buttonX = (screenWidth - _backButton->getWidth()) / 2;
    int16_t buttonY = (screenHeight - _backButton->getHeight()) / 2;
    _backButton->setPosition(buttonX, buttonY);
    
    // 添加按钮到布局
    _layout->addChild(_backButton);
    
    // 设置页面根视图
    setRootView(_layout);
    
    // 调用父类方法
    Page::onCreate();
}

void SettingsPage::onStart() {
    ESP_LOGI(TAG, "SettingsPage onStart");
    Page::onStart();
}

void SettingsPage::onResume() {
    ESP_LOGI(TAG, "SettingsPage onResume");
    // 可以在这里加载设置数据
    Page::onResume();
}

void SettingsPage::onPause() {
    ESP_LOGI(TAG, "SettingsPage onPause");
    // 可以在这里保存设置数据
    Page::onPause();
}

void SettingsPage::onStop() {
    ESP_LOGI(TAG, "SettingsPage onStop");
    Page::onStop();
}

void SettingsPage::onDestroy() {
    ESP_LOGI(TAG, "SettingsPage onDestroy");
    Page::onDestroy();
}