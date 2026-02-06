#pragma once

#include <M5Unified.h>
#include "esp_log.h"

/**
 * @brief 触摸手势检测器 - 用于检测滑动手势
 * 
 * 提供滑动方向检测功能，可用于页面切换等场景
 */
class TouchGestureDetector {
public:
    /**
     * @brief 滑动方向枚举
     */
    enum class SwipeDirection {
        NONE = 0,    ///< 无滑动
        LEFT,        ///< 向左滑动
        RIGHT,       ///< 向右滑动
        UP,          ///< 向上滑动
        DOWN         ///< 向下滑动
    };

    /**
     * @brief 构造函数
     */
    TouchGestureDetector();

    /**
     * @brief 更新触摸状态
     * @param touch 详细的触摸信息
     * @return 滑动方向
     */
    SwipeDirection updateTouch(const m5::touch_detail_t& touch);

    /**
     * @brief 重置状态
     */
    void reset();

private:
    bool _isTouching;                    ///< 是否正在触摸
    uint32_t _touchStartTime;           ///< 触摸开始时间
    int16_t _startX;                    ///< 触摸起始X坐标
    int16_t _startY;                    ///< 触摸起始Y坐标
    int16_t _endX;                      ///< 触摸结束X坐标
    int16_t _endY;                      ///< 触摸结束Y坐标
    
    static const int16_t MIN_SWIPE_DISTANCE = 30;    ///< 最小滑动距离阈值
    static const uint32_t MAX_SWIPE_TIME = 1000;     ///< 最大滑动时间阈值（毫秒）
    
    /**
     * @brief 计算两点间距离
     * @param x1 第一点X坐标
     * @param y1 第一点Y坐标
     * @param x2 第二点X坐标
     * @param y2 第二点Y坐标
     * @return 距离
     */
    float calculateDistance(int16_t x1, int16_t y1, int16_t x2, int16_t y2) const;
    
    /**
     * @brief 判断是否为有效滑动
     * @return true 如果是有效滑动，否则 false
     */
    bool isValidSwipe() const;
};