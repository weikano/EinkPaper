#pragma once

#include <string>

class CrashReport {
public:
    CrashReport() = default;
    ~CrashReport() = default;

    /**
     * @brief 获取 CrashReport 单例实例
     * @return CrashReport& CrashReport 单例实例
     */
    static CrashReport& getInstance() {
        static CrashReport instance;
        return instance;
    }
    /**
     * @brief 保存崩溃报告到 SD 卡
     */
    void saveCrashReport();

    /**
     * @brief 获取崩溃报告目录
     * @return const std::string& 崩溃报告目录
     */
    const std::string& getCrashReportDir() const;
    
};