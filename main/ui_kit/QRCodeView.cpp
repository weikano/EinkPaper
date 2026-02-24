#include "QRCodeView.h"
#include "esp_log.h"

static const char* TAG = "QRCodeView";

QRCodeView::QRCodeView(int16_t width, int16_t height)
    : View(width, height)
{
    ESP_LOGI(TAG, "QRCodeView created with width %d and height %d", width, height);
}

QRCodeView::~QRCodeView()
{
    ESP_LOGI(TAG, "QRCodeView destroyed");
}

void QRCodeView::setQRCode(const std::string& qrcode)
{
    if(_qrcode != qrcode) {
        _qrcode = qrcode;
        markDirty();
    }    
}

void QRCodeView::onDraw(lgfx::LovyanGFX& display)
{
    if(_qrcode.empty()) {
        return;
    }
    display.qrcode(_qrcode.c_str(), getLeft(), getTop(), getWidth(), 0, true);
}

void QRCodeView::onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec)
{
    View::onMeasure(widthMeasureSpec, heightMeasureSpec);    
}
