#include "HttpServerPage.h"

#include "../http/server/HttpServer.h"
#include "Page.h"
#include "View.h"
#include "esp_err.h"
#include "../ui_kit/QRCodeView.h"
#include "../page_manager/PageManager.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"

static const char *TAG = "HttpServerPage";

HttpServerPage::HttpServerPage() : Page(PageType::HTTP_SERVER, "HTTP服务器") {
  ESP_LOGI(TAG, "HttpServerPage created");
}

HttpServerPage::~HttpServerPage() { ESP_LOGI(TAG, "HttpServerPage destroyed"); }

void HttpServerPage::onCreate() {
  Page::onCreate();  
  if (HttpServer::getInstance().start() != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start HTTP server");
    PageManager::getInstance().finishActivity();
    return;
  }
  
  
  ESP_LOGI(TAG, "HTTP server started");
  std::string ipAddress = HttpServer::getInstance().getIpAddress();
  ESP_LOGI(TAG, "AP IP Address: %s", ipAddress.c_str());
  _container = new LinearLayout(M5.Display.width(), M5.Display.height(), LinearLayout::Orientation::VERTICAL);  
  _container->setPadding(12, 12, 12, 12);
  _wifiHint = new TextView(MATCH_PARENT, 50);
  _wifiHint->setText("扫描二维码连接WiFi");
  _container->addChild(_wifiHint);
  _serverHint = new TextView(MATCH_PARENT, 50);
  _serverHint->setText("扫描二维码打开HTTP服务器");
  _qrCodeWifi = new QRCodeView(240 , 240);  
  _qrCodeWifi->setQRCode(HttpServer::getInstance().getApQRCode().c_str());
  _container->addChild(_qrCodeWifi);
  _wifiName = new TextView(MATCH_PARENT, 50);
  wifi_config_t wifi_config;
  esp_wifi_get_config(WIFI_IF_AP, &wifi_config);
  char wifiName[100];
  sprintf(wifiName, "WiFi名称: %s", wifi_config.ap.ssid);
  _wifiName->setText(wifiName);
  _wifiPassword = new TextView(MATCH_PARENT, 50);
  char wifiPassword[100];
  sprintf(wifiPassword, "WiFi密码: %s", wifi_config.ap.password);
  _wifiPassword->setText(wifiPassword);
  _container->addChild(_wifiName);
  _container->addChild(_wifiPassword);
  _container->addChild(_serverHint);
  char httpServerIp[100];
  sprintf(httpServerIp, "http://%s", ipAddress.c_str());
  _qrCodeHttpServerIp = new QRCodeView(240, 240);  
  ESP_LOGI(TAG, "HTTP server IP: %s", httpServerIp);
  _qrCodeHttpServerIp->setQRCode(httpServerIp);
  _container->addChild(_qrCodeHttpServerIp);
  _httpServerIp = new TextView(MATCH_PARENT, 50);
  sprintf(wifiPassword, "HTTP服务器IP: %s", ipAddress.c_str());
  _httpServerIp->setText(wifiPassword);
  _container->addChild(_httpServerIp);
  
  setRootView(_container);
}

void HttpServerPage::onDestroy() {
  HttpServer::getInstance().stop();
  Page::onDestroy();
}
