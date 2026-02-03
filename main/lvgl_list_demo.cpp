#include "lvgl_list_demo.h"
#include <stdio.h>
void test_lvgl_list_demo(lv_obj_t* parent, int screen_width, int screen_height) {
    lv_obj_t* list = lv_list_create(parent);    
    lv_obj_set_size(list, screen_width - 20, screen_height - 20);
    lv_obj_center(list);
    lv_obj_set_style_text_font(list, &lv_font_source_han_sans_sc_16_cjk, 0);
    
    auto list_event_handler = [](lv_event_t* e) {
        lv_event_code_t code = lv_event_get_code(e);
        // C++ 显式转换：获取点击的按钮对象
        lv_obj_t * btn = static_cast<lv_obj_t *>(lv_event_get_target(e));        
        if(code == LV_EVENT_CLICKED) {
            
            // 获取按钮上的文字（假设这是文件路径）
            const char * path = lv_list_get_button_text(static_cast<lv_obj_t *>(lv_event_get_current_target(e)), btn);
            printf("Clicked on list item: %s\n", path);
            /* 创建消息框 (Message Box) */
            // 参数：父对象, 标题, 内容文本, 按钮文字数组, 是否添加关闭按钮
            static const char * btns[] = {"确定", ""}; // 底部按钮定义，必须以空字符串结尾
            
            lv_obj_t * mbox = lv_msgbox_create(NULL); // 传入 NULL 会自动在顶层创建
            // 获取消息框的背景（即那个全屏遮罩）并设为透明
            lv_obj_t * bg = lv_obj_get_parent(mbox); 
            if(bg != lv_screen_active()) {
                lv_obj_set_style_bg_opa(bg, LV_OPA_TRANSP, 0);
            }
            lv_msgbox_add_title(mbox, "文件详情");
            lv_msgbox_add_text(mbox, path);
            lv_obj_set_style_text_font(mbox, &lv_font_source_han_sans_sc_16_cjk, 0);            

            // 获取返回的按钮对象
            lv_obj_t * ok_btn = lv_msgbox_add_footer_button(mbox, "确定");

            // 重点：直接给按钮加回调，而不是给 mbox 加
            lv_obj_add_event_cb(ok_btn, [](lv_event_t * e) {
                // 这里的 current_target 就是 ok_btn                
                lv_obj_t * btn = (lv_obj_t *)lv_event_get_current_target(e);
                // 逻辑：按钮 -> Footer -> MsgBox (两层向上)
                lv_obj_t * m = lv_obj_get_parent(lv_obj_get_parent(btn));
                
                printf("Confirm button clicked, closing msgbox\n");
                lv_msgbox_close(m); 
                
                // 墨水屏：标记全屏重绘，防止残影
                lv_obj_invalidate(lv_screen_active());
            }, LV_EVENT_CLICKED, NULL);

            lv_obj_center(mbox);
        }
    };

    /* 添加列表项 (带图标和文字) */
    lv_list_add_text(list, "系统文件"); // 列表分组标题
    
    lv_obj_t * btn;
    btn = lv_list_add_button(list, LV_SYMBOL_FILE, "电子书_01.txt");
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_button(list, LV_SYMBOL_DIRECTORY, "我的收藏");
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_button(list, LV_SYMBOL_SETTINGS, "系统设置");
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_button(list, LV_SYMBOL_TRASH, "清除缓存");
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, NULL);

}