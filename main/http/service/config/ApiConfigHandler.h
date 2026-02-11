#ifndef API_CONFIG_HANDLER_H
#define API_CONFIG_HANDLER_H

#include "../AbstractHttpReqHandler.h"
#include "../../../config/DeviceConfig.h"
#include "esp_http_server.h"
#include <string>

/**
 * @brief API配置处理器，用于处理/api/config相关请求
 * @ingroup http_service
 */
class ApiConfigHandler : public AbstractHttpReqHandler {
public:
    /**
     * @brief 构造函数
     * @param[in] uri_prefix URI前缀
     */
    explicit ApiConfigHandler(const std::string& uri_prefix);

    /**
     * @brief 析构函数
     */
    ~ApiConfigHandler();

    /**
     * @brief 处理GET请求 - 获取配置
     * @param[in] req HTTP请求指针
     * @return esp_err_t ESP_OK表示成功
     */
    esp_err_t handleGetRequest(httpd_req_t *req) override;

    /**
     * @brief 处理POST请求 - 更新配置
     * @param[in] req HTTP请求指针
     * @return esp_err_t ESP_OK表示成功
     */
    esp_err_t handlePostRequest(httpd_req_t *req) override;

    /**
     * @brief 处理DELETE请求 - 重置配置
     * @param[in] req HTTP请求指针
     * @return esp_err_t ESP_OK表示成功
     */
    esp_err_t handleDeleteRequest(httpd_req_t *req) override;

private:
    /**
     * @brief 构建JSON响应
     * @param[in] config 设备配置
     * @return std::string JSON字符串
     */
    std::string buildConfigJson(const DeviceConfig& config);
    
    /**
     * @brief 重置为默认配置
     * @return esp_err_t ESP_OK表示成功
     */
    esp_err_t resetConfig();
};

#endif // API_CONFIG_HANDLER_H