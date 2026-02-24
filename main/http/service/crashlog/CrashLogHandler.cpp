#include "CrashLogHandler.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include <string>
#include "../PageFileResponder.h"

static const char *TAG = "CrashLogHandler";

CrashLogHandler::CrashLogHandler(const std::string& uri) : AbstractHttpReqHandler(uri) {}
CrashLogHandler::~CrashLogHandler() {}

esp_err_t CrashLogHandler::handleGetRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling crashlog download request: %s", req->uri);    
    return sendFileResponse(req, "/littlefs/crashlog.html", "text/html");
}

esp_err_t CrashLogHandler::handlePostRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling POST request for URI: %s", req->uri);
    return ESP_OK;
}

esp_err_t CrashLogHandler::handleDeleteRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling DELETE request for URI: %s", req->uri);
    return ESP_OK;
}
