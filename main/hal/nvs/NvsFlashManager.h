#pragma once
#include "esp_err.h"

/**
* 初始化nvs flash
* NVS 是 ESP32 系统里的“配置数据库 + 系统状态存储”，类似PC上的BIOS+系统设置
* 用来在断电/重启后，可靠地保存“少量但重要”的数据。
* 类似wifi重连上次ssid等功能，都会向nvs flash写入数据，这样在打开wifi后就会自动重连了
* 还有蓝牙等功能，也会通过nvs flash保存状态和配置
* OTA时用来记录升级状态以及回滚标志
* ESP-NETIF等网络组件也会使用nvs flash来保存网络状态和配置
* 需要在app_main中进行初始化
*/
class NvsFlashManager {
public:
  static NvsFlashManager &getInstance() {
    static NvsFlashManager instance;
    return instance;
  }
  /**
   * 初始化nvs flash
   * @return esp_err_t 错误码
   */
  esp_err_t init();
};