#pragma once

/**
 * @brief 页面类型枚举 - 用于区分不同类型的页面
 */
enum class PageType {
    UNKNOWN = 0,
    FILE_BROWSER,
    SETTINGS,
    READER,
    MENU,
    DIALOG,
    CUSTOM
};