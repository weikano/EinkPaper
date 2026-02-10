#pragma once
#include "../page_manager/Page.h"
#include "../ui_kit/LinearLayout.h"
#include "../ui_kit/QRCodeView.h"
#include "../ui_kit/TextView.h"
#include "LinearLayout.h"

class HttpServerPage : public Page {
public:
    HttpServerPage();
    ~HttpServerPage();    

    void onCreate() override;    
    void onDestroy() override;    
private:
    TextView * _wifiHint = nullptr;
    TextView * _serverHint = nullptr;
    LinearLayout *_container = nullptr;
    QRCodeView *_qrCodeWifi = nullptr;
    TextView * _wifiName = nullptr;
    TextView * _wifiPassword = nullptr;
    QRCodeView *_qrCodeHttpServerIp = nullptr;
    TextView * _httpServerIp = nullptr;
};