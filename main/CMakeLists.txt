file(GLOB_RECURSE SOURCES 
    "*.cpp"                         
)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "." "pages" "pages/file_browser" "pages/settings" "pages/launcher" "pages/message" "pages/ota" "refresh_counter" "hal/sdcard" "hal/ota" "hal/netif" "hal/littlefs" "ui_kit" "page_manager" "config" "gestures" "hal/wifi" "http" "pages/httpserver" "crash_report" "i18n"
                    REQUIRES fatfs sdmmc spi_flash esp_wifi esp_http_server esp_http_client esp_https_ota app_update mbedtls json nvs_flash espcoredump esp_partition littlefs
                    )

if(COMMAND littlefs_create_partition_image)
    littlefs_create_partition_image(storage assets FLASH_IN_PROJECT)
endif()
