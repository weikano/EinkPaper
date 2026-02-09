#pragma once

#include <esp_err.h>

/**
 * wifi管理类，用于启动和停止wifi热点，以及生成wifi热点的二维码
 */
class WifiManager {
public:
    static WifiManager& getInstance()
    {
        static WifiManager instance;
        return instance;
    }
    /**
     * 启动wifi热点
     * @return esp_err_t 错误码
     */
    esp_err_t startAp();
    /**
     * 停止wifi热点
     * @return esp_err_t 错误码
     */
    esp_err_t stopAp();
    /**
     * 生成wifi热点的二维码
     * @param out 输出二维码字符串的缓冲区
     * @param out_size 输出缓冲区的大小
     * @return esp_err_t 错误码
     */
    esp_err_t generate_ap_qr_code(char* out, size_t out_size);
private:
    /**
     * 初始化nvs flash
     * @return esp_err_t 错误码
     */
    esp_err_t init_nvs_flash();
    /**
     * 初始化wifi热点
     * @return esp_err_t 错误码
     */
    esp_err_t init_softap();
    /**
     * 设置wifi热点的用户名和密码
     * @return esp_err_t 错误码
     */
    esp_err_t setup_credentials();
};