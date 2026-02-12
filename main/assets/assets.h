#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>

// index.html content
extern const uint8_t binary_assets_index_html_start[] asm("_binary_index_html_start");
extern const uint8_t binary_assets_index_html_end[] asm("_binary_index_html_end");

// config.html content
extern const uint8_t binary_assets_config_html_start[] asm("_binary_config_html_start");
extern const uint8_t binary_assets_config_html_end[] asm("_binary_config_html_end");

// upload.html content
extern const uint8_t binary_assets_upload_html_start[] asm("_binary_upload_html_start");
extern const uint8_t binary_assets_upload_html_end[] asm("_binary_upload_html_end");

// crashlog.html content
extern const uint8_t binary_assets_crashlog_html_start[] asm("_binary_crashlog_html_start");
extern const uint8_t binary_assets_crashlog_html_end[] asm("_binary_crashlog_html_end");

#endif /* ASSETS_H */