#pragma once
#include <cstdint>
#include <string>

/**
 * 设备语言
 */
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

/**
 * 刷新模式
 */
enum RefreshMode {
    /**
     * 高清
     */
    Quality = 0, 
    /**
     * 快速
     */
    Fast, 
};

/**
 * 字体大小
 */
enum FontSize {
    Small = 0, 
    Medium = 1, 
    Large = 2, 
};

/**
 * 设备配置结构体
 */
struct DeviceConfig {
    uint8_t version = 0; // 配置版本号
    LanguageEnum language = LanguageEnum::Chinese;  // 语言设置    
    uint8_t refreshInterval = 10;    // 自动刷新间隔（秒）
    std::string fontPath = "";           // 字体文件路径  
    RefreshMode refreshMode = RefreshMode::Quality; // 刷新模式
    FontSize fontSize = FontSize::Medium; // 字体大小
};