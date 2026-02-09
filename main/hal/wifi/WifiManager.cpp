#include "WifiManager.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <string.h>
#include <sys/_intsup.h>

static const char *TAG = "WifiManager";

esp_err_t WifiManager::startAp() {
  esp_err_t err = ESP_OK;
  if ((err = init_nvs_flash()) != ESP_OK) {
    return err;
  }
  if ((err = init_softap()) != ESP_OK) {
    return err;
  }
  if ((err = setup_credentials()) != ESP_OK) {
    return err;
  }
  return err;
}

esp_err_t WifiManager::stopAp() { return esp_wifi_stop(); }

esp_err_t WifiManager::init_nvs_flash() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  return ret;
}

esp_err_t WifiManager::init_softap() {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_start());
  return ESP_OK;
}

static const char *SSID = "IReaderMCU";
static const char *PASSWORD = "12345678";

esp_err_t WifiManager::setup_credentials() {
  wifi_config_t wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char *)wifi_config.ap.ssid, SSID);         // 设置 SSID
  strcpy((char *)wifi_config.ap.password, PASSWORD); // 设置密码（至少8位）
  wifi_config.ap.channel = 1; // 设置频道，默认为1，范围1-13
  wifi_config.ap.authmode =
      WIFI_AUTH_WPA2_PSK; // 设置认证模式为 WPA2 PSK，即 WPA2 的密码认证方式
  wifi_config.ap.ssid_len =
      strlen((const char *)wifi_config.ap.ssid); // 设置 SSID 的长度
  /**
   * 只允许一个连接，避免多个设备同时连接后导致资源冲突，比如同时通过httpserver修改设备配置，同时通过httpserver传书
   */
  wifi_config.ap.max_connection = 1; // 设置最大连接数，范围1-8。设置
  ESP_ERROR_CHECK(
      esp_wifi_set_config(WIFI_IF_AP, &wifi_config)); // 应用配置到 AP 接口上
  return ESP_OK;
}

esp_err_t WifiManager::generate_ap_qr_code(char *out, size_t out_size) {
  wifi_config_t wifi_config;
  esp_err_t ret = esp_wifi_get_config(WIFI_IF_AP, &wifi_config);
  if (ret != ESP_OK) {
    return ret;
  }

  snprintf(out, out_size, "WIFI:S:%s;T:%s;P:%s;H:false;;", wifi_config.ap.ssid,
           "WPA2", wifi_config.ap.password);
  return ESP_OK;
}
