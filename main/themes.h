#pragma once
#include "lvgl.h"
// 1. 声明你生成的字体
extern "C"  {
    LV_FONT_DECLARE(fzyh_507r_20);
}

// 2. 定义主题应用回调函数
void new_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj) {
    // 全局字体设置
    lv_obj_set_style_text_font(obj, &fzyh_507r_20, 0);
    // 全局禁用所有 label 的跑马灯（建议在 setup_global_font 或 theme 初始化处执行）
    lv_obj_set_style_anim_duration(lv_screen_active(), 0, LV_PART_MAIN);    

    // 如果对象是按钮类，去掉按下时的位移和背景变色（墨水屏优化）
    if (lv_obj_check_type(obj, &lv_button_class)) {
        lv_obj_set_style_translate_y(obj, 0, LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_STATE_PRESSED);
        lv_obj_set_style_shadow_width(obj, 0, LV_STATE_PRESSED);        
    }
}

// 3. 在 app_main 初始化 LVGL 后调用
void setup_global_font() {
    lv_display_t * disp_default = lv_display_get_default();
    if(disp_default == NULL) return;

    // 获取当前的主题指针
    lv_theme_t * th_act = lv_display_get_theme(disp_default);
    if(th_act == NULL) return;

    // 重点：不要声明结构体变量，直接修改当前主题的回调
    // 或者如果你想基于默认主题创建，使用指针赋值
    lv_theme_set_apply_cb(th_act, new_theme_apply_cb);

    // 重新通知显示器使用该主题（触发刷新）
    lv_display_set_theme(disp_default, th_act);

    // 在 setup_global_font() 之后添加
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN);
}
