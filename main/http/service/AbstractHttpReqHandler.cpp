#include "AbstractHttpReqHandler.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <string>


static const char* TAG = "AbstractHttpReqHandler";

AbstractHttpReqHandler::AbstractHttpReqHandler(std::string uri) : uri(uri) {}

esp_err_t AbstractHttpReqHandler::handleRequest(httpd_req_t *req)
{
    if(!req) {
        return ESP_FAIL;
    }    
    
    switch(req->method) {
        case HTTP_GET:
            return handleGetRequest(req);
        case HTTP_POST:
            return handlePostRequest(req);
        default:
            return ESP_FAIL;
    }
}

