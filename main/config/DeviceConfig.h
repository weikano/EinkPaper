#pragma once
#include <cstdint>
#include <string>
#include "M5GFX.h"

/**
 * 设备语言
 */
struct Language {
    enum LanguageEnum {
        /**
         * 中文
         */
        Chinese = 0,
        /**
         * 英文
         */
        English,
        /**
         * 繁体中文
         */
        TraditionalChinese,
    };
};

/**
 * 刷新模式
 */
struct RefreshMode {
    enum RefreshModeEnum {
        /**
         * 高清
         */
        Quality = 0,
        /**
         * 快速
         */
        Fast,
    };

    static m5gfx::epd_mode_t getForceRefreshMode(RefreshModeEnum mode) {
        return mode == Quality ? m5gfx::epd_mode_t::epd_quality
                               : m5gfx::epd_mode_t::epd_fast;
    }
};

/**
 * 字体大小
 */
struct FontSize {
    enum FontSizeEnum {
        Small = 0,
        Medium = 1,
        Large = 2,
    };

    static float getFontScale(FontSizeEnum size) {
        switch (size) {
            case Small:
                return 0.8f;
            case Large:
                return 1.2f;
            case Medium:
            default:
                return 1.0f;
        }
    }
};

/**
 * 自动休眠时间
 */
struct AutoSleepDuration {
    enum AutoSleepDurationEnum {        
        Sleep1Min = 0,
        Sleep3Min = 1,
        Sleep5Min = 2,
        Sleep10Min = 3,
        Sleep30Min = 4,
        SleepNever = 5,
    };

    static int64_t getAutoSleepDurationInMS(AutoSleepDurationEnum duration) {
        switch (duration) {
            case Sleep1Min:
                return 60 * 1000LL;
            case Sleep3Min:
                return 3 * 60 * 1000LL;
            case Sleep5Min:
                return 5 * 60 * 1000LL;
            case Sleep10Min:
                return 10 * 60 * 1000LL;
            case Sleep30Min:
                return 30 * 60 * 1000LL;
            case SleepNever:
            default:
                return -1;
        }
    }
};

/**
 * 设备配置结构体
 */
struct DeviceConfig {
    uint8_t version = 1; // 配置版本号
    Language::LanguageEnum language = Language::Chinese;  // 语言设置
    uint8_t refreshInterval = 10;    // 自动刷新间隔（秒）
    std::string fontPath = "";           // 字体文件路径  
    RefreshMode::RefreshModeEnum refreshMode = RefreshMode::Quality; // 刷新模式
    FontSize::FontSizeEnum fontSize = FontSize::Medium; // 字体大小
    AutoSleepDuration::AutoSleepDurationEnum autoSleep = AutoSleepDuration::Sleep10Min; // 自动休眠时间
};
