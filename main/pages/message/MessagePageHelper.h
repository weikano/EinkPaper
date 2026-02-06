#include "MessagePage.h"
#include "page_manager/PageManager.h"
#include "esp_log.h"


/**
 * @brief 创建并显示消息页面
 * @param message 要显示的消息内容
 */
void showMessagePage(const std::string& message) {
    // 获取页面管理器
    PageManager& pageManager = PageManager::getInstance();
    
    // 创建参数
    auto msgParam = std::make_shared<std::string>(message);
    
    // 启动消息页面
    pageManager.startActivity(PageType::MESSAGE, msgParam);
    
    ESP_LOGI("MessagePageHelper", "Started MessagePage with message: %s", message.c_str());
}