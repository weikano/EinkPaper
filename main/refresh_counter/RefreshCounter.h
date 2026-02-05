#pragma once

#include "M5GFX.h"

/**
 * @brief 刷新计数器 - 控制墨水屏刷新模式
 * 
 * 单例模式，用于管理刷新次数并在达到阈值时执行全刷
 */
class RefreshCounter {
public:
    /**
     * @brief 获取单例实例
     * @return RefreshCounter单例实例引用
     */
    static RefreshCounter& getInstance();

    /**
     * @brief 初始化刷新计数器
     * @param fullRefreshThreshold 全刷阈值（每隔多少次刷新执行一次全刷）
     */
    void init(uint32_t fullRefreshThreshold = 10);

    /**
     * @brief 刷新计数器，增加计数并返回适当的刷新模式
     * @return epd_mode_t 刷新模式
     */
    m5gfx::epd_mode_t refresh();

    /**
     * @brief 重置计数器
     */
    void reset();

    /**
     * @brief 获取当前刷新计数
     * @return 当前刷新次数
     */
    uint32_t getCount() const { return _refreshCount; }

    /**
     * @brief 获取全刷阈值
     * @return 全刷阈值
     */
    uint32_t getThreshold() const { return _fullRefreshThreshold; }

private:
    uint32_t _refreshCount;          ///< 当前刷新次数
    uint32_t _fullRefreshThreshold;  ///< 全刷阈值
    bool _initialized;               ///< 是否已初始化

    /**
     * @brief 构造函数（私有，确保单例）
     */
    RefreshCounter();

    /**
     * @brief 析构函数
     */
    ~RefreshCounter() = default;
    
    // 删除拷贝构造和赋值操作符，确保单例特性
    RefreshCounter(const RefreshCounter&) = delete;
    RefreshCounter& operator=(const RefreshCounter&) = delete;
};