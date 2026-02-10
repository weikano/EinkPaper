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
    LinearLayout *_container = nullptr;
    QRCodeView *_qrCodeView = nullptr;
    TextView * _wifiName = nullptr;
    TextView * _wifiPassword = nullptr;

};