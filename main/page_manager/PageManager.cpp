#include "PageManager.h"
#include "esp_log.h"

static const char* TAG = "PageManager";

PageManager& PageManager::getInstance() {
    static PageManager instance;  // C++11标准保证线程安全
    return instance;
}

PageManager::PageManager() {
    ESP_LOGI(TAG, "PageManager initialized");
}

PageManager::~PageManager() {
    // 确保所有页面都被正确销毁
    while (!_pageStack.empty()) {
        auto page = std::move(_pageStack.back());
        _pageStack.pop_back();
        if (page) {
            page->onDestroy();
        }
    }
    ESP_LOGI(TAG, "PageManager destroyed");
}

void PageManager::registerPage(PageType pageType, 
                               std::function<std::unique_ptr<Page>()> factory) {
    _pageFactories[pageType] = factory;
    ESP_LOGI(TAG, "Registered page type: %d", static_cast<int>(pageType));
}

void PageManager::startActivity(PageType pageType, 
                                std::shared_ptr<void> params) {
    auto it = _pageFactories.find(pageType);
    if (it == _pageFactories.end()) {
        ESP_LOGE(TAG, "Page type %d not registered", static_cast<int>(pageType));
        return;
    }

    // 暂停当前页面
    if (!_pageStack.empty()) {
        pauseCurrentPage();
    }

    // 创建新页面
    auto newPage = it->second();
    if (!newPage) {
        ESP_LOGE(TAG, "Failed to create page type: %d", static_cast<int>(pageType));
        return;
    }

    // 设置页面参数
    newPage->setParams(params);

    // 设置页面生命周期
    newPage->onCreate();
    newPage->onStart();
    newPage->onResume();

    // 将页面压入栈（使用deque的back操作模拟栈行为）
    _pageStack.push_back(std::move(newPage));

    // 标记页面转换发生，需要重绘
    _pageTransitionOccurred = true;
    
    ESP_LOGI(TAG, "Started activity: %d", static_cast<int>(pageType));
}

void PageManager::goBack() {
    if (_pageStack.empty()) {
        ESP_LOGW(TAG, "No page to go back");
        return;
    }

    // 暂停当前页面
    auto currentPage = std::move(_pageStack.back());
    _pageStack.pop_back();
    currentPage->onPause();
    currentPage->onStop();
    currentPage->onDestroy();

    ESP_LOGI(TAG, "Destroyed page: %d", static_cast<int>(currentPage->getType()));

    // 如果还有页面，恢复上一个页面
    if (!_pageStack.empty()) {
        auto resumedPage = _pageStack.back().get();
        resumedPage->onRestart();
        resumedPage->onStart();
        resumedPage->onResume();
        ESP_LOGI(TAG, "Resumed page: %d", static_cast<int>(resumedPage->getType()));
    }

    // 标记页面转换发生，需要重绘
    _pageTransitionOccurred = true;
    
    ESP_LOGI(TAG, "Finished goBack operation");
}

void PageManager::startActivityClearTop(PageType pageType, 
                                       std::shared_ptr<void> params) {
    // 销毁所有页面
    while (!_pageStack.empty()) {
        auto page = std::move(_pageStack.back());
        _pageStack.pop_back();
        page->onPause();
        page->onStop();
        page->onDestroy();
        ESP_LOGI(TAG, "Destroyed page: %d", static_cast<int>(page->getType()));
    }

    // 启动新页面
    startActivity(pageType, params);
    
    // 标记页面转换发生，需要重绘
    _pageTransitionOccurred = true;
}

void PageManager::draw(m5gfx::M5GFX& display) {
    if (!_pageStack.empty()) {
        auto currentPage = _pageStack.back().get();
        currentPage->draw(display);
    }
}

bool PageManager::onClick(int16_t x, int16_t y) {
    if (!_pageStack.empty()) {
        auto currentPage = _pageStack.back().get();
        return currentPage->onClick(x, y);
    }
    return false;
}

Page* PageManager::getCurrentPage() {
    if (!_pageStack.empty()) {
        return _pageStack.back().get();
    }
    return nullptr;
}

std::string PageManager::getCurrentPageName() {
    if (!_pageStack.empty()) {
        auto currentPage = _pageStack.back().get();
        return currentPage->getName();
    }
    return "";
}

bool PageManager::isPageInStack(PageType pageType) const {
    // 使用deque的迭代能力来检查页面类型
    for (const auto& page : _pageStack) {
        if (page && page->getType() == pageType) {
            return true;
        }
    }
    return false;
}

size_t PageManager::getPageCountByType(PageType pageType) const {
    size_t count = 0;
    // 使用deque的迭代能力来统计页面类型
    for (const auto& page : _pageStack) {
        if (page && page->getType() == pageType) {
            count++;
        }
    }
    return count;
}

void PageManager::pauseCurrentPage() {
    if (!_pageStack.empty()) {
        auto currentPage = _pageStack.back().get();
        currentPage->onPause();
    }
}

void PageManager::resumePage(Page* page) {
    if (page) {
        page->onResume();
    }
}

void PageManager::stopPage(Page* page) {
    if (page) {
        page->onStop();
    }
}

void PageManager::destroyPage(Page* page) {
    if (page) {
        page->onDestroy();
    }
}

void PageManager::destroy() {
    while (!_pageStack.empty()) {
        auto page = std::move(_pageStack.back());
        _pageStack.pop_back();
        if (page) {
            page->onDestroy();
        }
    }
}

void PageManager::finishActivity() {
    if (!_pageStack.empty()) {
        // 获取当前页面
        auto currentPage = _pageStack.back().get();
        
        // 执行页面生命周期方法
        currentPage->onPause();
        currentPage->onStop();
        currentPage->onDestroy();
        
        // 从页面栈中移除页面
        _pageStack.pop_back();
        
        // 如果还有页面在栈中，恢复前一个页面
        if (!_pageStack.empty()) {
            auto previousPage = _pageStack.back().get();
            previousPage->onRestart();
            previousPage->onStart();
            previousPage->onResume();
        }
        
        _pageTransitionOccurred = true;
        ESP_LOGI("PageManager", "Finished current activity, stack size: %zu", _pageStack.size());
    } else {
        ESP_LOGW("PageManager", "No activity to finish");
    }
}

bool PageManager::isDirty() {
    bool result = _pageTransitionOccurred;  // 页面转换发生时需要重绘
    
    if (auto currentPage = getCurrentPage()) {
        result |= currentPage->isDirty();
    }
    
    // 重置转换标志
    if (_pageTransitionOccurred) {
        _pageTransitionOccurred = false;
    }
    
    return result;
}

void PageManager::onSwipe(TouchGestureDetector::SwipeDirection direction) {
    if (!_pageStack.empty()) {
        auto currentPage = _pageStack.back().get();
        currentPage->onSwipe(direction);
    }
}