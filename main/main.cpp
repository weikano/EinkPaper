#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "M5Unified.h"
#include "M5GFX.h"
#include "lgfx/Fonts/efont/lgfx_efont_cn.h"
#include "file_manager/file_manager.h"
#include "hal/sdcard/sdcard.h"
#include "ui_kit/UIKIT.h"


extern "C" void app_main(void) {
    // A. 初始化硬件
    auto cfg = m5::M5Unified::config();    
    M5.begin(cfg);    

    sdcard_init();
    
    // 初始清屏：使用 Quality 模式确保屏幕干净
    M5.Display.setFont(&fonts::efontCN_16);
    M5.Display.setEpdMode(lgfx::epd_mode_t::epd_quality);
    M5.Display.fillScreen(TFT_WHITE);
    M5.Display.display();

    int32_t disp_w = M5.Display.width();
    int32_t disp_h = M5.Display.height();

    // 初始化UI Kit组件
    LinearLayout* mainLayout = new LinearLayout(disp_w, disp_h, LinearLayout::VERTICAL);
    mainLayout->setSpacing(5);
    mainLayout->setPadding(30, 30, 30, 30);

    // Button* button = new Button(100, 50);  // 只指定宽高，位置由LinearLayout决定
    // button->setText("Click me");

    // mainLayout->addChild(button);
    
    // 初始化文件管理器
    file_manager_init(mainLayout);
    
    

    // UI主循环任务
    xTaskCreatePinnedToCore([](void* param) {
        LinearLayout* layout = static_cast<LinearLayout*>(param);
        m5gfx::M5GFX& display = M5.Display;
        
        while(1) {
            bool shouldUpdateDisplay = false;
            
            // 更新触摸状态
            M5.update();
            auto touch = M5.Touch.getDetail(0);
            
            if (touch.wasPressed()) {
                // 处理UI触摸事件
                layout->onTouch(touch.x, touch.y);
            }
            
            // 检查是否有视图需要重绘
            if (layout->isDirty()) {
                shouldUpdateDisplay = true;
            }
            
            if (shouldUpdateDisplay) {
                printf("UI需要重绘\n");
                // 绘制UI
                display.startWrite();
                display.fillRect(0, 0, display.width(), display.height(), TFT_WHITE);
                layout->draw(display);
                display.endWrite();
                
                // 显示更新
                // M5.Display.setEpdMode(m5gfx::epd_mode_t::epd_fast);
                M5.Display.display();
            }
            
            // 优化：如果没有需要更新的内容，适当延长延迟以节省电力
            if (shouldUpdateDisplay) {
                // 如果刚刚更新了显示，短暂延迟
                vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS
            } else {
                // 如果没有更新，可以更长时间休眠以节省电力
                vTaskDelay(pdMS_TO_TICKS(200)); // 5 FPS - 更低的轮询频率
            }
        }
    }, "ui_loop_task", 8192, mainLayout, 1, NULL, 1);
}