#pragma once

#include "../../config/DeviceConfig.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

enum class SettingKey : uint8_t {
    Language = 0,
    RefreshMode,
    FontSize,
    AutoSleep,
};

struct SettingsSubPageParams {
    SettingKey settingKey = SettingKey::Language;
};

const char* getSettingTitle(SettingKey key);
std::string getSettingCurrentValueText(SettingKey key, const DeviceConfig& config);
std::vector<std::pair<int, std::string>> getSettingOptions(SettingKey key);
void applySettingValue(DeviceConfig& config, SettingKey key, int value);
