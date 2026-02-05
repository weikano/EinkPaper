#include "SettingsPage.h"
#include "page_manager/PageManager.h"
#include "esp_log.h"

static const char* TAG = "SettingsPage";

SettingsPage::SettingsPage() 
    : Page(PageType::SETTINGS, "Settings"), _layout(nullptr) {
    ESP_LOGI(TAG, "SettingsPage constructed");
}

SettingsPage::~SettingsPage() {
    ESP_LOGI(TAG, "SettingsPage destructed");
    if (_layout) {
        delete _layout;
        _layout = nullptr;
    }
}

void SettingsPage::onCreate() {
    ESP_LOGI(TAG, "SettingsPage onCreate");
    
    // 创建主布局
    auto screenWidth = M5.Lcd.width();
    auto screenHeight = M5.Lcd.height();
    _layout = new LinearLayout(screenWidth, screenHeight);
    _layout->setOrientation(LinearLayout::Orientation::VERTICAL);
    
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