#ifndef SDCARD_H
#define SDCARD_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// SD卡挂载点
#define SDCARD_MOUNT_POINT "/sdcard"
#define SDCARD_BOOKS_DIR "/sdcard/books"

#define PIN_MISO GPIO_NUM_40
#define PIN_MOSI GPIO_NUM_38
#define PIN_SCLK GPIO_NUM_39
#define PIN_CS   GPIO_NUM_47

/**
 * @brief 初始化SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_init(void);

#ifdef __cplusplus
}
#endif

#endif // SDCARD_H