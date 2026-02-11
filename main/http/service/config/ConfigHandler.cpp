#include "ConfigHandler.h"
#include "../../../config/DeviceConfigManager.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "../../../assets/assets.h"
#include <string>
#include <cstring>
#include <cstdlib>

static const char *TAG = "ConfigHandler";

ConfigHandler::ConfigHandler(const std::string& uri_prefix) : AbstractHttpReqHandler(uri_prefix) {}

ConfigHandler::~ConfigHandler() {}

esp_err_t ConfigHandler::handleGetRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling GET request for URI: %s", req->uri);
    
    // 返回配置页面HTML
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "identity");
    
    // 使用嵌入的HTML资源
    const size_t html_size = binary_assets_config_html_end - binary_assets_config_html_start;
    const char* html_content = reinterpret_cast<const char*>(binary_assets_config_html_start);
    
    return httpd_resp_send(req, html_content, html_size);
}

esp_err_t ConfigHandler::handlePostRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling POST request for URI: %s", req->uri);
    
    // 从请求中读取JSON数据
    int total_len = req->content_len;
    int cur_len = 0;
    char* buf = (char*)malloc(total_len + 1);
    if (!buf) {
        httpd_resp_send_500(req);
        return ESP_ERR_NO_MEM;
    }
    
    while (cur_len < total_len) {
        int received = httpd_req_recv(req, buf + cur_len, total_len - cur_len);
        if (received <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            free(buf);
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';
    
    // 解析JSON - 使用简单字符串查找
    DeviceConfig config = DeviceConfigManager::getInstance().getConfig();
    
    // 查找language字段
    char* lang_pos = strstr(buf, "\"language\":");
    if (lang_pos) {
        lang_pos += strlen("\"language\":");
        while (*lang_pos == ' ' || *lang_pos == '\t') lang_pos++;
        if (*lang_pos == ':') lang_pos++;
        int lang_val = atoi(lang_pos);
        config.language = (LanguageEnum)lang_val;
    }
    
    // 查找refreshInterval字段
    char* interval_pos = strstr(buf, "\"refreshInterval\":");
    if (interval_pos) {
        interval_pos += strlen("\"refreshInterval\":");
        while (*interval_pos == ' ' || *interval_pos == '\t') interval_pos++;
        if (*interval_pos == ':') interval_pos++;
        int interval_val = atoi(interval_pos);
        config.refreshInterval = (uint8_t)interval_val;
    }
    

    
    // 查找refreshMode字段
    char* mode_pos = strstr(buf, "\"refreshMode\":");
    if (mode_pos) {
        mode_pos += strlen("\"refreshMode\":");
        while (*mode_pos == ' ' || *mode_pos == '\t') mode_pos++;
        if (*mode_pos == ':') mode_pos++;
        int mode_val = atoi(mode_pos);
        config.refreshMode = (RefreshMode)mode_val;
    }
    
    // 查找fontSize字段
    char* size_pos = strstr(buf, "\"fontSize\":");
    if (size_pos) {
        size_pos += strlen("\"fontSize\":");
        while (*size_pos == ' ' || *size_pos == '\t') size_pos++;
        if (*size_pos == ':') size_pos++;
        int size_val = atoi(size_pos);
        config.fontSize = (FontSize)size_val;
    }
    
    DeviceConfigManager::getInstance().setConfig(config);
    DeviceConfigManager::getInstance().saveConfigToSdCard();
    
    free(buf);
    
    // 返回成功响应
    httpd_resp_set_type(req, "application/json");
    const char* resp_str = "{\"status\":\"success\",\"message\":\"Configuration updated successfully\"}";
    httpd_resp_sendstr(req, resp_str);
    
    return ESP_OK;
}