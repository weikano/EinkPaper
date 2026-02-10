#include "HttpServer.h"

#include "WifiManager.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/inet.h"
#include <string>

static const char *TAG = "HttpServer";

static const char *URI_DEVICE_CONFIG = "/api/v1/deviceconfig";

static esp_err_t handleRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleRequest");

  return ESP_OK;
}

static esp_err_t handleRootRequest(httpd_req_t *req) {
  ESP_LOGI(TAG, "HttpServer handleRootRequest");
  httpd_send(req, "Hello, World!\n", strlen("Hello, World!\n"));
  return ESP_OK;
}

static void register_uri_handlers(httpd_handle_t server) {
  httpd_uri_t root_config_get = {.uri = "/",
                                       .method = HTTP_GET,
                                       .handler = handleRootRequest,
                                       .user_ctx = NULL};    
  httpd_uri_t uri_device_config_get = {.uri = URI_DEVICE_CONFIG,
                                       .method = HTTP_GET,
                                       .handler = handleRequest,
                                       .user_ctx = NULL};
  httpd_uri_t uri_device_config_post = {.uri = URI_DEVICE_CONFIG,
                                        .method = HTTP_POST,
                                        .handler = handleRequest,
                                        .user_ctx = NULL};
  httpd_uri_t uri_books_get = {.uri = "/api/v1/books",
                               .method = HTTP_GET,
                               .handler = handleRequest,
                               .user_ctx = NULL};
  httpd_uri_t uri_books_post = {.uri = "/api/v1/books",
                                .method = HTTP_POST,
                                .handler = handleRequest,
                                .user_ctx = NULL};
  httpd_register_uri_handler(server, &root_config_get);
  httpd_register_uri_handler(server, &uri_device_config_get);
  httpd_register_uri_handler(server, &uri_device_config_post);
  httpd_register_uri_handler(server, &uri_books_post);
  httpd_register_uri_handler(server, &uri_books_get);
}

esp_err_t HttpServer::start() {
  ESP_LOGI(TAG, "HttpServer start");
  esp_err_t ret = startWifiHotSpot();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "HttpServer startWifiHotSpot failed");
    return ret;
  }
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;
  ret = httpd_start(&_server, &config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "HttpServer httpd_start failed");
    return ret;
  }
  register_uri_handlers(_server);
  return ret;
}

esp_err_t HttpServer::stop() {
  ESP_LOGI(TAG, "HttpServer stop");
  stopWifiHotSpot();
  return httpd_stop(_server);
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