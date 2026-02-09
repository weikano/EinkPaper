#pragma once


#include "AbstractHttpReqHandler.h"fo
#include "esp_err.h"

class HttpDeviceConfigHandler  : public AbstractHttpReqHandler {
    /**
     * 构造函数
     */
    HttpDeviceConfigHandler(std::string uri);
    esp_err_t handleRequest(httpd_req_t *req) override;
};
