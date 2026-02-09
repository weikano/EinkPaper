#pragma once

#include "esp_http_server.h"
#include <string>

/**
 * 抽象http请求处理类
 */
class AbstractHttpReqHandler {
public:
    /**
     * 构造函数
     * @param uri 处理的uri
     */
    AbstractHttpReqHandler(std::string uri);
    virtual ~AbstractHttpReqHandler() = default;
    /**
     * 处理http请求
     * @param req http请求
     * @return esp_err_t 处理结果
     */
    esp_err_t handleRequest(httpd_req_t *req);
    inline std::string getUri() { return uri; }
private:
    std::string uri;
    virtual esp_err_t handleGetRequest(httpd_req_t *req) = 0;
    virtual esp_err_t handlePostRequest(httpd_req_t *req) = 0;
};