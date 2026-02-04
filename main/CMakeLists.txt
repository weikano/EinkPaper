idf_component_register(SRCS "main.cpp" "file_manager/file_manager.cpp" "hal/sdcard/sdcard.cpp" 
                    "ui_kit/View.cpp" "ui_kit/ViewGroup.cpp" "ui_kit/TextView.cpp" "ui_kit/Button.cpp" 
                    "ui_kit/LinearLayout.cpp" "ui_kit/FrameLayout.cpp" "ui_kit/ListView.cpp" "ui_kit/Dialog.cpp"
                    "ui_demo.cpp"
                    INCLUDE_DIRS "." "file_manager" "hal/sdcard" "ui_kit"
                    REQUIRES fatfs sdmmc spi_flash)