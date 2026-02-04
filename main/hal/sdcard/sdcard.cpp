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


static bool _is_sd_card_mounted = false;
static bool _spi_bus_initialized = false;
static sdmmc_card_t* _sd_card    = nullptr;

/**
 * @brief 初始化SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t sdcard_init(void)
{
    if (_is_sd_card_mounted)
    {
        printf("SD card already mounted.\n");
        return ESP_OK;
    }
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 16 * 1024};
        
    const char mount_point[] = SDCARD_MOUNT_POINT;
    printf("initializing SD card...\n");
    printf("using SPI peripheral\n");

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
            printf("failed to initialize SPI bus, error: %s\n", esp_err_to_name(ret));            
            return ret;
        }
        _spi_bus_initialized = true;
        printf("SPI bus initialized.\n");
    } else {
        printf("SPI bus already initialized.\n");
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs               = PIN_CS;
    slot_config.host_id               = (spi_host_device_t)host.slot;

    printf("mounting filesystem...\n");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &_sd_card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("failed to mount filesystem, error: %s\n", esp_err_to_name(ret));
        } else {
            printf("failed to initialize the card, make sure SD card lines have pull-up resistors\n");
        }

        // Don't clean up SPI bus on failure - leave it for retry
        printf("sd card init failed, but spi bus remains initialized for retry\n");
        return ret;
    }

    printf("filesystem mounted successfully.\n");

    sdmmc_card_print_info(stdout, _sd_card);

    _is_sd_card_mounted = true;
    return ret;
}