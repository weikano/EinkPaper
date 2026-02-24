#include "NvsFlashManager.h"
#include "nvs_flash.h"

esp_err_t NvsFlashManager::init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

        // NVS 分区结构不匹配或满了
        nvs_flash_erase();
        nvs_flash_init();
    }
    return ret;
}