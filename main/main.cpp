#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "M5Unified.h"
#include "lvgl.h"
#include "file_manager/file_manager.h"
#include "hal/sdcard/sdcard.h"
#include "lvgl_list_demo.h"
#include "themes.h"
#include "hal/sdcard/sdcard.h"

// LVGL 句柄
static lv_display_t * disp;
static SemaphoreHandle_t lvgl_mux = NULL;

// 记录刷新次数，用于定期全局全刷清残影
static int partial_refresh_count = 0;

// 1. 刷新回调：针对局部区域进行快速刷新
void my_disp_flush(lv_display_t * d, const lv_area_t * area, uint8_t * px_map) {
    auto w = lv_area_get_width(area);
    auto h = lv_area_get_height(area);

    printf("Flush area x:%ld y:%ld w:%ld h:%ld\n", area->x1, area->y1, w, h);

    // 设置为极速刷新模式，避免全屏黑白翻转（黑闪）
    // M5.Display.setEpdMode(lgfx::epd_mode_t::epd_fastest);
    
    // 仅推送 LVGL 标记为“脏”的局部区域
    M5.Display.pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);

    /**
     * 不再等待 is_last。
     * 只要 LVGL 绘制好一块区域，就立刻通过 display() 触发硬件波形更新该区域。
     * 这样能达到“指哪打哪”的效果，但也可能因为刷新太碎导致轻微卡顿。
     */
    M5.Display.setEpdMode(lgfx::epd_mode_t::epd_fast);
    // if(lv_display_flush_is_last(d)) {
        // 恢复为常用的标准模式，平衡速度与画质
        M5.Display.display(); 
    // }
    
    partial_refresh_count++;
    lv_display_flush_ready(d);
}

// 2. LVGL 计时器任务
void lv_tick_task(void *arg) {
    uint32_t last_tick = esp_log_timestamp();
    while (1) {
        if (xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY)) {
            uint32_t current_tick = esp_log_timestamp();
            lv_tick_inc(current_tick - last_tick);
            last_tick = current_tick;

            lv_timer_handler(); 
            
            xSemaphoreGiveRecursive(lvgl_mux);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

// 3. 触摸读取回调
void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {    
    M5.update(); 
    auto t = M5.Touch.getDetail(0); 

    if (t.isPressed()) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = t.x;
        data->point.y = t.y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;        
    }
}

void setup_touch() {
    lv_indev_t * indev = lv_indev_create();           
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);  
    lv_indev_set_read_cb(indev, my_touchpad_read);    
}

extern "C" void app_main(void) {
    // A. 初始化硬件
    auto cfg = m5::M5Unified::config();
    // cfg.external_psram = true; // 删掉此行，M5Unified 会自动处理
    M5.begin(cfg);
    M5.Display.setBrightness(128);

    sdcard_init();
    
    // 初始清屏：使用 Quality 模式确保屏幕干净
    M5.Display.setEpdMode(lgfx::epd_mode_t::epd_quality);
    M5.Display.fillScreen(TFT_WHITE);
    M5.Display.display();

    int32_t disp_w = M5.Display.width();
    int32_t disp_h = M5.Display.height();

    lvgl_mux = xSemaphoreCreateRecursiveMutex();    

    // B. 初始化 LVGL
    lv_init();

    // C. 局部缓冲区分配 (PSRAM)
    // 局部刷新不需要全屏缓冲，分配 1/10 屏幕高度即可，节省内存并提高响应速度
    uint32_t draw_buf_size = disp_w * (disp_h) * sizeof(lv_color_t);
    void * draw_buf = heap_caps_malloc(draw_buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (draw_buf == NULL) {
        draw_buf = heap_caps_malloc(draw_buf_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }

    // D. 配置显示驱动
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);

    disp = lv_display_create(disp_w, disp_h);
    lv_display_set_default(disp);
    
    /**
     * 关键修改点：
     * 1. 模式改为 LV_DISPLAY_RENDER_MODE_PARTIAL (仅绘制变化部分)
     * 2. 只使用一个较小的缓冲区，通过多次 flush 完成更新
     */
    lv_display_set_buffers(disp, draw_buf, NULL, draw_buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);

    setup_global_font();
    if(M5.Touch.isEnabled()) {
        setup_touch();     
    }

    file_manager_init(NULL);
    
    // test_lvgl_list_demo(lv_scr_act(), disp_w, disp_h);

    xTaskCreatePinnedToCore(lv_tick_task, "lv_tick", 16384, NULL, 5, NULL, 1);

    xSemaphoreGiveRecursive(lvgl_mux);

    // // G. 主循环中可以添加定时全屏清理逻辑
    // while(1) {
    //     // 如果局部刷新次数过多（残影严重），可以手动触发一次全刷
    //     if (partial_refresh_count > 50) { 
    //         xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
            
    //         M5.Display.setEpdMode(lgfx::epd_mode_t::epd_quality);
    //         // 强制 LVGL 重绘整个屏幕
    //         lv_obj_invalidate(lv_scr_act()); 
    //         partial_refresh_count = 0;
            
    //         xSemaphoreGiveRecursive(lvgl_mux);
    //     }
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}