file(GLOB_RECURSE SOURCES 
    "*.cpp"                         
)

file(GLOB_RECURSE EXTERN_FILES assets/*)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "." "pages" "pages/file_browser" "pages/settings" "pages/launcher" "pages/message" "refresh_counter" "hal/sdcard" "ui_kit" "page_manager" "config" "gestures" "hal/wifi" "http" "pages/httpserver" "assets"
                    EMBED_FILES ${EXTERN_FILES}
                    REQUIRES fatfs sdmmc spi_flash esp_wifi esp_http_server nvs_flash
                    )

