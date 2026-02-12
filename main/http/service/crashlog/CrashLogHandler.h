#pragma once

#include "../AbstractHttpReqHandler.h"
#include <string>

/**
 * @brief 处理crashlog下载请求的处理器
 */
class CrashLogHandler : public AbstractHttpReqHandler {
public:
    /**
     * @brief 构造函数
     * @param uri 处理的uri
     */
    explicit CrashLogHandler(const std::string& uri);

    /**
     * @brief 析构函数
     */
    virtual ~CrashLogHandler() ;

protected:
    /**
     * @brief 处理GET请求 - 下载特定文件或执行特殊操作
     * @param req http请求
     * @return esp_err_t 处理结果
     */
    virtual esp_err_t handleGetRequest(httpd_req_t *req) override;
    virtual esp_err_t handlePostRequest(httpd_req_t *req) override;
    virtual esp_err_t handleDeleteRequest(httpd_req_t *req) override;
};