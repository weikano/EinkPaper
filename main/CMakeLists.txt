idf_component_register(SRCS "main.cpp" "lvgl_list_demo.cpp" "file_manager/file_manager.cpp" "hal/sdcard/sdcard.cpp" "fonts/fzyh_507r_20.c"
                    "ui_kit/View.cpp" "ui_kit/ViewGroup.cpp" "ui_kit/TextView.cpp" "ui_kit/Button.cpp" 
                    "ui_kit/LinearLayout.cpp" "ui_kit/FrameLayout.cpp" "ui_kit/ListView.cpp" "ui_kit/Dialog.cpp"
                    "ui_demo.cpp"
                    INCLUDE_DIRS "." "file_manager" "hal/sdcard" "ui_kit"
                    REQUIRES fatfs sdmmc spi_flash)
# 在 idf_component_register 之后添加
# lv_font_conv 转换的字体文件有用到这个宏，如果不声明，会找lvgl/lvgl.h，但是实际上使用的是lvgl.h
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_LVGL_H_INCLUDE_SIMPLE=1")