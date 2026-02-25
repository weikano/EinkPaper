#include "SettingsCommon.h"

const char* getSettingTitle(SettingKey key) {
    switch (key) {
        case SettingKey::Language:
            return "语言";
        case SettingKey::RefreshMode:
            return "刷新模式";
        case SettingKey::FontSize:
            return "字体大小";
        case SettingKey::AutoSleep:
            return "自动休眠";
        default:
            return "未知设置";
    }
}

std::string getSettingCurrentValueText(SettingKey key, const DeviceConfig& config) {
    switch (key) {
        case SettingKey::Language:
            switch (config.language) {
                case Language::Chinese:
                    return "中文";
                case Language::English:
                    return "English";
                case Language::TraditionalChinese:
                    return "繁体中文";
                default:
                    return "未知";
            }
        case SettingKey::RefreshMode:
            return config.refreshMode == RefreshMode::Quality ? "高清" : "快速";
        case SettingKey::FontSize:
            switch (config.fontSize) {
                case FontSize::Small:
                    return "小";
                case FontSize::Medium:
                    return "中";
                case FontSize::Large:
                    return "大";
                default:
                    return "未知";
            }
        case SettingKey::AutoSleep:
            switch (config.autoSleep) {
                case AutoSleepDuration::Sleep1Min:
                    return "1分钟";
                case AutoSleepDuration::Sleep3Min:
                    return "3分钟";
                case AutoSleepDuration::Sleep5Min:
                    return "5分钟";
                case AutoSleepDuration::Sleep10Min:
                    return "10分钟";
                case AutoSleepDuration::Sleep30Min:
                    return "30分钟";
                case AutoSleepDuration::SleepNever:
                    return "永不";
                default:
                    return "未知";
            }
        default:
            return "未知";
    }
}

std::vector<std::pair<int, std::string>> getSettingOptions(SettingKey key) {
    switch (key) {
        case SettingKey::Language:
            return {
                {Language::Chinese, "中文"},
                {Language::English, "English"},
                {Language::TraditionalChinese, "繁体中文"},
            };
        case SettingKey::RefreshMode:
            return {
                {RefreshMode::Quality, "高清"},
                {RefreshMode::Fast, "快速"},
            };
        case SettingKey::FontSize:
            return {
                {FontSize::Small, "小"},
                {FontSize::Medium, "中"},
                {FontSize::Large, "大"},
            };
        case SettingKey::AutoSleep:
            return {
                {AutoSleepDuration::Sleep1Min, "1分钟"},
                {AutoSleepDuration::Sleep3Min, "3分钟"},
                {AutoSleepDuration::Sleep5Min, "5分钟"},
                {AutoSleepDuration::Sleep10Min, "10分钟"},
                {AutoSleepDuration::Sleep30Min, "30分钟"},
                {AutoSleepDuration::SleepNever, "永不"},
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
