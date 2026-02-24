#include "NetifManager.h"

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_log.h"

esp_err_t NetifManager::init() {
    ESP_LOGI("NetifManager", "Initializing network interface");
    esp_netif_init();
    esp_event_loop_create_default();
    return ESP_OK;
}


