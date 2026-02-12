#include "AbstractHttpReqHandler.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include <string>


AbstractHttpReqHandler::AbstractHttpReqHandler(const std::string& uri) : uri(uri) {}

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
        case HTTP_DELETE:
            return handleDeleteRequest(req);
        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
}