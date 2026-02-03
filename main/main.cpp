#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "M5Unified.h"
#include "lvgl.h"
#include "file_manager/file_manager.h"
#include "hal/sdcard/sdcard.h"
#include "lvgl_list_demo.h"
#include "themes.h"

// LVGL 句柄
static lv_display_t * disp;

// 在全局变量区域声明
// ESP32S3包含两个核心，lvgl不是线程安全的，所以这里需要加锁用来绘制
static SemaphoreHandle_t lvgl_mux = NULL;

// 1. 刷新回调：将 LVGL 像素数据推送到屏幕
void my_disp_flush(lv_display_t * d, const lv_area_t * area, uint8_t * px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    // M5Unified 的 pushImage 兼容 ESP-IDF
    M5.Display.pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);
    
    lv_display_flush_ready(d);
}

// 2. LVGL 计时器任务
void lv_tick_task(void *arg) {
    uint32_t last_tick = esp_log_timestamp(); // 获取当前系统毫秒数
    while (1) {
        if (xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY)) {
            // 1. 计算两次运行之间的时间差，并喂给 LVGL
            uint32_t current_tick = esp_log_timestamp();
            /**
             * LVGL 所有的定时器（包括轮询触摸屏的定时器）都依赖于一个内部时钟计数器。
             * 如果你使用的是 ESP-IDF 默认配置，LVGL 可能在等待你手动通过 lv_tick_inc(ms) 告诉它过了多久。
             * 如果心跳一直是 0，LVGL 就会认为时间从未流逝，因此永远不会去触发“每隔 30ms 读一次触摸”的定时任务。
             * 导致UI和触摸时间都不回调
             */
            lv_tick_inc(current_tick - last_tick);
            last_tick = current_tick;

            // 2. 执行 handler
            /**
             * LVGL 是一个**驱动驱动型（Handler-driven）**的库。它本身并不会自发地在后台运行，必须依靠你周期性地调用 lv_timer_handler() 来处理以下任务：
             * UI 渲染与刷新：计算哪些部件（Widgets）发生了变化，并调用 my_disp_flush 将像素推送到墨水屏。
             * 输入处理：调用你在 setup_touch 中注册的 my_touchpad_read 回调，检测是否有点击动作。
             * 动画与定时器：处理所有控件的动画效果以及各种 lv_timer 定时任务。
             * 事件分发：当你点击屏幕时，它负责判定点击了哪个按钮，并触发你写的 list_event_handler 回调。
             */
            lv_timer_handler(); 
            
            xSemaphoreGiveRecursive(lvgl_mux);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // 保持 10-20ms 的轮询频率
    }
}

// 1. 定义触摸读取回调函数
void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {    
    M5.update(); // 必须更新 M5 状态
    auto t = M5.Touch.getDetail(0); // 获取第一个手指的状态

    if (t.isPressed()) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = t.x;
        data->point.y = t.y;
        printf("Touch at (%ld, %ld)\n", data->point.x, data->point.y);
    } else {
        data->state = LV_INDEV_STATE_RELEASED;        
    }
}

// 必须在lvgl_init之后才能调用
void setup_touch() {
    printf("Setting up touch input device\n");
    lv_indev_t * indev = lv_indev_create();           // 创建输入设备
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);  // 设置为指针类型（触摸屏）
    lv_indev_set_read_cb(indev, my_touchpad_read);    // 绑定读取函数
}

extern "C" void app_main(void) {
    // A. 初始化 M5Stack 硬件
    auto cfg = m5::M5Unified::config();
    M5.begin(cfg);
    M5.Display.setBrightness(128);        

    // B. 获取动态宽高
    int32_t disp_w = M5.Display.width();
    int32_t disp_h = M5.Display.height();

    // 创建递归互斥锁
    lvgl_mux = xSemaphoreCreateRecursiveMutex();    

    // C. 初始化 LVGL
    lv_init();

    // D. 动态分配缓冲区 (分配 1/10 屏幕高度)
    uint32_t draw_buf_size = disp_w * (disp_h / 10) * sizeof(lv_color_t);
    void * draw_buf = heap_caps_malloc(draw_buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (draw_buf == NULL) {
        // 如果PSRAM分配失败，尝试使用内部RAM
        draw_buf = heap_caps_malloc(draw_buf_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (draw_buf == NULL) {
            printf("Failed to allocate display buffer in both PSRAM and internal RAM!\n");
            return;
        } else {
            printf("Display buffer allocated in internal RAM\n");
        }
    } else {
        printf("Display buffer allocated in PSRAM\n");
    }

    // 在对 LVGL 进行任何操作前，先拿锁
    xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);

    // E. 配置显示驱动
    disp = lv_display_create(disp_w, disp_h);
    lv_display_set_default(disp);
    lv_display_set_buffers(disp, draw_buf, NULL, draw_buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);
    setup_global_font();
    if(M5.Touch.isEnabled()) {
        printf("Touch screen is enabled\n");   
        setup_touch();     
    }else {
        printf("Touch screen is disabled\n");
    }
    

    // lv_obj_t* label = lv_label_create(lv_scr_act());
    // lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    // lv_obj_set_width(label, disp_w - 20);
    // lv_label_set_text(label, "你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界你好世界!今天天气好不好啊");    
    // lv_obj_set_style_text_font(label, &lv_font_source_han_sans_sc_16_cjk, 0);
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    test_lvgl_list_demo(lv_scr_act(), disp_w, disp_h);
    // F. 创建 LVGL 驱动任务 (核心 1) - 增加栈大小防止溢出
    xTaskCreatePinnedToCore(lv_tick_task, "lv_tick", 16384, NULL, 5, NULL, 1);

    // G. 初始化文件管理器
    // file_manager_init(NULL);

    printf("LVGL Initialized on %dx%d screen\n", (int)disp_w, (int)disp_h);

    printf("Checking indev... %p\n", lv_indev_get_next(NULL));

    // UI 创建完了，放开锁，让刷新任务可以开始工作
    xSemaphoreGiveRecursive(lvgl_mux);

    // while (1) {
    //     M5.update(); // 物理读取硬件数据
        
    //     xSemaphoreTakeRecursive(lvgl_mux, portMAX_DELAY);
    //     lv_tick_inc(10);
    //     lv_timer_handler(); // 理论上这里会触发 my_touchpad_read
    //     xSemaphoreGiveRecursive(lvgl_mux);
        
    //     auto t = M5.Touch.getDetail(0);
    //     if(t.isPressed()) {
    //         printf("M5 Hardware Touch: %d, %d\n", t.x, t.y);
    //     }
        
    //     vTaskDelay(pdMS_TO_TICKS(10));
    // }
}