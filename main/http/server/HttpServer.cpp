#include "HttpServer.h"

#include "../service/config/ApiConfigHandler.h"
#include "../service/config/ConfigHandler.h"
#include "../service/upload/UploadHandler.h"
#include "../service/crashlog/CrashLogHandler.h"
#include "../service/crashlog/CrashLogApiAllHandler.h"
#include "../service/crashlog/CrashLogApiOneHandler.h"
#include "WifiManager.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "http_parser.h"
#include "lwip/inet.h"
#include <string>
#include <sys/_intsup.h>

#include "assets/assets.h"

static const char *TAG = "HttpServer";

// 新的URL路径 - 页面路由
static const char *URI_PAGE_ROOT = "/";
static const char *URI_PAGE_CONFIG = "/page/config";
static const char *URI_PAGE_UPLOAD = "/page/upload";
static const char *URI_PAGE_CRASHLOGS = "/page/crashlogs";

// 新的URL路径 - API路由
static const char *URI_API_CONFIG = "/api/config";
static const char *URI_API_UPLOAD = "/api/upload";
static const char *URI_API_CRASHLOGS_ALL = "/api/crashlogs/all"; //所有crashlog文件
static const char *URI_API_CRASHLOGS_ONE = "/api/crashlogs/file"; //单个crashlog文件

// 创建处理器实例
static ConfigHandler *config_handler = nullptr;
static ApiConfigHandler *api_config_handler = nullptr;
static UploadHandler *upload_handler = nullptr;
static CrashLogHandler *crashlog_handler = nullptr;
static CrashLogApiAllHandler *crashlog_api_all_handler = nullptr;
static CrashLogApiOneHandler *crashlog_api_one_handler = nullptr;

static esp_err_t handleRootRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleRootRequest");

  // 设置响应头
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "identity");

  // 发送嵌入的HTML内容
  const size_t html_size =
      binary_assets_index_html_end - binary_assets_index_html_start;
  const char *html_content =
      reinterpret_cast<const char *>(binary_assets_index_html_start);

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

static esp_err_t handleCrashlogPageRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleCrashlogPageRequest for URI: %s", req->uri);
  if (crashlog_handler) {
    return crashlog_handler->handleRequest(req);
  }
  return ESP_ERR_NOT_FOUND;
}

static esp_err_t handleCrashlogApiAllRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleCrashlogApiAllRequest for URI: %s", req->uri);
  if (crashlog_api_all_handler) {
    return crashlog_api_all_handler->handleRequest(req);
  }
  return ESP_ERR_NOT_FOUND;
}

static esp_err_t handleCrashlogApiOneRequest(httpd_req_t *req)
{
  ESP_LOGI(TAG, "HttpServer handleCrashlogApiOneRequest for URI: %s", req->uri);
  if (crashlog_api_one_handler) {
    return crashlog_api_one_handler->handleRequest(req);
  }
  return ESP_ERR_NOT_FOUND;
}

