#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

static const char *TAG = "SD_CARD";

// SD卡挂载点
#define MOUNT_POINT "/sdcard"

/**
 * @brief 挂载SD卡
 * @return ESP_OK 成功，其他值失败
 */
esp_err_t mount_sd_card(void) {
    esp_err_t ret = ESP_FAIL;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;

    ESP_LOGI(TAG, "Mounting SD card...");
    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }

    // 打印卡信息
    sdmmc_card_print_info(stdout, card);
    ESP_LOGI(TAG, "SD card mounted successfully at %s", MOUNT_POINT);
    
    return ESP_OK;
}

/**
 * @brief 创建测试目录和文件
 */
void create_test_files(void) {
    // 确保books目录存在
    const char *books_dir = "/sdcard/books";
    struct stat st;
    if (stat(books_dir, &st) != 0) {
        if (mkdir(books_dir, 0755) == 0) {
            ESP_LOGI(TAG, "Created directory: %s", books_dir);
        } else {
            ESP_LOGE(TAG, "Failed to create directory: %s", books_dir);
        }
    }
    
    // 创建一些测试文件
    const char *test_files[] = {
        "/sdcard/books/test1.txt",
        "/sdcard/books/novel.epub",
        "/sdcard/books/sample.txt",
        "/sdcard/books/subdir/test2.txt"
    };
    
    // 创建子目录
    const char *subdir = "/sdcard/books/subdir";
    if (stat(subdir, &st) != 0) {
        mkdir(subdir, 0755);
    }
    
    for (int i = 0; i < sizeof(test_files)/sizeof(test_files[0]); i++) {
        FILE *f = fopen(test_files[i], "w");
        if (f) {
            fprintf(f, "Test content for %s\n", test_files[i]);
            fclose(f);
            ESP_LOGI(TAG, "Created test file: %s", test_files[i]);
        }
    }
}