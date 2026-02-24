#include "LittleFsManager.h"

#include "esp_littlefs.h"
#include "esp_log.h"

static const char* TAG = "LittleFsManager";
static constexpr const char* LFS_BASE_PATH = "/littlefs";
static constexpr const char* LFS_PARTITION_LABEL = "storage";

esp_err_t LittleFsManager::mount() {
    if (_mounted) {
        return ESP_OK;
    }

    esp_vfs_littlefs_conf_t conf = {};
    conf.base_path = LFS_BASE_PATH;
    conf.partition_label = LFS_PARTITION_LABEL;
    conf.format_if_mount_failed = false;
    conf.dont_mount = false;

    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount littlefs (%s)", esp_err_to_name(ret));
        return ret;
    }

    size_t total = 0;
    size_t used = 0;
    ret = esp_littlefs_info(LFS_PARTITION_LABEL, &total, &used);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "LittleFS mounted, total=%u used=%u", static_cast<unsigned>(total),
                 static_cast<unsigned>(used));
    }

    _mounted = true;
    return ESP_OK;
}

void LittleFsManager::unmount() {
    if (!_mounted) {
        return;
    }
    esp_vfs_littlefs_unregister(LFS_PARTITION_LABEL);
    _mounted = false;
}