static void register_uri_handlers(httpd_handle_t server) {
  // 主页
  httpd_uri_t root_get = {.uri = URI_PAGE_ROOT,
                          .method = HTTP_GET,
                          .handler = handleRootRequest,
                          .user_ctx = NULL};

  // 配置页面
  httpd_uri_t config_get = {.uri = URI_PAGE_CONFIG,
                            .method = HTTP_GET,
                            .handler = handleConfigRequest,
                            .user_ctx = NULL};

  // API配置 - GET
  httpd_uri_t api_config_get = {.uri = URI_API_CONFIG,
                                .method = HTTP_GET,
                                .handler = handleApiConfigRequest,
                                .user_ctx = NULL};

  // API配置 - POST
  httpd_uri_t api_config_post = {.uri = URI_API_CONFIG,
                                 .method = HTTP_POST,
                                 .handler = handleApiConfigRequest,
                                 .user_ctx = NULL};

  // API配置 - DELETE (重置)
  httpd_uri_t api_config_delete = {.uri = URI_API_CONFIG,
                                   .method = HTTP_DELETE,
                                   .handler = handleApiConfigRequest,
                                   .user_ctx = NULL};

  // 上传页面
  httpd_uri_t upload_get = {.uri = URI_PAGE_UPLOAD,
                            .method = HTTP_GET,
                            .handler = handleUploadRequest,
                            .user_ctx = NULL};

  // 上传API
  httpd_uri_t upload_post = {.uri = URI_API_UPLOAD,
                             .method = HTTP_POST,
                             .handler = handleUploadRequest,
                             .user_ctx = NULL};

  // 崩溃日志页面
  httpd_uri_t crashlogs_get = {.uri = URI_PAGE_CRASHLOGS,
                               .method = HTTP_GET,
                               .handler = handleCrashlogPageRequest,
                               .user_ctx = NULL};

  httpd_uri_t crashlogs_api_all_get = {.uri = URI_API_CRASHLOGS_ALL,
                                   .method = HTTP_GET,
                                   .handler = handleCrashlogApiAllRequest,
                                   .user_ctx = NULL};
  httpd_uri_t crashlogs_api_all_delete = {.uri = URI_API_CRASHLOGS_ALL,
                                   .method = HTTP_DELETE,
                                   .handler = handleCrashlogApiAllRequest,
                                   .user_ctx = NULL};                                                                      
  httpd_uri_t crashlogs_api_one_get = {.uri = URI_API_CRASHLOGS_ONE,
                                   .method = HTTP_GET,
                                   .handler = handleCrashlogApiOneRequest,
                                   .user_ctx = NULL};
  httpd_uri_t crashlogs_api_one_delete = {.uri = URI_API_CRASHLOGS_ONE,
                                   .method = HTTP_DELETE,
                                   .handler = handleCrashlogApiOneRequest,
                                   .user_ctx = NULL};                                   
  

  // 注册所有处理器
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &config_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_config_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_config_post));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api_config_delete));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &upload_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &upload_post));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &crashlogs_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &crashlogs_api_all_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &crashlogs_api_all_delete));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &crashlogs_api_one_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &crashlogs_api_one_delete));
}

esp_err_t HttpServer::start() {
  ESP_LOGI(TAG, "HttpServer start");
  esp_err_t ret = startWifiHotSpot();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "HttpServer startWifiHotSpot failed");
    return ret;
  }

  // 创建处理器实例
  config_handler = new ConfigHandler(std::string(URI_PAGE_CONFIG));
  api_config_handler = new ApiConfigHandler(std::string(URI_API_CONFIG));
  upload_handler = new UploadHandler(std::string(URI_PAGE_UPLOAD));
  crashlog_handler = new CrashLogHandler(std::string(URI_PAGE_CRASHLOGS));
  crashlog_api_all_handler = new CrashLogApiAllHandler(std::string(URI_API_CRASHLOGS_ALL));
  crashlog_api_one_handler = new CrashLogApiOneHandler(std::string(URI_API_CRASHLOGS_ONE));



  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 30;  // 增加URI处理器的最大数量
  config.max_open_sockets = 7;   // 增加最大连接数
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
  if (crashlog_handler) {
    delete crashlog_handler;
    crashlog_handler = nullptr;
  }
  if (crashlog_api_all_handler) {
    delete crashlog_api_all_handler;
    crashlog_api_all_handler = nullptr;
  }
  if (crashlog_api_one_handler) {
    delete crashlog_api_one_handler;
    crashlog_api_one_handler = nullptr;
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
    ESP_LOGE(TAG, "Could not get IP info for AP interface: %s",
             esp_err_to_name(err));
    return "";
  }

  char ip_str[INET_ADDRSTRLEN];
  inet_ntoa_r(ip_info.ip, ip_str, INET_ADDRSTRLEN);

  ESP_LOGI(TAG, "AP IP address: %s", ip_str);
  return std::string(ip_str);
}