#include "CrashReport.h"

#include "esp_log.h"
#include "esp_core_dump.h"
#include "esp_partition.h"
#include "sys/stat.h"
#include "../version.h"


static const char* TAG = "CrashReport";
static const std::string CRASH_REPORT_DIR = "/sdcard/crashlog/";

const std::string& CrashReport::getCrashReportDir() const { return CRASH_REPORT_DIR; }

/**
 * @brief 保存崩溃报告到 SD 卡
 * 需要通过sdkconfig设置core dump的data destination到flash
 * cmakelists.txt中添加espcoredump和esp_partition
 * partitions.csv分区表中需要添加coredump分区
 * 
 * 该函数会从 Flash 中读取核心转储镜像，并将其保存到 SD 卡的指定目录中。
 * 文件名格式为：core-<提交时间>-<提交哈希>.bin
 */
void CrashReport::saveCrashReport() {
    ESP_LOGI(TAG, "Saving core dump to SD card...\n");
    size_t size = 0;
    size_t address = 0; 

    // 获取 Core Dump 镜像信息
    if (esp_core_dump_image_get(&address, &size) != ESP_OK || size == 0) {
        ESP_LOGI(TAG, "No core dump found in flash.\n");
        return;
    }

    ESP_LOGI(TAG, "Found Core Dump! Size: %d bytes.\n", (int)size);
    char path[128];
    sprintf(path, "%s/coredump-%s-%s.bin", CRASH_REPORT_DIR.c_str(), GIT_COMMIT_TIME, GIT_COMMIT_HASH);
    // const char* path = "/sdcard/books/core.bin";
    mkdir(CRASH_REPORT_DIR.c_str(), 0777);
    FILE* f = fopen(path, "wb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open SD card file.\n");
        return;
    }

    // C++ 必须显式转换 malloc 的返回值类型
    uint8_t *buffer = (uint8_t *)malloc(1024);
    if (buffer) {
        // 查找核心转储分区
        const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, 
                                                               ESP_PARTITION_SUBTYPE_DATA_COREDUMP, 
                                                               NULL);
        if (part) {
            size_t read_pos = 0;
            while (read_pos < size) {
                size_t len = (size - read_pos > 1024) ? 1024 : (size - read_pos);
                // 读取分区数据
                esp_partition_read(part, read_pos, buffer, len);
                fwrite(buffer, 1, len, f);
                read_pos += len;
            }
            ESP_LOGI(TAG, "Core dump successfully saved to SD card.\n");
        } else {
            ESP_LOGE(TAG, "Coredump partition not found!\n");
        }
        free(buffer);
    }
    
    fclose(f);

    // 完成后清除 Flash 标记
    esp_core_dump_image_erase();
}