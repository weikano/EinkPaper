#pragma once

#include "DeviceConfig.h"
#include <cstdlib>
#include <cstring>

class DeviceConfigManager {
public:
    static DeviceConfigManager& getInstance() {
        static DeviceConfigManager instance;  // C++11标准保证线程安全
        return instance;
    }
    void loadConfigFromSdCard();
    void saveConfigToSdCard();
    DeviceConfig& getConfig() { return _config; }
    const DeviceConfig& getConfig() const { return _config; }
    void setConfig(const DeviceConfig& config) { 
        // 复制新配置
        _config.version = config.version;
        _config.language = config.language;
        _config.refreshInterval = config.refreshInterval;
        _config.refreshMode = config.refreshMode;
        _config.fontSize = config.fontSize;
        _config.fontPath = config.fontPath;  // std::string可以直接赋值
    }
private:
    DeviceConfig _config;
    

};