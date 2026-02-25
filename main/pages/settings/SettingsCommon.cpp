#include "SettingsCommon.h"

const char* getSettingTitle(SettingKey key) {
    switch (key) {
        case SettingKey::Language:
            return tr(StringId::SETTING_LANGUAGE);
        case SettingKey::RefreshMode:
            return tr(StringId::SETTING_REFRESH_MODE);
        case SettingKey::FontSize:
            return tr(StringId::SETTING_FONT_SIZE);
        case SettingKey::AutoSleep:
            return tr(StringId::SETTING_AUTO_SLEEP);
        default:
            return tr(StringId::VALUE_UNKNOWN);
    }
}

std::string getSettingCurrentValueText(SettingKey key, const DeviceConfig& config) {
    switch (key) {
        case SettingKey::Language:
            switch (config.language) {
                case Language::Chinese:
                    return tr(StringId::VALUE_LANGUAGE_CHINESE);
                case Language::English:
                    return tr(StringId::VALUE_LANGUAGE_ENGLISH);
                case Language::TraditionalChinese:
                    return tr(StringId::VALUE_LANGUAGE_TRADITIONAL_CHINESE);
                default:
                    return tr(StringId::VALUE_UNKNOWN);
            }
        case SettingKey::RefreshMode:
            return config.refreshMode == RefreshMode::Quality
                       ? tr(StringId::VALUE_REFRESH_QUALITY)
                       : tr(StringId::VALUE_REFRESH_FAST);
        case SettingKey::FontSize:
            switch (config.fontSize) {
                case FontSize::Small:
                    return tr(StringId::VALUE_FONT_SMALL);
                case FontSize::Medium:
                    return tr(StringId::VALUE_FONT_MEDIUM);
                case FontSize::Large:
                    return tr(StringId::VALUE_FONT_LARGE);
                default:
                    return tr(StringId::VALUE_UNKNOWN);
            }
        case SettingKey::AutoSleep:
            switch (config.autoSleep) {
                case AutoSleepDuration::Sleep1Min:
                    return tr(StringId::VALUE_SLEEP_1_MIN);
                case AutoSleepDuration::Sleep3Min:
                    return tr(StringId::VALUE_SLEEP_3_MIN);
                case AutoSleepDuration::Sleep5Min:
                    return tr(StringId::VALUE_SLEEP_5_MIN);
                case AutoSleepDuration::Sleep10Min:
                    return tr(StringId::VALUE_SLEEP_10_MIN);
                case AutoSleepDuration::Sleep30Min:
                    return tr(StringId::VALUE_SLEEP_30_MIN);
                case AutoSleepDuration::SleepNever:
                    return tr(StringId::VALUE_SLEEP_NEVER);
                default:
                    return tr(StringId::VALUE_UNKNOWN);
            }
        default:
            return tr(StringId::VALUE_UNKNOWN);
    }
}

std::vector<std::pair<int, std::string>> getSettingOptions(SettingKey key) {
    switch (key) {
        case SettingKey::Language:
            return {
                {Language::Chinese, tr(StringId::VALUE_LANGUAGE_CHINESE)},
                {Language::English, tr(StringId::VALUE_LANGUAGE_ENGLISH)},
                {Language::TraditionalChinese, tr(StringId::VALUE_LANGUAGE_TRADITIONAL_CHINESE)},
            };
        case SettingKey::RefreshMode:
            return {
                {RefreshMode::Quality, tr(StringId::VALUE_REFRESH_QUALITY)},
                {RefreshMode::Fast, tr(StringId::VALUE_REFRESH_FAST)},
            };
        case SettingKey::FontSize:
            return {
                {FontSize::Small, tr(StringId::VALUE_FONT_SMALL)},
                {FontSize::Medium, tr(StringId::VALUE_FONT_MEDIUM)},
                {FontSize::Large, tr(StringId::VALUE_FONT_LARGE)},
            };
        case SettingKey::AutoSleep:
            return {
                {AutoSleepDuration::Sleep1Min, tr(StringId::VALUE_SLEEP_1_MIN)},
                {AutoSleepDuration::Sleep3Min, tr(StringId::VALUE_SLEEP_3_MIN)},
                {AutoSleepDuration::Sleep5Min, tr(StringId::VALUE_SLEEP_5_MIN)},
                {AutoSleepDuration::Sleep10Min, tr(StringId::VALUE_SLEEP_10_MIN)},
                {AutoSleepDuration::Sleep30Min, tr(StringId::VALUE_SLEEP_30_MIN)},
                {AutoSleepDuration::SleepNever, tr(StringId::VALUE_SLEEP_NEVER)},
            };
        default:
            return {};
    }
}

void applySettingValue(DeviceConfig& config, SettingKey key, int value) {
    switch (key) {
        case SettingKey::Language:
            config.language = static_cast<Language::LanguageEnum>(value);
            break;
        case SettingKey::RefreshMode:
            config.refreshMode = static_cast<RefreshMode::RefreshModeEnum>(value);
            break;
        case SettingKey::FontSize:
            config.fontSize = static_cast<FontSize::FontSizeEnum>(value);
            break;
        case SettingKey::AutoSleep:
            config.autoSleep = static_cast<AutoSleepDuration::AutoSleepDurationEnum>(value);
            break;
        default:
            break;
    }
}

int getSelectedSettingValue(SettingKey key, const DeviceConfig& config) {
    switch (key) {
        case SettingKey::Language:
            return static_cast<int>(config.language);
        case SettingKey::RefreshMode:
            return static_cast<int>(config.refreshMode);
        case SettingKey::FontSize:
            return static_cast<int>(config.fontSize);
        case SettingKey::AutoSleep:
            return static_cast<int>(config.autoSleep);
        default:
            return 0;
    }
}
