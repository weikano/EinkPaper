#include "sdcard.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "ff.h"  // FatFs
#include "driver/gpio.h" // For GPIO_NUM definitions
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

static const char *TAG = "SDCARD";

// SD卡句柄
static sdmmc_card_t *sd_card_handle = NULL;
static bool is_card_mounted = false;

/**
 * @brief 初始化SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_init(void) {
    printf("Initializing SD card...\n");

    // 确保挂载点目录存在
    struct stat st;
    if (stat(SDCARD_MOUNT_POINT, &st) != 0) {
        if (mkdir(SDCARD_MOUNT_POINT, 0755) != 0) {
            printf("Failed to create mount point directory: %s\n", SDCARD_MOUNT_POINT);
            return ESP_FAIL;
        }
    }

    printf("SD card initialization completed\n");
    return ESP_OK;
}

/**
 * @brief 挂载SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_mount(void) {
    if (is_card_mounted) {
        printf("SD card is already mounted\n");
        return ESP_OK;
    }

    esp_err_t ret;

    // 配置SD卡挂载参数
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,  // 不要自动格式化
        .max_files = 8,                  // 最大同时打开文件数
        .allocation_unit_size = 16 * 1024 // 分配单元大小16KB
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    // 配置主机时钟速度为20MHz以提高稳定性
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    // 这个初始化适用于M5PaperS3的SD卡接口
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    // 根据M5PaperS3的引脚配置进行调整
    slot_config.clk = GPIO_NUM_11;
    slot_config.cmd = GPIO_NUM_12;
    slot_config.d0 = GPIO_NUM_13;
    slot_config.d1 = GPIO_NUM_14;
    slot_config.d2 = GPIO_NUM_15;
    slot_config.d3 = GPIO_NUM_16;
    slot_config.width = 4; // 4线模式

    printf("Attempting to mount SD card at %s\n", SDCARD_MOUNT_POINT);

    ret = esp_vfs_fat_sdmmc_mount(SDCARD_MOUNT_POINT, &host, &slot_config, &mount_config, &sd_card_handle);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount filesystem. "
                     "If you want the card to be formatted, set format_if_mount_failed = true.\n");
        } else {
            printf("Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.\n", esp_err_to_name(ret));
        }
        return ret;
    }

    // 打印SD卡信息
    sdmmc_card_print_info(stdout, sd_card_handle);
    
    is_card_mounted = true;
    
    printf("SD card mounted successfully at %s\n", SDCARD_MOUNT_POINT);

    // 确保书籍目录存在
    sdcard_ensure_books_dir();

    return ESP_OK;
}

/**
 * @brief 卸载SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_unmount(void) {
    if (!is_card_mounted) {
        printf("SD card is not mounted\n");
        return ESP_OK;
    }

    esp_err_t ret = esp_vfs_fat_sdmmc_unmount();
    if (ret != ESP_OK) {
        printf("Failed to unmount SD card, error: %s\n", esp_err_to_name(ret));
        return ret;
    }

    is_card_mounted = false;
    sd_card_handle = NULL;

    printf("SD card unmounted successfully\n");
    return ESP_OK;
}

/**
 * @brief 检查SD卡是否已挂载
 * @return true 已挂载，false 未挂载
 */
bool sdcard_is_mounted(void) {
    return is_card_mounted;
}

/**
 * @brief 获取SD卡总容量（字节）
 * @return 总容量，失败返回0
 */
uint64_t sdcard_get_total_bytes(void) {
    if (!is_card_mounted) {
        return 0;
    }

    size_t total_sectors = sd_card_handle->csd.capacity;
    size_t sector_size = sd_card_handle->csd.sector_size;
    
    return (uint64_t)total_sectors * sector_size;
}

/**
 * @brief 获取SD卡可用容量（字节）
 * @return 可用容量，失败返回0
 */
uint64_t sdcard_get_free_bytes(void) {
    if (!is_card_mounted) {
        return 0;
    }

    FATFS *fs;
    DWORD fre_clust;
    uint64_t total_bytes, free_bytes;

    // 获取FAT文件系统信息
    FRESULT fr = f_getfree(SDCARD_MOUNT_POINT + 1, &fre_clust, &fs); // +1 to skip leading '/'
    if (fr != FR_OK) {
        printf("Failed to get free space, error: %d\n", fr);
        return 0;
    }

    // 计算总扇区数和空闲扇区数
    DWORD tot_sect = (DWORD)(fs->n_fatent - 2) * fs->csize;
    DWORD fre_sect = (DWORD)fre_clust * fs->csize;

    // 转换为字节数 (使用默认扇区大小512，因为FATFS通常使用固定扇区大小)
    uint32_t sector_size = 512;
    total_bytes = (uint64_t)tot_sect * sector_size;
    free_bytes = (uint64_t)fre_sect * sector_size;

    return free_bytes;
}

/**
 * @brief 确保书籍目录存在
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_ensure_books_dir(void) {
    if (!is_card_mounted) {
        printf("SD card not mounted, cannot create books directory\n");
        return ESP_ERR_INVALID_STATE;
    }

    struct stat st;
    if (stat(SDCARD_BOOKS_DIR, &st) != 0) {
        // 目录不存在，创建它
        if (mkdir(SDCARD_BOOKS_DIR, 0755) == 0) {
            printf("Books directory created: %s\n", SDCARD_BOOKS_DIR);
            return ESP_OK;
        } else {
            printf("Failed to create books directory: %s\n", SDCARD_BOOKS_DIR);
            return ESP_FAIL;
        }
    } else {
        printf("Books directory already exists: %s\n", SDCARD_BOOKS_DIR);
    }

    return ESP_OK;
}