#pragma once

#include "esp_err.h"

/**
 * @brief 初始化 ESP-IDF 的网络接口框架（TCP/IP 栈 + 网络接口抽象层），
 * 为 Wi-Fi / Ethernet / PPP 等一切网络能力打基础。
 */
class NetifManager {
public:
    static NetifManager& getInstance() {
        static NetifManager instance;
        return instance;
    }

    esp_err_t init();
};