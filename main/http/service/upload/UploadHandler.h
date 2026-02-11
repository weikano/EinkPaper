#pragma once
#include "../AbstractHttpReqHandler.h"
#include "esp_http_server.h"
#include <string>

class UploadHandler : public AbstractHttpReqHandler {
public:
    explicit UploadHandler(const std::string& uri_prefix);
    virtual ~UploadHandler();

private:
    /**
     * @brief 处理GET请求 - 返回上传页面
     * @param req HTTP请求指针
     * @return esp_err_t 错误码
     */
    esp_err_t handleGetRequest(httpd_req_t *req) override;

    /**
     * @brief 处理POST请求 - 文件上传
     * @param req HTTP请求指针
     * @return esp_err_t 错误码
     */
    esp_err_t handlePostRequest(httpd_req_t *req) override;
};