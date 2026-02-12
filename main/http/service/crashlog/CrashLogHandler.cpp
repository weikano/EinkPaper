#include "CrashLogHandler.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include <string>
#include "../../../assets/assets.h"

static const char *TAG = "CrashLogHandler";

CrashLogHandler::CrashLogHandler(const std::string& uri) : AbstractHttpReqHandler(uri) {}
CrashLogHandler::~CrashLogHandler() {}

esp_err_t CrashLogHandler::handleGetRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling crashlog download request: %s", req->uri);    
    // 对于直接访问/crashlogs的请求，显示旧的列表页面（为了向后兼容）
    // 返回配置页面HTML
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "identity");
    
    // 使用嵌入的HTML资源
    const size_t html_size = binary_assets_crashlog_html_end - binary_assets_crashlog_html_start;
    const char* html_content = reinterpret_cast<const char*>(binary_assets_crashlog_html_start);    
    return httpd_resp_send(req, html_content, html_size);    
}

esp_err_t CrashLogHandler::handlePostRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling POST request for URI: %s", req->uri);
    return ESP_OK;
}

esp_err_t CrashLogHandler::handleDeleteRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling DELETE request for URI: %s", req->uri);
    return ESP_OK;
}
