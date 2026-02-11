#include "DeviceConfigManager.h"
#include "esp_log.h"
#include <fstream>
#include <iostream>
#include <cstring>

static const char* TAG = "DeviceConfigManager";
static const char* CONFIG_FILE_PATH = "/sdcard/config.bin";

void DeviceConfigManager::loadConfigFromSdCard() {
    ESP_LOGI(TAG, "Loading config from SD card: %s", CONFIG_FILE_PATH);
    
    // 初始化为默认值（来自结构体定义）
    _config = {};
    _config.version = 0;
    _config.language = LanguageEnum::Chinese;
    _config.refreshInterval = 10;
    _config.refreshMode = RefreshMode::Quality;
    _config.fontSize = FontSize::Medium;
    _config.fontPath = "";  // std::string 默认值
    
    // 尝试打开配置文件
    FILE* file = fopen(CONFIG_FILE_PATH, "rb");
    if (!file) {
        ESP_LOGW(TAG, "Config file not found, using default config: %s", CONFIG_FILE_PATH);
        return;
    }
    
    // 读取配置版本
    uint8_t version;
    if (fread(&version, sizeof(uint8_t), 1, file) != 1) {
        ESP_LOGW(TAG, "Failed to read config version, using default config");
        fclose(file);
        return;
    }
    
    // 验证配置版本，如果是旧版本可以在这里做兼容处理
    if (version != _config.version) {
        ESP_LOGW(TAG, "Config version mismatch, using default config");
        fclose(file);
        return;
    }
    
    // 读取配置数据
    if (fread(&_config.language, sizeof(LanguageEnum), 1, file) != 1 ||
        fread(&_config.refreshInterval, sizeof(uint8_t), 1, file) != 1 ||
        fread(&_config.refreshMode, sizeof(RefreshMode), 1, file) != 1 ||
        fread(&_config.fontSize, sizeof(FontSize), 1, file) != 1) {
        ESP_LOGW(TAG, "Failed to read basic config values, using default config");
        fclose(file);
        return;
    }
    
    // 读取字体路径长度
    uint32_t fontPathLen;
    if (fread(&fontPathLen, sizeof(uint32_t), 1, file) != 1) {
        ESP_LOGW(TAG, "Failed to read font path length, using default config");
        fclose(file);
        return;
    }
    
    // 限制最大长度以防止内存问题
    if (fontPathLen > 1024) {
        ESP_LOGW(TAG, "Font path too long (%u), using default config", fontPathLen);
        fclose(file);
        return;
    }
    
    // 读取字体路径内容
    if (fontPathLen > 0) {
        char* buffer = new char[fontPathLen + 1];  // +1 for null terminator
        if (fread(buffer, 1, fontPathLen, file) == fontPathLen) {
            buffer[fontPathLen] = '\0';  // 确保字符串以null结尾
            _config.fontPath = std::string(buffer);
        }
        delete[] buffer;
    } else {
        _config.fontPath = std::string("");
    }
    
    fclose(file);
    
    ESP_LOGI(TAG, "Config loaded successfully from SD card");
}

void DeviceConfigManager::saveConfigToSdCard() {
    ESP_LOGI(TAG, "Saving config to SD card: %s", CONFIG_FILE_PATH);
    
    // 将配置写入文件
    FILE* file = fopen(CONFIG_FILE_PATH, "wb");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open config file for writing: %s", CONFIG_FILE_PATH);
        return;
    }
    
    // 写入配置版本
    uint8_t version = _config.version;
    if (fwrite(&version, sizeof(uint8_t), 1, file) != 1) {
        ESP_LOGE(TAG, "Failed to write config version");
        fclose(file);
        return;
    }
    
    // 写入配置数据
    if (fwrite(&_config.language, sizeof(LanguageEnum), 1, file) != 1 ||
        fwrite(&_config.refreshInterval, sizeof(uint8_t), 1, file) != 1 ||
        fwrite(&_config.refreshMode, sizeof(RefreshMode), 1, file) != 1 ||
        fwrite(&_config.fontSize, sizeof(FontSize), 1, file) != 1) {
        ESP_LOGE(TAG, "Failed to write basic config values");
        fclose(file);
        return;
    }
    
    // 写入字体路径长度和内容
    uint32_t fontPathLen = _config.fontPath.length();
    if (fwrite(&fontPathLen, sizeof(uint32_t), 1, file) != 1) {
        ESP_LOGE(TAG, "Failed to write font path length");
        fclose(file);
        return;
    }
    
    if (fontPathLen > 0) {
        if (fwrite(_config.fontPath.c_str(), 1, fontPathLen, file) != fontPathLen) {
            ESP_LOGE(TAG, "Failed to write font path content");
            fclose(file);
            return;
        }
    }
    
    fclose(file);
    
    ESP_LOGI(TAG, "Config saved successfully to SD card");
}