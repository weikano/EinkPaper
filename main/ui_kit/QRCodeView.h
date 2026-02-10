#pragma once

#include "View.h"
#include <cstdint>

class QRCodeView : public View
{
public:
    QRCodeView(int16_t width, int16_t height);
    void setQRCode(const std::string& qrcode);    
    void measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;
    std::string className() const override { return "QRCodeView"; }
    ~QRCodeView();
    void onDraw(m5gfx::M5GFX &display) override;
private:
    std::string _qrcode;    
};