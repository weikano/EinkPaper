#include "LauncherPage.h"
#include "page_manager/PageManager.h"
#include "esp_log.h"
#include "message/MessagePageHelper.h"

static const char* TAG = "LauncherPage";

LauncherPage::LauncherPage() 
    : Page(PageType::MENU, "Launcher"), _layout(nullptr), _settingsButton(nullptr), _fileBrowserButton(nullptr) {
    ESP_LOGI(TAG, "LauncherPage constructed");
}

LauncherPage::~LauncherPage() {
    ESP_LOGI(TAG, "LauncherPage destructed");
    // 不需要手动删除控件，因为它们是_layout 的子控件，会在_layout 析构时自动清理
}

void LauncherPage::onCreate() {
    ESP_LOGI(TAG, "LauncherPage onCreate");
    
    // 创建主布局
    auto screenWidth = M5.Display.width();
    auto screenHeight = M5.Display.height();
    _layout = new LinearLayout(screenWidth, screenHeight);
    _layout->setOrientation(LinearLayout::Orientation::VERTICAL);    
    _layout->setSpacing(20);  // 设置间距
    
    // 创建设置按钮
    _settingsButton = new Button(200, 60);
    _settingsButton->setText("进入设置");
    _settingsButton->setOnClickListener([this]() {
        ESP_LOGI(TAG, "Settings button clicked");
        // 使用PageManager跳转到设置页面
        PageManager::getInstance().startActivity(PageType::SETTINGS);
    });
    
    // 创建文件浏览器按钮
    _fileBrowserButton = new Button(200, 60);
    _fileBrowserButton->setText("文件浏览");
    _fileBrowserButton->setOnClickListener([this]() {
        ESP_LOGI(TAG, "File Browser button clicked");
        // 使用PageManager跳转到文件浏览器页面
        PageManager::getInstance().startActivity(PageType::FILE_BROWSER);
    });

    _messsageButton = new Button(200, 60);
    _messsageButton->setText("显示消息");
    _messsageButton->setOnClickListener([this]() {
        ESP_LOGI(TAG, "Message button clicked");
        // 显示消息页面
        showMessagePage("这是一个消息页面！");
    });
    
    // 添加按钮到布局
    _layout->addChild(_settingsButton);
    _layout->addChild(_fileBrowserButton);
    _layout->addChild(_messsageButton);
    
    // 设置页面根视图
    setRootView(_layout);
    
    // 调用父类方法
    Page::onCreate();
}

void LauncherPage::onStart() {
    ESP_LOGI(TAG, "LauncherPage onStart");
    Page::onStart();
}

void LauncherPage::onResume() {
    ESP_LOGI(TAG, "LauncherPage onResume");
    Page::onResume();
}

void LauncherPage::onPause() {
    ESP_LOGI(TAG, "LauncherPage onPause");
    Page::onPause();
}

void LauncherPage::onStop() {
    ESP_LOGI(TAG, "LauncherPage onStop");
    Page::onStop();
}

void LauncherPage::onDestroy() {
    ESP_LOGI(TAG, "LauncherPage onDestroy");
    // 清理资源
    _settingsButton = nullptr;
    _fileBrowserButton = nullptr;
    _messsageButton = nullptr;
    _layout = nullptr;
    Page::onDestroy();
}