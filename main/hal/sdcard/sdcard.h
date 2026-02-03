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

/**
 * @brief 初始化SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_init(void);

/**
 * @brief 挂载SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_mount(void);

/**
 * @brief 卸载SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_unmount(void);

/**
 * @brief 检查SD卡是否已挂载
 * @return true 已挂载，false 未挂载
 */
bool sdcard_is_mounted(void);

/**
 * @brief 获取SD卡总容量（字节）
 * @return 总容量，失败返回0
 */
uint64_t sdcard_get_total_bytes(void);

/**
 * @brief 获取SD卡可用容量（字节）
 * @return 可用容量，失败返回0
 */
uint64_t sdcard_get_free_bytes(void);

/**
 * @brief 确保书籍目录存在
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_ensure_books_dir(void);

#ifdef __cplusplus
}
#endif

#endif // SDCARD_H