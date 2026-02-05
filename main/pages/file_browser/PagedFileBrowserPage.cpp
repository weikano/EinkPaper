#include "PagedFileBrowserPage.h"
#include "paged_file_browser.h"
#include "page_manager/PageManager.h"
#include "../ui_kit/UIKIT.h"
#include "esp_log.h"
#include "../ui_kit/Button.h"

static const char* TAG = "PagedFileBrowserPage";

PagedFileBrowserPage::PagedFileBrowserPage() 
    : Page(PageType::FILE_BROWSER, "PagedFileBrowser"), _layout(nullptr) {
    ESP_LOGI(TAG, "PagedFileBrowserPage constructed");
}

PagedFileBrowserPage::~PagedFileBrowserPage() {
    ESP_LOGI(TAG, "PagedFileBrowserPage destructed");
    if (_layout) {
        delete _layout;
        _layout = nullptr;
    }
}

void PagedFileBrowserPage::onCreate() {
    ESP_LOGI(TAG, "PagedFileBrowserPage onCreate");
    
    // 创建主布局
    auto screenWidth = M5.Display.width();
    auto screenHeight = M5.Display.height();
    _layout = new LinearLayout(screenWidth, screenHeight);
    _layout->setOrientation(LinearLayout::Orientation::VERTICAL);
    
    // // 创建顶部标题栏
    // TextView* titleView = new TextView(screenWidth, 40);
    // titleView->setText("文件浏览器");
    // titleView->setTextColor(TFT_BLACK);
    // titleView->setTextSize(2);
    // titleView->setTextAlign(1); // 居中对齐
    // titleView->setPadding(10, 10, 10, 10);
    // _layout->addChild(titleView);
    
    // 初始化分页文件浏览器（现在底部控制栏由PagedListView自己处理）
    paged_file_browser_init(_layout);
    
    // 设置返回回调
    set_paged_file_browser_back_callback([]() {
        PageManager::getInstance().goBack();
    });
    
    // 设置页面根视图
    setRootView(_layout);
    
    // 调用父类方法
    Page::onCreate();
}

void PagedFileBrowserPage::onStart() {
    ESP_LOGI(TAG, "PagedFileBrowserPage onStart");
    Page::onStart();
}

void PagedFileBrowserPage::onResume() {
    ESP_LOGI(TAG, "PagedFileBrowserPage onResume");
    // 可以在这里刷新文件列表
    paged_file_browser_force_refresh();
    Page::onResume();
}

void PagedFileBrowserPage::onPause() {
    ESP_LOGI(TAG, "PagedFileBrowserPage onPause");
    Page::onPause();
}

void PagedFileBrowserPage::onStop() {
    ESP_LOGI(TAG, "PagedFileBrowserPage onStop");
    Page::onStop();
}

void PagedFileBrowserPage::onDestroy() {
    ESP_LOGI(TAG, "PagedFileBrowserPage onDestroy");
    
    // 清理分页文件浏览器资源
    paged_file_browser_deinit();
    
    Page::onDestroy();
}