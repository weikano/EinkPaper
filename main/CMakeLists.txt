file(GLOB_RECURSE SOURCES 
    "*.cpp"                     
)

message(STATUS "SOURCES variable contains: ${SOURCES}")

idf_component_register(SRCS ${SOURCES} "main.cpp"
                    INCLUDE_DIRS "." "pages" "pages/file_browser" "pages/settings" "pages/launcher" "pages/message" "refresh_counter" "hal/sdcard" "ui_kit" "page_manager" "config" "gestures" "hal/wifi" "http" "pages/httpserver"
                    REQUIRES fatfs sdmmc spi_flash esp_wifi esp_http_server
                    )
