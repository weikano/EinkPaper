#pragma once

/**
 * 设备语言
 */
enum class LanguageEnum {    
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
 * 设备配置结构体
 */
struct DeviceConfig {
    LanguageEnum language;  // 语言设置    
    int refreshInterval = 10;    // 自动刷新间隔（秒）
    char* fontPath;           // 字体文件路径  
};