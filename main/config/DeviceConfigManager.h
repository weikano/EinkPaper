#pragma once

#include "DeviceConfig.h"

class DeviceConfigManager {
public:
    static DeviceConfigManager& getInstance() {
        static DeviceConfigManager instance;  // C++11标准保证线程安全
        return instance;
    }
    void loadConfigFromSdCard();
    void saveConfigToSdCard();
    DeviceConfig getConfig() const { return _config; }
private:
    DeviceConfig _config;
};    