#pragma once

#include "../AbstractHttpReqHandler.h"
#include "esp_err.h"
#include "esp_http_server.h"

class CrashLogApiOneHandler : public AbstractHttpReqHandler {
public:
  explicit CrashLogApiOneHandler(const std::string& uri) ;
  virtual ~CrashLogApiOneHandler() ;

protected:
    esp_err_t handleGetRequest(httpd_req_t *req) override;
    esp_err_t handleDeleteRequest(httpd_req_t *req) override;
    esp_err_t handlePostRequest(httpd_req_t *req) override;
};