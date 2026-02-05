#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "M5Unified.h"
#include "M5GFX.h"
#include "lgfx/Fonts/efont/lgfx_efont_cn.h"
#include "page_manager/PageManager.h"
#include "pages/file_browser/FileBrowserPage.h"
#include "pages/file_browser/PagedFileBrowserPage.h"
#include "pages/settings/SettingsPage.h"
#include "pages/launcher/LauncherPage.h"
#include "refresh_counter/RefreshCounter.h"
#include "hal/sdcard/sdcard.h"
#include "ui_kit/UIKIT.h"

#include "config/DeviceConfigManager.h"

extern "C" void app_main(void)
{
    // A. 初始化硬件
    auto cfg = m5::M5Unified::config();
    M5.begin(cfg);

    sdcard_init();
    DeviceConfigManager::getInstance().loadConfigFromSdCard();

    // 初始清屏：使用 Quality 模式确保屏幕干净
    M5.Display.setFont(&fonts::efontCN_16_b);
    M5.Display.setEpdMode(lgfx::epd_mode_t::epd_quality);
    M5.Display.fillScreen(TFT_WHITE);
    M5.Display.display();    

    // 获取页面管理器实例
    PageManager &pageManager = PageManager::getInstance();
    

    pageManager.registerPage(PageType::FILE_BROWSER, []()
                             { return std::make_unique<PagedFileBrowserPage>(); });

    pageManager.registerPage(PageType::SETTINGS, []()
                             { return std::make_unique<SettingsPage>(); });

    pageManager.registerPage(PageType::MENU, []()
                             { return std::make_unique<LauncherPage>(); });

    // 启动启动器页面（作为首页）
    pageManager.startActivity(PageType::MENU);

    // 初始化刷新计数器
    RefreshCounter::getInstance().init(DeviceConfigManager::getInstance().getConfig().refreshInterval); // 每10次刷新执行一次全刷
    // RefreshCounter::getInstance().init(10); // 每10次刷新执行一次全刷

    // UI主循环任务
    xTaskCreatePinnedToCore([](void *param)
                            {
        PageManager* pageMgr = static_cast<PageManager*>(param);
        m5gfx::M5GFX& display = M5.Display;
        
        while(1) {

            // 更新触摸状态
            M5.update();
            auto touch = M5.Touch.getDetail(0);            

            if (touch.wasPressed()) {
                // 处理页面触摸事件
                pageMgr->onTouch(touch.x, touch.y);
            } 
            
            bool shouldUpdateDisplay = pageMgr->getCurrentPage() ? pageMgr->getCurrentPage()->isDirty() : false;
                        
            
            if (shouldUpdateDisplay) {
                printf("UI需要重绘\n");
                // 绘制当前页面
                display.startWrite();
                pageMgr->draw(display);
                display.endWrite();                
                // 显示更新 - 使用刷新计数器来决定刷新模式
                M5.Display.setEpdMode(RefreshCounter::getInstance().refresh());
                M5.Display.display();
            }
            vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS
            // // 优化：如果没有需要更新的内容，适当延长延迟以节省电力
            // if (shouldUpdateDisplay) {
            //     // 如果刚刚更新了显示，短暂延迟
            //     vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS
            // } else {
            //     // 如果没有更新，可以更长时间休眠以节省电力
            //     vTaskDelay(pdMS_TO_TICKS(50)); // 5 FPS - 更低的轮询频率
            // }
        } }, "ui_loop_task", 8192, &pageManager, 1, NULL, 1);
}