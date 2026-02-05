idf_component_register(SRCS "main.cpp" "pages/file_browser/file_browser.cpp" "pages/file_browser/FileBrowserPage.cpp" "pages/settings/SettingsPage.cpp" "pages/launcher/LauncherPage.cpp" "page_manager/PageManager.cpp" "page_manager/Page.cpp" "hal/sdcard/sdcard.cpp" 
                    "ui_kit/View.cpp" "ui_kit/ViewGroup.cpp" "ui_kit/TextView.cpp" "ui_kit/Button.cpp" 
                    "ui_kit/LinearLayout.cpp" "ui_kit/FrameLayout.cpp" "ui_kit/ListView.cpp" "ui_kit/Dialog.cpp"
                    "ui_demo.cpp"
                    INCLUDE_DIRS "." "pages" "pages/file_browser" "pages/settings" "pages/launcher" "hal/sdcard" "ui_kit" "page_manager"
                    REQUIRES fatfs sdmmc spi_flash)