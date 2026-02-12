#include <memory>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "M5GFX.h"
#include "lgfx/Fonts/efont/lgfx_efont_cn.h"
#include "page_manager/PageManager.h"
#include "pages/file_browser/PagedFileBrowserPage.h"
#include "pages/settings/SettingsPage.h"
#include "pages/launcher/LauncherPage.h"
#include "pages/message/MessagePage.h"
#include "refresh_counter/RefreshCounter.h"
#include "pages/httpserver/HttpServerPage.h"
#include "hal/sdcard/sdcard.h"
#include "gestures/TouchGestureDetector.h"

#include "config/DeviceConfigManager.h"
#include "../version.h"

static const char *TAG = "Main";


/**
 * @brief 初始化页面管理器
 * 
 * 注册应用程序中使用的所有页面类型到页面管理器，
 * 为每个页面类型提供工厂函数以便按需创建页面实例
 */
static void initPageManager()
{
    // 获取页面管理器实例
    PageManager &pageManager = PageManager::getInstance();
    pageManager.registerPage(PageType::HTTP_SERVER, []()
                             { return std::make_unique<HttpServerPage>(); });
    // 注册文件浏览器页面 - 用于浏览和选择文件
    pageManager.registerPage(PageType::FILE_BROWSER, []()
                             { return std::make_unique<PagedFileBrowserPage>(); });
    
    // 注册设置页面 - 用于应用程序配置和设置
    pageManager.registerPage(PageType::SETTINGS, []()
                             { return std::make_unique<SettingsPage>(); });
    
    // 注册菜单页面 - 作为应用程序的主菜单/启动器
    pageManager.registerPage(PageType::MENU, []()
                             { return std::make_unique<LauncherPage>(); });
    
    // 注册消息页面 - 用于显示全屏文本消息
    pageManager.registerPage(PageType::MESSAGE, []()
                             { return std::make_unique<MessagePage>(); });
}


extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Version: %s, Commit Count: %s, Commit Time: %s, Build Time: %s", GIT_COMMIT_HASH, GIT_COMMIT_COUNT, GIT_COMMIT_TIME, BUILD_TIME);
    initPageManager();
    // A. 初始化硬件
    auto cfg = m5::M5Unified::config();
    M5.begin(cfg);

    if(sdcard_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SD card");
        PageManager::getInstance().startActivity(PageType::MESSAGE, std::make_shared<std::string>("SD card initialization failed!"));
        return;
    }
    DeviceConfigManager::getInstance().loadConfigFromSdCard();
    // 初始清屏：使用 Quality 模式确保屏幕干净
    M5.Display.setFont(&fonts::efontCN_16_b);
    // M5.Display.setEpdMode(lgfx::epd_mode_t::epd_quality);
    // M5.Display.fillScreen(TFT_WHITE);
    // M5.Display.display();    

    

    // 启动启动器页面（作为首页）
    PageManager::getInstance().startActivity(PageType::MENU);

    // 初始化刷新计数器
    RefreshCounter::getInstance().init(DeviceConfigManager::getInstance().getConfig().refreshInterval); // 每10次刷新执行一次全刷
    // RefreshCounter::getInstance().init(10); // 每10次刷新执行一次全刷

    // UI主循环任务
    xTaskCreatePinnedToCore([](void *param)
                            {
        PageManager* pageMgr = static_cast<PageManager*>(param);
        m5gfx::M5GFX& display = M5.Display;
        TouchGestureDetector gestureDetector;
        
        while(1) {

            // 更新触摸状态
            M5.update();
            auto touch = M5.Touch.getDetail(0);            

            // 更新手势检测器
            TouchGestureDetector::SwipeDirection direction = gestureDetector.updateTouch(touch);
            
            if (direction != TouchGestureDetector::SwipeDirection::NONE) {
                ESP_LOGI(TAG, "Detected swipe gesture: %d", direction);
                // 检测到滑动手势
                // 将滑动事件传递给当前页面
                pageMgr->onSwipe(direction);
            } else if (touch.wasPressed()) {
                // 普通触摸事件（非滑动）
                // 处理页面点击事件
                pageMgr->onClick(touch.x, touch.y);
            } 
            
            bool shouldUpdateDisplay = pageMgr->getCurrentPage() ? pageMgr->getCurrentPage()->isDirty() : false;
                        
            
            if (shouldUpdateDisplay) {
                ESP_LOGI(TAG, "UI需要重绘");
                // 绘制当前页面
                display.startWrite();
                pageMgr->draw(display);
                display.endWrite();                
                // 显示更新 - 使用刷新计数器来决定刷新模式
                M5.Display.setEpdMode(RefreshCounter::getInstance().refresh());            
                M5.Display.display();
            }            
            lgfx::v1::delay(10);
            // vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS
            // // 优化：如果没有需要更新的内容，适当延长延迟以节省电力
            // if (shouldUpdateDisplay) {
            //     // 如果刚刚更新了显示，短暂延迟
            //     vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS
            // } else {
            //     // 如果没有更新，可以更长时间休眠以节省电力
            //     vTaskDelay(pdMS_TO_TICKS(50)); // 5 FPS - 更低的轮询频率
            // }
        } }, "ui_loop_task", 8192, &PageManager::getInstance(), 1, NULL, 1);
}