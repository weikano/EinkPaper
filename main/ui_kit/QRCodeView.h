#pragma once

#include "View.h"
#include <cstdint>

class QRCodeView : public View
{
public:
    QRCodeView(int16_t width, int16_t height);
    void setQRCode(const std::string& qrcode);
    std::string className() const override { return "QRCodeView"; }
    ~QRCodeView();
protected:
    void onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec) override;    
    void onDraw(lgfx::LovyanGFX &display) override;

private:
    std::string _qrcode;    
};
