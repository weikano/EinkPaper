#include "sdcard.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "ff.h"          // FatFs
#include "driver/gpio.h" // For GPIO_NUM definitions
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>


static bool _is_sd_card_mounted = false;
static bool _spi_bus_initialized = false;
static sdmmc_card_t* _sd_card    = nullptr;

static const char *TAG = "SDCard";

/**
 * @brief 初始化SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_init(void)
{
    if (_is_sd_card_mounted)
    {
        ESP_LOGW(TAG, "SD card already mounted.");
        return ESP_OK;
    }
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 16 * 1024};
        
    const char mount_point[] = SDCARD_MOUNT_POINT;
    ESP_LOGI(TAG, "initializing SD card...");
    ESP_LOGI(TAG, "using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num     = PIN_MOSI,
        .miso_io_num     = PIN_MISO,
        .sclk_io_num     = PIN_SCLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = 4000,
    };

    if (!_spi_bus_initialized) {
        ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "failed to initialize SPI bus, error: %s", esp_err_to_name(ret));            
            return ret;
        }
        _spi_bus_initialized = true;
        ESP_LOGI(TAG, "SPI bus initialized.");
    } else {
        ESP_LOGI(TAG, "SPI bus already initialized.");
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs               = PIN_CS;
    slot_config.host_id               = (spi_host_device_t)host.slot;

    ESP_LOGI(TAG, "mounting filesystem...");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &_sd_card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "failed to mount filesystem, error: %s", esp_err_to_name(ret));
            ESP_LOGE(TAG, "failed to initialize the card, make sure SD card lines have pull-up resistors");
        }

        // Don't clean up SPI bus on failure - leave it for retry
        ESP_LOGW(TAG, "sd card init failed, but spi bus remains initialized for retry");
        return ret;
    }

    ESP_LOGI(TAG, "filesystem mounted successfully.");

    sdmmc_card_print_info(stdout, _sd_card);

    _is_sd_card_mounted = true;
    return ret;
}

/**
 * @brief 挂载SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_mount(void)
{
    if (_is_sd_card_mounted) {
        ESP_LOGW(TAG, "SD card already mounted.");
        return ESP_OK;
    }

    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 16 * 1024};

    const char mount_point[] = SDCARD_MOUNT_POINT;
    ESP_LOGI(TAG, "Mounting SD card to %s...", mount_point);

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_CS;
    slot_config.host_id = (spi_host_device_t)host.slot;

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &_sd_card);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card (0x%x)", ret);
        return ret;
    }

    ESP_LOGI(TAG, "SD card mounted successfully to %s", mount_point);

    sdmmc_card_print_info(stdout, _sd_card);

    _is_sd_card_mounted = true;
    return ESP_OK;
}

/**
 * @brief 卸载SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_unmount(void)
{
    if (!_is_sd_card_mounted) {
        ESP_LOGW(TAG, "SD card not mounted.");
        return ESP_OK;
    }

    esp_err_t ret = esp_vfs_fat_sdcard_unmount(SDCARD_MOUNT_POINT, _sd_card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unmount SD card (0x%x)", ret);
        return ret;
    }

    _is_sd_card_mounted = false;
    _sd_card = nullptr;

    ESP_LOGI(TAG, "SD card unmounted successfully from %s", SDCARD_MOUNT_POINT);
    return ESP_OK;
}

/**
 * @brief 检查SD卡是否已挂载
 * @return true 已挂载，false 未挂载
 */
bool sdcard_is_mounted(void)
{
    return _is_sd_card_mounted;
}

/**
 * @brief 获取SD卡总容量（字节）
 * @return 总容量（字节）
 */
uint64_t sdcard_get_total_bytes(void)
{
    if (!_is_sd_card_mounted) {
        return 0;
    }

    FATFS *fs;
    DWORD fre_clust;
    uint64_t total_sectors, free_sectors;

    FRESULT res = f_getfree("0:", &fre_clust, &fs);
    if (res != FR_OK) {
        ESP_LOGE(TAG, "Failed to get SD card info (0x%x)", res);
        return 0;
    }

    total_sectors = (fs->n_fatent - 2) * fs->csize;
    return total_sectors * 512;  // 512 bytes per sector
}

/**
 * @brief 获取SD卡可用容量（字节）
 * @return 可用容量（字节）
 */
uint64_t sdcard_get_free_bytes(void)
{
    if (!_is_sd_card_mounted) {
        return 0;
    }

    FATFS *fs;
    DWORD fre_clust;
    uint64_t total_sectors, free_sectors;

    FRESULT res = f_getfree("0:", &fre_clust, &fs);
    if (res != FR_OK) {
        ESP_LOGE(TAG, "Failed to get SD card free space (0x%x)", res);
        return 0;
    }

    free_sectors = fre_clust * fs->csize;
    return free_sectors * 512;  // 512 bytes per sector
}

/**
 * @brief 确保书籍目录存在
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_ensure_books_dir(void)
{
    if (!_is_sd_card_mounted) {
        ESP_LOGE(TAG, "SD card not mounted");
        return ESP_ERR_INVALID_STATE;
    }

    struct stat st;
    if (stat(SDCARD_BOOKS_DIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            ESP_LOGI(TAG, "Books directory already exists: %s", SDCARD_BOOKS_DIR);
            return ESP_OK;
        } else {
            ESP_LOGE(TAG, "Path exists but is not a directory: %s", SDCARD_BOOKS_DIR);
            return ESP_FAIL;
        }
    }

    // Create directory
    int ret = mkdir(SDCARD_BOOKS_DIR, 0755);
    if (ret == 0) {
        ESP_LOGI(TAG, "Created books directory: %s", SDCARD_BOOKS_DIR);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to create books directory: %s (errno=%d)", SDCARD_BOOKS_DIR, errno);
        return ESP_FAIL;
    }
}