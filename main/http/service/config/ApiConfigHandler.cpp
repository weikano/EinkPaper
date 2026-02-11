#include "ApiConfigHandler.h"
#include "../../../config/DeviceConfigManager.h"
#include "esp_log.h"
#include <string>
#include <cstring>
#include <cstdlib>

static const char *TAG = "ApiConfigHandler";

ApiConfigHandler::ApiConfigHandler(const std::string& uri_prefix) : AbstractHttpReqHandler(uri_prefix) {}

ApiConfigHandler::~ApiConfigHandler() {}

esp_err_t ApiConfigHandler::handleGetRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling GET request for API config: %s", req->uri);
    
    // 获取当前配置
    DeviceConfig config = DeviceConfigManager::getInstance().getConfig();
    
    // 构建JSON响应
    std::string json_response = buildConfigJson(config);
    
    // 设置响应头
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    
    // 发送响应
    return httpd_resp_sendstr(req, json_response.c_str());
}

esp_err_t ApiConfigHandler::handlePostRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling POST request for API config: %s", req->uri);
    
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
    
    // 更新配置
    DeviceConfigManager::getInstance().setConfig(config);
    DeviceConfigManager::getInstance().saveConfigToSdCard();
    
    free(buf);
    
    // 返回成功响应
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    const char* resp_str = "{\"status\":\"success\",\"message\":\"Configuration updated successfully\"}";
    return httpd_resp_sendstr(req, resp_str);
}

esp_err_t ApiConfigHandler::handleDeleteRequest(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handling DELETE request for API config reset: %s", req->uri);
    
    // 重置配置
    esp_err_t ret = resetConfig();
    if (ret != ESP_OK) {
        httpd_resp_send_500(req);
        return ret;
    }
    
    // 返回成功响应
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    const char* resp_str = "{\"status\":\"success\",\"message\":\"Configuration reset successfully\"}";
    return httpd_resp_sendstr(req, resp_str);
}

std::string ApiConfigHandler::buildConfigJson(const DeviceConfig& config) {
    // 构造 JSON 字符串，包含所有字段（包括fontPath，因为它是结构体的一部分）
    std::string json_str = "{";
    json_str += "\"version\":" + std::to_string(config.version) + ",";
    json_str += "\"language\":" + std::to_string(static_cast<int>(config.language)) + ",";
    json_str += "\"refreshInterval\":" + std::to_string(config.refreshInterval) + ",";
    json_str += "\"refreshMode\":" + std::to_string(static_cast<int>(config.refreshMode)) + ",";
    json_str += "\"fontSize\":" + std::to_string(static_cast<int>(config.fontSize)) + ",";
    json_str += "\"fontPath\":\"" + config.fontPath + "\"";  // 包含fontPath字段，即使前端不显示
    json_str += "}";
    
    return json_str;
}

esp_err_t ApiConfigHandler::resetConfig() {
    // 创建默认配置
    DeviceConfig defaultConfig = {};
    defaultConfig.version = 0;
    defaultConfig.language = LanguageEnum::Chinese;
    defaultConfig.refreshInterval = 10;
    defaultConfig.fontPath = std::string("");  // 使用std::string空字符串
    defaultConfig.refreshMode = RefreshMode::Quality;
    defaultConfig.fontSize = FontSize::Medium;
    
    // 设置默认配置
    DeviceConfigManager::getInstance().setConfig(defaultConfig);
    DeviceConfigManager::getInstance().saveConfigToSdCard();
    
    return ESP_OK;
}