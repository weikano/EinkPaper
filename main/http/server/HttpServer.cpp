#include "HttpServer.h"

#include "WifiManager.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/inet.h"
#include <string>
#include "../service/config/ConfigHandler.h"
#include "../service/config/ApiConfigHandler.h"
#include "../service/upload/UploadHandler.h"
#include "assets/assets.h"

static const char *TAG = "HttpServer";

// 新的URL路径
static const char *URI_CONFIG = "/config";
static const char *URI_API_CONFIG = "/api/config";
static const char *URI_UPLOAD = "/upload";

// 创建处理器实例
static ConfigHandler* config_handler = nullptr;
static ApiConfigHandler* api_config_handler = nullptr;
static UploadHandler* upload_handler = nullptr;

static esp_err_t handleRootRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleRootRequest");
  
  // 设置响应头
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "identity");
  
  // 发送嵌入的HTML内容
  const size_t html_size = binary_assets_index_html_end - binary_assets_index_html_start;
  const char* html_content = reinterpret_cast<const char*>(binary_assets_index_html_start);
  
  httpd_resp_send(req, html_content, html_size);
  
  return ESP_OK;
}

static esp_err_t handleConfigRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleConfigRequest for URI: %s", req->uri);
  if (config_handler) {
    return config_handler->handleRequest(req);
  }
  return ESP_ERR_NOT_FOUND;
}

static esp_err_t handleApiConfigRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleApiConfigRequest for URI: %s", req->uri);
  if (api_config_handler) {
    return api_config_handler->handleRequest(req);
  }
  return ESP_ERR_NOT_FOUND;
}

static esp_err_t handleUploadRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleUploadRequest for URI: %s", req->uri);
  if (upload_handler) {
    return upload_handler->handleRequest(req);
  }
  return ESP_ERR_NOT_FOUND;
}

static void register_uri_handlers(httpd_handle_t server) {
  // 主页
  httpd_uri_t root_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = handleRootRequest,
    .user_ctx = NULL
  };
  
  // 配置页面
  httpd_uri_t config_get = {
    .uri = URI_CONFIG,
    .method = HTTP_GET,
    .handler = handleConfigRequest,
    .user_ctx = NULL
  };
  
  // API配置 - GET
  httpd_uri_t api_config_get = {
    .uri = URI_API_CONFIG,
    .method = HTTP_GET,
    .handler = handleApiConfigRequest,
    .user_ctx = NULL
  };
  
  // API配置 - POST
  httpd_uri_t api_config_post = {
    .uri = URI_API_CONFIG,
    .method = HTTP_POST,
    .handler = handleApiConfigRequest,
    .user_ctx = NULL
  };
  
  // API配置 - DELETE (重置)
  httpd_uri_t api_config_delete = {
    .uri = URI_API_CONFIG,
    .method = HTTP_DELETE,
    .handler = handleApiConfigRequest,
    .user_ctx = NULL
  };
  
  // 上传页面
  httpd_uri_t upload_get = {
    .uri = URI_UPLOAD,
    .method = HTTP_GET,
    .handler = handleUploadRequest,
    .user_ctx = NULL
  };
  
  // 上传API
  httpd_uri_t upload_post = {
    .uri = URI_UPLOAD,
    .method = HTTP_POST,
    .handler = handleUploadRequest,
    .user_ctx = NULL
  };
  
  // 注册所有处理器
  httpd_register_uri_handler(server, &root_get);
  httpd_register_uri_handler(server, &config_get);
  httpd_register_uri_handler(server, &api_config_get);
  httpd_register_uri_handler(server, &api_config_post);
  httpd_register_uri_handler(server, &api_config_delete);
  httpd_register_uri_handler(server, &upload_get);
  httpd_register_uri_handler(server, &upload_post);
}

esp_err_t HttpServer::start() {
  ESP_LOGI(TAG, "HttpServer start");
  esp_err_t ret = startWifiHotSpot();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "HttpServer startWifiHotSpot failed");
    return ret;
  }
  
  // 创建处理器实例
  config_handler = new ConfigHandler(std::string(URI_CONFIG));
  api_config_handler = new ApiConfigHandler(std::string(URI_API_CONFIG));
  upload_handler = new UploadHandler(std::string(URI_UPLOAD));
  
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;
  ret = httpd_start(&_server, &config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "HttpServer httpd_start failed");
    delete config_handler;
    delete upload_handler;
    config_handler = nullptr;
    upload_handler = nullptr;
    return ret;
  }
  register_uri_handlers(_server);
  return ret;
}

esp_err_t HttpServer::stop() {
  ESP_LOGI(TAG, "HttpServer stop");
  stopWifiHotSpot();
  esp_err_t ret = httpd_stop(_server);
  
  // 清理处理器实例
  if (config_handler) {
    delete config_handler;
    config_handler = nullptr;
  }
  if (api_config_handler) {
    delete api_config_handler;
    api_config_handler = nullptr;
  }
  if (upload_handler) {
    delete upload_handler;
    upload_handler = nullptr;
  }
  
  return ret;
}

esp_err_t HttpServer::startWifiHotSpot() {
  return WifiManager::getInstance().startAp();
}
esp_err_t HttpServer::stopWifiHotSpot() {
  return WifiManager::getInstance().stopAp();
}

std::string HttpServer::getApQRCode() {
  return WifiManager::getInstance().generate_ap_qr_code();
}

std::string HttpServer::getIpAddress() {
    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    
    if (netif == nullptr) {
        ESP_LOGE(TAG, "Could not get netif handle for AP interface");
        return "";
    }
    
    esp_err_t err = esp_netif_get_ip_info(netif, &ip_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not get IP info for AP interface: %s", esp_err_to_name(err));
        return "";
    }
    
    char ip_str[INET_ADDRSTRLEN];
    inet_ntoa_r(ip_info.ip, ip_str, INET_ADDRSTRLEN);
    
    ESP_LOGI(TAG, "AP IP address: %s", ip_str);
    return std::string(ip_str);
}