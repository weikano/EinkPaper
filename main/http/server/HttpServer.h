#pragma once
#include "esp_http_server.h"
#include "esp_err.h"
#include <string>
/**
 * http服务，用于设置设备配置信息或者wifi传书
 */
class HttpServer {
public:
    static HttpServer& getInstance()
    {
        static HttpServer instance;
        return instance;
    }
    /**
     * 启动http服务
     */
    esp_err_t start();
    /**
     * 停止http服务
     */
    esp_err_t stop();
    /**
     * 获取AP模式下的二维码
     */
    std::string getApQRCode();
    /**
     * 获取AP模式下的IP地址
     */
    std::string getIpAddress();  // 获取AP模式下的IP地址
    
private:
    httpd_handle_t _server;
    /**
    * 启动wifi热点
    */
    esp_err_t startWifiHotSpot();
    /*
     * 停止wifi热点
     */
    esp_err_t stopWifiHotSpot();
    
};