#include "DeviceConfigManager.h"

#include "esp_log.h"

static const char* TAG = "DeviceConfigManager";
static const char* CONFIG_FILE_PATH = "/sdcard/config.bin";

void DeviceConfigManager::loadConfigFromSdCard() {
    ESP_LOGD(TAG, "loadConfigFromSdCard");
}

void DeviceConfigManager::saveConfigToSdCard() 
{
    ESP_LOGD(TAG, "saveConfigToSdCard");
}