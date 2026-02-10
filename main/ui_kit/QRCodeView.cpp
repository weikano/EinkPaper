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

void QRCodeView::onDraw(m5gfx::M5GFX& display)
{
    if(_qrcode.empty()) {
        return;
    }
    M5.Display.qrcode(_qrcode.c_str(), getLeft(), getTop(), getWidth(), 0, true);
}

void QRCodeView::onMeasure(int16_t widthMeasureSpec, int16_t heightMeasureSpec)
{
    View::onMeasure(widthMeasureSpec, heightMeasureSpec);    
}