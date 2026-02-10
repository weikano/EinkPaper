#include "HttpServerPage.h"

#include "../http/server/HttpServer.h"
#include "Page.h"
#include "esp_err.h"
#include "../ui_kit/QRCodeView.h"
#include "../ui_kit/FrameLayout.h"
#include "../page_manager/PageManager.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"

static const char *TAG = "HttpServerPage";

HttpServerPage::HttpServerPage() : Page(PageType::HTTP_SERVER, "HTTP服务器") {
  ESP_LOGD(TAG, "HttpServerPage created");
}

HttpServerPage::~HttpServerPage() { ESP_LOGD(TAG, "HttpServerPage destroyed"); }

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
  _qrCodeView = new QRCodeView(200, 200);  
  _qrCodeView->setQRCode(HttpServer::getInstance().getApQRCode().c_str());
  _container->addChild(_qrCodeView);
  _wifiName = new TextView(200, 50);
  wifi_config_t wifi_config;
  esp_wifi_get_config(WIFI_IF_AP, &wifi_config);
  char wifiName[100];
  sprintf(wifiName, "WiFi名称: %s", wifi_config.ap.ssid);
  _wifiName->setText(wifiName);
  _wifiPassword = new TextView(200, 50);
  char wifiPassword[100];
  sprintf(wifiPassword, "WiFi密码: %s", wifi_config.ap.password);
  _wifiPassword->setText(wifiPassword);
//   _container->addChild(_wifiName);
//   _container->addChild(_wifiPassword);
  
  setRootView(_container);
}

void HttpServerPage::onDestroy() {
  HttpServer::getInstance().stop();
  Page::onDestroy();
}
