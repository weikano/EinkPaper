idf_component_register(SRCS "main.cpp" "lvgl_list_demo.cpp" "file_manager/file_manager.cpp" "hal/sdcard/sdcard.cpp"
                    INCLUDE_DIRS "." "file_manager" "hal/sdcard"
                    REQUIRES fatfs sdmmc spi_flash)