#include "FileBrowserPage.h"
#include "file_browser/file_browser.h"
#include "page_manager/PageManager.h"
#include "../ui_kit/UIKIT.h"
#include "esp_log.h"

static const char* TAG = "FileBrowserPage";

FileBrowserPage::FileBrowserPage() 
    : Page(PageType::FILE_BROWSER, "FileBrowser"), _layout(nullptr) {
    ESP_LOGI(TAG, "FileBrowserPage constructed");
}

FileBrowserPage::~FileBrowserPage() {
    ESP_LOGI(TAG, "FileBrowserPage destructed");
    // 注意：_layout 作为 rootView 会被 Page 基类的析构函数自动删除
    // 这里不需要手动删除，避免双重删除问题
}

void FileBrowserPage::onCreate() {
    ESP_LOGI(TAG, "FileBrowserPage onCreate");
    
    // 创建主布局
    auto screenWidth = M5.Display.width();
    auto screenHeight = M5.Display.height();
    _layout = new LinearLayout(screenWidth, screenHeight);
    _layout->setOrientation(LinearLayout::Orientation::VERTICAL);
    Button *backButton = new Button(100, 40);
    backButton->setText("返回");
    backButton->setOnClickListener([]() {
        PageManager::getInstance().goBack();
    });
    _layout->addChild(backButton);
    // 初始化文件浏览器
    file_browser_init(_layout);
    
    // 设置页面根视图
    setRootView(_layout);
    
    // 调用父类方法
    Page::onCreate();
}

void FileBrowserPage::onStart() {
    ESP_LOGI(TAG, "FileBrowserPage onStart");
    Page::onStart();
}

void FileBrowserPage::onResume() {
    ESP_LOGI(TAG, "FileBrowserPage onResume");
    // 可以在这里刷新文件列表
    file_browser_force_refresh();
    Page::onResume();
}

void FileBrowserPage::onPause() {
    ESP_LOGI(TAG, "FileBrowserPage onPause");
    Page::onPause();
}

void FileBrowserPage::onStop() {
    ESP_LOGI(TAG, "FileBrowserPage onStop");
    Page::onStop();
}

void FileBrowserPage::onDestroy() {
    ESP_LOGI(TAG, "FileBrowserPage onDestroy");
    
    // 清理文件浏览器资源
    file_browser_deinit();
    
    Page::onDestroy();
}