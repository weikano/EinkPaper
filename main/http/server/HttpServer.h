#pragma once
#include "esp_http_server.h"
#include "esp_err.h"
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