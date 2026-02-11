#pragma once
#include "../AbstractHttpReqHandler.h"
#include "esp_http_server.h"
#include <string>

class ConfigHandler : public AbstractHttpReqHandler {
public:
    explicit ConfigHandler(const std::string& uri_prefix);
    virtual ~ConfigHandler();

private:
    /**
     * @brief 处理GET请求 - 返回配置页面
     * @param req HTTP请求指针
     * @return esp_err_t 错误码
     */
    esp_err_t handleGetRequest(httpd_req_t *req) override;

    /**
     * @brief 处理POST请求 - 更新配置
     * @param req HTTP请求指针
     * @return esp_err_t 错误码
     */
    esp_err_t handlePostRequest(httpd_req_t *req) override;


};